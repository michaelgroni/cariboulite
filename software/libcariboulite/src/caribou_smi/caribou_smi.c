#define ZF_LOG_LEVEL ZF_LOG_VERBOSE
#define ZF_LOG_DEF_SRCLOC ZF_LOG_SRCLOC_LONG
#define ZF_LOG_TAG "DMA_Utils"
#include "zf_log/zf_log.h"

#include <stdio.h>
#include <fcntl.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <math.h>
#include "caribou_smi.h"
#include "dma_utils.h"
#include "mbox_utils.h"
#include "register_utils.h"


//=====================================================================
// Initialise SMI, given data width, time step, and setup/hold/strobe counts
static void caribou_smi_init_registers( caribou_smi_st* dev, 
                                        caribou_smi_transaction_size_bits_en width, 
                                        caribou_smi_timing_st* timing)
{
    dev->smi_cs  = (SMI_CS_REG *) REG32(dev->smi_regs, SMI_CS);
    dev->smi_l   = (SMI_L_REG *)  REG32(dev->smi_regs, SMI_L);
    dev->smi_a   = (SMI_A_REG *)  REG32(dev->smi_regs, SMI_A);
    dev->smi_d   = (SMI_D_REG *)  REG32(dev->smi_regs, SMI_D);
    dev->smi_dmc = (SMI_DMC_REG *)REG32(dev->smi_regs, SMI_DMC);
    dev->smi_dsr0 = (SMI_DSR_REG *)REG32(dev->smi_regs, SMI_DSR0);
    dev->smi_dsw0 = (SMI_DSW_REG *)REG32(dev->smi_regs, SMI_DSW0);
    dev->smi_dsr1 = (SMI_DSR_REG *)REG32(dev->smi_regs, SMI_DSR1);
    dev->smi_dsw1 = (SMI_DSW_REG *)REG32(dev->smi_regs, SMI_DSW1);
    dev->smi_dsr2 = (SMI_DSR_REG *)REG32(dev->smi_regs, SMI_DSR2);
    dev->smi_dsw2 = (SMI_DSW_REG *)REG32(dev->smi_regs, SMI_DSW2);
    dev->smi_dsr3 = (SMI_DSR_REG *)REG32(dev->smi_regs, SMI_DSR3);
    dev->smi_dsw3 = (SMI_DSW_REG *)REG32(dev->smi_regs, SMI_DSW3);
    dev->smi_dcs = (SMI_DCS_REG *)REG32(dev->smi_regs, SMI_DCS);
    dev->smi_dca = (SMI_DCA_REG *)REG32(dev->smi_regs, SMI_DCA);
    dev->smi_dcd = (SMI_DCD_REG *)REG32(dev->smi_regs, SMI_DCD);

    dev->smi_cs->value = 0;
    dev->smi_l->value = 0;
    dev->smi_a->value = 0;
    dev->smi_dsr0->value = 0;
    dev->smi_dsw0->value = 0;
    dev->smi_dsr1->value = 0;
    dev->smi_dsw1->value = 0;
    dev->smi_dsr2->value = 0;
    dev->smi_dsw2->value = 0;
    dev->smi_dsr3->value = 0;
    dev->smi_dsw3->value = 0;
    dev->smi_dcs->value = 0;
    dev->smi_dca->value = 0;

    // if error exist in the SMI Control & Status (CS), latch it to clear
    if (dev->smi_cs->seterr)
    {
        dev->smi_cs->seterr = 1;
    }

    dev->smi_dsr0->rsetup = timing->setup_steps;
    dev->smi_dsw0->wsetup = timing->setup_steps;
    dev->smi_dsr0->rstrobe = timing->strobe_steps;
    dev->smi_dsw0->wstrobe = timing->strobe_steps;
    dev->smi_dsr0->rhold = timing->hold_steps;
    dev->smi_dsw0->whold = timing->hold_steps;
    dev->smi_dmc->panicr = 8;
    dev->smi_dmc->panicw = 8;
    dev->smi_dmc->reqr = 4;
    dev->smi_dmc->reqw = 4;
    dev->smi_dsr0->rwidth = width;
    dev->smi_dsw0->wwidth = width;
}

