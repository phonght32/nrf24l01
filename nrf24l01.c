#include "stdlib.h"
#include "nrf24l01.h"

/**
 * @brief   Command group.
 */
#define NRF24L01P_CMD_R_REGISTER                  0b00000000
#define NRF24L01P_CMD_W_REGISTER                  0b00100000
#define NRF24L01P_CMD_R_RX_PAYLOAD                0b01100001
#define NRF24L01P_CMD_W_TX_PAYLOAD                0b10100000
#define NRF24L01P_CMD_FLUSH_TX                    0b11100001
#define NRF24L01P_CMD_FLUSH_RX                    0b11100010
#define NRF24L01P_CMD_REUSE_TX_PL                 0b11100011
#define NRF24L01P_CMD_R_RX_PL_WID                 0b01100000
#define NRF24L01P_CMD_W_ACK_PAYLOAD               0b10101000
#define NRF24L01P_CMD_W_TX_PAYLOAD_NOACK          0b10110000
#define NRF24L01P_CMD_NOP                         0b11111111

/**
 * @brief   Register group.
 */
#define NRF24L01P_REG_CONFIG            0x00
#define NRF24L01P_REG_EN_AA             0x01
#define NRF24L01P_REG_EN_RXADDR         0x02
#define NRF24L01P_REG_SETUP_AW          0x03
#define NRF24L01P_REG_SETUP_RETR        0x04
#define NRF24L01P_REG_RF_CH             0x05
#define NRF24L01P_REG_RF_SETUP          0x06
#define NRF24L01P_REG_STATUS            0x07
#define NRF24L01P_REG_OBSERVE_TX        0x08
#define NRF24L01P_REG_RPD               0x09
#define NRF24L01P_REG_RX_ADDR_P0        0x0A
#define NRF24L01P_REG_RX_ADDR_P1        0x0B
#define NRF24L01P_REG_RX_ADDR_P2        0x0C
#define NRF24L01P_REG_RX_ADDR_P3        0x0D
#define NRF24L01P_REG_RX_ADDR_P4        0x0E
#define NRF24L01P_REG_RX_ADDR_P5        0x0F
#define NRF24L01P_REG_TX_ADDR           0x10
#define NRF24L01P_REG_RX_PW_P0          0x11
#define NRF24L01P_REG_RX_PW_P1          0x12
#define NRF24L01P_REG_RX_PW_P2          0x13
#define NRF24L01P_REG_RX_PW_P3          0x14
#define NRF24L01P_REG_RX_PW_P4          0x15
#define NRF24L01P_REG_RX_PW_P5          0x16
#define NRF24L01P_REG_FIFO_STATUS       0x17
#define NRF24L01P_REG_DYNPD             0x1C
#define NRF24L01P_REG_FEATURE           0x1D

#define SPI_ACTION_TIMEOUT 				1000

#define NRF24L01_CS_ACTIVE 				0
#define NRF24L01_CS_UNACTIVE 			1


typedef struct nrf24l01 {
	uint16_t  					channel; 		/*!< Channel */
	uint8_t 					payload_len;	/*!< Payload length */
	uint8_t  					crc_len; 		/*!< CRC length */
	uint8_t  					addr_width; 	/*!< Address width */
	uint8_t  					retrans_cnt; 	/*!< Re-transmit count */
	uint8_t  					retrans_delay; 	/*!< Re-transmit delay */
	nrf24l01_data_rate_t 		data_rate;		/*!< Data rate */
	nrf24l01_output_pwr_t 		output_pwr;		/*!< Output power */
	nrf24l01_mode_t 			mode;			/*!< Mode operation */
	nrf24l01_func_spi_send 		spi_send;		/*!< Function SPI send */
	nrf24l01_func_spi_recv 		spi_recv;		/*!< Function SPI receive */
	nrf24l01_func_set_cs 		set_cs;			/*!< Function set chip select pin */
	nrf24l01_func_set_ce 		set_ce;			/*!< Function set chip enable pin */
} nrf24l01_t;

