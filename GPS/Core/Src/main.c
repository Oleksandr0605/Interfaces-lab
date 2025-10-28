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
#include "bme280.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define DMA_RX_BUFFER_SIZE 256
#define GPS_PROCESS_BUFFER_SIZE 256
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
I2C_HandleTypeDef hi2c2;

SPI_HandleTypeDef hspi1;
DMA_HandleTypeDef hdma_spi1_tx;

UART_HandleTypeDef huart1;
DMA_HandleTypeDef hdma_usart1_rx;

/* USER CODE BEGIN PV */
float temperature;
float humidity;
float pressure;

float last_t;
float last_h;
float last_p;

struct bme280_dev dev;
struct bme280_data comp_data;
int8_t rslt;

char hum_string[50];
char temp_string[50];
char press_string[50];

uint8_t g_dma_rx_buffer[DMA_RX_BUFFER_SIZE];

uint8_t g_gps_process_buffer[GPS_PROCESS_BUFFER_SIZE];
volatile uint16_t g_gps_data_size = 0;
volatile uint8_t g_gps_data_ready = 0;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_SPI1_Init(void);
static void MX_I2C2_Init(void);
static void MX_USART1_UART_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
int8_t user_i2c_read(uint8_t id, uint8_t reg_addr, uint8_t *data, uint16_t len)
{
  if(HAL_I2C_Master_Transmit(&hi2c2, (id << 1), &reg_addr, 1, 10) != HAL_OK) return -1;
  if(HAL_I2C_Master_Receive(&hi2c2, (id << 1) | 0x01, data, len, 10) != HAL_OK) return -1;

  return 0;
}

void user_delay_ms(uint32_t period)
{
  HAL_Delay(period);
}

