/**
  @page I2C_WakeUpFromStop2 I2C Two Boards Communication IT Example on Stop 2 Mode
  
  @verbatim
  ******************************************************************************
  * @file    I2C/I2C_WakeUpFromStop2/readme.txt 
  * @author  MCD Application Team
  * @brief   Description of the Wake Up from Stop 2 mode example
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

@par Example Description 

How to handle I2C data buffer transmission/reception between two boards, 
using an interrupt when the device is in Stop 2 mode.

Board: P-NUCLEO-WB55 (embeds a STM32WB55RG device)
SCL Pin: PC0 (CN7, pin28)
SDA Pin: PC1 (CN7, pin30)

   _________________________                       _________________________ 
  |           ______________|                     |______________           |
  |          |I2C3          |                     |          I2C3|          |
  |          |              |                     |              |          |
  |          |          SCL |_____________________| SCL          |          |
  |          |              |                     |              |          |
  |          |              |                     |              |          |
  |          |              |                     |              |          |
  |          |          SDA |_____________________| SDA          |          |
  |          |              |                     |              |          |
  |          |______________|                     |______________|          |
  |                         |                     |                         |
  |                      GND|_____________________|GND                      |
  |_STM32_Board 1___________|                     |_STM32_Board 2___________|

At the beginning of the main program the HAL_Init() function is called to reset 
all the peripherals, initialize the Flash interface and the systick.
Then the SystemClock_Config() function is used to configure the system
clock (SYSCLK) to run at 16 Mhz.

The I2C peripheral configuration is ensured by the HAL_I2C_Init() function.
This later is calling the HAL_I2C_MspInit()function which core is implementing
the configuration of the needed I2C resources according to the used hardware (CLOCK, 
GPIO and NVIC). You may update this function to change I2C configuration.

The I2C communication is then initiated.
The project is split in two parts the Master Board and the Slave Board
- Master Board
  The HAL_I2C_Master_Receive_IT() and the HAL_I2C_Master_Transmit_IT() functions 
  allow respectively the reception and the transmission of a predefined data buffer
  in Master mode using interrupt.
- Slave Board
  The HAL_I2C_Slave_Receive_IT() and the HAL_I2C_Slave_Transmit_IT() functions 
  allow respectively the reception and the transmission of a predefined data buffer
  in Slave mode using interrupt.
The user can choose between Master and Slave through "#define MASTER_BOARD"
in the "main.c" file:
If the Master board is used, the "#define MASTER_BOARD" must be uncommented.
If the Slave board is used the "#define MASTER_BOARD" must be commented.

For this example the aTxBuffer is predefined and the aRxBuffer size is same as aTxBuffer.

In a first step after the user presses the User push-button (SW1) on the Master Board, I2C Master
starts the communication by sending aTxBuffer through HAL_I2C_Master_Transmit_IT() to 
I2C Slave which wakes up from Stop 2 mode and receives aRxBuffer through HAL_I2C_Slave_Receive_IT(). 
The second step starts when the user presses the User push-button (SW1) on the Master Board,
the I2C Slave after wake up from Stop 2 mode at address match, sends aTxBuffer through HAL_I2C_Slave_Transmit_IT()
to the I2C Master which receives aRxBuffer through HAL_I2C_Master_Receive_IT().
The end of this two steps are monitored through the HAL_I2C_GetState() function
result.
Finally, aTxBuffer and aRxBuffer are compared through Buffercmp() in order to 
check buffers correctness.  

P-NUCLEO-WB55's LEDs can be used to monitor the transfer status on the Slave Board :
 - LED2 is ON when the Transmission process is complete.
 - LED2 is OFF when the Reception process is complete.
 - LED3 is ON when there is an error in transmission/reception process.
 - LED1 is ON when Slave enters Stop 2 mode .
 - LED1 is OFF when Slave wakes up from Stop 2.    

@note Care must be taken when using HAL_Delay(), this function provides accurate delay (in milliseconds)
      based on variable incremented in SysTick ISR. This implies that if HAL_Delay() is called from
      a peripheral ISR process, then the SysTick interrupt must have higher priority (numerically lower)
      than the peripheral interrupt. Otherwise the caller ISR process will be blocked.
      To change the SysTick interrupt priority you have to use HAL_NVIC_SetPriority() function.

@note The application need to ensure that the SysTick time base is always set to 1 millisecond
      to have correct HAL operation.

@par Keywords

Connectivity, I2C, Communication, Transmission, Reception, SCL, SDA, Wake up, Interrupt

@par Directory contents 

  - I2C/I2C_WakeUpFromStop2/Inc/stm32wbxx_hal_conf.h    HAL configuration file
  - I2C/I2C_WakeUpFromStop2/Inc/stm32wbxx_it.h          I2C interrupt handlers header file
  - I2C/I2C_WakeUpFromStop2/Inc/main.h                  Header for main.c module
  - I2C/I2C_WakeUpFromStop2/Src/stm32wbxx_it.c          I2C interrupt handlers
  - I2C/I2C_WakeUpFromStop2/Src/main.c                  Main program
  - I2C/I2C_WakeUpFromStop2/Src/system_stm32wbxx.c      STM32WBxx system source file
  - I2C/I2C_WakeUpFromStop2/Src/stm32wbxx_hal_msp.c     HAL MSP file

@par Hardware and Software environment

  - This example runs on STM32WB55xx devices.
    
  - This example has been tested with P-NUCLEO-WB55 board and can be
    easily tailored to any other supported device and development board.

  - P-NUCLEO-WB55 Set-up

    - Connect I2C_SCL line of Master board (PC0, CN7, pin28) to I2C_SCL line of Slave Board (PC0, CN7, pin28).
    - Connect I2C_SDA line of Master board (PC1, CN7, pin30) to I2C_SDA line of Slave Board (PC1, CN7, pin30).
    - Connect GND of Master board to GND of Slave Board.

@par How to use it ? 

In order to make the program work, you must do the following :
 - Open your preferred toolchain 
 - Rebuild all files and load your image into target memory
    o Uncomment "#define MASTER_BOARD" and load the project in Master Board
    o Comment "#define MASTER_BOARD" and load the project in Slave Board
 - Run the example

 * <h3><center>&copy; COPYRIGHT STMicroelectronics</center></h3>
 */