static uint8_t nrf24l01_read_register(nrf24l01_handle_t handle, uint8_t reg)
{
	uint8_t command = NRF24L01P_CMD_R_REGISTER | reg;
	uint8_t read_val;

	handle->set_cs(NRF24L01_CS_ACTIVE);
	handle->spi_send(&command, 1, SPI_ACTION_TIMEOUT);
	handle->spi_recv(&read_val, 1, SPI_ACTION_TIMEOUT);
	handle->set_cs(NRF24L01_CS_UNACTIVE);

	return read_val;
}

static err_code_t nrf24l01_write_register(nrf24l01_handle_t handle, uint8_t reg, uint8_t value)
{
	uint8_t command = NRF24L01P_CMD_W_REGISTER | reg;
	uint8_t write_val = value;

	handle->set_cs(NRF24L01_CS_ACTIVE);
	handle->spi_send(&command, 1, SPI_ACTION_TIMEOUT);
	handle->spi_send(&write_val, 1, SPI_ACTION_TIMEOUT);
	handle->set_cs(NRF24L01_CS_UNACTIVE);

	return ERR_CODE_SUCCESS;
}

static err_code_t nrf24l01_read_rx_fifo(nrf24l01_handle_t handle, uint8_t* rx_payload)
{
	uint8_t command = NRF24L01P_CMD_R_RX_PAYLOAD;

	handle->set_cs(NRF24L01_CS_ACTIVE);
	handle->spi_send(&command, 1, SPI_ACTION_TIMEOUT);
	handle->spi_recv(rx_payload, handle->payload_len, SPI_ACTION_TIMEOUT);
	handle->set_cs(NRF24L01_CS_UNACTIVE);

	return ERR_CODE_SUCCESS;
}

static err_code_t nrf24l01_write_tx_fifo(nrf24l01_handle_t handle, uint8_t* tx_payload)
{
	uint8_t command = NRF24L01P_CMD_W_TX_PAYLOAD;

	handle->set_cs(NRF24L01_CS_ACTIVE);
	handle->spi_send(&command, 1, SPI_ACTION_TIMEOUT);
	handle->spi_send(tx_payload, handle->payload_len, SPI_ACTION_TIMEOUT);
	handle->set_cs(NRF24L01_CS_UNACTIVE);

	return ERR_CODE_SUCCESS;
}

static void nrf24l01_rx_set_payload_widths(nrf24l01_handle_t handle, uint8_t bytes)
{
	nrf24l01_write_register(handle, NRF24L01P_REG_RX_PW_P0, bytes);
}

static void nrf24l01_set_crc_length(nrf24l01_handle_t handle, uint8_t bytes)
{
	uint8_t reg_config = nrf24l01_read_register(handle, NRF24L01P_REG_CONFIG);

	switch (bytes)
	{
	/* CRCO bit in CONFIG resiger set 0 */
	case 1:
		reg_config &= 0xFB;
		break;
	/* CRCO bit in CONFIG resiger set 1 */
	case 2:
		reg_config |= 1 << 2;
		break;
	}

	nrf24l01_write_register(handle, NRF24L01P_REG_CONFIG, reg_config);
}

static void nrf24l01_set_address_widths(nrf24l01_handle_t handle, uint8_t bytes)
{
	nrf24l01_write_register(handle, NRF24L01P_REG_SETUP_AW, bytes - 2);
}

static void nrf24l01_auto_retransmit_count(nrf24l01_handle_t handle, uint8_t cnt)
{
	uint8_t setup_retr = nrf24l01_read_register(handle, NRF24L01P_REG_SETUP_RETR);

	/* Reset ARC register 0 */
	setup_retr |= 0xF0;
	setup_retr |= cnt;

	nrf24l01_write_register(handle, NRF24L01P_REG_SETUP_RETR, setup_retr);
}

static void nrf24l01_auto_retransmit_delay(nrf24l01_handle_t handle, uint16_t us)
{
	uint8_t setup_retr = nrf24l01_read_register(handle, NRF24L01P_REG_SETUP_RETR);

	/* Reset ARD register 0 */
	setup_retr |= 0x0F;
	setup_retr |= ((us / 250) - 1) << 4;

	nrf24l01_write_register(handle, NRF24L01P_REG_SETUP_RETR, setup_retr);
}

