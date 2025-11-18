/*
 * Copyright 2025 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "ncp_ble_api.h"
#include "ncp_cmd_ble.h"
#include "app.h"
#include "ncp_common.h"
#include <string.h>

static uint8_t g_transfer_buff_send[SEND_BUFF_SIZE] = {0};

void ncp_ble_set_adv_data(uint8_t *advertise_array, uint32_t adv_length)
{
	MCU_NCPCmd_DS_BLE_COMMAND	*ncp_command 	= NULL;
	NCP_COMMAND 			 	*header 		= NULL;
	NCP_CMD_SET_ADV_DATA 		*set_adv_da		= NULL;

	// Prepare command pointer -----------------------------------------------
	ncp_command = (MCU_NCPCmd_DS_BLE_COMMAND*)g_transfer_buff_send;
	// -----------------------------------------------------------------------

	// Prepare header --------------------------------------------------------
	header = (NCP_COMMAND*)(&(ncp_command->header));

	memset(header, 0, sizeof(NCP_COMMAND));
	header->cmd 	= NCP_CMD_BLE_GAP_SET_ADV_DATA;
	header->result 	= NCP_CMD_RESULT_OK;
	header->seqnum 	= 1;
	header->size 	= sizeof(NCP_COMMAND);
	// -----------------------------------------------------------------------

	// Prepare set advertise command -----------------------------------------
	set_adv_da = &(ncp_command->params.set_adv_data);

	set_adv_da->adv_length = adv_length;

	memcpy(set_adv_da->adv_data, advertise_array, adv_length);

	header->size += adv_length + sizeof(set_adv_da->adv_length);
	// -----------------------------------------------------------------------

	ncp_send((uint8_t*)&g_transfer_buff_send, header->size);
	// -------------------------------------------------
}

void ncp_ble_start_adv(void)
{
	MCU_NCPCmd_DS_BLE_COMMAND	*ncp_command 	= NULL;
	NCP_COMMAND 			 	*header 		= NULL;

	// Prepare command pointer -----------------------------------------------
	ncp_command = (MCU_NCPCmd_DS_BLE_COMMAND*)g_transfer_buff_send;
	// -----------------------------------------------------------------------

	// Prepare header --------------------------------------------------------
	header = (NCP_COMMAND*)(&(ncp_command->header));

	memset(header, 0, sizeof(NCP_COMMAND));
	header->cmd 	= NCP_CMD_BLE_GAP_START_ADV;
	header->result 	= NCP_CMD_RESULT_OK;
	header->seqnum 	= 1;
	header->size 	= sizeof(NCP_COMMAND);
	// -----------------------------------------------------------------------

	ncp_send((uint8_t*)&g_transfer_buff_send, header->size);
	// -------------------------------------------------
}

void ncp_ble_stop_adv(void)
{
	MCU_NCPCmd_DS_BLE_COMMAND	*ncp_command 	= NULL;
	NCP_COMMAND 			 	*header 		= NULL;

	// Prepare command pointer -----------------------------------------------
	ncp_command = (MCU_NCPCmd_DS_BLE_COMMAND*)g_transfer_buff_send;
	// -----------------------------------------------------------------------

	// Prepare header --------------------------------------------------------
	header = (NCP_COMMAND*)(&(ncp_command->header));

	memset(header, 0, sizeof(NCP_COMMAND));
	header->cmd 	= NCP_CMD_BLE_GAP_STOP_ADV;
	header->result 	= NCP_CMD_RESULT_OK;
	header->seqnum 	= 1;
	header->size 	= sizeof(NCP_COMMAND);
	// -----------------------------------------------------------------------

	ncp_send((uint8_t*)&g_transfer_buff_send, header->size);
	// -------------------------------------------------
}

/*
* option		-> Scan options
* interval 		-> Range from 4 to 16384 (Decimal value) \n
* scan_window 	-> Range: 4 to 16384 (Decimal value, window value shall be less than or equal to interval value)
*/

