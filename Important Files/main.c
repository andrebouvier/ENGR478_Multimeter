/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "stm32l476xx.h"	//include for original header for pins
#include "liquidcrystal_i2c.h"
#include <stdio.h>
#include <string.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
ADC_HandleTypeDef hadc1;
ADC_HandleTypeDef hadc2;

I2C_HandleTypeDef hi2c1;

/* USER CODE BEGIN PV */
typedef enum
{
	MODE_VOLTMETER,
	MODE_AMPMETER,
	MODE_CONTINUITY
}Mode;

Mode current_mode = MODE_VOLTMETER;
double voltage = 0.00;
double current = 0.00;
double voltage2 = 0.00;
double current2 = 0.00;
double voltageFinal = 0.00;
double currentFinal = 0.00;
int continuity = 0;
//char voltageArray[10];
//char currentArray[10];
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
void PeriphCommonClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_I2C1_Init(void);
static void MX_ADC1_Init(void);
static void MX_ADC2_Init(void);
/* USER CODE BEGIN PFP */
void configure_LCD(void);
void switchModes(void);
void configure_EXTI(void);
void SetLCD(void);

void getVoltage(void);
void getCurrent(void);
void getContinuity(void);

void getData(void);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

/* Configure the peripherals common clocks */
  PeriphCommonClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_I2C1_Init();
  MX_ADC1_Init();
  MX_ADC2_Init();
  /* USER CODE BEGIN 2 */
  //configure_pushButton_pin();
  configure_EXTI();
  HAL_NVIC_SetPriority(EXTI2_IRQn, 0, 0);

  HD44780_Init(2);	//initializing LCD
  HD44780_Clear();	//Clear LCD
  HD44780_SetCursor(5,0);
  HD44780_PrintStr("Welcome");
  HAL_Delay(1000);
  HD44780_Clear();

  //Configuring Systick to interrupt every 80 MHz
  HAL_SYSTICK_Config(SystemCoreClock);
  //Start timer
  HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);
  //Setting Systick Priority lower than the Button interrupt
  HAL_NVIC_SetPriority(SysTick_IRQn, 2, 0);

  //HAL_ADC_Start(&hadc1);

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
	  //SetLCD();
	  //HAL_Delay(100);
	  getVoltage();
	  getCurrent();
	  //getContinuity();
	  SetLCD();

  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 1;
  RCC_OscInitStruct.PLL.PLLN = 10;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV7;
  RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
  RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief Peripherals Common Clock Configuration
  * @retval None
  */