//=====================================================================
static float caribou_smi_calculate_clocking (caribou_smi_st* dev, float freq_sps, caribou_smi_timing_st* timing)
{
    float clock_rate = dev->sys_info.sys_clock_hz;
    float cc_in_ns = 1e9 / clock_rate;
    float sample_time_ns = 1e9 / freq_sps;
    
    int cc_budget = (int)(sample_time_ns / cc_in_ns);
    if (cc_budget < 6) 
    {
        printf("The clock cycle budget for the calculation is too small.\n");
        return -1;
    }
    // try to make a 1:2:1 ratio between setup, strobe and hold (but step needs to be even)
    timing->step_size = (int)(roundf((float)cc_budget / 8.0f))*2;
    timing->setup_steps = 1;
    timing->strobe_steps = 2;
    timing->hold_steps = 1;

    return 1e9 / (timing->step_size*cc_in_ns*(timing->setup_steps + timing->strobe_steps + timing->hold_steps));
}


//=====================================================================
int caribou_smi_init(caribou_smi_st* dev)
{
    // Get the RPI information
    //----------------------------------------
    io_utils_get_rpi_info(&dev->sys_info);

    printf("processor_type = %d, phys_reg_base = %08X, sys_clock_hz = %d Hz, bus_reg_base = %08X, RAM = %d Mbytes\n",
                    dev->sys_info.processor_type, dev->sys_info.phys_reg_base, dev->sys_info.sys_clock_hz, 
                    dev->sys_info.bus_reg_base, dev->sys_info.ram_size_mbytes);
    io_utils_print_rpi_info(&dev->sys_info);

    // Map devices and Allocate memory
    //----------------------------------------
    dma_utils_init (&dev->dma);
    map_periph(&dev->smi_regs, SMI_BASE, PAGE_SIZE);

    // the allocation is page_size rounded-up
    uint32_t single_buffer_size_round = PAGE_ROUNDUP(dev->sample_buf_length * SAMPLE_SIZE_BYTES);
    dev->videocore_alloc_size = PAGE_SIZE + dev->num_sample_bufs * single_buffer_size_round;
    dev->actual_sample_buf_length_sec = (float)(single_buffer_size_round) / 16e6;

    map_uncached_mem(&dev->vc_mem, dev->videocore_alloc_size);

    printf("INFO: caribou_smi_init - The actual single buffer contains %.2f usec of data\n", 
            dev->actual_sample_buf_length_sec * 1e6);

    // Initialize the GPIO modes and states
    //----------------------------------------
    for (int i=0; i<dev->num_data_pins; i++)
    {
        io_utils_set_gpio_mode(dev->data_0_pin + i, io_utils_alt_gpio_in);
    }
    io_utils_set_gpio_mode(dev->soe_pin, io_utils_alt_1);
    io_utils_set_gpio_mode(dev->swe_pin, io_utils_alt_1);

    io_utils_set_gpio_mode(dev->read_req_pin, io_utils_alt_1);
    io_utils_set_gpio_mode(dev->write_req_pin, io_utils_alt_1);

    for (int i=0; i<dev->num_addr_pins; i++)
    {
        io_utils_set_gpio_mode(dev->addr0_pin + i, io_utils_alt_1);
    }

    // Setup a reading rate of 32 MSPS (16 MSPS x2)
    //---------------------------------------------
    caribou_smi_timing_st timing = {0};
    float actual_sps = caribou_smi_calculate_clocking (dev, 32e6, &timing);
    caribou_smi_print_timing (actual_sps, &timing);
    caribou_smi_init_registers(dev, caribou_smi_transaction_size_8bits, &timing);



    dev->smi_dmc->dmaen = 1;
    dev->smi_cs->enable = 1;
    dev->smi_cs->clear = 1;

    void* rxbuff = adc_dma_start(&dev->vc_mem, NSAMPLES);
    smi_start(dev, NSAMPLES, 1);

    while (dma_active(dev->dma_channel)) ;
    
    adc_dma_end(dev, rxbuff, sample_data, NSAMPLES);

    disp_reg_fields(smi_cs_regstrs, "CS", *REG32(dev->smi_regs, SMI_CS));
    dev->smi_cs->enable = dev->smi_dcs->enable = 0;
    for (int i=0; i<NSAMPLES; i++)
    {
        printf("%1.3f\n", val_volts(sample_data[i]));
    }*/

    dev->initialized = 1;
    return 0;
}

