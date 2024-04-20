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

#define NRF24L01_IRQ_ACTIVE_LEVEL 		0
#define NRF24L01_IRQ_UNACTIVE_LEVEL 	1

typedef err_code_t (*nrf24l01_func_spi_send)(uint8_t *buf_send, uint16_t len);
typedef err_code_t (*nrf24l01_func_spi_recv)(uint8_t *buf_recv, uint16_t len);
typedef err_code_t (*nrf24l01_func_set_gpio)(uint8_t level);
typedef err_code_t (*nrf24l01_func_get_gpio)(uint8_t *level);
typedef void (*nrf24l01_func_delay)(uint32_t time_ms);

/**
 * @brief   NRF24L01 handle structure.
 */
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
	NRF24L01_TRANSCEIVER_MODE_TX = 0,			/*!< Mode transmitter */
	NRF24L01_TRANSCEIVER_MODE_RX				/*!< Mode receiver */
} nrf24l01_transceiver_mode_t;

/**
 * @brief   Configuration structure.
 */
typedef struct {
	uint16_t  					channel; 			/*!< Channel */
	uint8_t 					packet_len;			/*!< Packet length */
	uint8_t  					crc_len; 			/*!< CRC length */
	uint8_t  					addr_width; 		/*!< Address width */
	uint8_t  					retrans_cnt; 		/*!< Re-transmit count */
	uint8_t  					retrans_delay; 		/*!< Re-transmit delay */
	nrf24l01_data_rate_t 		data_rate;			/*!< Data rate */
	nrf24l01_output_pwr_t 		output_pwr;			/*!< Output power */
	nrf24l01_transceiver_mode_t transceiver_mode;	/*!< Mode operation */
	nrf24l01_func_spi_send 		spi_send;			/*!< Function SPI send */
	nrf24l01_func_spi_recv 		spi_recv;			/*!< Function SPI receive */
	nrf24l01_func_set_gpio 		set_cs;				/*!< Function set chip select pin */
	nrf24l01_func_set_gpio 		set_ce;				/*!< Function set chip enable pin */
	nrf24l01_func_get_gpio 		get_irq;			/*!< Function get irq pin */
	nrf24l01_func_delay			delay; 				/*!< Function delay */
} nrf24l01_cfg_t;

/*
 * @brief   Initialize nRF24L01 with default parameters.
 *
 * @note    This function must be called first.
 *
 * @param   None.
 *
 * @return
 *      - Handle structure: Success.
 *      - Others:           Fail.
 */
nrf24l01_handle_t nrf24l01_init(void);

/*
 * @brief   Set configuration parameters.
 *
 * @param 	handle Handle structure.
 * @param   config Configuration structure.
 *
 * @return
 *      - ERR_CODE_SUCCESS: Success.
 *      - Others:           Fail.
 */
err_code_t nrf24l01_set_config(nrf24l01_handle_t handle, nrf24l01_cfg_t config);

/*
 * @brief   Configure nRF24L01 to run.
 *
 * @param 	handle Handle structure.
 *
 * @return
 *      - ERR_CODE_SUCCESS: Success.
 *      - Others:           Fail.
 */
err_code_t nrf24l01_config(nrf24l01_handle_t handle);

/*
 * @brief   Transmit data. After that, monitor IRQ pin is necessary to call
 * 			"nrf24l01_clear_transmit_irq_flags" which clear transmitted interrupt flags.
 *
 * @param 	handle Handle structure.
 * @param 	tx_payload Transmit buffer.
 *
 * @return
 *      - ERR_CODE_SUCCESS: Success.
 *      - Others:           Fail.
 */
err_code_t nrf24l01_transmit(nrf24l01_handle_t handle, uint8_t* tx_payload);

