/**
  @page RCC_ClockConfig RCC Clock Config example

  @verbatim
  ******************************************************************************
  * @file    RCC/RCC_ClockConfig/readme.txt
  * @author  MCD Application Team
  * @brief   Description of the RCC Clock Config example.
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

Configuration of the system clock (SYSCLK) and modification of the clock settings in Run mode, using the RCC HAL API.

In this example, after startup SYSCLK is configured to the max frequency using the PLL with
MSI as clock source, the User push-button (SW1) (connected to External line 0) will be
used to change the PLL source:
- from HSE to HSI
- from HSI to MSI
- from MSI to HSE

Each time the User push-button (SW1) is pressed External line 0 interrupt is generated and in the ISR
the PLL oscillator source is checked using __HAL_RCC_GET_PLL_OSCSOURCE() macro:

- If the HSE oscillator is selected as PLL source, the following steps will be followed to switch
   the PLL source to HSI oscillator:
     a- Switch the system clock source to HSE to allow modification of the PLL configuration
     b- Enable HSI Oscillator, select it as PLL source and finally activate the PLL
     c- Select the PLL as system clock source and configure the HCLK, PCLK1 and PCLK2 clocks dividers
     d- Disable the HSE oscillator (optional, if the HSE is no more needed by the application)

- If the MSI oscillator is selected as PLL source, the following steps will be followed to switch
   the PLL source to HSE oscillator:
     a- Switch the system clock source to MSI to allow modification of the PLL configuration
     b- Enable HSE Oscillator, select it as PLL source and finally activate the PLL
     c- Select the PLL as system clock source and configure the HCLK, PCLK1 and PCLK2 clocks dividers
     d- Disable the MSI oscillator (optional, if the MSI is no more needed by the application)

- If the HSI oscillator is selected as PLL source, the following steps will be followed to switch
   the PLL source to MSI oscillator:
     a- Switch the system clock source to HSI to allow modification of the PLL configuration
     b- Enable MSI Oscillator, select it as PLL source and finally activate the PLL
     c- Select the PLL as system clock source and configure the HCLK, PCLK1 and PCLK2 clocks dividers
     d- Disable the HSI oscillator (optional, if the HSI is no more needed by the application)


LED2 is toggled with a timing defined by the HAL_Delay() API.

@note Care must be taken when using HAL_Delay(), this function provides accurate delay (in milliseconds)
      based on variable incremented in SysTick ISR. This implies that if HAL_Delay() is called from
      a peripheral ISR process, then the SysTick interrupt must have higher priority (numerically lower)
      than the peripheral interrupt. Otherwise the caller ISR process will be blocked.
      To change the SysTick interrupt priority you have to use HAL_NVIC_SetPriority() function.

@note The application need to ensure that the SysTick time base is always set to 1 millisecond
      to have correct HAL operation.

@par Keywords

RCC, System, Clock Configuration, HSE bypass mode, HSI, System clock, Oscillator, PLL

@par Directory contents

  - RCC/RCC_ClockConfig/Inc/nucleo_wb15cc_conf.h     BSP configuration file
  - RCC/RCC_ClockConfig/Inc/stm32wbxx_hal_conf.h    HAL configuration file
  - RCC/RCC_ClockConfig/Inc/stm32wbxx_it.h          Interrupt handlers header file
  - RCC/RCC_ClockConfig/Inc/main.h                  Header for main.c module
  - RCC/RCC_ClockConfig/Src/stm32wbxx_it.c          Interrupt handlers
  - RCC/RCC_ClockConfig/Src/main.c                  Main program
  - RCC/RCC_ClockConfig/Src/system_stm32wbxx.c      STM32WBxx system source file
  - RCC/RCC_ClockConfig/Src/stm32wbxx_hal_msp.c   HAL MSP module

@par Hardware and Software environment

  - This example runs on STM32WB15xx devices.

  - This example has been tested with NUCLEO-WB15CC
    board and can be easily tailored to any other supported device
    and development board.

@par How to use it ?

In order to make the program work, you must do the following :
 - Open your preferred toolchain
 - Rebuild all files and load your image into target memory
 - Run the example

 * <h3><center>&copy; COPYRIGHT STMicroelectronics</center></h3>
 */