void ncp_ble_set_scan_param(ble_scan_param_options_t option, uint16_t interval, uint16_t window)
{
	MCU_NCPCmd_DS_BLE_COMMAND	*ncp_command 	= NULL;
	NCP_COMMAND 			 	*header 		= NULL;
	NCP_CMD_SET_SCAN_PARAM 		*set_scan_param	= NULL;

	// Prepare command pointer -----------------------------------------------
	ncp_command = (MCU_NCPCmd_DS_BLE_COMMAND*)g_transfer_buff_send;
	// -----------------------------------------------------------------------

	// Prepare header --------------------------------------------------------
	header = (NCP_COMMAND*)(&(ncp_command->header));

	memset(header, 0, sizeof(NCP_COMMAND));
	header->cmd 	= NCP_CMD_BLE_GAP_SET_SCAN_PARAM;
	header->result 	= NCP_CMD_RESULT_OK;
	header->seqnum 	= 1;
	header->size 	= sizeof(NCP_COMMAND);
	// -----------------------------------------------------------------------

	// Prepare scan params ---------------------------------------------------
	set_scan_param = &(ncp_command->params.set_scan_parameter);

	set_scan_param->options = (uint32_t)option;
	set_scan_param->interval = interval;
	set_scan_param->window = window;

	header->size += sizeof(NCP_CMD_SET_SCAN_PARAM);
	// -----------------------------------------------------------------------

	ncp_send((uint8_t*)&g_transfer_buff_send, header->size);
	// -------------------------------------------------
}


void ncp_ble_start_scan(ble_scan_type_t scan_type)
{
	MCU_NCPCmd_DS_BLE_COMMAND	*ncp_command 	= NULL;
	NCP_COMMAND 			 	*header 		= NULL;
	NCP_CMD_SCAN_START 			*scan_start		= NULL;

	// Prepare command pointer -----------------------------------------------
	ncp_command = (MCU_NCPCmd_DS_BLE_COMMAND*)g_transfer_buff_send;
	// -----------------------------------------------------------------------

	// Prepare header --------------------------------------------------------
	header = (NCP_COMMAND*)(&(ncp_command->header));

	memset(header, 0, sizeof(NCP_COMMAND));
	header->cmd 	= NCP_CMD_BLE_GAP_START_SCAN;
	header->result 	= NCP_CMD_RESULT_OK;
	header->seqnum 	= 1;
	header->size 	= sizeof(NCP_COMMAND);
	// -----------------------------------------------------------------------

	scan_start = &(ncp_command->params.scan_start);
	scan_start->type = (scan_type == k_ble_passive_scan)? 0x01:0x09;

	header->size += sizeof(NCP_CMD_SCAN_START);

	ncp_send((uint8_t*)&g_transfer_buff_send, header->size);
	// -------------------------------------------------
}

void ncp_ble_stop_scan(void)
{
	MCU_NCPCmd_DS_BLE_COMMAND	*ncp_command 	= NULL;
	NCP_COMMAND 			 	*header 		= NULL;

	// Prepare command pointer -----------------------------------------------
	ncp_command = (MCU_NCPCmd_DS_BLE_COMMAND*)g_transfer_buff_send;
	// -----------------------------------------------------------------------

	// Prepare header --------------------------------------------------------
	header = (NCP_COMMAND*)(&(ncp_command->header));

	memset(header, 0, sizeof(NCP_COMMAND));
	header->cmd 	= NCP_CMD_BLE_GAP_STOP_SCAN;
	header->result 	= NCP_CMD_RESULT_OK;
	header->seqnum 	= 1;
	header->size 	= sizeof(NCP_COMMAND);
	// -----------------------------------------------------------------------

	ncp_send((uint8_t*)&g_transfer_buff_send, header->size);
	// -------------------------------------------------
}

void ncp_ble_connect(uint8_t addr[NCP_BLE_ADDR_LENGTH], ble_address_type_t type)
{
	MCU_NCPCmd_DS_BLE_COMMAND	*ncp_command 	= NULL;
	NCP_COMMAND 			 	*header 		= NULL;
	NCP_CMD_CONNECT 			*connect	= NULL;

	// Prepare command pointer -----------------------------------------------
	ncp_command = (MCU_NCPCmd_DS_BLE_COMMAND*)g_transfer_buff_send;
	// -----------------------------------------------------------------------

	// Prepare header --------------------------------------------------------
	header = (NCP_COMMAND*)(&(ncp_command->header));

	memset(header, 0, sizeof(NCP_COMMAND));
	header->cmd 	= NCP_CMD_BLE_GAP_CONNECT;
	header->result 	= NCP_CMD_RESULT_OK;
	header->seqnum 	= 1;
	header->size 	= sizeof(NCP_COMMAND);
	// -----------------------------------------------------------------------

	// Prepare connect -------------------------------------------------------
	connect = &(ncp_command->params.connect);

	memcpy(connect->addr, addr, NCP_BLE_ADDR_LENGTH);
	connect->type = (uint8_t)type;

	header->size += sizeof(NCP_CMD_CONNECT);
	// -----------------------------------------------------------------------

	ncp_send((uint8_t*)&g_transfer_buff_send, header->size);
	// -------------------------------------------------
}