/*
 * @brief   Transmit data and polling until IRQ is triggered or timeout. Function
 * 			"nrf24l01_clear_transmit_irq_flags" is called automatically to clear
 * 			transmitted interrupt flags if transmit success.
 *
 * @note 	Functions "get_irq" and "delay" need to be assigned.
 *
 * @param 	handle Handle structure.
 * @param 	tx_payload Transmit buffer.
 * @param 	timeout_ms Timeout in ms.
 *
 * @return
 *      - ERR_CODE_SUCCESS: Success.
 *      - Others:           Fail.
 */
err_code_t nrf24l01_transmit_polling(nrf24l01_handle_t handle, uint8_t* tx_payload, uint32_t timeout_ms);

/*
 * @brief   Receive data and call "nrf24l01_clear_receive_irq_flags" automatically
 * 			to clear received interrupt flags when receive success.
 * 			Monitor IRQ pin is neccessary to ensure data are received before
 * 			call this function.
 *
 * @param 	handle Handle structure.
 * @param 	rx_payload Received buffer.
 *
 * @return
 *      - ERR_CODE_SUCCESS: Success.
 *      - Others:           Fail.
 */
err_code_t nrf24l01_receive(nrf24l01_handle_t handle, uint8_t* rx_payload);

/*
 * @brief   Polling until IRQ pin is triggered or timeout. When data is received,
 * 			function "nrf24l01_clear_receive_irq_flags" is called automatically to
 * 			clear received interrupt flags.
 *
 * @note    Functions "get_irq" and "delay" need to be assigned.
 *
 * @param 	handle Handle structure.
 * @param 	rx_payload Received buffer.
 * @param 	timeout_ms Timeout in ms.
 *
 * @return
 *      - ERR_CODE_SUCCESS: Success.
 *      - Others:           Fail.
 */
err_code_t nrf24l01_receive_polling(nrf24l01_handle_t handle, uint8_t* rx_payload, uint32_t timeout_ms);

/*
 * @brief   Clear transmitted interrupt flags.
 *
 * @note 	After data sent by "nrf24l01_transmit", this function should be called
 * 			when IRQ pin triggered which notify transmit complete.
 *
 * @param 	handle Handle structure.
 *
 * @return
 *      - ERR_CODE_SUCCESS: Success.
 *      - Others:           Fail.
 */
err_code_t nrf24l01_clear_transmit_irq_flags(nrf24l01_handle_t handle);

/*
 * @brief   Clear received interrupt flags.
 *
 * @note 	This function should be called when IRQ pin triggered which notify
 * 			data ready. Then, "nrf24l01_receive" can be called to read received data.
 *
 * @param 	handle Handle structure.
 *
 * @return
 *      - ERR_CODE_SUCCESS: Success.
 *      - Others:           Fail.
 */
err_code_t nrf24l01_clear_receive_irq_flags(nrf24l01_handle_t handle);

/*
 * @brief   Flush receive buffer.
 *
 * @param 	handle Handle structure.
 *
 * @return
 *      - ERR_CODE_SUCCESS: Success.
 *      - Others:           Fail.
 */
err_code_t nrf24l01_flush_rx_fifo(nrf24l01_handle_t handle);

/*
 * @brief   Flush transmit buffer.
 *
 * @param 	handle Handle structure.
 *
 * @return
 *      - ERR_CODE_SUCCESS: Success.
 *      - Others:           Fail.
 */
err_code_t nrf24l01_flush_tx_fifo(nrf24l01_handle_t handle);

/*
 * @brief   Set nRF24L01 in power up mode.
 *
 * @note 	This function set bit PWR_UP in the CONFIG register.
 *
 * @param 	handle Handle structure.
 *
 * @return
 *      - ERR_CODE_SUCCESS: Success.
 *      - Others:           Fail.
 */
err_code_t nrf24l01_power_up(nrf24l01_handle_t handle);

