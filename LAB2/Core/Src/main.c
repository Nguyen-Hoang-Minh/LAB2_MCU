/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "software_timer.h"
#include <stdlib.h>
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
TIM_HandleTypeDef htim2;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_TIM2_Init(void);
/* USER CODE BEGIN PFP */

uint8_t matrix_buffer[8] = {0x18,0x24,0x42,0x42,0x7E,0x42,0x42,0x42};
uint8_t stored_buffer[8] = {0x18,0x24,0x42,0x42,0x7E,0x42,0x42,0x42};
const int MAX_LED_MATRIX = 8;

const int MAX_LED = 4;
int index_led = 0;
int led_buffer[4] = {0, 0, 0, 0};
void update_clock_buffer(int hour, int minute);
void init_clock();
void display7SEG(int number);
void update_display(int* display_flag);
void reset_all_row();
void display_row(uint8_t data);
void updateLEDMatrix(int* index, uint8_t* matrix_buffer);
void update_LED_buffer(uint8_t data[MAX_LED_MATRIX]);
void reset_buffer();

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
  MX_TIM2_Init();
  /* USER CODE BEGIN 2 */
  HAL_TIM_Base_Start_IT (& htim2 );
  set_Timer1(50);//counter for 7-segment LED
  set_Timer2(100);//counter for 2 LEDs
  set_Timer3(450);//counter for shilf led
  int display_flag = 0;//indicate for the first 7LED (number 1)
  int matrix_flag = 0;
  int shift_cycle_flag = 0;
  int hour = 23, minute = 59, second = 50;
    led_buffer[0] = hour/10;
    led_buffer[1] = hour%10;
    led_buffer[2] = minute/10;
    led_buffer[3] = minute%10;
    init_clock();

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
	  //HAL_GPIO_WritePin(GPIOA, ENM1_Pin, GPIO_PIN_RESET);
	  //HAL_GPIO_WritePin(GPIOB, ROW0_Pin|ROW1_Pin, GPIO_PIN_RESET);
	  if(timer_flag1 == 1){
		  set_Timer1(50);
	 	  update_display(&display_flag);
	 	  updateLEDMatrix(&matrix_flag, matrix_buffer);
	  }
	  if(timer_flag2 == 1){
	 	  set_Timer2(100);
	 	  HAL_GPIO_TogglePin(GPIOA, DOT_Pin);
	 	  HAL_GPIO_TogglePin(GPIOA, LED_RED_Pin);
	 	 second++;
	 	 		  if(second>=60){
	 	 			  second = 0;
	 	 			  minute++;
	 	 		  }
	 	 		  if(minute>=60){
	 	 			  minute = 0;
	 	 			  hour++;
	 	 		  }
	 	 		  if(hour>=24){
	 	 			  hour = 0;
	 	 		  }
	 	 		  update_clock_buffer(hour, minute);
	   }
	  if(timer_flag3 == 1){
		  set_Timer3(450);
		  shift_cycle_flag++;
		  update_LED_buffer(matrix_buffer);
		  if(shift_cycle_flag >= 8){
			  shift_cycle_flag = 0;
			  reset_buffer();
		  }
	  }
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
  * @brief TIM2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM2_Init(void)
{

  /* USER CODE BEGIN TIM2_Init 0 */

  /* USER CODE END TIM2_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM2_Init 1 */

  /* USER CODE END TIM2_Init 1 */
  htim2.Instance = TIM2;
  htim2.Init.Prescaler = 7999;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = 9;
  htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim2) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim2, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM2_Init 2 */

  /* USER CODE END TIM2_Init 2 */

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
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, ENM0_Pin|ENM1_Pin|DOT_Pin|LED_RED_Pin
                          |EN0_Pin|EN1_Pin|EN2_Pin|EN3_Pin
                          |ENM2_Pin|ENM3_Pin|ENM4_Pin|ENM5_Pin
                          |ENM6_Pin|ENM7_Pin, GPIO_PIN_SET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, SEG_A_Pin|SEG_B_Pin|SEG_C_Pin|ROW2_Pin
                          |ROW3_Pin|ROW4_Pin|ROW5_Pin|ROW6_Pin
                          |ROW7_Pin|SEG_D_Pin|SEG_E_Pin|SEG_F_Pin
                          |SEG_G_Pin|ROW0_Pin|ROW1_Pin, GPIO_PIN_SET);

  /*Configure GPIO pins : ENM0_Pin ENM1_Pin DOT_Pin LED_RED_Pin
                           EN0_Pin EN1_Pin EN2_Pin EN3_Pin
                           ENM2_Pin ENM3_Pin ENM4_Pin ENM5_Pin
                           ENM6_Pin ENM7_Pin */
  GPIO_InitStruct.Pin = ENM0_Pin|ENM1_Pin|DOT_Pin|LED_RED_Pin
                          |EN0_Pin|EN1_Pin|EN2_Pin|EN3_Pin
                          |ENM2_Pin|ENM3_Pin|ENM4_Pin|ENM5_Pin
                          |ENM6_Pin|ENM7_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : SEG_A_Pin SEG_B_Pin SEG_C_Pin ROW2_Pin
                           ROW3_Pin ROW4_Pin ROW5_Pin ROW6_Pin
                           ROW7_Pin SEG_D_Pin SEG_E_Pin SEG_F_Pin
                           SEG_G_Pin ROW0_Pin ROW1_Pin */
  GPIO_InitStruct.Pin = SEG_A_Pin|SEG_B_Pin|SEG_C_Pin|ROW2_Pin
                          |ROW3_Pin|ROW4_Pin|ROW5_Pin|ROW6_Pin
                          |ROW7_Pin|SEG_D_Pin|SEG_E_Pin|SEG_F_Pin
                          |SEG_G_Pin|ROW0_Pin|ROW1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim){
	timerRun();
}