static void nrf24l01_set_rf_channel(nrf24l01_handle_t handle, uint16_t MHz)
{
	uint16_t rf_ch = MHz - 2400;
	nrf24l01_write_register(handle, NRF24L01P_REG_RF_CH, rf_ch);
}

static void nrf24l01_set_rf_tx_output_power(nrf24l01_handle_t handle, nrf24l01_output_pwr_t dBm)
{
	uint8_t rf_setup = nrf24l01_read_register(handle, NRF24L01P_REG_RF_SETUP) & 0xF9;
	switch (dBm)
	{
	case NRF24L01_OUTPUT_PWR_0dBm:
		rf_setup |= (3 << 1);
		break;
	case NRF24L01_OUTPUT_PWR_6dBm:
		rf_setup |= (2 << 1);
		break;
	case NRF24L01_OUTPUT_PWR_12dBm:
		rf_setup |= (1 << 1);
		break;
	case NRF24L01_OUTPUT_PWR_18dBm:
		rf_setup |= (0 << 1);
		break;
	default:
		break;
	}

	nrf24l01_write_register(handle, NRF24L01P_REG_RF_SETUP, rf_setup);
}

static void nrf24l01_set_rf_air_data_rate(nrf24l01_handle_t handle, nrf24l01_data_rate_t bps)
{
	uint8_t rf_setup = nrf24l01_read_register(handle, NRF24L01P_REG_RF_SETUP) & 0xD7;

	switch (bps)
	{
	case NRF24L01_DATA_RATE_250Kbps:
		rf_setup |= 1 << 5;
		break;
	case NRF24L01_DATA_RATE_1Mbps:

		break;
	case NRF24L01_DATA_RATE_2Mbps:
		rf_setup |= 1 << 3;
		break;
	}
	nrf24l01_write_register(handle, NRF24L01P_REG_RF_SETUP, rf_setup);
}

nrf24l01_handle_t nrf24l01_init(void)
{
	nrf24l01_handle_t handle = calloc(1, sizeof(nrf24l01_t));
	if (handle == NULL)
	{
		return NULL;
	}

	return handle;
}

err_code_t nrf24l01_set_config(nrf24l01_handle_t handle, nrf24l01_cfg_t config)
{
	/* Check if handle structure is NULL */
	if (handle == NULL)
	{
		return ERR_CODE_NULL_PTR;
	}

	handle->channel = config.channel;
	handle->payload_len = config.payload_len;
	handle->crc_len = config.crc_len;
	handle->addr_width = config.addr_width;
	handle->retrans_cnt = config.retrans_cnt;
	handle->retrans_delay = config.retrans_delay;
	handle->data_rate = config.data_rate;
	handle->output_pwr = config.output_pwr;
	handle->mode = config.mode;
	handle->spi_send = config.spi_send;
	handle->spi_recv = config.spi_recv;
	handle->set_cs = config.set_cs;
	handle->set_ce = config.set_ce;

	return ERR_CODE_SUCCESS;
}

