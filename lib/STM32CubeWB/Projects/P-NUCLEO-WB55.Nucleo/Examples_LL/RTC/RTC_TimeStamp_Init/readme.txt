/**
  @page RTC_TimeStamp_Init RTC example
  
  @verbatim
  ******************************************************************************
  * @file    Examples_LL/RTC/RTC_TimeStamp_Init/readme.txt 
  * @author  MCD Application Team
  * @brief   Description of the RTC example.
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

Configuration of the Timestamp using the RTC LL API. The peripheral initialization 
uses LL unitary service functions for optimization purposes (performance and size).

In this example, after start-up, SYSCLK is configured to the max frequency using the PLL with
MSI as clock source.

The RTC peripheral configuration is ensured by the Configure_RTC() function 
(configure of the needed RTC resources according to the used hardware CLOCK, 
PWR, RTC clock source and BackUp). You may update this function to change RTC configuration.

@note LSE oscillator clock is used as RTC clock source by default.
      The user can use also LSI as RTC clock source. The user uncomment the adequate 
      line on the main.c file.
      @code
        #define RTC_CLOCK_SOURCE_LSE  
        /* #define RTC_CLOCK_SOURCE_LSI */
      @endcode
      LSI oscillator clock is delivered by a 32 kHz RC.
      LSE (when available on board) is delivered by a 32.768 kHz crystal.

Configure_RTC_TimeStamp() function is then called to initialize the time stamp feature 
with interrupt mode. It configures the time stamp pin to be rising edge and enables
the time stamp detection on time stamp pin.
LL_RTC_DATE_Config()and LL_RTC_TIME_Config() functions are then called to initialize the 
time and the date.

The associated firmware performs the following:
1. After start-up the program configure the RTC (Time date) and enable the feature 
   timeStamp. 

2. When applying a rising edge on the time stamp pin (PC.13), 
   a time stamp event is detected and the calendar is saved in the time stamp structures.
   The current date and time stamp are updated and displayed.
   Note: On this board, time stamp pin is not connected to User push-button (SW1) (pin PC.04).
         Therefore, an external connection is needed:
         - to generate a time stamp event using User push-button (SW1): connect PC.13 to PC.04.
         - to generate a time stamp event using external signal: connect PC.13 to voltage level 0V or 3.3V.
   on the debugger (live watch) in aShowTimeStamp and aShowDateStamp variables .
   The current time and date are updated and displayed on the debugger (live watch) in aShowTime, aShowDate variables .

- LED2 is toggling : This indicates that the system generates an error.
@par Keywords

RTC, Timer, Timestamp, Counter, LSE, LSI, Current time, Real Time Clock

@par Directory contents 

  - RTC/RTC_TimeStamp_Init/Inc/stm32wbxx_it.h          Interrupt handlers header file
  - RTC/RTC_TimeStamp_Init/Inc/main.h                  Header for main.c module
  - RTC/RTC_TimeStamp_Init/Inc/stm32_assert.h          Template file to include assert_failed function
  - RTC/RTC_TimeStamp_Init/Src/stm32wbxx_it.c          Interrupt handlers
  - RTC/RTC_TimeStamp_Init/Src/main.c                  Main program
  - RTC/RTC_TimeStamp_Init/Src/system_stm32wbxx.c      STM32WBxx system source file


@par Hardware and Software environment

  - This example runs on STM32WB55xx devices.
    
  - This example has been tested with P-NUCLEO-WB55 board and can be
    easily tailored to any other supported device and development board.


@par How to use it ? 

In order to make the program work, you must do the following :
 - Open your preferred toolchain
 - Rebuild all files and load your image into target memory
 - Run the example

 * <h3><center>&copy; COPYRIGHT STMicroelectronics</center></h3>
 */