void display7SEG(int number){
	int a[10][7] = {
		{0, 0, 0, 0, 0, 0, 1}, 	//0
		{1, 0, 0, 1, 1, 1, 1},  //1
		{0, 0, 1, 0, 0, 1, 0},  //2
		{0, 0, 0, 0, 1, 1, 0},  //3
		{1, 0, 0, 1, 1, 0, 0},	//4
		{0, 1, 0, 0, 1, 0, 0},  //5
		{0, 1, 0, 0, 0, 0, 0}, 	//6
		{0, 0, 0, 1, 1, 1, 1},	//7
		{0, 0, 0, 0, 0, 0, 0}, 	//8
		{0, 0, 0, 0, 1, 0, 0}   //9
	};
	HAL_GPIO_WritePin(GPIOB, SEG_A_Pin, a[number][0]);
	HAL_GPIO_WritePin(GPIOB, SEG_B_Pin, a[number][1]);
	HAL_GPIO_WritePin(GPIOB, SEG_C_Pin, a[number][2]);
	HAL_GPIO_WritePin(GPIOB, SEG_D_Pin, a[number][3]);
	HAL_GPIO_WritePin(GPIOB, SEG_E_Pin, a[number][4]);
	HAL_GPIO_WritePin(GPIOB, SEG_F_Pin, a[number][5]);
	HAL_GPIO_WritePin(GPIOB, SEG_G_Pin, a[number][6]);

}

