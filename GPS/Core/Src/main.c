/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
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
#include "ILI9341_STM32_Driver.h"
#include "ILI9341_GFX.h"
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
SPI_HandleTypeDef hspi1;
DMA_HandleTypeDef hdma_spi1_tx;

/* USER CODE BEGIN PV */
const char* gps_data[] = {
    "N 49°49.0609', E 024°01.3547', UTC: 07:58:25",
    "N 49°49.0596', E 024°01.3593', UTC: 07:58:26",
    "N 49°49.0514', E 024°01.3757', UTC: 07:58:27",
    "N 49°49.0428', E 024°01.3930', UTC: 07:58:28",
    "N 49°49.0412', E 024°01.4053', UTC: 07:58:30",
    "N 49°49.0368', E 024°01.4182', UTC: 07:58:31",
    "N 49°49.0366', E 024°01.4185', UTC: 07:58:32",
    "N 49°49.0366', E 024°01.4173', UTC: 07:58:33",
    "N 49°49.0371', E 024°01.4104', UTC: 07:58:36",
    "N 49°49.0357', E 024°01.4070', UTC: 07:58:38",
    "N 49°49.0357', E 024°01.4002', UTC: 07:58:41",
    "N 49°49.0353', E 024°01.3986', UTC: 07:58:42",
    "N 49°49.0339', E 024°01.4035', UTC: 07:58:45",
    "N 49°49.0317', E 024°01.3996', UTC: 07:58:47",
    "N 49°49.0303', E 024°01.4007', UTC: 07:58:48",
    "N 49°49.0289', E 024°01.3998', UTC: 07:58:49",
    "N 49°49.0292', E 024°01.4006', UTC: 07:58:50",
    "N 49°49.0293', E 024°01.4001', UTC: 07:58:51",
    "N 49°49.0292', E 024°01.4015', UTC: 07:58:52",
    "N 49°49.0293', E 024°01.4015', UTC: 07:58:53",
    "N 49°49.0283', E 024°01.4056', UTC: 07:58:56",
    "N 49°49.0281', E 024°01.4089', UTC: 07:58:57",
    "N 49°49.0276', E 024°01.4075', UTC: 07:58:58",
    "N 49°49.0266', E 024°01.4056', UTC: 07:58:59",
    "N 49°49.0265', E 024°01.4033', UTC: 07:59:00",
    "N 49°49.0269', E 024°01.4010', UTC: 07:59:01",
    "N 49°49.0269', E 024°01.3923', UTC: 07:59:03",
    "N 49°49.0270', E 024°01.3924', UTC: 07:59:05",
    "N 49°49.0267', E 024°01.3927', UTC: 07:59:06",
    "N 49°49.0260', E 024°01.3939', UTC: 07:59:07",
    "N 49°49.0261', E 024°01.3912', UTC: 07:59:09",
    "N 49°49.0285', E 024°01.3867', UTC: 07:59:11",
    "N 49°49.0313', E 024°01.3936', UTC: 07:59:17",
    "N 49°49.0315', E 024°01.3951', UTC: 07:59:18",
    "N 49°49.0318', E 024°01.3931', UTC: 07:59:20",
    "N 49°49.0334', E 024°01.3799', UTC: 07:59:23",
    "N 49°49.0328', E 024°01.3832', UTC: 07:59:26",
    "N 49°49.0314', E 024°01.3868', UTC: 07:59:27",
    "N 49°49.0309', E 024°01.3867', UTC: 07:59:28",
    "N 49°49.0302', E 024°01.3856', UTC: 07:59:29",
    "N 49°49.0300', E 024°01.3824', UTC: 07:59:31",
    "N 49°49.0284', E 024°01.3832', UTC: 07:59:33",
    "N 49°49.0281', E 024°01.3850', UTC: 07:59:35",
    "N 49°49.0292', E 024°01.3953', UTC: 07:59:37",
    "N 49°49.0288', E 024°01.4055', UTC: 07:59:38",
    "N 49°49.0282', E 024°01.4051', UTC: 07:59:39"
};
const int gps_data_count = sizeof(gps_data) / sizeof(gps_data[0]);
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_SPI1_Init(void);
/* USER CODE BEGIN PFP */

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

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_SPI1_Init();
  /* USER CODE BEGIN 2 */
  Init();


  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
   // Set orientation
   SetRotation(SCREEN_HORIZONTAL_1);

   // Clear the screen at the beginning
   FillScreen(WHITE);

   DrawText("Device ID:", FONT2, 10, 10, BLACK, WHITE);
   DrawText("Coordinates:", FONT2, 10, 40, BLACK, WHITE);
   DrawText("Time (UTC):", FONT2, 10, 70, BLACK, WHITE);

   uint32_t device_id_word0 = HAL_GetUIDw0();
   char id_str[20];
   sprintf(id_str, "0x%lX", device_id_word0);
   DrawText(id_str, FONT2, 110, 10, BLUE, WHITE);

   int gps_index = 0;
   char coords_buffer[45];
   char time_buffer[20];
  while (1)
  {
	  const char* full_gps_string = gps_data[gps_index];
	  const char* separator = ", UTC: ";
	  const char* time_ptr = strstr(full_gps_string, separator);

	  if (time_ptr) {
		  int coords_len = time_ptr - full_gps_string;
		  sprintf(coords_buffer, "%.*s ", coords_len, full_gps_string);

		  const char* time_start = time_ptr + strlen(separator);
		  sprintf(time_buffer, "%s ", time_start);
	  } else {
		  sprintf(coords_buffer, "%s ", full_gps_string);
		  sprintf(time_buffer, "N/A ");
	  }

	  DrawText(coords_buffer, FONT2, 10, 55, RED, WHITE);
	  DrawText(time_buffer, FONT2, 10, 85, RED, WHITE);

	  // Move to the next GPS data entry
	  gps_index = (gps_index + 1) % gps_data_count;

	  // Toggle the LED and delay
	  HAL_GPIO_TogglePin(GPIOD, GPIO_PIN_12);
	  HAL_Delay(1000);
    /* USER CODE END WHILE */

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

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief SPI1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI1_Init(void)
{

  /* USER CODE BEGIN SPI1_Init 0 */

  /* USER CODE END SPI1_Init 0 */

  /* USER CODE BEGIN SPI1_Init 1 */

  /* USER CODE END SPI1_Init 1 */
  /* SPI1 parameter configuration*/
  hspi1.Instance = SPI1;
  hspi1.Init.Mode = SPI_MODE_MASTER;
  hspi1.Init.Direction = SPI_DIRECTION_2LINES;
  hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi1.Init.NSS = SPI_NSS_SOFT;
  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_2;
  hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi1.Init.CRCPolynomial = 10;
  if (HAL_SPI_Init(&hspi1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI1_Init 2 */

  /* USER CODE END SPI1_Init 2 */

}

/**
  * Enable DMA controller clock
  */
static void MX_DMA_Init(void)
{

  /* DMA controller clock enable */
  __HAL_RCC_DMA2_CLK_ENABLE();

  /* DMA interrupt init */
  /* DMA2_Stream2_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA2_Stream2_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA2_Stream2_IRQn);

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
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(green_GPIO_Port, green_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6|GPIO_PIN_7|GPIO_PIN_8, GPIO_PIN_SET);

  /*Configure GPIO pin : green_Pin */
  GPIO_InitStruct.Pin = green_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(green_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : PB6 PB7 PB8 */
  GPIO_InitStruct.Pin = GPIO_PIN_6|GPIO_PIN_7|GPIO_PIN_8;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /* USER CODE BEGIN MX_GPIO_Init_2 */

  /* USER CODE END MX_GPIO_Init_2 */
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
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}
#ifdef USE_FULL_ASSERT
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
