/*
 * Copyright 2025 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "ncp_wifi_api.h"
#include "app.h"
#include "ncp_common.h"
#include <string.h>

static uint8_t g_transfer_buff_send[SEND_BUFF_SIZE] = {0};


void ncp_wifi_api_init(void)
{
}


// Command functions -----------------------------------------------------------------------------------------------
void ncp_wlan_scan_command(void)
{
	NCP_COMMAND header;
	// Prepare header  with command and size -----------
	header.cmd = NCP_WLAN_SCAN_SEND;
	header.size = sizeof(NCP_COMMAND);
	header.result = NCP_CMD_RESULT_OK;
	header.seqnum = 1;
	ncp_send((uint8_t*)&header, sizeof(NCP_COMMAND));
	// -------------------------------------------------
}

void ncp_wlan_add_command(uint8_t *network_name_array, uint8_t network_name_len,
						  uint8_t *ssid_array,         uint8_t ssid_len,
						  uint8_t *password_array,     uint8_t password_len,
						  wlan_security_type_t security)
{
	NCPCmd_DS_COMMAND 	*ncp_command 	= NULL;
	NCP_COMMAND 		*header 		= NULL;
	NCP_CMD_NETWORK_ADD *add_network 	= NULL;
    SSID_ParamSet_t 	*ssid 			= NULL;
    Security_ParamSet_t *security_wpa 	= NULL;

	// Prepare command pointer -----------------------------------------------
	ncp_command = (NCPCmd_DS_COMMAND*)g_transfer_buff_send;
	// -----------------------------------------------------------------------

	// Prepare header --------------------------------------------------------
	header = (NCP_COMMAND*)(&(ncp_command->header));

	memset(header, 0, sizeof(NCP_COMMAND));
	header->cmd 	= NCP_WLAN_ADD_SEND;
	header->result 	= NCP_CMD_RESULT_OK;
	header->seqnum 	= 1;
	header->size 	= sizeof(NCP_COMMAND);
	// -----------------------------------------------------------------------

	// Prepare add network command -------------------------------------------
	add_network = (NCP_CMD_NETWORK_ADD*)(&(ncp_command->params.network_add));

	memset(add_network, 0, sizeof(NCP_CMD_NETWORK_ADD));
	memcpy(add_network->name, network_name_array, network_name_len);

	header->size += sizeof(NCP_CMD_NETWORK_ADD) - sizeof(add_network->tlv_buf);
	// -----------------------------------------------------------------------

	// Prepare SSID pointers and values --------------------------------------
	ssid = (SSID_ParamSet_t*)(add_network->tlv_buf);

	memset(ssid, 0, sizeof(SSID_ParamSet_t));

	ssid->header.type = NCP_CMD_NETWORK_SSID_TLV;
	ssid->header.size = sizeof(ssid->ssid);
	memcpy(ssid->ssid, ssid_array, ssid_len);

	add_network->tlv_buf_len = sizeof(SSID_ParamSet_t);
	// -----------------------------------------------------------------------

	// Prepare security pointers and values ----------------------------------
	if((security != k_wlan_security_none) && (security != k_wlan_security_wep_open))
	{
		security_wpa = (Security_ParamSet_t*)(add_network->tlv_buf + add_network->tlv_buf_len);

		memset(security_wpa, 0, sizeof(Security_ParamSet_t));

		security_wpa->header.type = NCP_CMD_NETWORK_SECURITY_TLV;
		security_wpa->header.size = sizeof(Security_ParamSet_t) - sizeof(TypeHeader_t) - sizeof(security_wpa->password);
		security_wpa->header.size += password_len;

		security_wpa->type = (uint8_t)security;
		memcpy(security_wpa->password, password_array, password_len);
		security_wpa->password_len = password_len;

		add_network->tlv_buf_len += sizeof(Security_ParamSet_t) - sizeof(security_wpa->password) + password_len;
	}
	// -----------------------------------------------------------------------

	header->size += add_network->tlv_buf_len;

	ncp_send((uint8_t*)&g_transfer_buff_send, header->size);
	// -------------------------------------------------
}

void ncp_wlan_remove_command(uint8_t *network_name_array, uint8_t network_name_len)
{
	NCPCmd_DS_COMMAND 		*ncp_command 	= NULL;
	NCP_COMMAND 			*header 		= NULL;
	NCP_CMD_NETWORK_REMOVE *remove_network 	= NULL;

	// Prepare command pointer -----------------------------------------------
	ncp_command = (NCPCmd_DS_COMMAND*)g_transfer_buff_send;
	// -----------------------------------------------------------------------

	// Prepare header --------------------------------------------------------
	header = (NCP_COMMAND*)(&(ncp_command->header));

	memset(header, 0, sizeof(NCP_COMMAND));
	header->cmd 	= NCP_WLAN_REMOVE_SEMD;
	header->result 	= NCP_CMD_RESULT_OK;
	header->seqnum 	= 1;
	header->size 	= sizeof(NCP_COMMAND);
	// -----------------------------------------------------------------------

	// Prepare remove network command ----------------------------------------
	remove_network = (NCP_CMD_NETWORK_REMOVE*)(&(ncp_command->params.network_remove));

	memset(remove_network, 0, sizeof(NCP_CMD_NETWORK_REMOVE));
	memcpy(remove_network->name, network_name_array, network_name_len);
	remove_network->remove_state = WM_SUCCESS;

	header->size += sizeof(NCP_CMD_NETWORK_REMOVE);
	// -----------------------------------------------------------------------

	ncp_send((uint8_t*)&g_transfer_buff_send, header->size);
	// -------------------------------------------------
}

void ncp_wlan_connect_command(uint8_t *network_name_array, uint8_t network_name_len)
{
	NCPCmd_DS_COMMAND *ncp_command;
	NCP_COMMAND *header;
	NCP_CMD_NETWORK_ADD *add_network = NULL;

	// Prepare header  with command and size -----------
	ncp_command = (NCPCmd_DS_COMMAND*)g_transfer_buff_send;
	header = &(ncp_command->header);
	add_network = (NCP_CMD_NETWORK_ADD*)(g_transfer_buff_send + sizeof(NCP_COMMAND));
	add_network->tlv_buf_len = 0;

	header->cmd 	= NCP_CMD_WLAN_CONNECT;
	header->size 	= sizeof(NCP_COMMAND) + sizeof(add_network->name) + sizeof(add_network->tlv_buf_len);
	header->result 	= NCP_CMD_RESULT_OK;
	header->seqnum 	= 1;

	memset(add_network->name, 0, WLAN_NETWORK_NAME_MAX_LENGTH);
	memcpy(add_network->name, network_name_array, network_name_len);

	ncp_send((uint8_t*)&g_transfer_buff_send, header->size);
	// -------------------------------------------------
}

void ncp_wlan_disconnect_command(void)
{
	NCPCmd_DS_COMMAND *ncp_command;
	NCP_COMMAND *header;

	ncp_command = (NCPCmd_DS_COMMAND*)g_transfer_buff_send;

	// Prepare header --------------------------------------------------------
	header = &(ncp_command->header);
	memset(header, 0, sizeof(NCP_COMMAND));

	header->cmd 	= NCP_CMD_WLAN_DISCONNECT;
	header->result 	= NCP_CMD_RESULT_OK;
	header->seqnum 	= 1;
	header->size 	= sizeof(NCP_COMMAND);
	// -----------------------------------------------------------------------

	ncp_send((uint8_t*)&g_transfer_buff_send, header->size);
	// -------------------------------------------------
}

void ncp_wlan_version_command(void)
{
	NCPCmd_DS_COMMAND *ncp_command;
	NCP_COMMAND *header;

	ncp_command = (NCPCmd_DS_COMMAND*)g_transfer_buff_send;

	// Prepare header --------------------------------------------------------
	header = &(ncp_command->header);
	memset(header, 0, sizeof(NCP_COMMAND));

	header->cmd 	= NCP_CMD_WLAN_VERSION;
	header->result 	= NCP_CMD_RESULT_OK;
	header->seqnum 	= 1;
	header->size 	= sizeof(NCP_COMMAND);
	// -----------------------------------------------------------------------

	ncp_send((uint8_t*)&g_transfer_buff_send, header->size);
	// -------------------------------------------------
}

void ncp_wlan_stat_command(void)
{
	NCPCmd_DS_COMMAND *ncp_command;
	NCP_COMMAND *header;

	ncp_command = (NCPCmd_DS_COMMAND*)g_transfer_buff_send;

	// Prepare header --------------------------------------------------------
	header = &(ncp_command->header);
	memset(header, 0, sizeof(NCP_COMMAND));

	header->cmd 	= NCP_CMD_WLAN_STAT;
	header->result 	= NCP_CMD_RESULT_OK;
	header->seqnum 	= 1;
	header->size 	= sizeof(NCP_COMMAND);
	// -----------------------------------------------------------------------

	ncp_send((uint8_t*)&g_transfer_buff_send, header->size);
	// -------------------------------------------------
}

void ncp_wlan_info_command(void)
{
	NCPCmd_DS_COMMAND *ncp_command;
	NCP_COMMAND *header;

	ncp_command = (NCPCmd_DS_COMMAND*)g_transfer_buff_send;

	// Prepare header --------------------------------------------------------
	header = &(ncp_command->header);
	memset(header, 0, sizeof(NCP_COMMAND));

	header->cmd 	= NCP_CMD_WLAN_NETWORK_INFO;
	header->result 	= NCP_CMD_RESULT_OK;
	header->seqnum 	= 1;
	header->size 	= sizeof(NCP_COMMAND);
	// -----------------------------------------------------------------------

	ncp_send((uint8_t*)&g_transfer_buff_send, header->size);
	// -------------------------------------------------
}

void ncp_wlan_socket_open(socket_protocol_t protocol)
{
	NCPCmd_DS_COMMAND 		*ncp_command 	= NULL;
	NCP_COMMAND 			*header 		= NULL;
	NCP_CMD_SOCKET_OPEN_CFG *wlan_socket 	= NULL;

	// Prepare command pointer -----------------------------------------------
	ncp_command = (NCPCmd_DS_COMMAND*)g_transfer_buff_send;
	// -----------------------------------------------------------------------

	// Prepare header --------------------------------------------------------
	header = &(ncp_command->header);

	memset(header, 0, sizeof(NCP_COMMAND));
	header->cmd 	= NCP_CMD_SOCKET_OPEN;
	header->result 	= NCP_CMD_RESULT_OK;
	header->seqnum 	= 1;
	header->size 	= sizeof(NCP_COMMAND);
	// -----------------------------------------------------------------------

	// Prepare socket command ------------------------------------------------
	wlan_socket = (NCP_CMD_SOCKET_OPEN_CFG*)(&(ncp_command->params.wlan_socket_open));
	memset(wlan_socket, 0, sizeof(NCP_CMD_SOCKET_OPEN_CFG));
	// Type ----------------------
	switch(protocol)
	{
		case k_socket_tcp:
			memcpy(wlan_socket->socket_type, "tcp", strlen("tcp"));
		break;
		case k_socket_udp:
			memcpy(wlan_socket->socket_type, "udp", strlen("udp"));
		break;
		default:
			memcpy(wlan_socket->socket_type, "raw", strlen("raw"));
		break;
	}
	// Domain --------------------
	memcpy(wlan_socket->domain_type, "ipv4", strlen("ipv4"));
	// Protocol ------------------
	memcpy(wlan_socket->protocol, "tcp", strlen("tcp"));

	header->size += sizeof(NCP_CMD_SOCKET_OPEN_CFG);
	// -----------------------------------------------------------------------

	ncp_send((uint8_t*)&g_transfer_buff_send, header->size);
	// -------------------------------------------------
}

void ncp_wlan_socket_connect(uint8_t handle , uint8_t *ip, uint32_t port)
{
	NCPCmd_DS_COMMAND 		*ncp_command 	= NULL;
	NCP_COMMAND 			*header 		= NULL;
	NCP_CMD_SOCKET_CON_CFG  *wlan_socket 	= NULL;

	// Prepare command pointer -----------------------------------------------
	ncp_command = (NCPCmd_DS_COMMAND*)g_transfer_buff_send;
	// -----------------------------------------------------------------------

	// Prepare header --------------------------------------------------------
	header = (NCP_COMMAND*)(&(ncp_command->header));

	memset(header, 0, sizeof(NCP_COMMAND));
	header->cmd 	= NCP_CMD_SOCKET_CON;
	header->result 	= NCP_CMD_RESULT_OK;
	header->seqnum 	= 1;
	header->size 	= sizeof(NCP_COMMAND);
	// -----------------------------------------------------------------------

	// Prepare socket command ------------------------------------------------
	wlan_socket = (NCP_CMD_SOCKET_CON_CFG*)(&(ncp_command->params.wlan_socket_con));
	memset(wlan_socket, 0, sizeof(NCP_CMD_SOCKET_CON_CFG));

	// Socket connection parameters ----
	wlan_socket->handle = handle;
	wlan_socket->port = port;
	memcpy(wlan_socket->ip_addr, ip, strlen((char*)ip));
	// ---------------------------------

	header->size += sizeof(NCP_CMD_SOCKET_CON_CFG);
	// -----------------------------------------------------------------------

	ncp_send((uint8_t*)&g_transfer_buff_send, header->size);
	// -------------------------------------------------
}

void ncp_wlan_socket_bind(uint8_t handle , uint8_t *ip, uint32_t port)
{
	NCPCmd_DS_COMMAND 		*ncp_command 	= NULL;
	NCP_COMMAND 			*header 		= NULL;
	NCP_CMD_SOCKET_BIND_CFG  *wlan_socket 	= NULL;

	// Prepare command pointer -----------------------------------------------
	ncp_command = (NCPCmd_DS_COMMAND*)g_transfer_buff_send;
	// -----------------------------------------------------------------------

	// Prepare header --------------------------------------------------------
	header = (NCP_COMMAND*)(&(ncp_command->header));

	memset(header, 0, sizeof(NCP_COMMAND));
	header->cmd 	= NCP_CMD_SOCKET_BIND;
	header->result 	= NCP_CMD_RESULT_OK;
	header->seqnum 	= 1;
	header->size 	= sizeof(NCP_COMMAND);
	// -----------------------------------------------------------------------

	// Prepare socket command ------------------------------------------------
	wlan_socket = (NCP_CMD_SOCKET_BIND_CFG*)(&(ncp_command->params.wlan_socket_bind));
	memset(wlan_socket, 0, sizeof(NCP_CMD_SOCKET_BIND_CFG));

	wlan_socket->handle = handle;
	wlan_socket->port = port;
	memcpy(wlan_socket->ip_addr, ip, strlen((char*)ip));

	header->size += sizeof(NCP_CMD_SOCKET_BIND_CFG);
	// -----------------------------------------------------------------------

	ncp_send((uint8_t*)&g_transfer_buff_send, header->size);
	// -------------------------------------------------
}

void ncp_wlan_socket_close(uint8_t handle)
{
	NCPCmd_DS_COMMAND 		 *ncp_command 	= NULL;
	NCP_COMMAND 			 *header 		= NULL;
	NCP_CMD_SOCKET_CLOSE_CFG *wlan_socket 	= NULL;

	// Prepare command pointer -----------------------------------------------
	ncp_command = (NCPCmd_DS_COMMAND*)g_transfer_buff_send;
	// -----------------------------------------------------------------------

	// Prepare header --------------------------------------------------------
	header = (NCP_COMMAND*)(&(ncp_command->header));

	memset(header, 0, sizeof(NCP_COMMAND));
	header->cmd 	= NCP_CMD_SOCKET_CLOSE;
	header->result 	= NCP_CMD_RESULT_OK;
	header->seqnum 	= 1;
	header->size 	= sizeof(NCP_COMMAND);
	// -----------------------------------------------------------------------

	// Prepare socket command ------------------------------------------------
	wlan_socket = (NCP_CMD_SOCKET_CLOSE_CFG*)(&(ncp_command->params.wlan_socket_close));
	memset(wlan_socket, 0, sizeof(NCP_CMD_SOCKET_CLOSE_CFG));

	wlan_socket->handle = handle;

	header->size += sizeof(NCP_CMD_SOCKET_CLOSE_CFG);
	// -----------------------------------------------------------------------

	ncp_send((uint8_t*)&g_transfer_buff_send, header->size);
	// -------------------------------------------------
}

void ncp_wlan_socket_listen(uint8_t handle, uint8_t number)
{
	NCPCmd_DS_COMMAND 		 *ncp_command 	= NULL;
	NCP_COMMAND 			 *header 		= NULL;
	NCP_CMD_SOCKET_LISTEN_CFG *wlan_socket 	= NULL;

	// Prepare command pointer -----------------------------------------------
	ncp_command = (NCPCmd_DS_COMMAND*)g_transfer_buff_send;
	// -----------------------------------------------------------------------

	// Prepare header --------------------------------------------------------
	header = (NCP_COMMAND*)(&(ncp_command->header));

	memset(header, 0, sizeof(NCP_COMMAND));
	header->cmd 	= NCP_CMD_SOCKET_LISTEN;
	header->result 	= NCP_CMD_RESULT_OK;
	header->seqnum 	= 1;
	header->size 	= sizeof(NCP_COMMAND);
	// -----------------------------------------------------------------------

	// Prepare socket command ------------------------------------------------
	wlan_socket = (NCP_CMD_SOCKET_LISTEN_CFG*)(&(ncp_command->params.wlan_socket_listen));
	memset(wlan_socket, 0, sizeof(NCP_CMD_SOCKET_LISTEN_CFG));

	wlan_socket->handle = handle;
	wlan_socket->number = number;

	header->size += sizeof(NCP_CMD_SOCKET_LISTEN_CFG);
	// -----------------------------------------------------------------------

	ncp_send((uint8_t*)&g_transfer_buff_send, header->size);
	// -------------------------------------------------
}

void ncp_wlan_socket_accept(uint8_t handle, int32_t accept)
{
	NCPCmd_DS_COMMAND 		 *ncp_command 	= NULL;
	NCP_COMMAND 			 *header 		= NULL;
	NCP_CMD_SOCKET_ACCEPT_CFG *wlan_socket 	= NULL;

	// Prepare command pointer -----------------------------------------------
	ncp_command = (NCPCmd_DS_COMMAND*)g_transfer_buff_send;
	// -----------------------------------------------------------------------

	// Prepare header --------------------------------------------------------
	header = (NCP_COMMAND*)(&(ncp_command->header));

	memset(header, 0, sizeof(NCP_COMMAND));
	header->cmd 	= NCP_CMD_SOCKET_ACCEPT;
	header->result 	= NCP_CMD_RESULT_OK;
	header->seqnum 	= 1;
	header->size 	= sizeof(NCP_COMMAND);
	// -----------------------------------------------------------------------

	// Prepare socket command ------------------------------------------------
	wlan_socket = (NCP_CMD_SOCKET_ACCEPT_CFG*)(&(ncp_command->params.wlan_socket_accept));
	memset(wlan_socket, 0, sizeof(NCP_CMD_SOCKET_ACCEPT_CFG));

	wlan_socket->handle = handle;
	wlan_socket->accepted_handle = accept;

	header->size += sizeof(NCP_CMD_SOCKET_ACCEPT_CFG);
	// -----------------------------------------------------------------------

	ncp_send((uint8_t*)&g_transfer_buff_send, header->size);
	// -------------------------------------------------
}

void ncp_wlan_socket_send(uint8_t handle, uint8_t *send_buff, uint8_t send_buff_size)
{
	NCPCmd_DS_COMMAND 		 *ncp_command 	= NULL;
	NCP_COMMAND 			 *header 		= NULL;
	NCP_CMD_SOCKET_SEND_CFG *wlan_socket 	= NULL;

	// Prepare command pointer -----------------------------------------------
	ncp_command = (NCPCmd_DS_COMMAND*)g_transfer_buff_send;
	// -----------------------------------------------------------------------

	// Prepare header --------------------------------------------------------
	header = (NCP_COMMAND*)(&(ncp_command->header));

	memset(header, 0, sizeof(NCP_COMMAND));
	header->cmd 	= NCP_CMD_SOCKET_SEND;
	header->result 	= NCP_CMD_RESULT_OK;
	header->seqnum 	= 1;
	header->size 	= sizeof(NCP_COMMAND);
	// -----------------------------------------------------------------------

	// Prepare socket command ------------------------------------------------
	wlan_socket = (NCP_CMD_SOCKET_SEND_CFG*)(&(ncp_command->params.wlan_socket_send));
	memset(wlan_socket, 0, sizeof(NCP_CMD_SOCKET_SEND_CFG));

	wlan_socket->handle = handle;
	memcpy((uint8_t*)wlan_socket->send_data, send_buff, send_buff_size);
	wlan_socket->size = send_buff_size;

	header->size += sizeof(NCP_CMD_SOCKET_SEND_CFG) - sizeof(wlan_socket->send_data) + send_buff_size;
	// -----------------------------------------------------------------------

	ncp_send((uint8_t*)&g_transfer_buff_send, header->size);
	// -------------------------------------------------
}

void ncp_wlan_socket_sendto(uint8_t handle, uint8_t *send_buff, uint8_t send_buff_size, uint8_t *ip, uint32_t port)
{
	NCPCmd_DS_COMMAND 		 *ncp_command 	= NULL;
	NCP_COMMAND 			 *header 		= NULL;
	NCP_CMD_SOCKET_SENDTO_CFG *wlan_socket 	= NULL;

	// Prepare command pointer -----------------------------------------------
	ncp_command = (NCPCmd_DS_COMMAND*)g_transfer_buff_send;
	// -----------------------------------------------------------------------

	// Prepare header --------------------------------------------------------
	header = (NCP_COMMAND*)(&(ncp_command->header));

	memset(header, 0, sizeof(NCP_COMMAND));
	header->cmd 	= NCP_CMD_SOCKET_SENDTO;
	header->result 	= NCP_CMD_RESULT_OK;
	header->seqnum 	= 1;
	header->size 	= sizeof(NCP_COMMAND);
	// -----------------------------------------------------------------------

	// Prepare socket command ------------------------------------------------
	wlan_socket = (NCP_CMD_SOCKET_SENDTO_CFG*)(&(ncp_command->params.wlan_socket_sendto));
	memset(wlan_socket, 0, sizeof(NCP_CMD_SOCKET_SENDTO_CFG));

	wlan_socket->handle = handle;
	wlan_socket->port = port;
	memcpy((uint8_t*)wlan_socket->ip_addr, ip, strlen((char*)ip));

	memcpy((uint8_t*)wlan_socket->send_data, send_buff, send_buff_size);
	wlan_socket->size = send_buff_size;

	header->size += sizeof(NCP_CMD_SOCKET_SENDTO_CFG) - sizeof(wlan_socket->send_data) + send_buff_size;
	// -----------------------------------------------------------------------

	ncp_send((uint8_t*)&g_transfer_buff_send, header->size);
	// -------------------------------------------------
}

void ncp_wlan_socket_receive(uint8_t handle, uint32_t size, uint32_t timeout)
{
	NCPCmd_DS_COMMAND 		 *ncp_command 	= NULL;
	NCP_COMMAND 			 *header 		= NULL;
	NCP_CMD_SOCKET_RECEIVE_CFG *wlan_socket 	= NULL;

	// Prepare command pointer -----------------------------------------------
	ncp_command = (NCPCmd_DS_COMMAND*)g_transfer_buff_send;
	// -----------------------------------------------------------------------

	// Prepare header --------------------------------------------------------
	header = (NCP_COMMAND*)(&(ncp_command->header));

	memset(header, 0, sizeof(NCP_COMMAND));
	header->cmd 	= NCP_CMD_SOCKET_RECV;
	header->result 	= NCP_CMD_RESULT_OK;
	header->seqnum 	= 1;
	header->size 	= sizeof(NCP_COMMAND);
	// -----------------------------------------------------------------------

	// Prepare socket command ------------------------------------------------
	wlan_socket = (NCP_CMD_SOCKET_RECEIVE_CFG*)(&(ncp_command->params.wlan_socket_receive));
	memset(wlan_socket, 0, sizeof(NCP_CMD_SOCKET_RECEIVE_CFG));

	wlan_socket->handle = handle;
	wlan_socket->recv_size = size;
	wlan_socket->timeout = timeout;

	header->size += sizeof(NCP_CMD_SOCKET_RECEIVE_CFG);
	// -----------------------------------------------------------------------

	ncp_send((uint8_t*)&g_transfer_buff_send, header->size);
	// -------------------------------------------------
}

void ncp_wlan_socket_recvfrom(uint8_t handle, uint32_t size, uint32_t timeout, uint8_t *ip, uint32_t port)
{
	NCPCmd_DS_COMMAND 		 *ncp_command 	= NULL;
	NCP_COMMAND 			 *header 		= NULL;
	NCP_CMD_SOCKET_RECVFROM_CFG *wlan_socket 	= NULL;

	// Prepare command pointer -----------------------------------------------
	ncp_command = (NCPCmd_DS_COMMAND*)g_transfer_buff_send;
	// -----------------------------------------------------------------------

	// Prepare header --------------------------------------------------------
	header = (NCP_COMMAND*)(&(ncp_command->header));

	memset(header, 0, sizeof(NCP_COMMAND));
	header->cmd 	= NCP_CMD_SOCKET_RECV;
	header->result 	= NCP_CMD_RESULT_OK;
	header->seqnum 	= 1;
	header->size 	= sizeof(NCP_COMMAND);
	// -----------------------------------------------------------------------

	// Prepare socket command ------------------------------------------------
	wlan_socket = (NCP_CMD_SOCKET_RECVFROM_CFG*)(&(ncp_command->params.wlan_socket_recvfrom));
	memset(wlan_socket, 0, sizeof(NCP_CMD_SOCKET_RECVFROM_CFG));

	wlan_socket->handle = handle;
	wlan_socket->recv_size = size;
	wlan_socket->timeout = timeout;
	wlan_socket->peer_port = port;
	memcpy((uint8_t*)wlan_socket->peer_ip, ip, strlen((char*)ip));

	header->size += sizeof(NCP_CMD_SOCKET_RECVFROM_CFG);
	// -----------------------------------------------------------------------

	ncp_send((uint8_t*)&g_transfer_buff_send, header->size);
	// -------------------------------------------------
}

void ncp_wlan_uap_provider_start(void)
{
	NCPCmd_DS_COMMAND 		 	*ncp_command 	= NULL;
	NCP_COMMAND 			 	*header 		= NULL;

	// Prepare command pointer -----------------------------------------------
	ncp_command = (NCPCmd_DS_COMMAND*)g_transfer_buff_send;
	// -----------------------------------------------------------------------

	// Prepare header --------------------------------------------------------
	header = (NCP_COMMAND*)(&(ncp_command->header));

	memset(header, 0, sizeof(NCP_COMMAND));
	header->cmd 	= NCP_CMD_WLAN_BASIC_WLAN_UAP_PROV_START;
	header->result 	= NCP_CMD_RESULT_OK;
	header->seqnum 	= 1;
	header->size 	= sizeof(NCP_COMMAND);
	// -----------------------------------------------------------------------

	ncp_send((uint8_t*)&g_transfer_buff_send, header->size);
	// -------------------------------------------------
}

void ncp_wlan_uap_provider_reset(void)
{
	NCPCmd_DS_COMMAND 		 	*ncp_command 	= NULL;
	NCP_COMMAND 			 	*header 		= NULL;

	// Prepare command pointer -----------------------------------------------
	ncp_command = (NCPCmd_DS_COMMAND*)g_transfer_buff_send;
	// -----------------------------------------------------------------------

	// Prepare header --------------------------------------------------------
	header = (NCP_COMMAND*)(&(ncp_command->header));

	memset(header, 0, sizeof(NCP_COMMAND));
	header->cmd 	= NCP_CMD_WLAN_BASIC_WLAN_UAP_PROV_RESET;
	header->result 	= NCP_CMD_RESULT_OK;
	header->seqnum 	= 1;
	header->size 	= sizeof(NCP_COMMAND);
	// -----------------------------------------------------------------------

	ncp_send((uint8_t*)&g_transfer_buff_send, header->size);
	// -------------------------------------------------
}

void ncp_wlan_uap_provider_set_config(uint8_t *ssid_array,     uint8_t ssid_len,
									  uint8_t *password_array, uint8_t password_len,
									  wlan_security_type_t security)
{
	NCPCmd_DS_COMMAND 		 	*ncp_command 	= NULL;
	NCP_COMMAND 			 	*header 		= NULL;
	NCP_CMD_UAP_PROV_SET_UAPCFG *uap_config	    = NULL;

	// Prepare command pointer -----------------------------------------------
	ncp_command = (NCPCmd_DS_COMMAND*)g_transfer_buff_send;
	// -----------------------------------------------------------------------

	// Prepare header --------------------------------------------------------
	header = (NCP_COMMAND*)(&(ncp_command->header));

	memset(header, 0, sizeof(NCP_COMMAND));
	header->cmd 	= NCP_CMD_WLAN_BASIC_WLAN_UAP_PROV_SET_UAPCFG;
	header->result 	= NCP_CMD_RESULT_OK;
	header->seqnum 	= 1;
	header->size 	= sizeof(NCP_COMMAND);
	// -----------------------------------------------------------------------

	// Prepare uap prov config -----------------------------------------------
	uap_config = &(ncp_command->params.prov_set_uapcfg);
	memset(uap_config, 0, sizeof(NCP_CMD_UAP_PROV_SET_UAPCFG));

	memcpy((uint8_t*)uap_config->ssid, ssid_array, ssid_len);
	memcpy((uint8_t*)uap_config->uapPass, password_array, password_len);

	uap_config->security_type = (uint32_t)security;

	header->size += sizeof(NCP_CMD_UAP_PROV_SET_UAPCFG);
	// -----------------------------------------------------------------------

	ncp_send((uint8_t*)&g_transfer_buff_send, header->size);
	// -------------------------------------------------
}
// -----------------------------------------------------------------------------------------------------------------
