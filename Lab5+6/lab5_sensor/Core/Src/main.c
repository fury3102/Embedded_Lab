/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
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
#include "vl53l0x_api.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

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
CAN_HandleTypeDef hcan;

I2C_HandleTypeDef hi2c1;

UART_HandleTypeDef huart1;

/* USER CODE BEGIN PV */
uint8_t Message[64];
uint8_t MessageLen;
uint8_t LsensorValue = 0;
uint8_t RsensorValue = 0;
VL53L0X_Dev_t L_sensor;
VL53L0X_Dev_t R_sensor;
VL53L0X_RangingMeasurementData_t L_measurement;
VL53L0X_RangingMeasurementData_t R_measurement;
VL53L0X_DEV    Lsensor = &L_sensor;
VL53L0X_DEV    Rsensor = &R_sensor;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART1_UART_Init(void);
static void MX_CAN_Init(void);
static void MX_I2C1_Init(void);
/* USER CODE BEGIN PFP */
void sendCanData(uint8_t LsensorValue, uint8_t RsensorValue);

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
CAN_TxHeaderTypeDef TxHeader;
CAN_RxHeaderTypeDef RxHeader;
uint8_t TxData[8];
uint8_t RxData[8];

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */
	  uint32_t refSpadCount;
	    uint8_t isApertureSpads;
	    uint8_t VhvSettings;
	    uint8_t PhaseCal;
  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */
  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_USART1_UART_Init();
  MX_CAN_Init();
  MX_I2C1_Init();
  /* USER CODE BEGIN 2 */

  Lsensor->I2cHandle = &hi2c1;
  Lsensor->I2cDevAddr = 0x30;
  Rsensor->I2cHandle = &hi2c1;
  Rsensor->I2cDevAddr = 0x31;

  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_15, GPIO_PIN_SET);
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_8, GPIO_PIN_SET);
//------------------------------------------
  	  	VL53L0X_WaitDeviceBooted(Lsensor);
  	  	VL53L0X_DataInit(Lsensor);
  	  	VL53L0X_StaticInit(Lsensor);
  	  	VL53L0X_PerformRefCalibration(Lsensor, &VhvSettings, &PhaseCal);
   	   	VL53L0X_PerformRefSpadManagement(Lsensor, &refSpadCount, &isApertureSpads);
   	   	VL53L0X_SetDeviceMode(Lsensor, VL53L0X_DEVICEMODE_SINGLE_RANGING);

   // Enable/Disable Sigma and Signal check
    	VL53L0X_SetLimitCheckEnable(Lsensor, VL53L0X_CHECKENABLE_SIGMA_FINAL_RANGE, 1);
     	VL53L0X_SetLimitCheckEnable(Lsensor, VL53L0X_CHECKENABLE_SIGNAL_RATE_FINAL_RANGE, 1);
     	VL53L0X_SetLimitCheckValue(Lsensor, VL53L0X_CHECKENABLE_SIGNAL_RATE_FINAL_RANGE, (FixPoint1616_t)(0.1*65536));
     	VL53L0X_SetLimitCheckValue(Lsensor, VL53L0X_CHECKENABLE_SIGMA_FINAL_RANGE, (FixPoint1616_t)(60*65536));
     	VL53L0X_SetMeasurementTimingBudgetMicroSeconds(Lsensor, 33000);
     	VL53L0X_SetVcselPulsePeriod(Lsensor, VL53L0X_VCSEL_PERIOD_PRE_RANGE, 18);
     	VL53L0X_SetVcselPulsePeriod(Lsensor, VL53L0X_VCSEL_PERIOD_FINAL_RANGE, 14);

//-----------------------------------------------
     	VL53L0X_WaitDeviceBooted(Rsensor);
     	VL53L0X_DataInit(Rsensor);
     	VL53L0X_StaticInit(Rsensor);
     	VL53L0X_PerformRefCalibration(Rsensor, &VhvSettings, &PhaseCal);
     	VL53L0X_PerformRefSpadManagement(Rsensor, &refSpadCount, &isApertureSpads);
     	VL53L0X_SetDeviceMode(Rsensor, VL53L0X_DEVICEMODE_SINGLE_RANGING);

     // Enable/Disable Sigma and Signal check
     	VL53L0X_SetLimitCheckEnable(Rsensor, VL53L0X_CHECKENABLE_SIGMA_FINAL_RANGE, 1);
     	VL53L0X_SetLimitCheckEnable(Rsensor, VL53L0X_CHECKENABLE_SIGNAL_RATE_FINAL_RANGE, 1);
     	VL53L0X_SetLimitCheckValue(Rsensor, VL53L0X_CHECKENABLE_SIGNAL_RATE_FINAL_RANGE, (FixPoint1616_t)(0.1*65536));
     	VL53L0X_SetLimitCheckValue(Rsensor, VL53L0X_CHECKENABLE_SIGMA_FINAL_RANGE, (FixPoint1616_t)(60*65536));
     	VL53L0X_SetMeasurementTimingBudgetMicroSeconds(&R_sensor, 33000);
     	VL53L0X_SetVcselPulsePeriod(Rsensor, VL53L0X_VCSEL_PERIOD_PRE_RANGE, 18);
     	VL53L0X_SetVcselPulsePeriod(Rsensor, VL53L0X_VCSEL_PERIOD_FINAL_RANGE, 14);

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */
	  VL53L0X_PerformSingleRangingMeasurement(Lsensor, &L_measurement);
		  VL53L0X_PerformSingleRangingMeasurement(Rsensor, &R_measurement);
		  MessageLen = sprintf((char*)Message, "Ldistance: %i\n\r", L_measurement.RangeMilliMeter);
		  MessageLen = sprintf((char*)Message, "Rdistance: %i\n\r", R_measurement.RangeMilliMeter);
		  HAL_UART_Transmit(&huart1, Message, MessageLen, 100);
		  LsensorValue = (uint8_t)L_measurement.RangeMilliMeter;
		  RsensorValue = (uint8_t)R_measurement.RangeMilliMeter;
		  sendCanData(LsensorValue, RsensorValue);
		  HAL_Delay(1000);
    /* USER CODE BEGIN 3 */
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

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
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
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief CAN Initialization Function
  * @param None
  * @retval None
  */
