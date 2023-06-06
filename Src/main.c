
#include "main.h"

void initialise_monitor_handles(void);

I2C_HandleTypeDef hi2c1;

UART_HandleTypeDef huart2;

void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_I2C1_Init(void);

uint16_t RTC_SLAVE_ADD = 0x68;
uint8_t rtc_txt_data[2] ={0x00, 0x00};
uint8_t rtc_rcv_data[2] ={0,0};

uint8_t bcd2dec(uint8_t c)
{
	uint8_t dec;

	dec = (c>>4) & 0x07;
	dec = (dec<<3)*10 + (c & 0x0F);

	return dec;
}

uint8_t dec2bcd(uint8_t c){
	if( c < 10 ) return c;
	else{
		uint8_t q = c/10;
		return (q << 4 ) | (c-q*10);
	}
}


uint8_t getSecond(){

	rtc_txt_data[0] = 0x00;
	if(HAL_I2C_Master_Transmit(&hi2c1,RTC_SLAVE_ADD<<1, rtc_txt_data,1, 100 ) != HAL_OK){
		Error_Handler();
	}
	if(HAL_I2C_Master_Receive(&hi2c1,RTC_SLAVE_ADD<<1, rtc_rcv_data, 1, 100 ) != HAL_OK){
		Error_Handler();
	}



	uint8_t unitTime = rtc_rcv_data[0] & 0x0F;
	uint8_t tenthTime = rtc_rcv_data[0] & 0x70;
	tenthTime = tenthTime >> 4;

	rtc_txt_data[0] = 0x00;
	return tenthTime*10+unitTime;
}


uint8_t getMinute(){

	rtc_txt_data[0] = 0x01;
	if(HAL_I2C_Master_Transmit(&hi2c1,RTC_SLAVE_ADD<<1, rtc_txt_data,1, 100 ) != HAL_OK){
		Error_Handler();
	}
	if(HAL_I2C_Master_Receive(&hi2c1,RTC_SLAVE_ADD<<1, rtc_rcv_data, 1, 100 ) != HAL_OK){
		Error_Handler();
	}



	uint8_t unitTime = rtc_rcv_data[0] & 0x0F;
	uint8_t tenthTime = rtc_rcv_data[0] & 0x70;
	tenthTime = tenthTime >> 4;

	rtc_txt_data[0] = 0x00;
	return tenthTime*10+unitTime;
}


uint8_t getHour(){

	rtc_txt_data[0] = 0x02;
	if(HAL_I2C_Master_Transmit(&hi2c1,RTC_SLAVE_ADD<<1, rtc_txt_data,1, 100 ) != HAL_OK){
		Error_Handler();
	}
	if(HAL_I2C_Master_Receive(&hi2c1,RTC_SLAVE_ADD<<1, rtc_rcv_data, 1, 100 ) != HAL_OK){
		Error_Handler();
	}


	uint8_t unitTime = rtc_rcv_data[0] & 0x0F;
	uint8_t tenthTime = rtc_rcv_data[0] & 0x70;
	tenthTime = tenthTime >> 4;

	rtc_txt_data[0] = 0x00;
	return tenthTime*10+unitTime;
}

void setTime(uint8_t hour, uint8_t minute, uint8_t second){
	rtc_txt_data[0] = 0x00;
	rtc_txt_data[1] = second;
	if(HAL_I2C_Master_Transmit(&hi2c1,RTC_SLAVE_ADD<<1, rtc_txt_data,2, 100 ) != HAL_OK){
		  Error_Handler();
	}

	rtc_txt_data[0] = 0x01;
	rtc_txt_data[1] = minute;
	if(HAL_I2C_Master_Transmit(&hi2c1,RTC_SLAVE_ADD<<1, rtc_txt_data,2, 100 ) != HAL_OK){
		Error_Handler();
	}


	rtc_txt_data[0] = 0x02;
	rtc_txt_data[1] = hour;
	if(HAL_I2C_Master_Transmit(&hi2c1,RTC_SLAVE_ADD<<1, rtc_txt_data,2, 100 ) != HAL_OK){
		Error_Handler();
	}
}


int main(void)
{

  initialise_monitor_handles();
  HAL_Init();
  SystemClock_Config();
  MX_GPIO_Init();
  MX_USART2_UART_Init();
  MX_I2C1_Init();
  HAL_GPIO_TogglePin(LED_GREEN_GPIO_Port, LED_GREEN_Pin);

  if( HAL_I2C_IsDeviceReady(&hi2c1, RTC_SLAVE_ADD<<1, 2, 100) == HAL_OK){
	  HAL_GPIO_TogglePin(LED_GREEN_GPIO_Port, LED_GREEN_Pin);
  }


  /*
  if(HAL_I2C_Master_Transmit(&hi2c1,RTC_SLAVE_ADD<<1, rtc_txt_data,2, 100 ) != HAL_OK){
	  Error_Handler();
  }
  */


  HAL_GPIO_TogglePin(LED_GREEN_GPIO_Port, LED_GREEN_Pin);

  uint8_t seconds = 0;
  uint8_t minutes = 0;
  uint8_t hours = 0;

  setTime(dec2bcd(1),dec2bcd(32),dec2bcd(0));

  while(1){

	  HAL_GPIO_TogglePin(LED_GREEN_GPIO_Port, LED_GREEN_Pin);
	  seconds = getSecond();
	  minutes = getMinute();
	  hours = getHour();
	  printf("Times : %d: %d : %d \n", hours, minutes, seconds);
	  HAL_Delay(1000);

  }

  return 0;
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Configure the main internal regulator output voltage
  */
  HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1);
  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSIDiv = RCC_HSI_DIV1;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the peripherals clocks
  */
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART2|RCC_PERIPHCLK_I2C1;
  PeriphClkInit.Usart2ClockSelection = RCC_USART2CLKSOURCE_PCLK1;
  PeriphClkInit.I2c1ClockSelection = RCC_I2C1CLKSOURCE_PCLK1;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
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
  hi2c1.Init.Timing = 0x00303D5B;
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
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  huart2.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart2.Init.ClockPrescaler = UART_PRESCALER_DIV1;
  huart2.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetTxFifoThreshold(&huart2, UART_TXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetRxFifoThreshold(&huart2, UART_RXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_DisableFifoMode(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOF_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(LED_GREEN_GPIO_Port, LED_GREEN_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : LED_GREEN_Pin */
  GPIO_InitStruct.Pin = LED_GREEN_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(LED_GREEN_GPIO_Port, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */

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
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