//=====================================================================
int caribou_smi_close(caribou_smi_st* dev)
{
    if (!dev->initialized)
    {
        return 0;
    }

    dev->initialized = 0;

    // GPIO Setting back to default
    for (int i=0; i<dev->num_data_pins; i++)
    {
        io_utils_set_gpio_mode(dev->data_0_pin + i, io_utils_alt_gpio_in);
    }
    io_utils_set_gpio_mode(dev->soe_pin, io_utils_alt_gpio_in);
    io_utils_set_gpio_mode(dev->swe_pin, io_utils_alt_gpio_in);
    io_utils_set_gpio_mode(dev->read_req_pin, io_utils_alt_gpio_in);
    io_utils_set_gpio_mode(dev->write_req_pin, io_utils_alt_gpio_in);
    for (int i=0; i<dev->num_addr_pins; i++)
    {
        io_utils_set_gpio_mode(dev->addr0_pin + i, io_utils_alt_gpio_in);
    }

    
    if (dev->smi_regs.virt)
    {
        *REG32(dev->smi_regs, SMI_CS) = 0;
    }
    dma_utils_stop_dma(&dev->dma, dev->dma_channel);

    unmap_periph_mem(&dev->vc_mem);
    unmap_periph_mem(&dev->smi_regs);
    dma_utils_close (&dev->dma);

    return 0;
}

//===========================================================================
// Start SMI, given number of samples, optionally pack bytes into words
void caribou_smi_start(caribou_smi_st* dev, int nsamples, int pre_samp, int packed)
{
    dev->smi_l->len = nsamples + pre_samp;
    dev->smi_cs->pxldat = (packed != 0);
    dev->smi_cs->enable = 1;
    dev->smi_cs->clear = 1;
    dev->smi_cs->start = 1;
}

