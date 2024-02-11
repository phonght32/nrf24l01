// MIT License

// Copyright (c) 2024 phonght32

// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:

// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#ifndef __NRF24L01_H__
#define __NRF24L01_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "err_code.h"

typedef struct nrf24l01* nrf24l01_handle_t;

/**
 * @brief   Data rate.
 */
typedef enum {
	NRF24L01_DATA_RATE_250Kbps = 0,				/*!< Data rate 250 Kbps */
	NRF24L01_DATA_RATE_1Mbps,					/*!< Data rate 1 Mbps */
	NRF24L01_DATA_RATE_2Mbps,					/*!< Data rate 2 Mbps */
} nrf24l01_data_rate_t;

/**
 * @brief   Output power.
 */
typedef enum {
	NRF24L01_OUTPUT_PWR_0dBm = 0,				/*!< Output power 0 dBm */
	NRF24L01_OUTPUT_PWR_6dBm,					/*!< Output power 6 dBm */
	NRF24L01_OUTPUT_PWR_12dBm,					/*!< Output power 12 dBm */
	NRF24L01_OUTPUT_PWR_18dBm					/*!< Output power 18 dBm */
} nrf24l01_output_pwr_t;

/**
 * @brief   Mode.
 */
typedef enum {
	NRF24L01_MODE_TRANSMITTER = 0,				/*!< Mode transmitter */
	NRF24L01_MODE_RECEIVER						/*!< Mode receiver */
} nrf24l01_mode_t;

/**
 * @brief   Configuration structure.
 */
typedef struct {
	nrf24l01_data_rate_t 		data_rate;		/*!< Data rate */
	nrf24l01_output_pwr_t 		output_pwr;		/*!< Output power */
	nrf24l01_mode_t 			mode;			/*!< Mode operation */
} nrf24l01_cfg_t;

#ifdef __cplusplus
}
#endif

#endif /* __ENCODER_H__ */