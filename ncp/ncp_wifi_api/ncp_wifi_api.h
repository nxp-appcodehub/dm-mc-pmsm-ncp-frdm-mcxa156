/*
 * Copyright 2025 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef NCP_WIFI_AHI_H_
#define NCP_WIFI_AHI_H_

#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include "checksum_generator.h"
#include "commands.h"
#include "ncp_cmd_wifi.h"


/* -----------------------------------------------------
  	  	  	       AVAILABLE NCP FEATURES
  	  	  	  	  	- wlan-scan
  	  	  	  	  	- wlan-add
  	  	  	  	  		- WPA2 security
  	  	  	  	  		- Without security
                    - wlan-remove
  	  	  	  	  	- wlan-connect
  	  	  	  	  	- wlan-disconnect
  	  	  	  	  	- wlan-version
  	  	  	  	  	- wlan-stat
   ----------------------------------------------------- */

typedef enum _wlan_security_type_t
{
	k_wlan_security_none,
	k_wlan_security_wep_open,
	k_wlan_security_wep_shared,
	k_wlan_security_wpa,
	k_wlan_security_wpa2,
	k_wlan_security_wpa2_mixed,
	k_wlan_security_wpa2_ft,
	k_wlan_security_wpa3_sae,
} wlan_security_type_t;

typedef enum _socket_protocol_t
{
	k_socket_tcp,
	k_socket_udp,
	k_socket_raw
} socket_protocol_t;


void ncp_wifi_api_init(void);

// Command functions -----------------------------------------------------------------------------------------------
void ncp_wlan_scan_command(void);
void ncp_wlan_add_command(uint8_t *network_name_array, uint8_t network_name_len,
						  uint8_t *ssid_array,         uint8_t ssid_len,
						  uint8_t *password_array,     uint8_t password_len,
						  wlan_security_type_t security);
void ncp_wlan_remove_command(uint8_t *network_name_array, uint8_t network_name_len);
void ncp_wlan_connect_command(uint8_t *network_name_array, uint8_t network_name_len);
void ncp_wlan_disconnect_command(void);
void ncp_wlan_version_command(void);
void ncp_wlan_stat_command(void);
void ncp_wlan_info_command(void);

// UAP ------------------------------------------------------------------------------------
void ncp_wlan_uap_provider_start(void);
void ncp_wlan_uap_provider_reset(void);
void ncp_wlan_uap_provider_set_config(uint8_t *ssid_array,     uint8_t ssid_len,
									  uint8_t *password_array, uint8_t password_len,
									  wlan_security_type_t security);
// ----------------------------------------------------------------------------------------

void ncp_wlan_socket_open(socket_protocol_t protocol);
void ncp_wlan_socket_connect(uint8_t handle , uint8_t *ip, uint32_t port);
void ncp_wlan_socket_bind(uint8_t handle , uint8_t *ip, uint32_t port);
void ncp_wlan_socket_close(uint8_t handle);
void ncp_wlan_socket_listen(uint8_t handle, uint8_t number);
void ncp_wlan_socket_accept(uint8_t handle, int32_t accept);
void ncp_wlan_socket_send(uint8_t handle, uint8_t *send_buff, uint8_t send_buff_size);
void ncp_wlan_socket_sendto(uint8_t handle, uint8_t *send_buff, uint8_t send_buff_size, uint8_t *ip, uint32_t port);
void ncp_wlan_socket_receive(uint8_t handle, uint32_t size, uint32_t timeout);
void ncp_wlan_socket_recvfrom(uint8_t handle, uint32_t size, uint32_t timeout, uint8_t *ip, uint32_t port);
// -----------------------------------------------------------------------------------------------------------------

#endif /* NCP_WIFI_AHI_H_ */
