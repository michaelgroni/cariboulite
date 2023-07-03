#ifndef __CARIBOULITE_H__
#define __CARIBOULITE_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "cariboulite_config_default.h"

#include <signal.h>
#include <linux/limits.h>						// for file system path max length

#include "hat/hat.h"
#include "ustimer/ustimer.h"
#include "io_utils/io_utils.h"
#include "io_utils/io_utils_spi.h"
#include "io_utils/io_utils_sys_info.h"
#include "rffc507x/rffc507x.h"
#include "at86rf215/at86rf215.h"

#include "caribou_programming/caribou_prog.h"
#include "caribou_fpga/caribou_fpga.h"
#include "caribou_smi/caribou_smi.h"

#include "cariboulite_radio.h"

// GENERAL SETTINGS
struct sys_st_t;

typedef void (*signal_handler)( struct sys_st_t *sys,       // the current cariboulite low-level management struct
								void* context,                      // custom context - can be a higher level app class
								int signal_number,                  // the signal number
								siginfo_t *si);

typedef enum
{
    signal_handler_op_last = 0,             // The curtom sighandler operates (if present) after the default sig handler
    signal_handler_op_first = 1,            // The curtom sighandler operates (if present) before the default sig handler
    signal_handler_op_override = 2,         // The curtom sighandler operates (if present) instead of the default sig handler
} signal_handler_operation_en;

typedef enum
{
    system_type_unknown = 0,
    system_type_cariboulite_full = 1,
    system_type_cariboulite_ism = 2,
} system_type_en;

typedef enum
{
    cariboulite_ext_ref_src_modem = 0,
    cariboulite_ext_ref_src_connector = 1,
    cariboulite_ext_ref_src_txco = 2,
    cariboulite_ext_ref_src_na = 3,             // not applicable
} cariboulite_ext_ref_src_en;

typedef enum
{
	sys_status_unintialized = 0,
	sys_status_minimal_init = 1,
	sys_status_full_init = 2,
} sys_status_en;

typedef struct
{
    cariboulite_ext_ref_src_en src;
    double freq_hz;
} cariboulite_ext_ref_settings_st;

typedef struct sys_st_t
{
	// board information
    hat_board_info_st board_info;
	system_type_en sys_type;

    // SoC level
    io_utils_spi_st spi_dev;
    caribou_smi_st smi;
    ustimer_t timer;

    // Peripheral chips
    caribou_fpga_st fpga;
    at86rf215_st modem;
    cariboulite_ext_ref_settings_st ext_ref_settings;
	rffc507x_st mixer;
	
    // Configuration
    int reset_fpga_on_startup;
	int force_fpga_reprogramming;
	int fpga_config_resistor_state;
    char firmware_path_operational[PATH_MAX];
    char firmware_path_testing[PATH_MAX];
	
	// Radios
	cariboulite_radio_state_st radio_low;
	cariboulite_radio_state_st radio_high;

    // Signals
    signal_handler signal_cb;
    void* singal_cb_context;
    signal_handler_operation_en sig_op;

    // Management
    caribou_fpga_versions_st fpga_versions;
    uint8_t fpga_error_status;
	int fpga_config_res_state;
	// Initialization
	sys_status_en system_status;
} sys_st;

#ifdef __cplusplus
}
#endif


#endif // __CARIBOULITE_H__
