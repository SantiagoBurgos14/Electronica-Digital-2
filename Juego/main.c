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
#include "fatfs.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "ili9341.h"
#include "fatfs_sd.h"
#include "string.h"
#include "stdio.h"
#include "bitmaps.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
SPI_HandleTypeDef hspi1;
FATFS fs;
FATFS *pfs;
FIL fil;
FRESULT fres;
DWORD fre_clust;
uint32_t totalSpace, freeSpace;

char buffer[100];
#define MENU_JUGAR 0
#define MENU_PERSONAJE 1
int opcion_actual = MENU_JUGAR;

extern unsigned char fondo[];



extern uint8_t vaquero[205 * 42 * 2];
extern uint8_t vaquero2[205 * 42 * 2];
extern uint8_t bala[13 * 20 * 2];
extern uint8_t shoot1[4 * 4 * 2];
extern uint8_t shoot2[4 * 4 * 2];
extern uint8_t ronda1[15 * 15 * 2];
extern uint8_t ronda2[15 * 15 * 2];
extern uint8_t ronda3[15 * 15 * 2];

volatile uint8_t tecla_uart2 = 0;
volatile uint8_t tecla_uart5 = 0;
uint8_t enviado_desde_menu = 0;

uint32_t ultimo_envio_menu = 0;     // Para temporizar envíos de 'P'
uint8_t enviado_inicio_juego = 0;   // Para evitar múltiples envíos de 'K'

uint32_t tiempo_ultimo_disparoV1 = 0;
uint32_t tiempo_ultimo_disparoV2 = 0;
uint32_t intervalo_disparo = 1; // milisegundos


char direccion_jugador1 = 0;
char direccion_jugador2 = 0;

//extern unsigned char newcowboy1[];

int rondas_jugador1 = 0;
int rondas_jugador2 = 0;
int rondas_jugadas = 0;
int juego_activo = 1;  // 1: sí se aceptan entradas, 0: esperando
uint32_t tiempo_fin_ronda = 0;

uint8_t ganador_mostrado = 0;

uint8_t bloquear_entrada_tecla = 0;







int colision(int x1, int y1, int w1, int h1, int x2, int y2, int w2, int h2) {
	return !(x1 + w1 < x2 || x1 > x2 + w2 || y1 + h1 < y2 || y1 > y2 + h2);
}

int sprite_x = 20, sprite_y = 100;
int sprite_x2 = 260, sprite_y2 = 100;

int sprite_w = 41, sprite_h = 42;
int sprite_w2 = 41, sprite_h2 = 42;

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
SPI_HandleTypeDef hspi1;

UART_HandleTypeDef huart4;
UART_HandleTypeDef huart5;
UART_HandleTypeDef huart2;

/* USER CODE BEGIN PV */
char msg = 'p';

typedef enum {
	MENU, DOS_JUGADORES, FIN_JUEGO, PAUSA
// otros estados posibles
} GameState;

GameState estado_actual = MENU;

// Fuera del main, declaras tu estructura:
typedef struct {
	int x;
	int y;
	int activo;
} Disparo;

Disparo disparoV1 = { 0, 0, 0 };

typedef enum {
	SPRITE_IDLE = 0,
	SPRITE_DISPARANDO = 1,
	SPRITE_HERIDO1 = 2,
	SPRITE_HERIDO2 = 3,
	SPRITE_HERIDO3 = 4
} EstadoSprite;

EstadoSprite estado_vao = SPRITE_IDLE;
EstadoSprite estado_vao2 = SPRITE_IDLE;
uint32_t tiempo_ultimo_cambio = 0;

typedef struct {
	int x;
	int y;
	int activo;
} Disparo2;

Disparo2 disparoV2 = { 0, 0, 0 };

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
void MX_GPIO_Init(void);
void MX_SPI1_Init(void);
void MX_USART2_UART_Init(void);
void MX_UART4_Init(void);
void MX_UART5_Init(void);
/* USER CODE BEGIN PFP */

void SD_Montar(void);
void SD_Abrir(const char *filename, BYTE mode);
void SD_Cerrar(void);
void SD_Desmontar(void);
void SD_Imprimir(const char *filename);
void MostrarMenuConFlecha(int seleccion);

void SD_RestaurarFondoDesdeFlash(int x, int y, int w, int h);

void Tick_2Jugadores(char tecla);
void IniciarRonda(void);

void MostrarNumeroRonda(int numero);

void ProcesarTecla(char tecla);
void ProcesarTeclaJugador1(char tecla);
void ProcesarTeclaJugador2(char tecla);
void ReiniciarJuego(void);

