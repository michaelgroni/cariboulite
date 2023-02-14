#ifndef ZF_LOG_LEVEL
    #define ZF_LOG_LEVEL ZF_LOG_VERBOSE
#endif

#define ZF_LOG_DEF_SRCLOC ZF_LOG_SRCLOC_LONG
#define ZF_LOG_TAG "CARIBOU_PROG"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "zf_log/zf_log.h"
#include "caribou_prog.h"


#define LATTICE_ICE40_BUFSIZE 512
#define LATTICE_ICE40_TO_COUNT 200

//---------------------------------------------------------------------------
/**
 * @brief check whether the fpga is currently in a "programmed" mode
 * 
 * @param dev programmer context
 * @return int success(0), error(-1)
 */
static int caribou_prog_check_if_programmed(caribou_prog_st* dev)
{
	if (dev == NULL)
	{
		ZF_LOGE("device pointer NULL");
		return -1;
	}

	if (!dev->initialized)
	{
		ZF_LOGE("device not initialized");
		return -1;
	}

	return io_utils_read_gpio(dev->cdone_pin);
}

//---------------------------------------------------------------------------
/**
 * @brief initialize programmer context
 * 
 * @param dev programmer device context
 * @param io_spi spi device wrapper
 * @return int success(0) / error(-1)
 */
int caribou_prog_init(caribou_prog_st *dev, io_utils_spi_st* io_spi)
{
	if (dev == NULL)
	{
		ZF_LOGE("device pointer NULL");
		return -1;
	}

	if (dev->initialized)
	{
		ZF_LOGE("device already initialized");
		return 0;
	}

	dev->io_spi = io_spi;

	// Init pin directions
	io_utils_setup_gpio(dev->cdone_pin, io_utils_dir_input, io_utils_pull_up);
	io_utils_setup_gpio(dev->cs_pin, io_utils_dir_output, io_utils_pull_up);
	io_utils_setup_gpio(dev->reset_pin, io_utils_dir_output, io_utils_pull_up);

	dev->io_spi_handle = io_utils_spi_add_chip(	dev->io_spi, 
												dev->cs_pin, 
												5000000, 
												0, 
												0,
												io_utils_spi_chip_ice40_prog, NULL);

	dev->initialized = 1;

	// check if the FPGA is already configures
	if (caribou_prog_check_if_programmed(dev) == 1)
	{
		ZF_LOGI("FPGA is already configured and running");
	}

	ZF_LOGI("device init completed");

	return 0;
}

//---------------------------------------------------------------------------
/**
 * @brief release the fpga programmer context
 * 
 * @param dev device context
 * @return int success(0) / error(-1)
 */
int caribou_prog_release(caribou_prog_st *dev)
{
	if (dev == NULL)
	{
		ZF_LOGE("device pointer NULL");
		return -1;
	}

	if (!dev->initialized)
	{
		ZF_LOGE("device not initialized");
		return 0;
	}

	// Release the SPI device
	io_utils_spi_remove_chip(dev->io_spi, dev->io_spi_handle);

	// Release the GPIO functions
	io_utils_setup_gpio(dev->cdone_pin, io_utils_dir_input, io_utils_pull_up);
	io_utils_setup_gpio(dev->cs_pin, io_utils_dir_input, io_utils_pull_up);
	io_utils_setup_gpio(dev->reset_pin, io_utils_dir_input, io_utils_pull_up);

	dev->initialized = 0;
	ZF_LOGI("device release completed");

	return 0;
}

//---------------------------------------------------------------------------
/**
 * @brief performs preparation steps towards bitstream programming
 * 
 * @param dev device context
 * @return int success(0) / error(-1)
 */