err_code_t nrf24l01_config(nrf24l01_handle_t handle)
{
	/* Check if handle structure is NULL */
	if (handle == NULL)
	{
		return ERR_CODE_NULL_PTR;
	}

	handle->set_cs(NRF24L01_CS_UNACTIVE);
	handle->set_ce(0);

	/* Reset register */
	nrf24l01_write_register(handle, NRF24L01P_REG_CONFIG, 0x08);
	nrf24l01_write_register(handle, NRF24L01P_REG_EN_AA, 0x3F);
	nrf24l01_write_register(handle, NRF24L01P_REG_EN_RXADDR, 0x03);
	nrf24l01_write_register(handle, NRF24L01P_REG_SETUP_AW, 0x03);
	nrf24l01_write_register(handle, NRF24L01P_REG_SETUP_RETR, 0x03);
	nrf24l01_write_register(handle, NRF24L01P_REG_RF_CH, 0x02);
	nrf24l01_write_register(handle, NRF24L01P_REG_RF_SETUP, 0x07);
	nrf24l01_write_register(handle, NRF24L01P_REG_STATUS, 0x7E);
	nrf24l01_write_register(handle, NRF24L01P_REG_RX_PW_P0, 0x00);
	nrf24l01_write_register(handle, NRF24L01P_REG_RX_PW_P0, 0x00);
	nrf24l01_write_register(handle, NRF24L01P_REG_RX_PW_P1, 0x00);
	nrf24l01_write_register(handle, NRF24L01P_REG_RX_PW_P2, 0x00);
	nrf24l01_write_register(handle, NRF24L01P_REG_RX_PW_P3, 0x00);
	nrf24l01_write_register(handle, NRF24L01P_REG_RX_PW_P4, 0x00);
	nrf24l01_write_register(handle, NRF24L01P_REG_RX_PW_P5, 0x00);
	nrf24l01_write_register(handle, NRF24L01P_REG_FIFO_STATUS, 0x11);
	nrf24l01_write_register(handle, NRF24L01P_REG_DYNPD, 0x00);
	nrf24l01_write_register(handle, NRF24L01P_REG_FEATURE, 0x00);

	/* Reset FIFO */
	nrf24l01_flush_rx_fifo(handle);
	nrf24l01_flush_tx_fifo(handle);

	uint8_t reg_config_data = 0;
	if (handle->mode == NRF24L01_MODE_TRANSMITTER)
	{
		reg_config_data = nrf24l01_read_register(handle, NRF24L01P_REG_CONFIG);
		reg_config_data &= 0xFE;
		nrf24l01_write_register(handle, NRF24L01P_REG_CONFIG, reg_config_data);
	}
	else
	{
		reg_config_data = nrf24l01_read_register(handle, NRF24L01P_REG_CONFIG);
		reg_config_data |= 1 << 0;

		nrf24l01_write_register(handle, NRF24L01P_REG_CONFIG, reg_config_data);
	}

	nrf24l01_power_up(handle);

	if (handle->mode == NRF24L01_MODE_RECEIVER)
	{
		nrf24l01_rx_set_payload_widths(handle, handle->payload_len);
	}

	nrf24l01_set_rf_channel(handle, handle->channel);
	nrf24l01_set_rf_air_data_rate(handle, handle->data_rate);
	nrf24l01_set_rf_tx_output_power(handle, handle->output_pwr);

	nrf24l01_set_crc_length(handle, handle->crc_len);
	nrf24l01_set_address_widths(handle, handle->addr_width);

	nrf24l01_auto_retransmit_count(handle, handle->retrans_cnt);
	nrf24l01_auto_retransmit_delay(handle, handle->retrans_delay);

	handle->set_ce(1);


	return ERR_CODE_SUCCESS;
}

err_code_t nrf24l01_receive(nrf24l01_handle_t handle, uint8_t* rx_payload)
{
	/* Check if handle structure is NULL */
	if (handle == NULL)
	{
		return ERR_CODE_NULL_PTR;
	}

	nrf24l01_read_rx_fifo(handle, rx_payload);
	nrf24l01_clear_rx_dr(handle);

	return ERR_CODE_SUCCESS;
}

err_code_t nrf24l01_transmit(nrf24l01_handle_t handle, uint8_t* tx_payload)
{
	/* Check if handle structure is NULL */
	if (handle == NULL)
	{
		return ERR_CODE_NULL_PTR;
	}

	nrf24l01_write_tx_fifo(handle, tx_payload);

	return ERR_CODE_SUCCESS;
}

err_code_t nrf24l01_transmit_irq(nrf24l01_handle_t handle)
{
	/* Check if handle structure is NULL */
	if (handle == NULL)
	{
		return ERR_CODE_NULL_PTR;
	}

	uint8_t tx_ds;
	nrf24l01_get_status(handle, &tx_ds);
	tx_ds &= 0x20;

	if (tx_ds)
	{
		nrf24l01_clear_tx_ds(handle);
	}

	else
	{
		nrf24l01_clear_max_rt(handle);
	}

	return ERR_CODE_SUCCESS;
}

err_code_t nrf24l01_flush_rx_fifo(nrf24l01_handle_t handle)
{
	/* Check if handle structure is NULL */
	if (handle == NULL)
	{
		return ERR_CODE_NULL_PTR;
	}

	uint8_t command = NRF24L01P_CMD_FLUSH_RX;

	handle->set_cs(NRF24L01_CS_ACTIVE);
	handle->spi_send(&command, 1, SPI_ACTION_TIMEOUT);
	handle->set_cs(NRF24L01_CS_UNACTIVE);

	return ERR_CODE_SUCCESS;
}

