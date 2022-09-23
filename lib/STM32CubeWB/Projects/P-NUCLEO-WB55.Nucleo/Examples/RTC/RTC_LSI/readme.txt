/**
  @page RTC_LSI RTC prescaler adjustment with LSI Measurements example
  
  @verbatim
  ******************************************************************************
  * @file    RTC/RTC_LSI/readme.txt 
  * @author  MCD Application Team
  * @brief   Description of the RTC prescaler adjustment with LSI Measurements example.
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

Use of the LSI clock source autocalibration to get a precise RTC clock. 
In the associated software, the system clock is set to 64 MHz.
As an application example, it demonstrates how to configure the TIM16 timer
internally connected to LSI clock output, in order to adjust the RTC prescaler. 

The Low Speed Internal (LSI) clock is used as RTC clock source.
After reset, the RTC prescaler is set with the default LSI frequency (32 kHz). 
The RTC WakeUp is configured to generate an interrupt each 1s.

A key value is written in backup data register 1 to indicate if the wakeup timer has
already been enabled. If so the wakeup timer is disabled prior configuring the RTC 
automatic wakeup.

LED2 is toggled inside the RTC WakeUp interrupt each 1s.

The inaccuracy of the LSI clock causes the RTC WakeUp Interrupt to be inaccurate.
The RTC CK_SPRE signal can be monitored by LED2 which is toggled into the RTC 
Wakeup interrupt service routine.

The program waits until User push-button (SW1) is pressed to begin the auto calibration.

Procedure:
 - Configure the TIM16 to remap internally the TIM16 TIM_CHANNEL_1
   Input Capture to the LSI clock output.
 - Enable the TIM16 Input Capture interrupt: after one cycle of LSI clock, the
   period value is stored in a variable and compared to the HCLK clock to get
   its real value.
 - The RTC prescaler is adjusted with this LSI frequency value so that the RTC
   CK_SPRE value become more accurate.
 - When calibration is done the variable uwLsiFreq is visualized into the debugger
   to indicate the end of this operation.
 - The LSI value is updated and displayed on the debugger in uwLsiFreq variable.

- LED3 is On: This indicates that the system generates an error.

@note Care must be taken when using HAL_Delay(), this function provides accurate
      delay (in milliseconds) based on variable incremented in SysTick ISR. This
      implies that if HAL_Delay() is called from a peripheral ISR process, then 
      the SysTick interrupt must have higher priority (numerically lower)
      than the peripheral interrupt. Otherwise the caller ISR process will be blocked.
      To change the SysTick interrupt priority you have to use HAL_NVIC_SetPriority() function.
      
@note The application need to ensure that the SysTick time base is always set to 1 millisecond
      to have correct HAL operation.

@par Keywords

System, RTC, LSI, Internal time stamp, Wake up, interrupt

@par Directory contents  

  - RTC/RTC_LSI/Inc/stm32wbxx_hal_conf.h     HAL configuration file
  - RTC/RTC_LSI/Inc/main.h                   Header file for main.c
  - RTC/RTC_LSI/Inc/stm32wbxx_it.h           Header for stm32wbxx_it.c
  - RTC/RTC_LSI/Src/system_stm32wbxx.c       STM32WBxx system clock configuration file
  - RTC/RTC_LSI/Src/stm32wbxx_it.c           Interrupt handlers
  - RTC/RTC_LSI/Src/main.c                   Main program
  - RTC/RTC_LSI/Src/stm32wbxx_hal_msp.c      HAL MSP module

@par Hardware and Software environment

  - This example runs on STM32WB55xx devices.
    
  - This example has been tested with P-NUCLEO-WB55  board and can be
    easily tailored to any other supported device and development board.

  - P-NUCLEO-WB55 Set-up
    - Use LED2 connected to PB.00 pin (pin 22 in CN10 connector).
    - Use the User push-button (SW1) connected to pin PC.04 (External line 4)

@par How to use it ? 

In order to make the program work, you must do the following :
 - Open your preferred toolchain 
 - Rebuild all files and load your image into target memory
 - Run the example

 * <h3><center>&copy; COPYRIGHT STMicroelectronics</center></h3>
 */