//===========================================================================
// Start DMA for SMI ADC, return Rx data buffer
uint32_t *adc_dma_start(MEM_MAP *mp, int nsamp)
{
    DMA_CB *cbs=mp->virt;
    uint32_t *data=(uint32_t *)(cbs+4), *pindata=data+8, *modes=data+0x10;
    uint32_t *modep1=data+0x18, *modep2=modep1+1, *rxdata=data+0x20, i;

    // Get current mode register values
    for (i=0; i<3; i++)
    {
        modes[i] = modes[i+3] = *REG32(gpio_regs, GPIO_MODE0 + i*4);
    }

    // Get mode values with ADC pins set to SMI
    for (i=ADC_D0_PIN; i<ADC_D0_PIN+ADC_NPINS; i++)
    {
        mode_word(&modes[i/10], i%10, GPIO_ALT1);
    }

    // Copy mode values into 32-bit words
    *modep1 = modes[1];
    *modep2 = modes[2];
    *pindata = 1 << TEST_PIN;
    enable_dma(dev->dma_channel);
    // Control blocks 0 and 1: enable SMI I/P pins
    cbs[0].ti = DMA_SRCE_DREQ | (DMA_SMI_DREQ << 16) | DMA_WAIT_RESP;
    cbs[0].tfr_len = 4;
    cbs[0].srce_ad = MEM_BUS_ADDR(mp, modep1);
    cbs[0].dest_ad = REG_BUS_ADDR(gpio_regs, GPIO_MODE0+4);
    cbs[0].next_cb = MEM_BUS_ADDR(mp, &cbs[1]);

    cbs[1].tfr_len = 4;
    cbs[1].srce_ad = MEM_BUS_ADDR(mp, modep2);
    cbs[1].dest_ad = REG_BUS_ADDR(gpio_regs, GPIO_MODE0+8);
    cbs[1].next_cb = MEM_BUS_ADDR(mp, &cbs[2]);

    // Control block 2: read data
    cbs[2].ti = DMA_SRCE_DREQ | (DMA_SMI_DREQ << 16) | DMA_CB_DEST_INC;
    cbs[2].tfr_len = (nsamp + PRE_SAMP) * SAMPLE_SIZE;
    cbs[2].srce_ad = REG_BUS_ADDR(smi_regs, SMI_D);
    cbs[2].dest_ad = MEM_BUS_ADDR(mp, rxdata);
    cbs[2].next_cb = MEM_BUS_ADDR(mp, &cbs[3]);

    // Control block 3: disable SMI I/P pins
    cbs[3].ti = DMA_CB_SRCE_INC | DMA_CB_DEST_INC;
    cbs[3].tfr_len = 3 * 4;
    cbs[3].srce_ad = MEM_BUS_ADDR(mp, &modes[3]);
    cbs[3].dest_ad = REG_BUS_ADDR(gpio_regs, GPIO_MODE0);

    start_dma(mp, dev->dma_channel, &cbs[0], 0);
    return(rxdata);
}

//===========================================================================
// ADC DMA is complete, get data
int adc_dma_end(void *buff, uint16_t *data, int nsamp)
{
    uint16_t *bp = (uint16_t *)buff;
    int i;

    for (i=0; i<nsamp+PRE_SAMP; i++)
    {
        if (i >= PRE_SAMP)
            *data++ = bp[i] >> 4;
    }
    return(nsamp);
}

//===========================================================================
// Display bit values in register
void caribou_smi_disp_reg_fields(char *regstrs, char *name, uint32_t val)
{
    char *p=regstrs, *q, *r=regstrs;
    uint32_t nbits, v;

    printf("%s %08X", name, val);
    while ((q = strchr(p, ':')) != 0)
    {
        p = q + 1;
        nbits = 0;
        while (*p>='0' && *p<='9')
        {
            nbits = nbits * 10 + *p++ - '0';
        }
        v = val & ((1 << nbits) - 1);
        val >>= nbits;
        if (v && *r!='_')
        {
            printf(" %.*s=%X", q-r, r, v);
        }
        while (*p==',' || *p==' ')
        {
            p = r = p + 1;
        }
    }
    printf("\n");
}


//===========================================================================
// Display SMI registers
// SMI register names for diagnostic print
static char *smi_regstrs[] = {
    "CS","LEN","A","D","DSR0","DSW0","DSR1","DSW1",
    "DSR2","DSW2","DSR3","DSW3","DMC","DCS","DCA","DCD",""
};

void caribou_smi_disp_smi(caribou_smi_st* dev)
{
    volatile uint32_t *p=REG32(dev->smi_regs, SMI_CS);
    int i=0;

    while (smi_regstrs[i][0])
    {
        printf("%4s=%08X ", smi_regstrs[i++], *p++);
        if (i%8==0 || smi_regstrs[i][0]==0)
            printf("\n");
    }
}

//=====================================================================
void caribou_smi_print_timing (float actual_freq, caribou_smi_timing_st* timing)
{
    printf("Timing: step: %d, setup: %d, strobe: %d, hold: %d, actual_freq: %.2f MHz\n", 
                timing->step_size, timing->setup_steps, timing->strobe_steps, timing->hold_steps, actual_freq / 1e6);
}