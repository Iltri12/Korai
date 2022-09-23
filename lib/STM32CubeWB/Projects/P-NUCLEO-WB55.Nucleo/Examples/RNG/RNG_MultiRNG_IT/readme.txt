/**
  @page RNG_MultiRNG_IT Multiple Random Numbers Generator under interruption example
  
  @verbatim
  ******************************************************************************
  * @file    RNG/RNG_MultiRNG_IT/readme.txt 
  * @author  MCD Application Team
  * @brief   Description of multiple random numbers generation under interruption example.
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

Configuration of the RNG using the HAL API. This example uses RNG interrupts to generate 32-bit long random numbers.

At the beginning of the main program the HAL_Init() function is called to reset 
all the peripherals, initialize the Flash interface and the systick.
Then the SystemClock_Config() function is used to configure the system
clock (SYSCLK) to run at 64 MHz.

The RNG peripheral configuration is ensured by the HAL_RNG_Init() function.
The latter is calling the HAL_RNG_MspInit() function which implements
the configuration of the needed RNG resources according to the used hardware (CLOCK, 
GPIO, DMA and NVIC). You may update this function to change RNG configuration.

After startup, user is asked to press User push-button (SW1).
The 8-entry array aRandom32bit[] is filled up by 32-bit long random numbers 
at each User push-button (SW1) press.

Each random number generation triggers an interruption which is handled by 
HAL_RNG_ReadyDataCallback() API to retrieve and store the random number.
 
The random numbers can be displayed on the debugger in aRandom32bit variable.

In case of error, LED3 is toggling at a frequency of 1Hz.

@par Keywords

Analog, RNG, Random, FIPS PUB 140-2, Analog Random number generator, Entropy, Period, interrupt

@par Directory contents 

  - RNG/RNG_MultiRNG_IT/Inc/stm32wbxx_hal_conf.h    HAL configuration file
  - RNG/RNG_MultiRNG_IT/Inc/stm32wbxx_it.h          Interrupt handlers header file
  - RNG/RNG_MultiRNG_IT/Inc/main.h                  Header for main.c module
  - RNG/RNG_MultiRNG_IT/Src/stm32wbxx_it.c          Interrupt handlers
  - RNG/RNG_MultiRNG_IT/Src/main.c                  Main program
  - RNG/RNG_MultiRNG_IT/Src/stm32wbxx_hal_msp.c     HAL MSP module 
  - RNG/RNG_MultiRNG_IT/Src/system_stm32wbxx.c      STM32WBxx system source file

     
@par Hardware and Software environment

  - This example runs on STM32WB55xx devices.
  
  - This example has been tested with P-NUCLEO-WB55 board and can be
    easily tailored to any other supported device and development board.

@par How to use it ? 

In order to make the program work, you must do the following:
 - Open your preferred toolchain 
 - Rebuild all files and load your image into target memory
 - Run the example
 
 * <h3><center>&copy; COPYRIGHT STMicroelectronics</center></h3>
 */
 