void LimpiarTeclasUART(void);

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
void transmit_uart(const char *string) {
	uint8_t len = strlen(string);
	HAL_UART_Transmit(&huart2, (uint8_t*) string, len, 200);
}

char recibir_uart(void) {
	char rx;
	HAL_UART_Receive(&huart2, (uint8_t*) &rx, 1, HAL_MAX_DELAY);
	return rx;
}




void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
    if (bloquear_entrada_tecla) {
        bloquear_entrada_tecla = 0;  // Solo se bloquea una vez
        HAL_UART_Receive_IT(&huart2, (uint8_t*)&tecla_uart2, 1);
        HAL_UART_Receive_IT(&huart5, (uint8_t*)&tecla_uart5, 1);
        return;
    }

    if (huart->Instance == USART2) {
        uint8_t tecla = tecla_uart2;
        HAL_UART_Receive_IT(&huart2, (uint8_t*)&tecla_uart2, 1);
        ProcesarTeclaJugador1(tecla);
    } else if (huart->Instance == UART5) {
        uint8_t tecla = tecla_uart5;
        HAL_UART_Receive_IT(&huart5, (uint8_t*)&tecla_uart5, 1);
        ProcesarTecla(tecla);
        ProcesarTeclaJugador1(tecla);
        ProcesarTeclaJugador2(tecla);
    }
}




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
	MX_SPI1_Init();
	MX_FATFS_Init();
	MX_USART2_UART_Init();
	MX_UART4_Init();
	MX_UART5_Init();
	/* USER CODE BEGIN 2 */

	HAL_UART_Receive_IT(&huart2, (uint8_t*) &tecla_uart2, 1);
	HAL_UART_Receive_IT(&huart5, (uint8_t*) &tecla_uart5, 1);

	LCD_Init();
	SD_Montar();

	LCD_Bitmap(0, 0, 320, 240, fondo);  // Mostrar fondo

	LeerSpriteDesdeSD("newcowboy1.bin", 41, 42, 5); // Carga 5 sprites de 41x42 en `vaquero`

	LeerSpriteDesdeSD2("nuevocowboy2.bin", 41, 42, 5);
	LeerSpriteDesdeSDBala("bulletYtumba.bin", 13, 20, 1, 2);
	LeerSpriteDesdeSDDisparo("bala.bin", 4, 4, 0, 1);

	LeerSpriteRonda("Rondas15.bin", 15, 15, 0, 3, ronda1);
	LeerSpriteRonda("Rondas15.bin", 15, 15, 1, 3, ronda2);
	LeerSpriteRonda("Rondas15.bin", 15, 15, 2, 3, ronda3);

	MostrarMenuConFlecha(opcion_actual);  // Mostrar el menú con la flecha


	/* USER CODE END 2 */

	/* Infinite loop */
	/* USER CODE BEGIN WHILE */
	while (1)
	{
		/* USER CODE END WHILE */
		if (estado_actual == DOS_JUGADORES) {
		        Tick_2Jugadores(0);  // Solo ejecuta la lógica del juego cuando se está jugando
		    }

		    uint32_t ahora = HAL_GetTick();

		    // Enviar 'P' cada 500 ms mientras estás en el menú
		    if (estado_actual == MENU) {
		        if (ahora - ultimo_envio_menu > 500) {
		            char msg = 'P';
		            HAL_UART_Transmit(&huart4, (uint8_t*) &msg, 1, HAL_MAX_DELAY);
		            ultimo_envio_menu = ahora;
		        }
			/* USER CODE BEGIN 3 */
		}
	}
}
		/* USER CODE END 3 */

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
			RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK
					| RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1
					| RCC_CLOCKTYPE_PCLK2;
			RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
			RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
			RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
			RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

			if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2)
					!= HAL_OK) {
				Error_Handler();
			}
		}

		/**
		 * @brief SPI1 Initialization Function
		 * @param None
		 * @retval None
		 */
		void MX_SPI1_Init(void) {

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
			if (HAL_SPI_Init(&hspi1) != HAL_OK) {
				Error_Handler();
			}
			/* USER CODE BEGIN SPI1_Init 2 */

			/* USER CODE END SPI1_Init 2 */

		}

		/**
		 * @brief UART4 Initialization Function
		 * @param None
		 * @retval None
		 */
		void MX_UART4_Init(void) {

			/* USER CODE BEGIN UART4_Init 0 */

			/* USER CODE END UART4_Init 0 */

			/* USER CODE BEGIN UART4_Init 1 */

			/* USER CODE END UART4_Init 1 */
			huart4.Instance = UART4;
			huart4.Init.BaudRate = 115200;
			huart4.Init.WordLength = UART_WORDLENGTH_8B;
			huart4.Init.StopBits = UART_STOPBITS_1;
			huart4.Init.Parity = UART_PARITY_NONE;
			huart4.Init.Mode = UART_MODE_TX_RX;
			huart4.Init.HwFlowCtl = UART_HWCONTROL_NONE;
			huart4.Init.OverSampling = UART_OVERSAMPLING_16;
			if (HAL_UART_Init(&huart4) != HAL_OK) {
				Error_Handler();
			}
			/* USER CODE BEGIN UART4_Init 2 */

			/* USER CODE END UART4_Init 2 */

		}

		/**
		 * @brief UART5 Initialization Function
		 * @param None
		 * @retval None
		 */
		void MX_UART5_Init(void) {

			/* USER CODE BEGIN UART5_Init 0 */

			/* USER CODE END UART5_Init 0 */

			/* USER CODE BEGIN UART5_Init 1 */

			/* USER CODE END UART5_Init 1 */
			huart5.Instance = UART5;
			huart5.Init.BaudRate = 9600;
			huart5.Init.WordLength = UART_WORDLENGTH_8B;
			huart5.Init.StopBits = UART_STOPBITS_1;
			huart5.Init.Parity = UART_PARITY_NONE;
			huart5.Init.Mode = UART_MODE_TX_RX;
			huart5.Init.HwFlowCtl = UART_HWCONTROL_NONE;
			huart5.Init.OverSampling = UART_OVERSAMPLING_16;
			if (HAL_UART_Init(&huart5) != HAL_OK) {
				Error_Handler();
			}
			/* USER CODE BEGIN UART5_Init 2 */

			/* USER CODE END UART5_Init 2 */

		}

		/**
		 * @brief USART2 Initialization Function
		 * @param None
		 * @retval None
		 */
		void MX_USART2_UART_Init(void) {

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
			if (HAL_UART_Init(&huart2) != HAL_OK) {
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
		void MX_GPIO_Init(void) {
			GPIO_InitTypeDef GPIO_InitStruct = { 0 };
			/* USER CODE BEGIN MX_GPIO_Init_1 */
			/* USER CODE END MX_GPIO_Init_1 */

			/* GPIO Ports Clock Enable */
			__HAL_RCC_GPIOH_CLK_ENABLE();
			__HAL_RCC_GPIOC_CLK_ENABLE();
			__HAL_RCC_GPIOA_CLK_ENABLE();
			__HAL_RCC_GPIOB_CLK_ENABLE();
			__HAL_RCC_GPIOD_CLK_ENABLE();

			/*Configure GPIO pin Output Level */
			HAL_GPIO_WritePin(GPIOC, LCD_RST_Pin | LCD_D1_Pin, GPIO_PIN_RESET);

			/*Configure GPIO pin Output Level */
			HAL_GPIO_WritePin(GPIOA,
					LCD_RD_Pin | LCD_WR_Pin | LCD_RS_Pin | LCD_D7_Pin
							| LCD_D0_Pin | LCD_D2_Pin, GPIO_PIN_RESET);

			/*Configure GPIO pin Output Level */
			HAL_GPIO_WritePin(GPIOB,
					LCD_CS_Pin | LCD_D6_Pin | LCD_D3_Pin | LCD_D5_Pin
							| LCD_D4_Pin, GPIO_PIN_RESET);

			/*Configure GPIO pin Output Level */
			HAL_GPIO_WritePin(SD_SS_GPIO_Port, SD_SS_Pin, GPIO_PIN_SET);

			/*Configure GPIO pins : LCD_RST_Pin LCD_D1_Pin */
			GPIO_InitStruct.Pin = LCD_RST_Pin | LCD_D1_Pin;
			GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
			GPIO_InitStruct.Pull = GPIO_NOPULL;
			GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
			HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

			/*Configure GPIO pins : LCD_RD_Pin LCD_WR_Pin LCD_RS_Pin LCD_D7_Pin
			 LCD_D0_Pin LCD_D2_Pin */
			GPIO_InitStruct.Pin = LCD_RD_Pin | LCD_WR_Pin | LCD_RS_Pin
					| LCD_D7_Pin | LCD_D0_Pin | LCD_D2_Pin;
			GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
			GPIO_InitStruct.Pull = GPIO_NOPULL;
			GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
			HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

			/*Configure GPIO pins : LCD_CS_Pin LCD_D6_Pin LCD_D3_Pin LCD_D5_Pin
			 LCD_D4_Pin */
			GPIO_InitStruct.Pin = LCD_CS_Pin | LCD_D6_Pin | LCD_D3_Pin
					| LCD_D5_Pin | LCD_D4_Pin;
			GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
			GPIO_InitStruct.Pull = GPIO_NOPULL;
			GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
			HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

			/*Configure GPIO pin : SD_SS_Pin */
			GPIO_InitStruct.Pin = SD_SS_Pin;
			GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
			GPIO_InitStruct.Pull = GPIO_NOPULL;
			GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_MEDIUM;
			HAL_GPIO_Init(SD_SS_GPIO_Port, &GPIO_InitStruct);

			/* USER CODE BEGIN MX_GPIO_Init_2 */
			/* USER CODE END MX_GPIO_Init_2 */
		}

		/* USER CODE BEGIN 4 */
		void SD_Montar(void) {							// MOnta la memoria SD
			fres = f_mount(&fs, "/", 0);
			if (fres == FR_OK) {
				LCD_Clear(0x0000);  // Fondo negro
				transmit_uart("Micro SD card was mounted successfully...\n");
			} else if (fres != FR_OK) {
				LCD_Clear(0x0000);  // Fondo negro
				transmit_uart("Micro SD card's mount error!\n");
			}
		}

		void SD_Abrir(const char *filename, BYTE mode) { //Abre el archivo en la memoria

			fres = f_open(&fil, filename, mode);
			if (fres == FR_OK) {
				transmit_uart("File opened for reading.\n");
			} else {
				transmit_uart("File was not opened for reading correctly!\n");
			}
		}

		void SD_Cerrar(void) {	// Cierra el archivo
			fres = f_close(&fil);
			if (fres == FR_OK) {
				transmit_uart("The file was closed.\n");
			} else if (fres != FR_OK) {
				transmit_uart("The file was not closed correctly!\n");
			}
		}

		void SD_Desmontar(void) {					// Desmonta la memoria
			f_mount(NULL, "", 1);
			if (fres == FR_OK) {
				transmit_uart("Micro SD card was unmounted successfully...\n");
			} else if (fres != FR_OK) {
				transmit_uart("Micro SD card's dismounting error!\n");
			}
		}

		void SD_Imprimir(const char *filename) {// Imprime en el monitor la imagen

			fres = f_open(&fil, filename, FA_READ);			// Abrir el archivo
			if (fres != FR_OK) {
				sprintf(buffer, "Error al abrir '%s' (código %d)\n", filename,
						fres);
				transmit_uart(buffer);
				return;
			}

			transmit_uart("\n--- Contenido de ");
			transmit_uart(filename);
			transmit_uart(" ---\n");

			while (f_gets(buffer, sizeof(buffer), &fil)) {// Leer e imprime el archivo línea por línea
				transmit_uart(buffer);
			}

			if (f_close(&fil) != FR_OK) {					// Cierra el archivo
				transmit_uart("Error al cerrar el archivo\n");
			} else {
				transmit_uart("--- Fin del archivo ---\n");
			}
		}

//		void MostrarMenuConFlecha(int seleccion) {
//			LCD_Bitmap(0, 0, 320, 240, fondo);  // Fondo del juego
//
//			if (seleccion == MENU_JUGAR) {
//				LCD_Print("> Jugar", 90, 160, 1, 0x0000, 0xFFFF80);
//				LCD_Print("  Seleccionar personaje", 90, 190, 1, 0x0000,
//						0xFFFF80);
//			} else {
//				LCD_Print("  Jugar", 90, 160, 1, 0x0000, 0xFFFF80);
//				LCD_Print("> Seleccionar personaje", 90, 190, 1, 0x0000,
//						0xFFFF80);
//			}
//		}

		void MostrarMenuConFlecha(int seleccion) {
		    LCD_Bitmap(0, 0, 320, 240, fondo);  // Fondo del juego

		    LCD_Print("> Jugar", 90, 160, 1, 0x0000, 0xFFFF80);
		}

		void SD_RestaurarFondoDesdeFlash(int x, int y, int w, int h) {
			for (int row = 0; row < h; row++) {
				SetWindows(x, y + row, x + w - 1, y + row);
				LCD_CMD(0x2C);
				HAL_GPIO_WritePin(LCD_RS_GPIO_Port, LCD_RS_Pin, GPIO_PIN_SET);
				HAL_GPIO_WritePin(LCD_CS_GPIO_Port, LCD_CS_Pin, GPIO_PIN_RESET);

				int start = ((y + row) * 320 + x) * 2;
				for (int col = 0; col < w * 2; col++) {
					LCD_DATA(fondo[start + col]);
				}

				HAL_GPIO_WritePin(LCD_CS_GPIO_Port, LCD_CS_Pin, GPIO_PIN_SET);
			}
		}

		void Tick_2Jugadores(char tecla) {

			uint32_t ahora = HAL_GetTick();

			/*------------------------------------------------------------
			 * 1. Control de fin de ronda
			 *-----------------------------------------------------------*/
			if (!juego_activo) {
				if (ahora >= tiempo_fin_ronda) {
					rondas_jugadas++;

					// Condición de victoria
					if ((rondas_jugador1 == 3 || rondas_jugador2 == 3 || rondas_jugadas == 5) && !ganador_mostrado) {
					    ganador_mostrado = 1;

					    LCD_Bitmap(0, 0, 320, 240, fondo);

					    if (rondas_jugador1 > rondas_jugador2)
					        LCD_Print("Pablo gana!", 80, 100, 2, 0x0000, 0x87CEEB);
					    else
					        LCD_Print("Katha gana!", 80, 100, 2, 0x0000, 0x87CEEB);

					    LCD_Print("> Revancha", 80, 160, 1, 0xFFFF, 0x0000);
					    LCD_Print("  Salir", 80, 180, 1, 0xFFFF, 0x0000);

					    opcion_actual = 0;
					    estado_actual = FIN_JUEGO;
					    return;
					}

					// Reinicio de ronda

					disparoV1.activo = disparoV2.activo = 0;
					juego_activo = 1;

					LCD_Bitmap(0, 0, 320, 240, fondo);

					IniciarRonda();

					return;
				}
				return;
			}

			/*------------------------------------------------------------
			 * 2. Movimiento automático de disparos
			 *-----------------------------------------------------------*/



			// 2. Movimiento automático de disparos
			if (juego_activo) {
			    if (disparoV1.activo && ahora - tiempo_ultimo_disparoV1 >= intervalo_disparo) {
			        SD_RestaurarFondoDesdeFlash(disparoV1.x, disparoV1.y, 4, 4);
			        disparoV1.x += 3;
			        tiempo_ultimo_disparoV1 = ahora;

			        if (disparoV1.x > 320) {
			            disparoV1.activo = 0;
			            estado_vao = SPRITE_IDLE;
			        } else {
			            LCD_Bitmap(disparoV1.x, disparoV1.y, 4, 4, shoot1);
			        }
			    }

			    if (disparoV2.activo && ahora - tiempo_ultimo_disparoV2 >= intervalo_disparo) {
			        SD_RestaurarFondoDesdeFlash(disparoV2.x, disparoV2.y, 4, 4);
			        disparoV2.x -= 3;
			        tiempo_ultimo_disparoV2 = ahora;

			        if (disparoV2.x < 0) {
			            disparoV2.activo = 0;
			            estado_vao2 = SPRITE_IDLE;
			        } else {
			            LCD_Bitmap(disparoV2.x, disparoV2.y, 4, 4, shoot2);
			        }
			    }
			}

			/*------------------------------------------------------------
			 * 3. Colisiones
			 *-----------------------------------------------------------*/
			if (disparoV1.activo
					&& colision(disparoV1.x, disparoV1.y, 4, 4, sprite_x2,	sprite_y2, sprite_w2, sprite_h2)) {
				disparoV1.activo = 0;

				estado_vao2 = SPRITE_HERIDO1;
				LCD_Sprite(sprite_x2, sprite_y2, sprite_w2, sprite_h2, vaquero2, 5, estado_vao2, 0, 0);
				HAL_Delay(500);

				estado_vao2 = SPRITE_HERIDO2;
				LCD_Sprite(sprite_x2, sprite_y2, sprite_w2, sprite_h2, vaquero2, 5, estado_vao2, 0, 0);
				HAL_Delay(500);

				estado_vao2 = SPRITE_HERIDO3;
				LCD_Sprite(sprite_x2, sprite_y2, sprite_w2, sprite_h2, vaquero2, 5, estado_vao2, 0, 0);
				rondas_jugador1++;  // Aumentar puntaje
				tiempo_ultimo_cambio = ahora;
				juego_activo = 0;
				tiempo_fin_ronda = ahora + 1000;
			}

			if (disparoV2.activo
					&& colision(disparoV2.x, disparoV2.y, 4, 4, sprite_x,sprite_y, sprite_w, sprite_h)) {
				estado_vao = SPRITE_HERIDO1;
				LCD_Sprite(sprite_x, sprite_y, sprite_w, sprite_h, vaquero, 5, estado_vao, 0, 0);
				HAL_Delay(500);

				estado_vao = SPRITE_HERIDO2;
				LCD_Sprite(sprite_x, sprite_y, sprite_w, sprite_h, vaquero, 5, estado_vao, 0, 0);
				HAL_Delay(500);

				estado_vao = SPRITE_HERIDO3;
				LCD_Sprite(sprite_x, sprite_y, sprite_w, sprite_h, vaquero, 5, estado_vao, 0, 0);
				rondas_jugador2++;  // Aumentar puntaje
				tiempo_ultimo_cambio = ahora;
				juego_activo = 0;
				tiempo_fin_ronda = ahora + 1000;
			}

			/*------------------------------------------------------------
			 * 4. Entrada del usuario
			 *-----------------------------------------------------------*/
//	if (tecla == 0)
//
//		return;  // No se presionó tecla
			if (tecla == 'O' || tecla == 'o') {
				estado_actual = PAUSA;
				LCD_Bitmap(0, 0, 320, 240, fondo);
				LCD_Print("> Continuar", 80, 160, 1, 0xFFFF, 0x0000);
				LCD_Print("  Salir", 80, 180, 1, 0xFFFF, 0x0000);
				opcion_actual = 0;
				return;
			}

			// Disparo jugador 1
			if ((tecla == 'B' || tecla == 'b') && !disparoV1.activo) {
				disparoV1.x = sprite_x + sprite_w;
				disparoV1.y = sprite_y + sprite_h / 2;
				disparoV1.activo = 1;
				estado_vao = SPRITE_DISPARANDO;
				tiempo_ultimo_cambio = ahora;
			}

			// Disparo jugador 2
			if ((tecla == 'R' || tecla == 'r') && !disparoV2.activo) {
				disparoV2.x = sprite_x2 - 4;
				disparoV2.y = sprite_y2 + sprite_h2 / 2;
				estado_vao2 = SPRITE_DISPARANDO;
				disparoV2.activo = 1;
			}

			if (tecla == 'W' || tecla == 'w' || tecla == 'A' || tecla == 'a'
					|| tecla == 'S' || tecla == 's' || tecla == 'D'
					|| tecla == 'd') {
				direccion_jugador1 = tecla;
			}

			if (direccion_jugador1 == 'W' || direccion_jugador1 == 'w') {
				if (sprite_y > 85)
					sprite_y -= 1;
			} else if (direccion_jugador1 == 'S' || direccion_jugador1 == 's') {
				if (sprite_y < 240 - sprite_h)
					sprite_y += 1;
			} else if (direccion_jugador1 == 'A' || direccion_jugador1 == 'a') {
				if (sprite_x > 0)
					sprite_x -= 1;
			} else if (direccion_jugador1 == 'D' || direccion_jugador1 == 'd') {
				if (sprite_x < 160 - sprite_w)
					sprite_x += 1;
			}
			if (estado_actual == DOS_JUGADORES)
				LCD_Sprite(sprite_x, sprite_y, sprite_w, sprite_h, vaquero, 5,
						estado_vao, 0, 0);

			if (tecla == 'I' || tecla == 'i' || tecla == 'J' || tecla == 'j'
					|| tecla == 'K' || tecla == 'k' || tecla == 'L'
					|| tecla == 'l') {
				direccion_jugador2 = tecla;
			}

			if (direccion_jugador2 == 'I' || direccion_jugador2 == 'i') {
				if (sprite_y2 > 85)
					sprite_y2 -= 1;
			} else if (direccion_jugador2 == 'K' || direccion_jugador2 == 'k') {
				if (sprite_y2 < 240 - sprite_h2)
					sprite_y2 += 1;
			} else if (direccion_jugador2 == 'J' || direccion_jugador2 == 'j') {
				if (sprite_x2 > 160)
					sprite_x2 -= 1;
			} else if (direccion_jugador2 == 'L' || direccion_jugador2 == 'l') {
				if (sprite_x2 < 320 - sprite_w2)
					sprite_x2 += 1;
			}
			if (estado_actual == DOS_JUGADORES)
				LCD_Sprite(sprite_x2, sprite_y2, sprite_w2, sprite_h2, vaquero2,
						5, estado_vao2, 0, 0);

			/*------------------------------------------------------------
			 * 5. Animaciones de daño (herido)
			 *-----------------------------------------------------------*/
			if (estado_vao == SPRITE_HERIDO1
					&& ahora - tiempo_ultimo_cambio > 200) {
				estado_vao = SPRITE_HERIDO2;
				tiempo_ultimo_cambio = ahora;
			} else if (estado_vao == SPRITE_HERIDO2
					&& ahora - tiempo_ultimo_cambio > 200) {
				estado_vao = SPRITE_HERIDO3;
				tiempo_ultimo_cambio = ahora;
			} else if (estado_vao == SPRITE_HERIDO3
					&& ahora - tiempo_ultimo_cambio > 500) {
				estado_vao = SPRITE_IDLE;
			}

			if (!disparoV1.activo && estado_vao >= SPRITE_IDLE
					&& estado_vao <= SPRITE_HERIDO3) {
				estado_vao = SPRITE_IDLE;
			}

			if (estado_vao2 == SPRITE_HERIDO1
					&& ahora - tiempo_ultimo_cambio > 200) {
				estado_vao2 = SPRITE_HERIDO2;
				tiempo_ultimo_cambio = ahora;
			} else if (estado_vao2 == SPRITE_HERIDO2
					&& ahora - tiempo_ultimo_cambio > 200) {
				estado_vao2 = SPRITE_HERIDO3;
				tiempo_ultimo_cambio = ahora;
			} else if (estado_vao2 == SPRITE_HERIDO3
					&& ahora - tiempo_ultimo_cambio > 500) {
				estado_vao2 = SPRITE_IDLE;
			}

			if (!disparoV2.activo && estado_vao2 >= SPRITE_IDLE
					&& estado_vao2 <= SPRITE_HERIDO3) {
				estado_vao2 = SPRITE_IDLE;
			}
		}

		void IniciarRonda(void) {
//			tecla_uart2 = 0;
//			tecla_uart5 = 0;
			LimpiarTeclasUART();


		    // Reiniciar posiciones
		    sprite_x = 20;
		    sprite_y = 100;
		    sprite_x2 = 260;
		    sprite_y2 = 100;

		    disparoV1.activo = disparoV2.activo = 0;
		    juego_activo = 1;

		    // 🟡 Asegúrate de reiniciar los estados a IDLE
		    estado_vao = SPRITE_IDLE;
		    estado_vao2 = SPRITE_IDLE;

		    // Dibujar fondo y sprites
		    LCD_Bitmap(0, 0, 320, 240, fondo);
		    LCD_Sprite(sprite_x, sprite_y, 41, 42, vaquero, 5, estado_vao, 0, 0);
		    LCD_Sprite(sprite_x2, sprite_y2, 41, 42, vaquero2, 5, estado_vao2, 0, 0);

		    MostrarNumeroRonda(rondas_jugadas);

		    char msg = 'P';
		    HAL_UART_Transmit(&huart4, (uint8_t*) &msg, 1, HAL_MAX_DELAY);
		}


		void MostrarNumeroRonda(int numero) {
			char texto_ronda[20];
			sprintf(texto_ronda, "RONDA %d ", numero + 1);
			LCD_Print(texto_ronda, 100, 220, 1, 0x0000, 0xFFFF80); // Centrado en la parte inferior

			char marcador[30];
			sprintf(marcador, "Pablo: %d  -  Katha: %d", rondas_jugador1,
					rondas_jugador2);
			//LCD_Print(marcador, 85, 205, 1, 0xFFFF, 0x0000);  // Justo arriba del texto de ronda
			LCD_Print(marcador, 90, 10, 1, 0x0000, 0x87CEEB); // Negro sobre azul cielo

		}

		void ProcesarTecla(char tecla) {
			if (enviado_desde_menu) {
			        enviado_desde_menu = 0;
			        return;
			}
			switch (estado_actual) {
			case MENU:
//				i
				if (tecla == 'B' || tecla == 'R') {
				    enviado_desde_menu = 1;
				    estado_actual = DOS_JUGADORES;
				    IniciarRonda();
				}



				break;

			case DOS_JUGADORES:
				Tick_2Jugadores(tecla);

				break;

			case PAUSA:
				if (tecla == 'W' || tecla == 'w' || tecla == 'I'
						|| tecla == 'i') {
					opcion_actual = 0;
					LCD_Print("> Continuar", 80, 160, 1, 0xFFFF, 0x0000);
					LCD_Print("  Salir", 80, 180, 1, 0xFFFF, 0x0000);
				} else if (tecla == 'S' || tecla == 's' || tecla == 'K'
						|| tecla == 'k') {
					opcion_actual = 1;
					LCD_Print("  Continuar", 80, 160, 1, 0xFFFF, 0x0000);
					LCD_Print("> Salir", 80, 180, 1, 0xFFFF, 0x0000);
				} else if (tecla == 'B' || tecla == 'R') {
					if (opcion_actual == 0) {
						estado_actual = DOS_JUGADORES;
						LCD_Bitmap(0, 0, 320, 240, fondo);
						LCD_Sprite(sprite_x, sprite_y, sprite_w, sprite_h,
								vaquero, 5, estado_vao, 0, 0);
						LCD_Sprite(sprite_x2, sprite_y2, sprite_w2, sprite_h2,
								vaquero2, 5, estado_vao2, 0, 0);
						MostrarNumeroRonda(rondas_jugadas);
					} else {
						estado_actual = MENU;
						rondas_jugador1 = rondas_jugador2 = rondas_jugadas = 0;
						LCD_Bitmap(0, 0, 320, 240, fondo);
						opcion_actual = MENU_JUGAR;
						MostrarMenuConFlecha(opcion_actual);
						enviado_desde_menu = 1;
						LimpiarTeclasUART();

					}
				}
				break;

			case FIN_JUEGO:
			    if (tecla == 'w' || tecla == 'W' || tecla == 'i' || tecla == 'I') {
			        opcion_actual = 0;
			        LCD_Print("> Revancha", 80, 160, 1, 0xFFFF, 0x0000);
			        LCD_Print("  Salir", 80, 180, 1, 0xFFFF, 0x0000);
			    } else if (tecla == 's' || tecla == 'S' || tecla == 'k' || tecla == 'K') {
			        opcion_actual = 1;
			        LCD_Print("  Revancha", 80, 160, 1, 0xFFFF, 0x0000);
			        LCD_Print("> Salir", 80, 180, 1, 0xFFFF, 0x0000);
			    } else if (tecla == 'B' || tecla == 'R') {
			        if (opcion_actual == 0) {
			            ReiniciarJuego();
			            estado_actual = DOS_JUGADORES;
			            IniciarRonda();
			        } else {
			            ReiniciarJuego();
			            estado_actual = MENU;
			            LCD_Bitmap(0, 0, 320, 240, fondo);
			            MostrarMenuConFlecha(opcion_actual);
			            enviado_desde_menu = 1;
			            LimpiarTeclasUART();
			        }
			    }
			    break;

			default:
				break;
			}
		}

		void ProcesarTeclaJugador1(char tecla) {
		    if (estado_actual == DOS_JUGADORES && juego_activo) {
		        if (enviado_desde_menu) {
		            enviado_desde_menu = 0;
		            return;
		        }
		        if (tecla == 'B' || tecla == 'b') {
		            if (!disparoV1.activo) {
		                disparoV1.x = sprite_x + sprite_w;
		                disparoV1.y = sprite_y + sprite_h / 2;
		                disparoV1.activo = 1;
		                estado_vao = SPRITE_DISPARANDO;
		                tiempo_ultimo_cambio = HAL_GetTick();

		                // Enviar señal al UART4
		                HAL_UART_Transmit(&huart4, (uint8_t *)"B", 1, HAL_MAX_DELAY);
		            }
		        } else if (strchr("wasdWASD", tecla)) {
		            direccion_jugador1 = tecla;
		        }
		    }
		}


		        void ProcesarTeclaJugador2(char tecla) {
		            if (estado_actual == DOS_JUGADORES && juego_activo) {
		                if (enviado_desde_menu) {
		                    enviado_desde_menu = 0;
		                    return;
		                }
		                if (tecla == 'R' || tecla == 'r') {
		                    if (!disparoV2.activo) {
		                        disparoV2.x = sprite_x2 - 4;
		                        disparoV2.y = sprite_y2 + sprite_h2 / 2;
		                        disparoV2.activo = 1;
		                        estado_vao2 = SPRITE_DISPARANDO;
		                        tiempo_ultimo_cambio = HAL_GetTick();

		                    }
		                } else if (strchr("ijklIJKL", tecla)) {
		                    direccion_jugador2 = tecla;
		                }
		            }

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


		void ReiniciarJuego(void) {
		    rondas_jugador1 = 0;
		    rondas_jugador2 = 0;
		    rondas_jugadas = 0;

		    sprite_x = 20;
		    sprite_y = 100;
		    sprite_x2 = 260;
		    sprite_y2 = 100;

		    direccion_jugador1 = 0;
		    direccion_jugador2 = 0;

		    estado_vao = SPRITE_IDLE;
		    estado_vao2 = SPRITE_IDLE;

		    disparoV1.activo = 0;
		    disparoV2.activo = 0;

		    juego_activo = 1;
		    ganador_mostrado = 0;
		    opcion_actual = MENU_JUGAR;
		}

		void LimpiarTeclasUART(void) {
		    tecla_uart2 = 0;
		    tecla_uart5 = 0;
		    __HAL_UART_CLEAR_FLAG(&huart2, UART_FLAG_RXNE);
		    __HAL_UART_CLEAR_FLAG(&huart5, UART_FLAG_RXNE);
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