err_code_t nrf24l01_flush_tx_fifo(nrf24l01_handle_t handle)
{
	/* Check if handle structure is NULL */
	if (handle == NULL)
	{
		return ERR_CODE_NULL_PTR;
	}

	uint8_t command = NRF24L01P_CMD_FLUSH_TX;

	handle->set_cs(NRF24L01_CS_ACTIVE);
	handle->spi_send(&command, 1, SPI_ACTION_TIMEOUT);
	handle->set_cs(NRF24L01_CS_UNACTIVE);

	return ERR_CODE_SUCCESS;
}

err_code_t nrf24l01_power_up(nrf24l01_handle_t handle)
{
	/* Check if handle structure is NULL */
	if (handle == NULL)
	{
		return ERR_CODE_NULL_PTR;
	}

	uint8_t reg_config_data = nrf24l01_read_register(handle, NRF24L01P_REG_CONFIG);
	reg_config_data |= 1 << 1;

	nrf24l01_write_register(handle, NRF24L01P_REG_CONFIG, reg_config_data);

	return ERR_CODE_SUCCESS;
}

err_code_t nrf24l01_power_down(nrf24l01_handle_t handle)
{
	/* Check if handle structure is NULL */
	if (handle == NULL)
	{
		return ERR_CODE_NULL_PTR;
	}

	uint8_t reg_config_data = nrf24l01_read_register(handle, NRF24L01P_REG_CONFIG);
	reg_config_data &= 0xFD;

	nrf24l01_write_register(handle, NRF24L01P_REG_CONFIG, reg_config_data);

	return ERR_CODE_SUCCESS;
}

err_code_t nrf24l01_get_status(nrf24l01_handle_t handle, uint8_t *status)
{
	/* Check if handle structure is NULL */
	if (handle == NULL)
	{
		return ERR_CODE_NULL_PTR;
	}

	handle->set_cs(NRF24L01_CS_ACTIVE);
	handle->spi_recv(status, 1, SPI_ACTION_TIMEOUT);
	handle->set_cs(NRF24L01_CS_UNACTIVE);

	return ERR_CODE_SUCCESS;
}

err_code_t nrf24l01_get_fifo_status(nrf24l01_handle_t handle, uint8_t *status)
{
	/* Check if handle structure is NULL */
	if (handle == NULL)
	{
		return ERR_CODE_NULL_PTR;
	}

	*status =  nrf24l01_read_register(handle, NRF24L01P_REG_FIFO_STATUS);

	return ERR_CODE_SUCCESS;
}

err_code_t nrf24l01_clear_tx_ds(nrf24l01_handle_t handle)
{
	/* Check if handle structure is NULL */
	if (handle == NULL)
	{
		return ERR_CODE_NULL_PTR;
	}

	uint8_t status;
	nrf24l01_get_status(handle, &status);
	status |= 0x20;

	nrf24l01_write_register(handle, NRF24L01P_REG_STATUS, status);

	return ERR_CODE_SUCCESS;
}

err_code_t nrf24l01_clear_max_rt(nrf24l01_handle_t handle)
{
	/* Check if handle structure is NULL */
	if (handle == NULL)
	{
		return ERR_CODE_NULL_PTR;
	}

	uint8_t status;
	nrf24l01_get_status(handle, &status);
	status |= 0x10;

	nrf24l01_write_register(handle, NRF24L01P_REG_STATUS, status);

	return ERR_CODE_SUCCESS;
}

err_code_t nrf24l01_clear_rx_dr(nrf24l01_handle_t handle)
{
	/* Check if handle structure is NULL */
	if (handle == NULL)
	{
		return ERR_CODE_NULL_PTR;
	}

	uint8_t status;
	nrf24l01_get_status(handle, &status);

	status |= 0x40;
	nrf24l01_write_register(handle, NRF24L01P_REG_STATUS, status);

	return ERR_CODE_SUCCESS;
}