void ncp_ble_disconnect(uint8_t addr[NCP_BLE_ADDR_LENGTH], ble_address_type_t type)
{
	MCU_NCPCmd_DS_BLE_COMMAND	*ncp_command 	= NULL;
	NCP_COMMAND 			 	*header 		= NULL;
	NCP_CMD_CONNECT 			*disconnect		= NULL;

	// Prepare command pointer -----------------------------------------------
	ncp_command = (MCU_NCPCmd_DS_BLE_COMMAND*)g_transfer_buff_send;
	// -----------------------------------------------------------------------

	// Prepare header --------------------------------------------------------
	header = (NCP_COMMAND*)(&(ncp_command->header));

	memset(header, 0, sizeof(NCP_COMMAND));
	header->cmd 	= NCP_CMD_BLE_GAP_DISCONNECT;
	header->result 	= NCP_CMD_RESULT_OK;
	header->seqnum 	= 1;
	header->size 	= sizeof(NCP_COMMAND);
	// -----------------------------------------------------------------------

	// Prepare disconnect ----------------------------------------------------
	disconnect = &(ncp_command->params.connect);

	memcpy(disconnect->addr, addr, NCP_BLE_ADDR_LENGTH);
	disconnect->type = (uint8_t)type;

	header->size += sizeof(NCP_CMD_CONNECT);
	// -----------------------------------------------------------------------

	ncp_send((uint8_t*)&g_transfer_buff_send, header->size);
	// -------------------------------------------------
}

void ncp_ble_set_data_len(uint8_t addr[NCP_BLE_ADDR_LENGTH], ble_address_type_t type, uint16_t tx_max_len, uint16_t tx_max_time)
{
	MCU_NCPCmd_DS_BLE_COMMAND	*ncp_command 	= NULL;
	NCP_COMMAND 			 	*header 		= NULL;
	NCP_CMD_SET_DATA_LEN 		*set_data_len   = NULL;

	// Prepare command pointer -----------------------------------------------
	ncp_command = (MCU_NCPCmd_DS_BLE_COMMAND*)g_transfer_buff_send;
	// -----------------------------------------------------------------------

	// Prepare header --------------------------------------------------------
	header = (NCP_COMMAND*)(&(ncp_command->header));

	memset(header, 0, sizeof(NCP_COMMAND));
	header->cmd 	= NCP_CMD_BLE_GAP_SET_DATA_LEN;
	header->result 	= NCP_CMD_RESULT_OK;
	header->seqnum 	= 1;
	header->size 	= sizeof(NCP_COMMAND);
	// -----------------------------------------------------------------------

	// Prepare data len ------------------------------------------------------
	set_data_len = &(ncp_command->params.set_data_len);

	memcpy(set_data_len->address, addr, NCP_BLE_ADDR_LENGTH);
	set_data_len->address_type = (uint8_t)type;
	set_data_len->tx_max_len = tx_max_len;

	if(tx_max_time > 0)
	{
		set_data_len->time_flag = 1;
		set_data_len->tx_max_time = tx_max_time;
	}
	else
	{
		set_data_len->time_flag = 0;
		set_data_len->tx_max_time = 0;
	}

	header->size += sizeof(NCP_CMD_SET_DATA_LEN);
	// -----------------------------------------------------------------------

	ncp_send((uint8_t*)&g_transfer_buff_send, header->size);
	// -------------------------------------------------
}