/*
 * @brief   Set nRF24L01 in power up down.
 *
 * @note 	In power down mode nRF24L01+ is disabled using minimal current
 * 			consumption. All registers values available are maintained and the
 * 			SPI is kept active, enabling change of configuration and the
 * 			uploading/downloading of data registers.
 * 			This function set bit PWR_UP in the CONFIG register.
 *
 * @param 	handle Handle structure.
 *
 * @return
 *      - ERR_CODE_SUCCESS: Success.
 *      - Others:           Fail.
 */
err_code_t nrf24l01_power_down(nrf24l01_handle_t handle);

/*
 * @brief   Get data on STATUS register.
 *
 * @note 	In parallel to the SPI command word applied on the MOSI pin, the STATUS
 * 			register is shifted serially out on the MISO pin.
 *
 * @param 	handle Handle structure.
 * @param 	status Status.
 *
 * @return
 *      - ERR_CODE_SUCCESS: Success.
 *      - Others:           Fail.
 */
err_code_t nrf24l01_get_status(nrf24l01_handle_t handle, uint8_t *status);

/*
 * @brief   Get data on FIFO_STATUS register.
 *
 * @note 	For more detail of data content, please see in nRF24L01 Specification.
 * 			Bit 7: Reserved.
 * 			Bit 6: TX_REUSE. Used for a PTX device. Pulse the rfce high for at
 * 			least 10us to reuse last transmitted payload. TX payload reuse is active
 * 			until W_TX_PAYLOAD or FLUSH_TX is executed.
 * 			Bit 5: TX_FULL. TX FIFO full flag. 1: TX FIFO full, 0: available
 * 			locations in TX FIFO.
 * 			Bit 4: TX_EMPTY. TX FIFO empty flag. 1: TX FIFO empty. 0: data in TX FIFO.
 *			Bit 3: Reserved.
 * 			Bit 2: Reserved.
 * 			Bit 1: RX_FULL: RX FIFO full flag. 1: RX FIFO full, 0: available
 * 			locations in RX FIFO.
 * 			Bit 0: RX_EMPTY. RX FIFO empty flag. 1: RX FIFO empty. 0: data in RX FIFO.
 *
 * @param 	handle Handle structure.
 * @param 	status Status.
 *
 * @return
 *      - ERR_CODE_SUCCESS: Success.
 *      - Others:           Fail.
 */
err_code_t nrf24l01_get_fifo_status(nrf24l01_handle_t handle, uint8_t *status);

/*
 * @brief   Clear interrupt triggered by data sent completely on TX FIFO.
 *
 * @note 	Asserted when packet transmitted on TX. If AUTO_ACK is activated,
 * 			this bit is set high only when ACK is received.
 * 			This function write 1 to TX_DS bit to clear.
 *
 * @param 	handle Handle structure.
 *
 * @return
 *      - ERR_CODE_SUCCESS: Success.
 *      - Others:           Fail.
 */
err_code_t nrf24l01_clear_tx_ds(nrf24l01_handle_t handle);

/*
 * @brief   Clear interrupt triggered when maximum number of TX retransmits is reached.
 *
 * @note 	If MAX_RT is asserted it must be cleared to enable further communication.
 * 			This function write 1 to MAX_RT bit to clear.
 *
 * @param 	handle Handle structure.
 *
 * @return
 *      - ERR_CODE_SUCCESS: Success.
 *      - Others:           Fail.
 */
err_code_t nrf24l01_clear_max_rt(nrf24l01_handle_t handle);

/*
 * @brief   Clear interrupt triggered when data ready on RX FIFO.
 *
 * @note 	Asserted when new data arrives on RX FIFO.
 * 			This function write 1 to RX_DR bit to clear.
 *
 * @param 	handle Handle structure.
 *
 * @return
 *      - ERR_CODE_SUCCESS: Success.
 *      - Others:           Fail.
 */
err_code_t nrf24l01_clear_rx_dr(nrf24l01_handle_t handle);

#ifdef __cplusplus
}
#endif

#endif /* __NRF24L01_H__ */