int8_t user_i2c_write(uint8_t id, uint8_t reg_addr, uint8_t *data, uint16_t len)
{
  int8_t *buf;
  buf = malloc(len +1);
  buf[0] = reg_addr;
  memcpy(buf +1, data, len);

  if(HAL_I2C_Master_Transmit(&hi2c2, (id << 1), (uint8_t*)buf, len + 1, HAL_MAX_DELAY) != HAL_OK) return -1;

  free(buf);
  return 0;
}

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
  MX_I2C2_Init();
  MX_USART1_UART_Init();
  /* USER CODE BEGIN 2 */
  Init();
  dev.dev_id = BME280_I2C_ADDR_PRIM;
  dev.intf = BME280_I2C_INTF;
  dev.read = user_i2c_read;
  dev.write = user_i2c_write;
  dev.delay_ms = user_delay_ms;
  rslt = bme280_init(&dev);
  if (rslt != BME280_OK)
  {
      Error_Handler();
  }
  if(HAL_UARTEx_ReceiveToIdle_DMA(&huart1, g_dma_rx_buffer, DMA_RX_BUFFER_SIZE) != HAL_OK) {
	  Error_Handler();
  }
  __HAL_DMA_DISABLE_IT(huart1.hdmarx, DMA_IT_HT);
  dev.settings.osr_h = BME280_OVERSAMPLING_1X;
  dev.settings.osr_p = BME280_OVERSAMPLING_16X;
  dev.settings.osr_t = BME280_OVERSAMPLING_2X;
  dev.settings.filter = BME280_FILTER_COEFF_16;
  uint8_t settings_sel = BME280_OSR_PRESS_SEL | BME280_OSR_TEMP_SEL | BME280_OSR_HUM_SEL | BME280_FILTER_SEL;
  rslt = bme280_set_sensor_settings(settings_sel, &dev);
  if (rslt != BME280_OK)
  {
      Error_Handler();
  }
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
   SetRotation(SCREEN_HORIZONTAL_1);

   FillScreen(WHITE);

   DrawText("Device ID:", FONT2, 10, 10, BLACK, WHITE);
   DrawText("Coordinates:", FONT2, 10, 40, BLACK, WHITE);
   DrawText("Time (UTC):", FONT2, 10, 70, BLACK, WHITE);
   DrawText("GPS Info:", FONT2, 10, 100, BLACK, WHITE);

   uint32_t device_id_word0 = HAL_GetUIDw0();
   char id_str[20];
   sprintf(id_str, "0x%lX", device_id_word0);
   DrawText(id_str, FONT2, 110, 10, BLUE, WHITE);

   char coords_buffer[45];
   char time_buffer[20];

  while (1)
  {
	  rslt = bme280_set_sensor_mode(BME280_FORCED_MODE, &dev);
	  dev.delay_ms(40);
	  rslt = bme280_get_sensor_data(BME280_ALL, &comp_data, &dev);

	  if (g_gps_data_ready) {
		  char* line = strtok((char*)g_gps_process_buffer, "\r\n");

		            while (line != NULL)
		            {
		                if (strncmp(line, "$GNGGA", 6) == 0 ||
		                    strncmp(line, "$GPGGA", 6) == 0) {

		                    float nmea_time, nmea_lat, nmea_lon, altitude;
		                    char lat_dir, lon_dir;
		                    int fix_quality, num_sats;
		                    int hh, mm, ss;
		                    int lat_deg, lon_deg;
		                    float lat_min, lon_min;

		                    char sat_alt_buffer[40];

		                    // Find the first comma in THIS line
		                    const char* data_start = strchr(line, ',');
		                    if (data_start) {
		                        // Parse the data from this line
		                        int items = sscanf(data_start + 1, "%f,%f,%c,%f,%c,%d,%d,%*f,%f,",
		                                           &nmea_time, &nmea_lat, &lat_dir, &nmea_lon, &lon_dir,
		                                           &fix_quality, &num_sats, &altitude);

		                        if (items >= 8 && fix_quality > 0) {

		                            hh = (int)nmea_time / 10000;
		                            mm = ((int)nmea_time / 100) % 100;
		                            ss = (int)nmea_time % 100;
		                            sprintf(time_buffer, "%02d:%02d:%02d ", hh, mm, ss);


		                            lat_deg = (int)nmea_lat / 100;
		                            lat_min = nmea_lat - (lat_deg * 100);
		                            lon_deg = (int)nmea_lon / 100;
		                            lon_min = nmea_lon - (lon_deg * 100);

		                            sprintf(coords_buffer, "%c %d°%07.4f', %c %d°%07.4f'",
		                                      lat_dir, lat_deg, lat_min,
		                                      lon_dir, lon_deg, lon_min);


		                            sprintf(sat_alt_buffer, "Sats: %02d  Alt: %.1f m  ", num_sats, altitude);


		                            DrawText(coords_buffer, FONT2, 10, 55, RED, WHITE);
		                            DrawText(time_buffer, FONT2, 10, 85, RED, WHITE);
		                            DrawText(sat_alt_buffer, FONT2, 10, 115, RED, WHITE);

		                        } else {

		                            DrawText("No GPS Fix          ", FONT2, 10, 55, RED, WHITE);
		                            DrawText("Waiting...          ", FONT2, 10, 85, RED, WHITE);
		                            DrawText("Sats: --  Alt: --- m ", FONT2, 10, 115, RED, WHITE);
		                        }
		                    }
		                }

		                line = strtok(NULL, "\r\n");
		            }


		  g_gps_data_ready = 0;
	  }

	  if(rslt == BME280_OK)
		{
		  temperature = comp_data.temperature / 100.0;
		  humidity = comp_data.humidity / 1024.0;
		  pressure = comp_data.pressure / 10000.0;

		  last_t = temperature;
		  last_h = humidity;
		  last_p = pressure;

		  /*Display Data */
		  memset(hum_string, 0, sizeof(hum_string));
		  memset(temp_string, 0, sizeof(temp_string));
		  memset(press_string, 0, sizeof(press_string));

		  sprintf(hum_string, "Humidity %03.1f %%       ", humidity);
		  sprintf(temp_string, "Temperature %03.1f C      ", temperature);
		  sprintf(press_string, "Pressure %03.1f hPa       ", pressure);

		  DrawText(temp_string, FONT2, 10, 135, RED, WHITE);
		  DrawText(hum_string, FONT2, 10, 155, RED, WHITE);
		  DrawText(press_string, FONT2, 10, 175, RED, WHITE);
		} else {
		  memset(hum_string, 0, sizeof(hum_string));
		  memset(temp_string, 0, sizeof(temp_string));
		  memset(press_string, 0, sizeof(press_string));

		  sprintf(hum_string, "stale Humidity %03.1f %% ", last_h);
		  sprintf(temp_string, "stale Temperature %03.1f C ", last_t);
		  sprintf(press_string, "stale Pressure %03.1f hPa ", last_p);

		  DrawText(temp_string, FONT2, 10, 135, RED, WHITE);
		  DrawText(hum_string, FONT2, 10, 155, RED, WHITE);
		  DrawText(press_string, FONT2, 10, 175, RED, WHITE);
		}
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
  * @brief I2C2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C2_Init(void)
{

  /* USER CODE BEGIN I2C2_Init 0 */

  /* USER CODE END I2C2_Init 0 */

  /* USER CODE BEGIN I2C2_Init 1 */

  /* USER CODE END I2C2_Init 1 */
  hi2c2.Instance = I2C2;
  hi2c2.Init.ClockSpeed = 100000;
  hi2c2.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c2.Init.OwnAddress1 = 0;
  hi2c2.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c2.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c2.Init.OwnAddress2 = 0;
  hi2c2.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c2.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C2_Init 2 */

  /* USER CODE END I2C2_Init 2 */

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
  huart1.Init.BaudRate = 9600;
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
  /* DMA2_Stream5_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA2_Stream5_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA2_Stream5_IRQn);

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
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();

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

void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size) {
	if (huart->Instance == USART1) {

		if (Size > 0 && Size < GPS_PROCESS_BUFFER_SIZE) {
			memcpy(g_gps_process_buffer, g_dma_rx_buffer, Size);
			g_gps_process_buffer[Size] = '\0';

			g_gps_data_size = Size;
			g_gps_data_ready = 1;
		}

		HAL_UART_AbortReceive(&huart1);
		if (HAL_UARTEx_ReceiveToIdle_DMA(&huart1, g_dma_rx_buffer, DMA_RX_BUFFER_SIZE) != HAL_OK) {
			Error_Handler();
		}
		__HAL_DMA_DISABLE_IT(huart1.hdmarx, DMA_IT_HT);
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