void ncp_ble_set_phy(uint8_t addr[NCP_BLE_ADDR_LENGTH], ble_address_type_t type, ble_connection_phy_t connect_phy, ble_transfer_phy_t tx_phy, ble_transfer_phy_t rx_phy)
{
	MCU_NCPCmd_DS_BLE_COMMAND	*ncp_command 	= NULL;
	NCP_COMMAND 			 	*header 		= NULL;
	NCP_CMD_SET_PHY 			*set_phy   		= NULL;

	// Prepare command pointer -----------------------------------------------
	ncp_command = (MCU_NCPCmd_DS_BLE_COMMAND*)g_transfer_buff_send;
	// -----------------------------------------------------------------------

	// Prepare header --------------------------------------------------------
	header = (NCP_COMMAND*)(&(ncp_command->header));

	memset(header, 0, sizeof(NCP_COMMAND));
	header->cmd 	= NCP_CMD_BLE_GAP_SET_PHY;
	header->result 	= NCP_CMD_RESULT_OK;
	header->seqnum 	= 1;
	header->size 	= sizeof(NCP_COMMAND);
	// -----------------------------------------------------------------------

	// Prepare phy -----------------------------------------------------------
	set_phy = &(ncp_command->params.set_phy);

	memcpy(set_phy->address, addr, NCP_BLE_ADDR_LENGTH);
	set_phy->address_type = (uint8_t)type;

	set_phy->options 		= (uint16_t)connect_phy;
	set_phy->pref_tx_phy 	= (uint8_t)tx_phy;
	set_phy->pref_rx_phy 	= (uint8_t)rx_phy;

	header->size += sizeof(NCP_CMD_SET_PHY);
	// -----------------------------------------------------------------------

	ncp_send((uint8_t*)&g_transfer_buff_send, header->size);
	// -------------------------------------------------
}

void ncp_ble_connect_param_update(uint8_t addr[NCP_BLE_ADDR_LENGTH], ble_address_type_t type, uint16_t interval_min, uint16_t interval_max, uint16_t latency, uint16_t timeout)
{
	MCU_NCPCmd_DS_BLE_COMMAND	*ncp_command 		= NULL;
	NCP_COMMAND 			 	*header 			= NULL;
	NCP_CMD_CONN_PARA_UPDATE 	*conn_param_update	= NULL;

	// Prepare command pointer -----------------------------------------------
	ncp_command = (MCU_NCPCmd_DS_BLE_COMMAND*)g_transfer_buff_send;
	// -----------------------------------------------------------------------

	// Prepare header --------------------------------------------------------
	header = (NCP_COMMAND*)(&(ncp_command->header));

	memset(header, 0, sizeof(NCP_COMMAND));
	header->cmd 	= NCP_CMD_BLE_GAP_CONN_PARAM_UPDATE;
	header->result 	= NCP_CMD_RESULT_OK;
	header->seqnum 	= 1;
	header->size 	= sizeof(NCP_COMMAND);
	// -----------------------------------------------------------------------

	// Prepare param update --------------------------------------------------
	conn_param_update = &(ncp_command->params.conn_param_update);

	memcpy(conn_param_update->addr, addr, NCP_BLE_ADDR_LENGTH);
	conn_param_update->type = (uint8_t)type;
	conn_param_update->interval_min = interval_min;
	conn_param_update->interval_max = interval_max;
	conn_param_update->latency = latency;
	conn_param_update->timeout = timeout;

	header->size += sizeof(NCP_CMD_CONN_PARA_UPDATE);
	// -----------------------------------------------------------------------

	ncp_send((uint8_t*)&g_transfer_buff_send, header->size);
	// -------------------------------------------------
}

void ncp_ble_start_encryption(uint8_t addr[NCP_BLE_ADDR_LENGTH], ble_address_type_t type)
{
	MCU_NCPCmd_DS_BLE_COMMAND	*ncp_command 	= NULL;
	NCP_COMMAND 			 	*header 		= NULL;
	NCP_CMD_ENCRYPTION 			*encryption		= NULL;

	// Prepare command pointer -----------------------------------------------
	ncp_command = (MCU_NCPCmd_DS_BLE_COMMAND*)g_transfer_buff_send;
	// -----------------------------------------------------------------------

	// Prepare header --------------------------------------------------------
	header = (NCP_COMMAND*)(&(ncp_command->header));

	memset(header, 0, sizeof(NCP_COMMAND));
	header->cmd 	= NCP_CMD_BLE_GAP_PAIR;
	header->result 	= NCP_CMD_RESULT_OK;
	header->seqnum 	= 1;
	header->size 	= sizeof(NCP_COMMAND);
	// -----------------------------------------------------------------------

	// Prepare encryption ----------------------------------------------------
	encryption = &(ncp_command->params.conn_encryption);

	memcpy(encryption->addr, addr, NCP_BLE_ADDR_LENGTH);
	encryption->type = (uint8_t)type;

	header->size += sizeof(NCP_CMD_ENCRYPTION);
	// -----------------------------------------------------------------------

	ncp_send((uint8_t*)&g_transfer_buff_send, header->size);
	// -------------------------------------------------
}

