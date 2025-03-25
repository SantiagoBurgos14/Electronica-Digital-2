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

#include <stdint.h>

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

uint8_t p1 = 0;	// jugador 1
uint8_t p2 = 0;	// jugador 2
uint8_t inicio = 0;	// inica juego
uint8_t fin = 0;		// se acaba el juego
uint8_t p1Wins = 0;		// gana el jugador 1
uint8_t p2Wins = 0;		// gana el jugador 2
uint8_t display[16] = { 0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07, 0x7F,
		0x6F, 0x77, 0x7C, 0x39, 0x5E, 0x79, 0x71 };
uint8_t counter = 5;

typedef struct {
	GPIO_TypeDef *GPIOx;  // Puerto GPIO (Ej: GPIOA, GPIOB, etc.)
	uint16_t GPIO_Pin;    // Pin específico del LED
//uint8_t estado;       // Estado del LED (0 = apagado, 1 = encendido)
} LEDP1_t;

LEDP1_t J1[] = { { LED_A_GPIO_Port, LED_A_Pin }, { LED_B_GPIO_Port, LED_B_Pin },
		{ LED_C_GPIO_Port, LED_C_Pin }, { LED_D_GPIO_Port, LED_D_Pin } };

LEDP1_t J2[] = { { LED_E_GPIO_Port, LED_E_Pin }, { LED_F_GPIO_Port, LED_F_Pin },
		{ LED_G_GPIO_Port, LED_G_Pin }, { LED_DP_GPIO_Port, LED_DP_Pin } };

LEDP1_t Disp[] = { { LED_A_GPIO_Port, LED_A_Pin },
		{ LED_B_GPIO_Port, LED_B_Pin }, { LED_C_GPIO_Port, LED_C_Pin }, {
		LED_D_GPIO_Port, LED_D_Pin }, { LED_E_GPIO_Port, LED_E_Pin }, {
		LED_F_GPIO_Port, LED_F_Pin }, { LED_G_GPIO_Port, LED_G_Pin }, {
		LED_DP_GPIO_Port, LED_DP_Pin } };
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
void Mux1(void);
void Mux2(void);
/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
 * @brief  The application entry point.
 * @retval int
 */
int main(void) {

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
	/* USER CODE BEGIN 2 */

	/* USER CODE END 2 */

	/* Infinite loop */
	/* USER CODE BEGIN WHILE */
	while (1) {


		if (inicio == 1) {
			HAL_GPIO_WritePin(Disp_7S_GPIO_Port, Disp_7S_Pin, 1); // Enciende el transistor del display
			HAL_GPIO_WritePin(LEDS_GPIO_Port, LEDS_Pin, 0);	// Enciende el transistor de los leds

			for (int i = 5; i >= 0; i--) {
				for (int j = 0; j < 7; j++)
					HAL_GPIO_WritePin(Disp[j].GPIOx, Disp[j].GPIO_Pin,
							(display[i] >> j) & 1);

				HAL_Delay(250);
				for (int j = 0; j < 7; j++) {
					HAL_GPIO_WritePin(Disp[j].GPIOx, Disp[j].GPIO_Pin, 0);
				}

			}
			inicio = 2;
			HAL_GPIO_WritePin(LEDS_GPIO_Port, LEDS_Pin, 0);	// Apaga el transistor de los leds

		}


		if (p1Wins == 1) {
			// Muestra el número 2
			HAL_GPIO_WritePin(LEDS_GPIO_Port, LEDS_Pin, 0); // Apaga LEDs
			HAL_GPIO_WritePin(Disp_7S_GPIO_Port, Disp_7S_Pin, 1); // Enciende display

			uint8_t valor = display[1]; // Mostrar número 2
			for (int i = 0; i < 8; i++)
				HAL_GPIO_WritePin(Disp[i].GPIOx, Disp[i].GPIO_Pin, (valor >> i) & 1);

			HAL_Delay(5);

			HAL_GPIO_WritePin(Disp_7S_GPIO_Port, Disp_7S_Pin, 0); // Apaga display
			HAL_GPIO_WritePin(LEDS_GPIO_Port, LEDS_Pin, 1); // Enciende LEDs

			// Mostrar LEDs del jugador 2
			for (int i = 0; i < 4; i++)
				HAL_GPIO_WritePin(J1[i].GPIOx, J1[i].GPIO_Pin, 1);

			HAL_Delay(5);
		}

		if (p2Wins == 1) {
			// Muestra el número 2
			HAL_GPIO_WritePin(LEDS_GPIO_Port, LEDS_Pin, 0); // Apaga LEDs
			HAL_GPIO_WritePin(Disp_7S_GPIO_Port, Disp_7S_Pin, 1); // Enciende display

			uint8_t valor = display[2]; // Mostrar número 2
			for (int i = 0; i < 8; i++)
				HAL_GPIO_WritePin(Disp[i].GPIOx, Disp[i].GPIO_Pin, (valor >> i) & 1);

			HAL_Delay(5);

			HAL_GPIO_WritePin(Disp_7S_GPIO_Port, Disp_7S_Pin, 0); // Apaga display
			HAL_GPIO_WritePin(LEDS_GPIO_Port, LEDS_Pin, 1); // Enciende LEDs

			// Mostrar LEDs del jugador 2
			for (int i = 0; i < 4; i++){
			HAL_GPIO_WritePin(J2[i].GPIOx, J2[i].GPIO_Pin, 1);
			HAL_GPIO_WritePin(J1[i].GPIOx, J1[i].GPIO_Pin, 0);
			}

			HAL_Delay(5);
		}

		/* USER CODE BEGIN 3 */
	}


	/* USER CODE END 3 */
}