void update_display(int* display_flag){
	switch (*display_flag){
		case 0:{
			(*display_flag)++;
			HAL_GPIO_WritePin(GPIOA, EN0_Pin, GPIO_PIN_SET);
			HAL_GPIO_WritePin(GPIOA, EN1_Pin, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(GPIOA, EN2_Pin, GPIO_PIN_SET);
			HAL_GPIO_WritePin(GPIOA, EN3_Pin, GPIO_PIN_SET);
			display7SEG(led_buffer[1]);
			break;
		}
		case 1:{
			(*display_flag)++;
			HAL_GPIO_WritePin(GPIOA, EN0_Pin, GPIO_PIN_SET);
			HAL_GPIO_WritePin(GPIOA, EN1_Pin, GPIO_PIN_SET);
			HAL_GPIO_WritePin(GPIOA, EN2_Pin, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(GPIOA, EN3_Pin, GPIO_PIN_SET);
			display7SEG(led_buffer[2]);
			break;
		}
		case 2:{
			(*display_flag)++;
			HAL_GPIO_WritePin(GPIOA, EN0_Pin, GPIO_PIN_SET);
			HAL_GPIO_WritePin(GPIOA, EN1_Pin, GPIO_PIN_SET);
			HAL_GPIO_WritePin(GPIOA, EN2_Pin, GPIO_PIN_SET);
			HAL_GPIO_WritePin(GPIOA, EN3_Pin, GPIO_PIN_RESET);
			display7SEG(led_buffer[3]);
			break;
		}
		case 3:{
			(*display_flag) = 0;
			HAL_GPIO_WritePin(GPIOA, EN0_Pin, GPIO_PIN_RESET);
			HAL_GPIO_WritePin(GPIOA, EN1_Pin, GPIO_PIN_SET);
			HAL_GPIO_WritePin(GPIOA, EN2_Pin, GPIO_PIN_SET);
			HAL_GPIO_WritePin(GPIOA, EN3_Pin, GPIO_PIN_SET);
			display7SEG(led_buffer[0]);
			break;
		}
		default:{
			break;
		}
	}
}

uint16_t col_trigger[] = {ENM0_Pin, ENM1_Pin, ENM2_Pin, ENM3_Pin, ENM4_Pin, ENM5_Pin, ENM6_Pin, ENM7_Pin};
uint16_t row_trigger[] = {ROW0_Pin, ROW1_Pin, ROW2_Pin, ROW3_Pin, ROW4_Pin, ROW5_Pin, ROW6_Pin, ROW7_Pin};

int index_led_matrix = 0;

void updateLEDMatrix(int* matrix_flag, uint8_t* matrix_buffer){
    switch (*matrix_flag){
        case 0:{
        	(*matrix_flag)++;
        	reset_all_row();
        	HAL_GPIO_WritePin(GPIOB, ROW0_Pin, 0);
        	display_row(matrix_buffer[0]);
        	break;
        }
        case 1:{
        	(*matrix_flag)++;
        	reset_all_row();
        	HAL_GPIO_WritePin(GPIOB, ROW1_Pin, 0);
        	display_row(matrix_buffer[1]);
        	break;
        }
        case 2:{
        	(*matrix_flag)++;
        	reset_all_row();
        	HAL_GPIO_WritePin(GPIOB, ROW2_Pin, 0);
        	display_row(matrix_buffer[2]);
        	break;
        }
        case 3:{
        	(*matrix_flag)++;
        	reset_all_row();
        	HAL_GPIO_WritePin(GPIOB, ROW3_Pin, 0);
        	display_row(matrix_buffer[3]);
        	break;
        }
        case 4:{
        	(*matrix_flag)++;
        	reset_all_row();
        	HAL_GPIO_WritePin(GPIOB, ROW4_Pin, 0);
        	display_row(matrix_buffer[4]);
        	break;
        }
        case 5:{
        	(*matrix_flag)++;
        	reset_all_row();
        	HAL_GPIO_WritePin(GPIOB, ROW5_Pin, 0);
        	display_row(matrix_buffer[5]);
        	break;
        }
        case 6:{
        	(*matrix_flag)++;
        	reset_all_row();
        	HAL_GPIO_WritePin(GPIOB, ROW6_Pin, 0);
        	display_row(matrix_buffer[6]);
        	break;
        }
        case 7:{
        	(*matrix_flag) = 0;
        	reset_all_row();
        	HAL_GPIO_WritePin(GPIOB, ROW7_Pin, 0);
        	display_row(matrix_buffer[7]);
        	break;
        }
        default:
            break;
    }
}

void display_row(uint8_t data){
	for(int i = 0; i<MAX_LED_MATRIX; i++){
		HAL_GPIO_WritePin(GPIOA, col_trigger[i], !(data&0x80));
		data = data<<1;
	}
}

void set_row(){
	HAL_GPIO_WritePin(GPIOB, row_trigger[0], GPIO_PIN_RESET);
	for(int i = 0; i<MAX_LED_MATRIX; i++){
			HAL_GPIO_WritePin(GPIOA, col_trigger[i], GPIO_PIN_SET);
	}

}
void reset_all_row(){
	for(int i = 0; i<MAX_LED_MATRIX; i++){
		HAL_GPIO_WritePin(GPIOB, row_trigger[i], GPIO_PIN_SET);
	}
}

void update_LED_buffer(uint8_t data[MAX_LED_MATRIX]){
	for(int i = 0; i<MAX_LED_MATRIX; i++){
		data[i] = data[i]<<1;
	}
}

void reset_buffer(){
	for(int i = 0; i<MAX_LED_MATRIX; i++){
		matrix_buffer[i] = stored_buffer[i];
	}
}

void init_clock(){
	HAL_GPIO_WritePin(GPIOA, EN0_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOA, EN1_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOA, EN2_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOA, EN3_Pin, GPIO_PIN_SET);
	display7SEG(led_buffer[0]);
}
void update_clock_buffer(int hour, int minute){
	led_buffer[0] = hour/10;
	led_buffer[1] = hour%10;
	led_buffer[2] = minute/10;
	led_buffer[3] = minute%10;
	if(led_buffer[3]>=10){
		led_buffer[3] = 0;
		led_buffer[2]++;
	}
	if(led_buffer[2]>=6){
		led_buffer[1]++;
		led_buffer[2] =0;
	}
	if(led_buffer[1]>=10){
		led_buffer[0]++;
		led_buffer[1] =0;
	}
	if(led_buffer[0] == 2 && led_buffer[1] == 4){
		led_buffer[0] = 0;
		led_buffer[1] = 0;
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

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
