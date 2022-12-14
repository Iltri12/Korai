/**
  @page Zigbee_DevTemp_Client_Router application
  
  @verbatim
  ******************************************************************************
  * @file    Zigbee/Zigbee_DevTemp_Client_Router/readme.txt 
  * @author  MCD Application Team
  * @brief   Description of a Zigbee Device Temperature Cluster application on 
  *          the device acting as a Client with Router role on a Centralized 
  *          Zigbee network.
  ******************************************************************************
  *
  * Copyright (c) 2020-2021 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  @endverbatim

@par Application Description 

How to use the Device Temperature cluster on a device acting as a Client with Router role within a Centralized Zigbee network.

The purpose of this application is to show how to create a centralized Zigbee network, and how to communicate
from one node to another one using the DevTemp cluster. Once the Zigbee mesh network is created, the 
user can send requests to the server boards through the push buttons in order to have current temperature,
min temperature experienced, max temperature experienced attributes value.

Each device acts as a client. 


              Device 1                                      Device 2
        
             ---------                                      ---------
             |       |         ZbZclReadReq                 |       |
PushB (SW1)=>|Client | -----------------------------------> |Server | <= PushB (SW1) ---> Increase Temp +2C ->ZbZclAttrIntegerWrite
             |       |                                      |       | <= PushB (SW2) ---> Decrease Temp -2C ->ZbZclAttrIntegerWrite
             |       |                                      |       |
             |       |         ZbZclReadRsp                 |       |=>LED Green
   Display <=|       | <----------------------------------- |       |=>LED Red
   Temp info |       |                                      |       |=> Display attributes written
              --------                                      ---------


This application requires at minimum two STM32WB55xx nucleo boards, but you can use up four boards. 

This application requests having the stm32wb5x_Zigbee_FFD_fw.bin binary flashed on the wireless coprocessor 
and the correct binary flashed on the application processor.

Important note :
================ 
As it is a centralized network, to run this application, you need to have one device acting 
as coordinator and all the others devices acting as routers.

One board must be flashed with the binary generated by the application located under Zigbee/Zigbee_DevTemp_Server_Coord.
The other boards must be flashed with the binary generated by this application (Zigbee/Zigbee_DevTemp_Client_Router)

To run the application :

  a)  Once the boards have been flashed, eboot the different devices one by one. 
      There must be one board configured in coordinator mode, and all 
      other boards should be configured in router mode.

  b)  Wait for a few seconds (around 5 sec), in order to let the zigbee mesh network being created. 
        
  c)  At this stage, a Zigbee network is automatically created and it is possible to request DevTempCluster attributes
      value in unicast mode by pressing on the SW1 push button. 
      Client traces the current temperature, the min and max temperature experienced values .

 
              ----------             -----------
              |Device 1 |............|Device 2 |           
              |(Coord.) |            |(Router) |
              -----------            -----------
                  .                       .
                  .                       .
                  .                       .
             -----------                  .
             |Device 3 |...................      
             |(Router) |
             -----------

@par Keywords

Zigbee
                   .                       
@par Hardware and Software environment

  - This example runs on STM32WB55xx devices (Nucleo board) 
  
  - This example has been tested with an STMicroelectronics STM32WB55RG_Nucleo 
    board and can be easily tailored to any other supported device 
    and development board.
    
  - On STM32WB55RG_Nucleo, the jumpers must be configured as described
    in this section. Starting from the top left position up to the bottom 
    right position, the jumpers on the Board must be set as follows: 

     CN11:    GND         [OFF]
     JP4:     VDDRF       [ON]
     JP6:     VC0         [ON]
     JP2:     +3V3        [ON] 
     JP1:     USB_STL     [ON]   All others [OFF]
     CN12:    GND         [OFF]
     CN7:     <All>       [OFF]
     JP3:     VDD_MCU     [ON]
     JP5:     GND         [OFF]  All others [ON]
     CN10:    <All>       [OFF]


@par How to use it ? 

=> Loading of the stm32wb5x_Zigbee_FFD_fw.bin binary

  This application requests having the stm32wb5x_Zigbee_FFD_fw.bin binary flashed on the Wireless Coprocessor.
  If it is not the case, you need to use STM32CubeProgrammer to load the appropriate binary.
  All available binaries are located under /Projects/STM32_Copro_Wireless_Binaries directory.
  Refer to UM2237 to learn how to use/install STM32CubeProgrammer.
  Refer to /Projects/STM32_Copro_Wireless_Binaries/ReleaseNote.html for the detailed procedure to change the
  Wireless Coprocessor binary. 

=> Getting traces
  To get the traces you need to connect your Board to the Hyperterminal (through the STLink Virtual COM Port).
  The UART must be configured as follows:

    - BaudRate = 115200 baud  
    - Word Length = 8 Bits 
    - Stop Bit = 1 bit
    - Parity = none
    - Flow control = none

=> Running the application

  Refer to the Application description at the beginning of this readme.txt

 * <h3><center>&copy; COPYRIGHT STMicroelectronics</center></h3>