static int caribou_prog_configure_prepare(caribou_prog_st *dev)
{
	long ct;
	uint8_t byte = 0xFF;
	uint8_t rxbyte = 0;

	// set SS low for spi config
	io_utils_write_gpio_with_wait(dev->cs_pin, 0, 200);

	// pulse RST low min 200 us ns
	io_utils_write_gpio_with_wait(dev->reset_pin, 0, 200);
	io_utils_usleep(200);

	// Wait for DONE low
	ZF_LOGI("RESET low, Waiting for CDONE low");
	ct = LATTICE_ICE40_TO_COUNT;

	while(io_utils_read_gpio(dev->cdone_pin)==1 && ct--)
	{
		asm volatile ("nop");
	}
	if (!ct)
	{
		ZF_LOGE("CDONE didn't fall during RESET='0'");
		return -1;
	}
	io_utils_write_gpio_with_wait(dev->reset_pin, 1, 200);
	io_utils_usleep(1200);

	// Send 8 dummy clocks with SS high
	io_utils_write_gpio_with_wait(dev->cs_pin, 1, 200);
	io_utils_spi_transmit(dev->io_spi, dev->io_spi_handle, &byte, &rxbyte, 1, io_utils_spi_write);

	return 0;
}

//---------------------------------------------------------------------------
/**
 * @brief performs finalization steps after bitstream programming
 * 
 * @param dev device context
 * @return int success(0) / error(-1)
 */
static int caribou_prog_configure_finish(caribou_prog_st *dev)
{
	int ct = 0;
	uint8_t byte = 0xFF;
	uint8_t rxbyte = 0;
	unsigned char dummybuf[10] = {0};
	// Transmit at least 49 clock cycles of clock
	io_utils_spi_transmit(dev->io_spi, dev->io_spi_handle,
								dummybuf,
								dummybuf,
								8,
								io_utils_spi_write);


	/* send dummy data while waiting for DONE */
 	ZF_LOGI("sending dummy clocks, waiting for CDONE to rise (or fail)");

	ct = LATTICE_ICE40_TO_COUNT;
	while(caribou_prog_check_if_programmed(dev)==0 && ct--)
	{
		io_utils_spi_transmit(dev->io_spi, dev->io_spi_handle,
								&byte, &rxbyte, 1, io_utils_spi_write);
	}

	if(ct)
	{
	 	ZF_LOGI("%d dummy clocks sent", (LATTICE_ICE40_TO_COUNT-ct)*8);
    }
	else
	{
		ZF_LOGI("timeout waiting for CDONE");
	}

	/* return status */
	if (caribou_prog_check_if_programmed(dev)==0)
	{
		ZF_LOGE("config failed - CDONE not high");
		return -1;
	}
	return 0;
}

//---------------------------------------------------------------------------
/**
 * @brief starts programming sequence from a memory buffer
 * 
 * @param dev device context
 * @param dest the destination of the bitstream
 * @param buffer bitstream buffer pointer
 * @param buffer_size bitstream buffer length in bytes
 * @return int success(0), error (-1)
 */
int caribou_prog_configure_from_buffer(	caribou_prog_st *dev, 
										uint8_t *buffer, 
										uint32_t buffer_size)
{
	int ct = 0;
	int progress = 0;

	if (dev == NULL)
	{
		ZF_LOGE("device pointer NULL");
		return -1;
	}

	if (!dev->initialized)
	{
		ZF_LOGE("device not initialized");
		return -1;
	}

	// CONFIGURATION PROLOG
	// --------------------
	if (caribou_prog_configure_prepare(	dev ) != 0)
	{
		ZF_LOGE("Preparation for bitstream sending to fpga failed");
		return -1;
	}

	// CONFIGURATION
	// -------------
	// Read file & send bitstream to FPGA via SPI with CS LOW
	ZF_LOGI("Sending bitstream of size %d", buffer_size);
	ct = 0;
	io_utils_write_gpio_with_wait(dev->cs_pin, 0, 200);
	while( ct < (int)(buffer_size) )
	{
		unsigned char dummybuf[LATTICE_ICE40_BUFSIZE];
		char* readbuf = (char*)(buffer + ct);
		int length = (buffer_size-ct)<LATTICE_ICE40_BUFSIZE ? buffer_size-ct : LATTICE_ICE40_BUFSIZE;

		// Send bitstream
		io_utils_spi_transmit(dev->io_spi, dev->io_spi_handle,
								(unsigned char *)readbuf,
								dummybuf,
								length,
								io_utils_spi_write);
		ct += length;

		// progress
		progress = (ct * 100) / buffer_size;
		printf("[%2d%%]\r", progress); fflush(stdout);
	}
	io_utils_write_gpio_with_wait(dev->cs_pin, 1, 200);
	ZF_LOGI("bitstream sent %d bytes", ct);

	// CONFIGURATION EPILOGUE
	// ----------------------
	if (caribou_prog_configure_finish(dev) != 0)
	{
		ZF_LOGE("Finishing the bitstream sending to fpga failed");
		return -1;
	}

	ZF_LOGI("FPGA programming - Success!\n");

	return 0;
}

