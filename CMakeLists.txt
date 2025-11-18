
# Copyright 2025 NXP
#
# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.22.0)

include(${SdkRootDirPath}/cmake/extension/mcux.cmake)

project(mc_pmsm_enc LANGUAGES C CXX ASM)

include(${SdkRootDirPath}/CMakeLists.txt)

if(CMAKE_C_COMPILER_ID STREQUAL "GNU" OR CMAKE_C_COMPILER_ID MATCHES "Clang")
  target_compile_options(mc_pmsm_enc PRIVATE -Wno-error=cpp)
endif()

mcux_add_source(
    SOURCES frdmmcxa156/clock_config.c
            frdmmcxa156/clock_config.h
            pmsm_enc/m1_pmsm_appconfig.h
            source/main.c
            pmsm_enc/mc_periph_init.c
            pmsm_enc/mc_periph_init.h
            frdmmcxa156/pin_mux.c
            frdmmcxa156/pin_mux.h
            motor_control/motor_control.c
            motor_control/motor_control.h
            frdmmcxa156/FreeRTOSConfig_Gen.h
            frdmmcxa156/hardware_init.c
            source/app.h
            ncp/ncp_ble_api/ncp_ble_api.h
            ncp/ncp_ble_api/ncp_ble_api.c
            ncp/ncp_ble_api/ncp_cmd_ble.h
            ncp/ncp_ble_api/service.h
            ncp/ncp_wifi_api/checksum_generator.h
            ncp/ncp_wifi_api/checksum_generator.c
            ncp/ncp_wifi_api/ncp_cmd_wifi.h
            ncp/ncp_wifi_api/ncp_host_config.h
            ncp/ncp_wifi_api/ncp_wifi_api.h
            ncp/ncp_wifi_api/ncp_wifi_api.c
            ncp/commands.h
            ncp/ncp_common.h
            ncp/ncp_common.c
)

mcux_add_include(
    INCLUDES pmsm_enc
             motor_control
             frdmmcxa156
             source
             ncp
             ncp/ncp_ble_api
             ncp/ncp_wifi_api
)
mcux_add_iar_configuration(
    CC "--diag_suppress Pa082,Pa050,Ta021,Ta022,Ta023"
)

mcux_add_macro(
    CC "-DMC_EXAMPLE"
)

include(${SdkRootDirPath}/${board_root}/${board}/demo_apps/mc_pmsm/pmsm_enc/${multicore_foldername}/reconfig.cmake OPTIONAL)
include(${SdkRootDirPath}/${board_root}/${board}/freertos_examples/freertos_sem/reconfig.cmake OPTIONAL)

mcux_convert_binary(BINARY ${APPLICATION_BINARY_DIR}/${MCUX_SDK_PROJECT_NAME}.bin)