/**
 * @brief System Clock Configuration
 * @retval None
 */
void SystemClock_Config(void) {
	RCC_OscInitTypeDef RCC_OscInitStruct = { 0 };
	RCC_ClkInitTypeDef RCC_ClkInitStruct = { 0 };

	/** Configure the main internal regulator output voltage
	 */
	__HAL_RCC_PWR_CLK_ENABLE();
	__HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE3);

	/** Initializes the RCC Oscillators according to the specified parameters
	 * in the RCC_OscInitTypeDef structure.
	 */
	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
	RCC_OscInitStruct.HSIState = RCC_HSI_ON;
	RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
	RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
	RCC_OscInitStruct.PLL.PLLM = 8;
	RCC_OscInitStruct.PLL.PLLN = 80;
	RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
	RCC_OscInitStruct.PLL.PLLQ = 2;
	RCC_OscInitStruct.PLL.PLLR = 2;
	if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
		Error_Handler();
	}

	/** Initializes the CPU, AHB and APB buses clocks
	 */
	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK
			| RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

	if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK) {
		Error_Handler();
	}
}

/**
 * @brief GPIO Initialization Function
 * @param None
 * @retval None
 */
static void MX_GPIO_Init(void) {
	GPIO_InitTypeDef GPIO_InitStruct = { 0 };
	/* USER CODE BEGIN MX_GPIO_Init_1 */

	/* USER CODE END MX_GPIO_Init_1 */

	/* GPIO Ports Clock Enable */
	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_GPIOB_CLK_ENABLE();
	__HAL_RCC_GPIOC_CLK_ENABLE();

	/*Configure GPIO pin Output Level */
	HAL_GPIO_WritePin(GPIOA,
	LEDS_Pin | Disp_7S_Pin | LED_DP_Pin | TR_Pin | LED_C_Pin, GPIO_PIN_RESET);

	/*Configure GPIO pin Output Level */
	HAL_GPIO_WritePin(GPIOB,
	LED_G_Pin | LED_D_Pin | LED_F_Pin | LED_E_Pin | LED_B_Pin, GPIO_PIN_RESET);

	/*Configure GPIO pin Output Level */
	HAL_GPIO_WritePin(LED_A_GPIO_Port, LED_A_Pin, GPIO_PIN_RESET);

	/*Configure GPIO pins : BTN_START_Pin BTN_P1_Pin BTN_P2_Pin */
	GPIO_InitStruct.Pin = BTN_START_Pin | BTN_P1_Pin | BTN_P2_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

	/*Configure GPIO pins : LEDS_Pin Disp_7S_Pin LED_DP_Pin TR_Pin
	 LED_C_Pin */
	GPIO_InitStruct.Pin = LEDS_Pin | Disp_7S_Pin | LED_DP_Pin | TR_Pin
			| LED_C_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

	/*Configure GPIO pins : LED_G_Pin LED_D_Pin LED_F_Pin LED_E_Pin
	 LED_B_Pin */
	GPIO_InitStruct.Pin = LED_G_Pin | LED_D_Pin | LED_F_Pin | LED_E_Pin
			| LED_B_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

	/*Configure GPIO pin : LED_A_Pin */
	GPIO_InitStruct.Pin = LED_A_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(LED_A_GPIO_Port, &GPIO_InitStruct);

	/* EXTI interrupt init*/
	HAL_NVIC_SetPriority(EXTI0_IRQn, 0, 0);
	HAL_NVIC_EnableIRQ(EXTI0_IRQn);

	HAL_NVIC_SetPriority(EXTI1_IRQn, 0, 0);
	HAL_NVIC_EnableIRQ(EXTI1_IRQn);

	HAL_NVIC_SetPriority(EXTI4_IRQn, 0, 0);
	HAL_NVIC_EnableIRQ(EXTI4_IRQn);

	/* USER CODE BEGIN MX_GPIO_Init_2 */

	/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {

	// BOTON para iniciar////////////////////////
	if (GPIO_Pin == BTN_START_Pin && inicio == 0) {

		HAL_GPIO_WritePin(Disp_7S_GPIO_Port, Disp_7S_Pin, 1); // Enciende el tr5ansistor del display
		HAL_GPIO_WritePin(LEDS_GPIO_Port, LEDS_Pin, 0);	// APAGA el transistor de los leds
		inicio = 1;

	}
/////////////// JUGADOR 1 //////////////////
	/*if (GPIO_Pin == BTN_P1_Pin && inicio == 2) {

		HAL_GPIO_WritePin(J1[p1].GPIOx, J1[p1].GPIO_Pin, 1);
		HAL_GPIO_WritePin(LEDS_GPIO_Port, LEDS_Pin, 1);	// Enciende el transistor de los leds
		HAL_GPIO_WritePin(Disp_7S_GPIO_Port, Disp_7S_Pin, 0); // apaga el tr5ansistor del display

		if (p1 > 1){
		 HAL_GPIO_WritePin(J1[p1 - 1].GPIOx, J1[p1 - 1].GPIO_Pin, 0);
		 p1++;
		}
		 if (p1 >= 4) {

		 p1 = 3;
		 p2 = -1;
		 p1Wins = 1;
		 for (int i = 0; i < 4; i++)
		 HAL_GPIO_WritePin(J2[i].GPIOx, J2[i].GPIO_Pin, 0);
		 }
		 if (p1Wins == 1)
		 for (int i = 0; i < 4; i++)
		 HAL_GPIO_WritePin(J1[i].GPIOx, J1[i].GPIO_Pin, 1);

	}*/

	if (GPIO_Pin == BTN_P1_Pin && inicio == 2) {

		HAL_GPIO_WritePin(J1[p1].GPIOx, J1[p1].GPIO_Pin, 1);
		HAL_GPIO_WritePin(LEDS_GPIO_Port, LEDS_Pin, 1);
		HAL_GPIO_WritePin(Disp_7S_GPIO_Port, Disp_7S_Pin, 0);

		if (p1 > 0)
			HAL_GPIO_WritePin(J1[p1 - 1].GPIOx, J1[p1 - 1].GPIO_Pin, 0);

		p1++;

		if (p1 >= 4) {
			p1 = 3;
			p2 = -1;
			p1Wins = 1;
			for (int i = 0; i < 4; i++)
				HAL_GPIO_WritePin(J2[i].GPIOx, J2[i].GPIO_Pin, 0);
		}

		if (p1Wins == 1) {
			for (int i = 0; i < 4; i++)
				HAL_GPIO_WritePin(J1[i].GPIOx, J1[i].GPIO_Pin, 1);
		}
	}

///////////// JUGADOR 2 ///////////////////////////
	/*if (GPIO_Pin == BTN_P2_Pin && inicio == 2) {

		HAL_GPIO_WritePin(J2[p2].GPIOx, J2[p2].GPIO_Pin, 1);
		HAL_GPIO_WritePin(LEDS_GPIO_Port, LEDS_Pin, 1);	// Enciende el transistor de los leds
		HAL_GPIO_WritePin(Disp_7S_GPIO_Port, Disp_7S_Pin, 0); // Enciende el tr5ansistor del display

		if (p2 > 1){
		 HAL_GPIO_WritePin(J2[p2 - 1].GPIOx, J2[p2 - 1].GPIO_Pin, 0);
		 p2++;
		}
		 if (p2 >= 4) {

		 p2 = 3;
		 p1 = -1;
		 p2Wins = 1;
		 for (int i = 0; i < 4; i++)
		 HAL_GPIO_WritePin(J1[i].GPIOx, J1[i].GPIO_Pin, 0);

		 }
		 if (p2Wins == 1)
		 for (int i = 0; i < 4; i++)HAL_GPIO_WritePin(J2[i].GPIOx, J2[i].GPIO_Pin, 1);;

	}*/
	if (GPIO_Pin == BTN_P2_Pin && inicio == 2) {

		HAL_GPIO_WritePin(J2[p2].GPIOx, J2[p2].GPIO_Pin, 1);
		HAL_GPIO_WritePin(LEDS_GPIO_Port, LEDS_Pin, 1);
		HAL_GPIO_WritePin(Disp_7S_GPIO_Port, Disp_7S_Pin, 0);

		if (p2 > 0)
			HAL_GPIO_WritePin(J2[p2 - 1].GPIOx, J2[p2 - 1].GPIO_Pin, 0);

		p2++;

		if (p2 >= 4) {
			p2 = 3;
			p1 = -1;
			p2Wins = 1;
			for (int i = 0; i < 4; i++)
				HAL_GPIO_WritePin(J1[i].GPIOx, J1[i].GPIO_Pin, 0);
		}

		if (p2Wins == 1) {
			for (int i = 0; i < 4; i++)
				HAL_GPIO_WritePin(J2[i].GPIOx, J2[i].GPIO_Pin, 1);
		}
	}

}

void Mux1(void) {

	HAL_GPIO_WritePin(LEDS_GPIO_Port, LEDS_Pin, 0);	// apaga el transistor de los leds
	HAL_GPIO_WritePin(Disp_7S_GPIO_Port, Disp_7S_Pin, 1); // Enciende el transistor del display
	HAL_Delay(3);

	HAL_GPIO_WritePin(Disp_7S_GPIO_Port, Disp_7S_Pin, 0); // Enciende el transistor del display
	HAL_GPIO_WritePin(LEDS_GPIO_Port, LEDS_Pin, 1);	// Enciende el transistor de los leds
	HAL_Delay(3);

}

void Mux2(void) {
	HAL_GPIO_WritePin(LEDS_GPIO_Port, LEDS_Pin, 0);	// apaga el transistor de los leds
	HAL_GPIO_WritePin(Disp_7S_GPIO_Port, Disp_7S_Pin, 1); // Enciende el transistor del display
	HAL_Delay(3);

	HAL_GPIO_WritePin(Disp_7S_GPIO_Port, Disp_7S_Pin, 0); // Enciende el transistor del display
	HAL_GPIO_WritePin(LEDS_GPIO_Port, LEDS_Pin, 1);	// Enciende el transistor de los leds
	HAL_Delay(3);
}
/* USER CODE END 4 */

/**
 * @brief  This function is executed in case of error occurrence.
 * @retval None
 */
void Error_Handler(void) {
	/* USER CODE BEGIN Error_Handler_Debug */
	/* User can add his own implementation to report the HAL error return state */
	__disable_irq();
	while (1) {
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