//---------------------------------------------------------------------------
/**
 * @brief starts programming sequence from a binary file
 * 
 * @param dev device context
 * @param dest the destination of the bitstream
 * @param bitfilename path to the file containing the fpga bitstream
 * @return int success(0), error (-1)
 */
int caribou_prog_configure(caribou_prog_st *dev, char *bitfilename)
{
	FILE *fd = NULL;
	int ct = 0;
	int read;
	unsigned char dummybuf[LATTICE_ICE40_BUFSIZE];
	char readbuf[LATTICE_ICE40_BUFSIZE];
	int progress = 0;
	long file_length = 0;

	if (dev == NULL)
	{
		ZF_LOGE("device pointer NULL");
		return -1;
	}

	if (!dev->initialized)
	{
		ZF_LOGE("device not initialized");
		return -1;
	}

	// FILE OPENING
	// ------------
	if(!(fd = fopen(bitfilename, "r")))
	{
		ZF_LOGE("open file %s failed", bitfilename);
		return -1;
	}
	else
	{
		fseek(fd, 0L, SEEK_END);
		file_length = ftell(fd);
		ZF_LOGI("opened bitstream file %s", bitfilename);
		fseek(fd, 0L, SEEK_SET);
	}

	// CONFIGURATION PROLOG
	// --------------------
	if (caribou_prog_configure_prepare(	dev ) != 0)
	{
		ZF_LOGE("Preparation for bitstream sending to fpga failed");
		return -1;
	}

	// CONFIGURATION
	// -------------
	// Read file & send bitstream to FPGA via SPI with CS LOW
	ZF_LOGI("Sending bitstream of size %ld", file_length);
	ct = 0;
	io_utils_write_gpio_with_wait(dev->cs_pin, 0, 200);
	while( (read=fread(readbuf, sizeof(char), LATTICE_ICE40_BUFSIZE, fd)) > 0 )
	{
		// Send bitstream
		io_utils_spi_transmit(dev->io_spi, dev->io_spi_handle,
								(unsigned char *)readbuf,
								dummybuf,
								read,
								io_utils_spi_write);
		ct += read;

		// progress
		progress = (ct * 100) / file_length;
		printf("[%2d%%]\r", progress); fflush(stdout);
	}
	io_utils_write_gpio_with_wait(dev->cs_pin, 1, 200);
	
	// close file
	ZF_LOGI("bitstream sent, closing file - sent %d bytes", ct);
	fclose(fd);

	// CONFIGURATION EPILOGUE
	// ----------------------
	if (caribou_prog_configure_finish(dev) != 0)
	{
		ZF_LOGE("Finishing the bitstream sending to fpga failed");
		return -1;
	}

	ZF_LOGI("FPGA programming - Success!\n");

	return 0;
}

//---------------------------------------------------------------------------
int caribou_prog_hard_reset(caribou_prog_st *dev, int level)
{
	if (level == 0 || level == -1)
	{
		ZF_LOGD("Resetting FPGA reset pin to 0");
		io_utils_write_gpio_with_wait(dev->reset_pin, 0, 200);
		io_utils_usleep(1000);
	}
	if (level == 1 || level == -1)
	{
		ZF_LOGD("Setting FPGA reset pin to 1");
		io_utils_write_gpio_with_wait(dev->reset_pin, 1, 200);
		io_utils_usleep(1000);
	}
	return 0;
}