/**
  @page BLE_p2pClient Application

  @verbatim
  ******************************************************************************
  * @file    BLE/BLE_p2pClient/readme.txt 
  * @author  MCD Application Team
  * @brief   Description of the BLE_p2pClient application
  ******************************************************************************
  *
  * Copyright (c) 2019-2021 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  @endverbatim

@par Application Description

How to demonstrate Point-to-Point communication using BLE component (as GATT client). 

Two STM32WB55xx boards are used, one acting as GATT client, and one as GATT server.
For example, BLE_p2pClient application is downloaded in a Nucleo board (MB1355C) and BLE P2P_Server application in a USB DONGLE board (MB1293C).


@par Keywords

Connectivity, BLE, IPCC, HSEM, RTC, UART, PWR, BLE protocol, BLE pairing, BLE profile, Dual core

@par Directory contents 
  
  - BLE/BLE_p2pClient/Core/Inc/stm32wbxx_hal_conf.h		HAL configuration file
  - BLE/BLE_p2pClient/Core/Inc/stm32wbxx_it.h          	Interrupt handlers header file
  - BLE/BLE_p2pClient/Core/Inc/main.h                  	Header for main.c module
  - BLE/BLE_p2pClient/STM32_WPAN/App/app_ble.h          Header for app_ble.c module
  - BLE/BLE_p2pClient/Core/Inc/app_common.h            	Header for all modules with common definition
  - BLE/BLE_p2pClient/Core/Inc/app_conf.h              	Parameters configuration file of the application
  - BLE/BLE_p2pClient/Core/Inc/app_entry.h            	Parameters configuration file of the application
  - BLE/BLE_p2pClient/STM32_WPAN/App/ble_conf.h         BLE Services configuration
  - BLE/BLE_p2pClient/STM32_WPAN/App/ble_dbg_conf.h     BLE Traces configuration of the BLE services
  - BLE/BLE_p2pClient/STM32_WPAN/App/p2p_client_app.h   Header for p2p_lcient_app.c module
  - BLE/BLE_p2pClient/Core/Inc/hw_conf.h           		Configuration file of the HW
  - BLE/BLE_p2pClient/Core/Inc/utilities_conf.h    		Configuration file of the utilities
  - BLE/BLE_p2pClient/Core/Src/stm32wbxx_it.c          	Interrupt handlers
  - BLE/BLE_p2pClient/Core/Src/main.c                  	Main program
  - BLE/BLE_p2pClient/Core/Src/system_stm32wbxx.c      	stm32wbxx system source file
  - BLE/BLE_p2pClient/STM32_WPAN/App/app_ble.c      	BLE Profile implementation
  - BLE/BLE_p2pClient/Core/Src/app_entry.c      		Initialization of the application
  - BLE/BLE_p2pClient/STM32_WPAN/App/p2p_client_app.c   P2P Client Application Implementation
  - BLE/BLE_p2pClient/STM32_WPAN/Target/hw_ipcc.c      	IPCC Driver
  - BLE/BLE_p2pClient/Core/Src/stm32_lpm_if.c			Low Power Manager Interface
  - BLE/BLE_p2pClient/Core/Src/hw_timerserver.c 		Timer Server based on RTC
  - BLE/BLE_p2pClient/Core/Src/hw_uart.c 				UART Driver
  
@par Hardware and Software environment

    - This application runs on STM32WB55xx Nucleo board (MB1355C)
    
    - Nucleo board (MB1355C) Set-up    
       - Connect that Board to your PC with a USB cable type A to mini-B to ST-LINK connector (USB_STLINK).
       - Please ensure that the ST-LINK connectors and jumpers are fitted.

@par How to use it ? 

This application requires having the stm32wb5x_BLE_Stack_full_fw.bin binary flashed on the Wireless Coprocessor.
If it is not the case, you need to use STM32CubeProgrammer to load the appropriate binary.
All available binaries are located under /Projects/STM32_Copro_Wireless_Binaries directory.
Refer to UM2237 to learn how to use/install STM32CubeProgrammer.
Refer to /Projects/STM32_Copro_Wireless_Binaries/ReleaseNote.html for the detailed procedure to change the
Wireless Coprocessor binary.  
   
In order to make the program work, you must do the following :
 - Open your preferred toolchain 
 - Rebuild all files and load the image into Target memory
 - OR use the BLE_p2pClient_reference.hex from Binary directory
 - This must be done for BLE_p2pServer (MB1293C) for example, and BLE_p2pClient (MB1355C).

 - BLE_p2pServer may be connected by BLE_p2pClient.
 - Once the code (BLE_p2pServer & BLE_p2pClient) is downloaded into the two STM32WB55xx boards and executed, the modules are initialized. 

 - The Peripheral device (BLE_p2pServer) starts advertising (during 1 minute), the green led blinks for each advertising event.
 - The Central device (BLE_p2pClient) starts scanning when pressing the User button (SW1) on the USB Dongle board. 
   - BLE_p2pClient blue led becomes on. 
   - Scan req takes about 5 seconds.
   - Make sure BLE_p2pServer advertises, if not press reset button or switch off/on to restart advertising.
 - Then, it automatically connects to the BLE_p2pServer. 
   - Blue led turns off and green led starts blinking as on the MB1355C. Connection is done.
 - When pressing SW1 on a board, the blue led toggles on the other one.
   - The SW1 button can be pressed independently on the GATT Client or on the GATT Server.
 - When the server is located on a MB1355C, the connection interval can be modified from 50ms to 1s and vice-versa using SW2. 
 - The green led on the 2 boards blinks for each advertising event, it means quickly when 50ms and slowly when 1s. 
 - Passing from 50ms to 1s is instantaneous, but from 1s to 50ms takes around 10 seconds.
 - The SW1 event, switch on/off blue led, depends on the connection Interval event. 
   - So the delay from SW1 action and blue led change is more or less fast.
  
For more details refer to the Application Note: 
  AN5289 - Building a Wireless application 
 
 * <h3><center>&copy; COPYRIGHT STMicroelectronics</center></h3>
 */