void ncp_ble_write_characteristic(uint8_t *uuid, uint8_t uuid_len, uint8_t *data, uint16_t data_len)
{
	MCU_NCPCmd_DS_BLE_COMMAND	*ncp_command 	= NULL;
	NCP_COMMAND 			 	*header 		= NULL;
	NCP_SET_VALUE_CMD 			*set_value		= NULL;

	// Prepare command pointer -----------------------------------------------
	ncp_command = (MCU_NCPCmd_DS_BLE_COMMAND*)g_transfer_buff_send;
	// -----------------------------------------------------------------------

	// Prepare header --------------------------------------------------------
	header = (NCP_COMMAND*)(&(ncp_command->header));

	memset(header, 0, sizeof(NCP_COMMAND));
	header->cmd 	= NCP_CMD_BLE_GATT_SET_VALUE;
	header->result 	= NCP_CMD_RESULT_OK;
	header->seqnum 	= 1;
	header->size 	= sizeof(NCP_COMMAND);
	// -----------------------------------------------------------------------

	// Prepare write ---------------------------------------------------------
	set_value = &(ncp_command->params.gatt_set_value);

	memset(&(set_value->uuid), 0, SERVER_MAX_UUID_LEN);
	memcpy(set_value->uuid, uuid, uuid_len);
	set_value->uuid_length = uuid_len;

	memset(&(set_value->value), 0, 512);
	memcpy(set_value->value, data, data_len);
	set_value->len = data_len;

	header->size += sizeof(NCP_SET_VALUE_CMD) - sizeof(set_value->value) + set_value->len;
	// -----------------------------------------------------------------------

	ncp_send((uint8_t*)&g_transfer_buff_send, header->size);
	// -------------------------------------------------
}

void ncp_ble_read_characteristic(uint8_t addr[NCP_BLE_ADDR_LENGTH], ble_address_type_t type, uint16_t handle)
{
	MCU_NCPCmd_DS_BLE_COMMAND	*ncp_command 	= NULL;
	NCP_COMMAND 			 	*header 		= NULL;
	NCP_GATT_READ_CMD 			*read		= NULL;

	// Prepare command pointer -----------------------------------------------
	ncp_command = (MCU_NCPCmd_DS_BLE_COMMAND*)g_transfer_buff_send;
	// -----------------------------------------------------------------------

	// Prepare header --------------------------------------------------------
	header = (NCP_COMMAND*)(&(ncp_command->header));

	memset(header, 0, sizeof(NCP_COMMAND));
	header->cmd 	= NCP_CMD_BLE_GATT_READ;
	header->result 	= NCP_CMD_RESULT_OK;
	header->seqnum 	= 1;
	header->size 	= sizeof(NCP_COMMAND);
	// -----------------------------------------------------------------------

	// Prepare read ----------------------------------------------------------
	read = &(ncp_command->params.gatt_read_char);

	read->type = type;
    memcpy(read->addr, addr, NCP_BLE_ADDR_LENGTH);
    read->handle = handle;

	header->size += sizeof(NCP_GATT_READ_CMD);
	// -----------------------------------------------------------------------

	ncp_send((uint8_t*)&g_transfer_buff_send, header->size);
	// -------------------------------------------------
}

