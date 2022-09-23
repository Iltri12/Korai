/**
  @page HAL_TimeBase_TIM HAL Time base example
  
  @verbatim
  ******************************************************************************
  * @file    HAL/HAL_TimeBase_TIM/readme.txt 
  * @author  MCD Application Team
  * @brief   Description of the HAL time base example.
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

How to customize HAL using a general-purpose timer as main source of time base 
instead of Systick.

In this example the used timer is TIM2.

Time base duration is kept unchanged: 1ms  since PPP_TIMEOUT_VALUEs are defined 
and handled in milliseconds basis.

The example brings, in user file, a new implementation of the following HAL weak functions:

HAL_InitTick() 
HAL_SuspendTick()
HAL_ResumeTick()

This implementation will overwrite native implementation in stm32wbxx_hal.c
and so user functions will be invoked instead when called.

The following time base functions are kept as implemented natively:

HAL_IncTick()
HAL_Delay()

When user pushes the User push-button (SW1), the Tick increment is suspended if it is already
enabled, else it will be resumed.
In an infinite loop, LED2 toggles spaced out over 1s delay.

@note Care must be taken when using HAL_Delay(), this function provides accurate delay (in milliseconds)
      based on variable incremented in TIM2 ISR. This implies that if HAL_Delay() is called from
      a peripheral ISR process, then the TIM2 interrupt must have higher priority (numerically lower)
      than the peripheral interrupt. Otherwise the caller ISR process will be blocked.
      To change the TIM2 interrupt priority you have to use HAL_NVIC_SetPriority() function.
      
@note The application needs to ensure that the TIM2 time base is always set to 1 millisecond
      to have correct HAL operation.

@par Keywords

System, TIM, Time base, HAL

@par Directory contents

  - HAL/HAL_TimeBase_TIM/Inc/nucleo_wb15cc_conf.h     BSP configuration file
  - HAL/HAL_TimeBase_TIM/Inc/stm32wbxx_hal_conf.h          HAL configuration file
  - HAL/HAL_TimeBase_TIM/Inc/stm32wbxx_it.h                Interrupt handlers header file
  - HAL/HAL_TimeBase_TIM/Inc/stm32wbxx.h                   Header for main.c module  
  - HAL/HAL_TimeBase_TIM/Src/stm32wbxx_it.c                Interrupt handlers
  - HAL/HAL_TimeBase_TIM/Src/main.c                              Main program
  - HAL/HAL_TimeBase_TIM/Src/stm32wbxx_hal_msp.c           HAL MSP file
  - HAL/HAL_TimeBase_TIM/Src/stm32wbxx_hal_timebase_tim.c  HAL time base functions
  - HAL/HAL_TimeBase_TIM/Src/system_stm32wbxx.c            STM32WBxx system source file

@par Hardware and Software environment

  - This example runs on STM32WB15CCUx devices.
    
  - This example has been tested with STMicroelectronics NUCLEO-WB15CC board and can be
    easily tailored to any other supported device and development board.      

@par How to use it ? 

In order to make the program work, you must do the following :
 - Open your preferred toolchain 
 - Rebuild all files and load your image into target memory
 - Run the example

 * <h3><center>&copy; COPYRIGHT STMicroelectronics</center></h3>
 */
