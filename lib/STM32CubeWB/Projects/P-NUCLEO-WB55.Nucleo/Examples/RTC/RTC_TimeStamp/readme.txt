/**
  @page RTC_TimeStamp RTC time stamp Example
  
  @verbatim
  ******************************************************************************
  * @file    RTC/RTC_TimeStamp/readme.txt 
  * @author  MCD Application Team
  * @brief   Description of the RTC time stamp example.
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
Configuration of the RTC HAL API to demonstrate the timestamp feature.

At the beginning of the main program the HAL_Init() function is called to reset 
all the peripherals, initialize the Flash interface and the systick.
Then the SystemClock_Config() function is used to configure the system
clock (SYSCLK) to run at 64 MHz.

The RTC peripheral configuration is ensured by the HAL_RTC_Init() function.
This later is calling the HAL_RTC_MspInit()function which core is implementing
the configuration of the needed RTC resources according to the used hardware (CLOCK, 
PWR, RTC clock source and BackUp). You may update this function to change RTC configuration.

@note LSI oscillator clock is used as RTC clock source by default.
      The user can use also LSE as RTC clock source. The user uncomment the adequate 
      line on the main.h file.
      @code
        #define RTC_CLOCK_SOURCE_LSI  
        /* #define RTC_CLOCK_SOURCE_LSE */
      @endcode
      LSI oscillator clock is delivered by a 32 kHz RC.
      LSE (when available on board) is delivered by a 32.768 kHz crystal.

HAL_RTCEx_SetTimeStamp_IT()function is then called to initialize the time stamp feature 
with interrupt mode. It configures the time stamp pin to be rising edge and enables
the time stamp detection on time stamp pin.
HAL_RTC_SetTime()and HAL_RTC_SetDate() functions are then called to initialize the 
time and the date.

The associated firmware performs the following:
1. After startup the program configure the RTC (Time date) and enable the feature 
   timeStamp.
   
2. Apply a low level on RTC time stamp pin PC13,
     (Note: On board STM32WB Nucleo68, User push-button (SW1) can be used to control PC13 with a specific configuration: solder bridge SB48 must be closed.
     Alternate solution: connect a wire between PC4 (pin connected to User push-button (SW1) by default) and PC13)
   a time stamp event is detected and the calendar is saved in the time stamp structures.
   Each time the user presses on the User push-button (SW1), the current time and time stamp are updated and displayed
   on the debugger in aShowTime and aShowTimeStamp variables.

- LED3 is toggling : This indicates that the system generates an error.
       
@note Care must be taken when using HAL_Delay(), this function provides accurate delay (in milliseconds)
      based on variable incremented in SysTick ISR. This implies that if HAL_Delay() is called from
      a peripheral ISR process, then the SysTick interrupt must have higher priority (numerically lower)
      than the peripheral interrupt. Otherwise the caller ISR process will be blocked.
      To change the SysTick interrupt priority you have to use HAL_NVIC_SetPriority() function.
      
@note The application need to ensure that the SysTick time base is always set to 1 millisecond
      to have correct HAL operation.

@par Keywords

RTC, Timer, Timestamp, Counter, LSE, LSI, Current time, Real Time Clock

@par Directory contents 

  - RTC/RTC_TimeStamp/Inc/stm32wbxx_hal_conf.h    HAL configuration file
  - RTC/RTC_TimeStamp/Inc/stm32wbxx_it.h          Interrupt handlers header file
  - RTC/RTC_TimeStamp/Inc/main.h                  Header for main.c module  
  - RTC/RTC_TimeStamp/Src/stm32wbxx_it.c          Interrupt handlers
  - RTC/RTC_TimeStamp/Src/main.c                  Main program
  - RTC/RTC_TimeStamp/Src/stm32wbxx_hal_msp.c     HAL MSP module
  - RTC/RTC_TimeStamp/Src/system_stm32wbxx.c      STM32WBxx system source file


@par Hardware and Software environment

  - This example runs on STM32WB55xx devices.
  - This example has been tested with STMicroelectronics P-NUCLEO-WB55 
    board and can be easily tailored to any other supported device 
    and development board.


@par How to use it ? 

In order to make the program work, you must do the following :
 - Open your preferred toolchain 
 - Rebuild all files and load your image into target memory
 - Run the example


 * <h3><center>&copy; COPYRIGHT STMicroelectronics</center></h3>
 */
 