static void MX_CAN_Init(void)
{

  /* USER CODE BEGIN CAN_Init 0 */

  /* USER CODE END CAN_Init 0 */

  /* USER CODE BEGIN CAN_Init 1 */

  /* USER CODE END CAN_Init 1 */
  hcan.Instance = CAN1;
  hcan.Init.Prescaler = 4;
  hcan.Init.Mode = CAN_MODE_NORMAL;
  hcan.Init.SyncJumpWidth = CAN_SJW_2TQ;
  hcan.Init.TimeSeg1 = CAN_BS1_13TQ;
  hcan.Init.TimeSeg2 = CAN_BS2_4TQ;
  hcan.Init.TimeTriggeredMode = DISABLE;
  hcan.Init.AutoBusOff = DISABLE;
  hcan.Init.AutoWakeUp = DISABLE;
  hcan.Init.AutoRetransmission = DISABLE;
  hcan.Init.ReceiveFifoLocked = DISABLE;
  hcan.Init.TransmitFifoPriority = DISABLE;
  if (HAL_CAN_Init(&hcan) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN CAN_Init 2 */
  CAN_FilterTypeDef canfilterconfig;

    canfilterconfig.FilterActivation = CAN_FILTER_ENABLE;
    canfilterconfig.FilterBank = 12;  // which filter bank to use from the assigned ones
    canfilterconfig.FilterFIFOAssignment = CAN_FILTER_FIFO1;
    canfilterconfig.FilterIdHigh = 0x0a2<<5;
    canfilterconfig.FilterIdLow = 0;
    canfilterconfig.FilterMaskIdHigh = 0x0a2<<5;
    canfilterconfig.FilterMaskIdLow = 0x0000;
    canfilterconfig.FilterMode = CAN_FILTERMODE_IDMASK;
    canfilterconfig.FilterScale = CAN_FILTERSCALE_32BIT;
    canfilterconfig.SlaveStartFilterBank = 13;  // how many filters to assign to the CAN1 (master can)
    HAL_CAN_ConfigFilter(&hcan, &canfilterconfig);
  /* USER CODE END CAN_Init 2 */

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
  hi2c1.Init.ClockSpeed = 100000;
  hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C1_Init 2 */

  /* USER CODE END I2C1_Init 2 */

}

/**
  * @brief USART1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART1_UART_Init(void)
{

  /* USER CODE BEGIN USART1_Init 0 */

  /* USER CODE END USART1_Init 0 */

  /* USER CODE BEGIN USART1_Init 1 */

  /* USER CODE END USART1_Init 1 */
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 115200;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART1_Init 2 */

  /* USER CODE END USART1_Init 2 */

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
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_15, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, LEDR_MCU2_Pin|LEDG_MCU2_Pin|GPIO_PIN_8, GPIO_PIN_RESET);

  /*Configure GPIO pin : PC14 */
  GPIO_InitStruct.Pin = GPIO_PIN_14;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pin : PC15 */
  GPIO_InitStruct.Pin = GPIO_PIN_15;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : BTN1_Pin BTN2_Pin PB9 */
  GPIO_InitStruct.Pin = BTN1_Pin|BTN2_Pin|GPIO_PIN_9;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pins : LEDR_MCU2_Pin LEDG_MCU2_Pin */
  GPIO_InitStruct.Pin = LEDR_MCU2_Pin|LEDG_MCU2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pin : PB8 */
  GPIO_InitStruct.Pin = GPIO_PIN_8;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */
void sendCanData(uint8_t LsensorValue, uint8_t RsensorValue)
{
    CAN_TxHeaderTypeDef txHeader;
    uint8_t sendData[8] = {0,0,0,0,0,0,0,0};

    if (LsensorValue < 100)
    {
        sendData[0] = 1;
    }
    else if (LsensorValue > 100){
    	sendData[0] = 0;
    	sendData[1] = 1;
    }

    else if (RsensorValue < 50)
    {

        sendData[2] = 1;
    }
    else if (RsensorValue > 100)
       {
           sendData[2] = 0;
           sendData[3] = 1;
       }
    txHeader.StdId = 0x012;
    txHeader.IDE = CAN_ID_STD;
    txHeader.RTR = CAN_RTR_DATA;
    txHeader.DLC = 8;
    HAL_CAN_AddTxMessage(&hcan, &txHeader, sendData, NULL);
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