void ncp_ble_set_power_mode(ble_power_mode_t power_mode)
{
	MCU_NCPCmd_DS_BLE_COMMAND	*ncp_command 	= NULL;
	NCP_COMMAND 			 	*header 		= NULL;
	NCP_CMD_SET_POWER_MODE 		*set_power_mode	= NULL;

	// Prepare command pointer -----------------------------------------------
	ncp_command = (MCU_NCPCmd_DS_BLE_COMMAND*)g_transfer_buff_send;
	// -----------------------------------------------------------------------

	// Prepare header --------------------------------------------------------
	header = (NCP_COMMAND*)(&(ncp_command->header));

	memset(header, 0, sizeof(NCP_COMMAND));
	header->cmd 	= NCP_CMD_BLE_VENDOR_POWER_MODE;
	header->result 	= NCP_CMD_RESULT_OK;
	header->seqnum 	= 1;
	header->size 	= sizeof(NCP_COMMAND);
	// -----------------------------------------------------------------------

	// Prepare power mode ----------------------------------------------------
	set_power_mode = &(ncp_command->params.set_pw_mode);

	set_power_mode->mode = power_mode;

	header->size += sizeof(NCP_CMD_SET_POWER_MODE);
	// -----------------------------------------------------------------------

	ncp_send((uint8_t*)&g_transfer_buff_send, header->size);
	// -------------------------------------------------
}

void ncp_ble_register_service(uint8_t *service_id, uint8_t service_len)
{
	MCU_NCPCmd_DS_BLE_COMMAND	*ncp_command 	  = NULL;
	NCP_COMMAND 			 	*header 		  = NULL;
	NCP_REGISTER_SERVICE 		*register_service = NULL;

	// Prepare command pointer -----------------------------------------------
	ncp_command = (MCU_NCPCmd_DS_BLE_COMMAND*)g_transfer_buff_send;
	// -----------------------------------------------------------------------

	// Prepare header --------------------------------------------------------
	header = (NCP_COMMAND*)(&(ncp_command->header));

	memset(header, 0, sizeof(NCP_COMMAND));
	header->cmd 	= NCP_CMD_BLE_GATT_REGISTER_SERVICE;
	header->result 	= NCP_CMD_RESULT_OK;
	header->seqnum 	= 1;
	header->size 	= sizeof(NCP_COMMAND);
	// -----------------------------------------------------------------------

	// Prepare register service ----------------------------------------------
	register_service = &(ncp_command->params.register_service);

	register_service->svc_length = service_len;

	memset(register_service->service, 0, sizeof(register_service->service));
    memcpy(register_service->service, service_id, service_len);

	header->size += sizeof(register_service->svc_length) + register_service->svc_length;
	// -----------------------------------------------------------------------

	ncp_send((uint8_t*)&g_transfer_buff_send, header->size);
	// -------------------------------------------------
}

void ncp_ble_start_service(uint8_t service_id)
{
	MCU_NCPCmd_DS_BLE_COMMAND	*ncp_command 	  = NULL;
	NCP_COMMAND 			 	*header 		  = NULL;
	NCP_CMD_START_SERVICE 		*start_service 	  = NULL;

	// Prepare command pointer -----------------------------------------------
	ncp_command = (MCU_NCPCmd_DS_BLE_COMMAND*)g_transfer_buff_send;
	// -----------------------------------------------------------------------

	// Prepare header --------------------------------------------------------
	header = (NCP_COMMAND*)(&(ncp_command->header));

	memset(header, 0, sizeof(NCP_COMMAND));
	header->cmd 	= NCP_CMD_BLE_GATT_START_SERVICE;
	header->result 	= NCP_CMD_RESULT_OK;
	header->seqnum 	= 1;
	header->size 	= sizeof(NCP_COMMAND);
	// -----------------------------------------------------------------------

	// Prepare start service -------------------------------------------------
	start_service = (NCP_CMD_START_SERVICE*)&(ncp_command->params);

	start_service->form_host = 1;
	start_service->svc_id = service_id;

	header->size += sizeof(NCP_CMD_START_SERVICE);
	// -----------------------------------------------------------------------

	ncp_send((uint8_t*)&g_transfer_buff_send, header->size);
	// -------------------------------------------------
}

