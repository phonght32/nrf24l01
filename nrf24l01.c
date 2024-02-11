#include "nrf24l01.h"

typedef struct nrf24l01 {
	nrf24l01_data_rate_t 		data_rate;		/*!< Data rate */
	nrf24l01_output_pwr_t 		output_pwr;		/*!< Output power */
	nrf24l01_mode_t 			mode;			/*!< Mode operation */
} nrf24l01_t;