void PeriphCommonClock_Config(void)
{
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Initializes the peripherals clock
  */
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_ADC;
  PeriphClkInit.AdcClockSelection = RCC_ADCCLKSOURCE_PLLSAI1;
  PeriphClkInit.PLLSAI1.PLLSAI1Source = RCC_PLLSOURCE_HSI;
  PeriphClkInit.PLLSAI1.PLLSAI1M = 1;
  PeriphClkInit.PLLSAI1.PLLSAI1N = 8;
  PeriphClkInit.PLLSAI1.PLLSAI1P = RCC_PLLP_DIV7;
  PeriphClkInit.PLLSAI1.PLLSAI1Q = RCC_PLLQ_DIV2;
  PeriphClkInit.PLLSAI1.PLLSAI1R = RCC_PLLR_DIV2;
  PeriphClkInit.PLLSAI1.PLLSAI1ClockOut = RCC_PLLSAI1_ADC1CLK;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief ADC1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_ADC1_Init(void)
{

  /* USER CODE BEGIN ADC1_Init 0 */

  /* USER CODE END ADC1_Init 0 */

  ADC_MultiModeTypeDef multimode = {0};
  ADC_ChannelConfTypeDef sConfig = {0};

  /* USER CODE BEGIN ADC1_Init 1 */

  /* USER CODE END ADC1_Init 1 */

  /** Common config
  */
  hadc1.Instance = ADC1;
  hadc1.Init.ClockPrescaler = ADC_CLOCK_ASYNC_DIV1;
  hadc1.Init.Resolution = ADC_RESOLUTION_12B;
  hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc1.Init.ScanConvMode = ADC_SCAN_DISABLE;
  hadc1.Init.EOCSelection = ADC_EOC_SEQ_CONV;
  hadc1.Init.LowPowerAutoWait = DISABLE;
  hadc1.Init.ContinuousConvMode = DISABLE;
  hadc1.Init.NbrOfConversion = 1;
  hadc1.Init.DiscontinuousConvMode = DISABLE;
  hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc1.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
  hadc1.Init.DMAContinuousRequests = DISABLE;
  hadc1.Init.Overrun = ADC_OVR_DATA_PRESERVED;
  hadc1.Init.OversamplingMode = DISABLE;
  if (HAL_ADC_Init(&hadc1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure the ADC multi-mode
  */
  multimode.Mode = ADC_MODE_INDEPENDENT;
  if (HAL_ADCEx_MultiModeConfigChannel(&hadc1, &multimode) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Regular Channel
  */
  sConfig.Channel = ADC_CHANNEL_11;
  sConfig.Rank = ADC_REGULAR_RANK_1;
  sConfig.SamplingTime = ADC_SAMPLETIME_2CYCLES_5;
  sConfig.SingleDiff = ADC_SINGLE_ENDED;
  sConfig.OffsetNumber = ADC_OFFSET_NONE;
  sConfig.Offset = 0;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN ADC1_Init 2 */

  /* USER CODE END ADC1_Init 2 */

}

/**
  * @brief ADC2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_ADC2_Init(void)
{

  /* USER CODE BEGIN ADC2_Init 0 */

  /* USER CODE END ADC2_Init 0 */

  ADC_ChannelConfTypeDef sConfig = {0};

  /* USER CODE BEGIN ADC2_Init 1 */

  /* USER CODE END ADC2_Init 1 */

  /** Common config
  */
  hadc2.Instance = ADC2;
  hadc2.Init.ClockPrescaler = ADC_CLOCK_ASYNC_DIV1;
  hadc2.Init.Resolution = ADC_RESOLUTION_12B;
  hadc2.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc2.Init.ScanConvMode = ADC_SCAN_DISABLE;
  hadc2.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
  hadc2.Init.LowPowerAutoWait = DISABLE;
  hadc2.Init.ContinuousConvMode = DISABLE;
  hadc2.Init.NbrOfConversion = 1;
  hadc2.Init.DiscontinuousConvMode = DISABLE;
  hadc2.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc2.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
  hadc2.Init.DMAContinuousRequests = DISABLE;
  hadc2.Init.Overrun = ADC_OVR_DATA_PRESERVED;
  hadc2.Init.OversamplingMode = DISABLE;
  if (HAL_ADC_Init(&hadc2) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Regular Channel
  */
  sConfig.Channel = ADC_CHANNEL_12;
  sConfig.Rank = ADC_REGULAR_RANK_1;
  sConfig.SamplingTime = ADC_SAMPLETIME_2CYCLES_5;
  sConfig.SingleDiff = ADC_SINGLE_ENDED;
  sConfig.OffsetNumber = ADC_OFFSET_NONE;
  sConfig.Offset = 0;
  if (HAL_ADC_ConfigChannel(&hadc2, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN ADC2_Init 2 */

  /* USER CODE END ADC2_Init 2 */

}

/**
  * @brief I2C1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C1_Init(void)
{

  /* USER CODE BEGIN I2C1_Init 0 */

  /* USER CODE END I2C1_Init 0 */

  /* USER CODE BEGIN I2C1_Init 1 */

  /* USER CODE END I2C1_Init 1 */
  hi2c1.Instance = I2C1;
  hi2c1.Init.Timing = 0x10909CEC;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.OwnAddress2Masks = I2C_OA2_NOMASK;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Analogue filter
  */
  if (HAL_I2CEx_ConfigAnalogFilter(&hi2c1, I2C_ANALOGFILTER_ENABLE) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Digital filter
  */
  if (HAL_I2CEx_ConfigDigitalFilter(&hi2c1, 0) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C1_Init 2 */

  /* USER CODE END I2C1_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
/* USER CODE BEGIN MX_GPIO_Init_1 */
/* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(Continuity_Output_GPIO_Port, Continuity_Output_Pin, GPIO_PIN_SET);

  /*Configure GPIO pin : Pushbutton_Pin */
  GPIO_InitStruct.Pin = Pushbutton_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(Pushbutton_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : Continuity_Output_Pin */
  GPIO_InitStruct.Pin = Continuity_Output_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_MEDIUM;
  HAL_GPIO_Init(Continuity_Output_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : Continuity_Input_Pin */
  GPIO_InitStruct.Pin = Continuity_Input_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(Continuity_Input_GPIO_Port, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI2_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI2_IRQn);

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

void switchModes(void)
{
	current_mode = (current_mode + 1) % 3;
}


void configure_EXTI(void)
{
//Enable specific interrupt in NVIC
	NVIC_EnableIRQ(EXTI2_IRQn);
//enable clock to syscfg
	RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;
//clear and set bits to setup the interrupt source
	SYSCFG->EXTICR[0] &= ~SYSCFG_EXTICR1_EXTI2;	//MIGHT NEED TO CHECK THIS LINE WITH ALEX
	SYSCFG->EXTICR[0] |=  SYSCFG_EXTICR1_EXTI2_PC;
//enable interrupt
	EXTI->IMR1 |= (1<<2);
//enable interrupt trigger for rising and falling edges
	EXTI->RTSR1 |= (1<<2);  //Rising trigger selection register (RTSR):0 = disabled, 1 = enabled
	EXTI->FTSR1 |= (1<<2);
}


void EXTI2_IRQHandler(void)
{
//Check if the pending interrupt is triggered by the pushbutton
	if ((EXTI->PR1 & EXTI_PR1_PIF2) == EXTI_PR1_PIF2)
		{
			//Check if the pushbutton was pressed
			if ((GPIOC->IDR & (1<<2)) == 0)
			{
				switchModes();
				SetLCD();
			}
			EXTI->PR1 |= EXTI_PR1_PIF2;	//clear pending interrupt
		}
}


void SetLCD(void)
{
	HD44780_Clear();
	if (current_mode == MODE_VOLTMETER)
	{
		//Display Mode
		HD44780_SetCursor(4,0);
		HD44780_PrintStr("Voltmeter");

		//Display Data
		HD44780_SetCursor(5,1);
		HD44780_Float(voltageFinal, 5);

		//Variable
		HD44780_SetCursor(15,1);
		HD44780_PrintStr("V");
	} else if (current_mode == MODE_AMPMETER)
	{
		//Display Mode
		HD44780_SetCursor(4,0);
		HD44780_PrintStr("Ampmeter");

		//Display Data
		HD44780_SetCursor(5,1);
		HD44780_Float(currentFinal, 5);

		//Display Variable
		HD44780_SetCursor(14,1);
		HD44780_PrintSpecialChar("µ");
		HD44780_SetCursor(15,1);
		HD44780_PrintStr("A");
	} else if (current_mode == MODE_CONTINUITY)
	{
		HD44780_SetCursor(3,0);
		HD44780_PrintStr("Continuity");
		if (continuity == 1)
		{
			HD44780_SetCursor(3,1);
			HD44780_PrintStr("Continuous");
		} else
		{
			HD44780_SetCursor(6,1);
			HD44780_PrintStr("Open");
		}

	}
}


void getVoltage()
{
	//Start ADC conversion
	HAL_ADC_Start(&hadc1);
	HAL_ADC_Start(&hadc2);

	//HAL Function to Poll to see if Conversion has finished
	HAL_ADC_PollForConversion(&hadc1, 20);
	HAL_ADC_PollForConversion(&hadc2, 20);

	//setting variables to Conversion
	voltage = HAL_ADC_GetValue(&hadc1);
	voltage2 = HAL_ADC_GetValue(&hadc2);

	//Converting Data
	voltage = (voltage / 4096.0) * 3.3;
	voltage2 = (voltage2 / 4096.0) * 3.3;

	//Difference of Probes
	voltageFinal = voltage - voltage2;

	//Stop ADC Collection, Delay for LCD Readability
	HAL_ADC_Stop(&hadc1);
	HAL_Delay(100);
}


void getCurrent()
{
	//Start ADC conversion
	HAL_ADC_Start(&hadc1);
	HAL_ADC_Start(&hadc2);

	//HAL Function to Poll to see if Conversion has finished
	HAL_ADC_PollForConversion(&hadc1, 20);
	HAL_ADC_PollForConversion(&hadc2, 20);

	//setting variables to Conversion
	current = HAL_ADC_GetValue(&hadc1);
	current2 = HAL_ADC_GetValue(&hadc2);

	//Converting Data
	current = (current / 4096.0) * 3.3; //0.150;
	current2 = (current2 / 4096.0) * 3.3; //0.150;

	//Convert voltage value into current with 0.01 ohm shunt
	current = current / 0.01;
	current2 = current2/ 0.01;

	currentFinal = (current - current2);

	//Stop ADC Collection, Delay for LCD Readability
	HAL_ADC_Stop(&hadc1);
	HAL_Delay(100);
}

//maybe make input pin pull up to ensure that you don't get accidental continuous
void getContinuity()
{
	if((GPIOC->IDR & (1<<9)) != 0)
	{
		continuity = 1;
	}
	else
	{
	continuity = 0;
	}
}

void getData(void)
{
	if (current_mode == MODE_VOLTMETER)
	{
		getVoltage();
	} else if (current_mode == MODE_AMPMETER)
	{
		getCurrent();
	} else if (current_mode == MODE_CONTINUITY)
	{
		getContinuity();
	}
}

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