void ncp_ble_subscribe_service(uint8_t addr[NCP_BLE_ADDR_LENGTH], ble_address_type_t type, ble_subscribe_type_t subs_type, uint16_t handle, uint8_t enable)
{
	MCU_NCPCmd_DS_BLE_COMMAND	*ncp_command 	  = NULL;
	NCP_COMMAND 			 	*header 		  = NULL;
	NCP_CFG_SUBSCRIBE_CMD 		*ble_subscribe 	  = NULL;

	// Prepare command pointer -----------------------------------------------
	ncp_command = (MCU_NCPCmd_DS_BLE_COMMAND*)g_transfer_buff_send;
	// -----------------------------------------------------------------------

	// Prepare header --------------------------------------------------------
	header = (NCP_COMMAND*)(&(ncp_command->header));

	memset(header, 0, sizeof(NCP_COMMAND));
	if(subs_type == k_subscribe_indicate) header->cmd = NCP_CMD_BLE_GATT_CFG_INDICATE;
	else								  header->cmd = NCP_CMD_BLE_GATT_CFG_NOTIFY;
	header->result 	= NCP_CMD_RESULT_OK;
	header->seqnum 	= 1;
	header->size 	= sizeof(NCP_COMMAND);
	// -----------------------------------------------------------------------

	// Prepare subscribe service ---------------------------------------------
	ble_subscribe = &(ncp_command->params.cfg_subscribe);

    ble_subscribe->address_type = type;
    memcpy(ble_subscribe->address, addr, NCP_BLE_ADDR_LENGTH);
    ble_subscribe->enable = enable;
    ble_subscribe->ccc_handle = handle;

	header->size += sizeof(NCP_CFG_SUBSCRIBE_CMD);
	// -----------------------------------------------------------------------

	ncp_send((uint8_t*)&g_transfer_buff_send, header->size);
	// -------------------------------------------------
}

void ncp_ble_l2cap_connect(uint8_t addr[NCP_BLE_ADDR_LENGTH], ble_address_type_t type, uint16_t protocol_service_multiplexer, ble_l2cap_security_t security)
{
	MCU_NCPCmd_DS_BLE_COMMAND	*ncp_command 	  = NULL;
	NCP_COMMAND 			 	*header 		  = NULL;
	NCP_L2CAP_CONNECT_CMD 		*l2cap_connect 	  = NULL;

	// Prepare command pointer -----------------------------------------------
	ncp_command = (MCU_NCPCmd_DS_BLE_COMMAND*)g_transfer_buff_send;
	// -----------------------------------------------------------------------

	// Prepare header --------------------------------------------------------
	header = (NCP_COMMAND*)(&(ncp_command->header));

	memset(header, 0, sizeof(NCP_COMMAND));
	header->cmd 	= NCP_CMD_BLE_L2CAP_CONNECT;
	header->result 	= NCP_CMD_RESULT_OK;
	header->seqnum 	= 1;
	header->size 	= sizeof(NCP_COMMAND);
	// -----------------------------------------------------------------------

	// Prepare l2cap connect -------------------------------------------------
	l2cap_connect = &(ncp_command->params.l2cap_connect);

	l2cap_connect->address_type = type;
    memcpy(l2cap_connect->address, addr, NCP_BLE_ADDR_LENGTH);
	l2cap_connect->psm = protocol_service_multiplexer;

    if(security != k_no_encryption_no_authentication)
    {
    	l2cap_connect->sec_flag = 1;
    	l2cap_connect->sec = security;
    }
    else
    {
    	l2cap_connect->sec_flag = 0;
    	l2cap_connect->sec = security;
    }

	header->size += sizeof(NCP_L2CAP_CONNECT_CMD);
	// -----------------------------------------------------------------------

	ncp_send((uint8_t*)&g_transfer_buff_send, header->size);
	// -------------------------------------------------
}

void ncp_ble_l2cap_disconnect(uint8_t addr[NCP_BLE_ADDR_LENGTH], ble_address_type_t type)
{
	MCU_NCPCmd_DS_BLE_COMMAND	*ncp_command 	  = NULL;
	NCP_COMMAND 			 	*header 		  = NULL;
	NCP_L2CAP_DISCONNECT_CMD 	*l2cap_disconnect = NULL;

	// Prepare command pointer -----------------------------------------------
	ncp_command = (MCU_NCPCmd_DS_BLE_COMMAND*)g_transfer_buff_send;
	// -----------------------------------------------------------------------

	// Prepare header --------------------------------------------------------
	header = (NCP_COMMAND*)(&(ncp_command->header));

	memset(header, 0, sizeof(NCP_COMMAND));
	header->cmd 	= NCP_CMD_BLE_L2CAP_DISCONNECT;
	header->result 	= NCP_CMD_RESULT_OK;
	header->seqnum 	= 1;
	header->size 	= sizeof(NCP_COMMAND);
	// -----------------------------------------------------------------------

	// Prepare l2cap disconnect ----------------------------------------------
	l2cap_disconnect = &(ncp_command->params.l2cap_disconnect);

	l2cap_disconnect->address_type = type;
    memcpy(l2cap_disconnect->address, addr, NCP_BLE_ADDR_LENGTH);

	header->size += sizeof(NCP_L2CAP_DISCONNECT_CMD);
	// -----------------------------------------------------------------------

	ncp_send((uint8_t*)&g_transfer_buff_send, header->size);
	// -------------------------------------------------
}

void ncp_ble_l2cap_send(uint8_t addr[NCP_BLE_ADDR_LENGTH], ble_address_type_t type, uint16_t times)
{
	MCU_NCPCmd_DS_BLE_COMMAND	*ncp_command 	  = NULL;
	NCP_COMMAND 			 	*header 		  = NULL;
	NCP_L2CAP_SEND_CMD 			*l2cap_send 	  = NULL;

	// Prepare command pointer -----------------------------------------------
	ncp_command = (MCU_NCPCmd_DS_BLE_COMMAND*)g_transfer_buff_send;
	// -----------------------------------------------------------------------

	// Prepare header --------------------------------------------------------
	header = (NCP_COMMAND*)(&(ncp_command->header));

	memset(header, 0, sizeof(NCP_COMMAND));
	header->cmd 	= NCP_CMD_BLE_L2CAP_SEND;
	header->result 	= NCP_CMD_RESULT_OK;
	header->seqnum 	= 1;
	header->size 	= sizeof(NCP_COMMAND);
	// -----------------------------------------------------------------------

	// Prepare l2cap send ----------------------------------------------------
	l2cap_send = &(ncp_command->params.l2cap_send);

	l2cap_send->address_type = type;
    memcpy(l2cap_send->address, addr, NCP_BLE_ADDR_LENGTH);
    l2cap_send->times = times;

	header->size += sizeof(NCP_L2CAP_SEND_CMD);
	// -----------------------------------------------------------------------

	ncp_send((uint8_t*)&g_transfer_buff_send, header->size);
	// -------------------------------------------------
}

void ncp_ble_l2cap_register(uint16_t protocol_service_multiplexer, ble_l2cap_security_t security, uint8_t policy)
{
	MCU_NCPCmd_DS_BLE_COMMAND	*ncp_command 	  = NULL;
	NCP_COMMAND 			 	*header 		  = NULL;
	NCP_L2CAP_REGISTER_CMD 		*l2cap_register	  = NULL;

	// Prepare command pointer -----------------------------------------------
	ncp_command = (MCU_NCPCmd_DS_BLE_COMMAND*)g_transfer_buff_send;
	// -----------------------------------------------------------------------

	// Prepare header --------------------------------------------------------
	header = (NCP_COMMAND*)(&(ncp_command->header));

	memset(header, 0, sizeof(NCP_COMMAND));
	header->cmd 	= NCP_CMD_BLE_L2CAP_REGISTER;
	header->result 	= NCP_CMD_RESULT_OK;
	header->seqnum 	= 1;
	header->size 	= sizeof(NCP_COMMAND);
	// -----------------------------------------------------------------------

	// Prepare l2cap register ------------------------------------------------
	l2cap_register = &(ncp_command->params.l2cap_register);

	l2cap_register->psm = protocol_service_multiplexer;
	if(security != k_no_encryption_no_authentication)
	{
		l2cap_register->sec_flag = 1;
	}
	else
	{
		l2cap_register->sec_flag = 0;
	}
	l2cap_register->sec_level = security;
	if(policy != 0)
	{
		l2cap_register->policy_flag = 1;
	}
	else
	{
		l2cap_register->policy_flag = 0;
	}
	l2cap_register->policy = policy;

	header->size += sizeof(NCP_L2CAP_REGISTER_CMD);
	// -----------------------------------------------------------------------

	ncp_send((uint8_t*)&g_transfer_buff_send, header->size);
	// -------------------------------------------------
}
