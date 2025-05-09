/*
 * ili9341.c
 *
 *  Created on: Aug 20, 2024
 *      Author: Pablo Mazariegos
 */
#include <stdlib.h> // malloc()
#include <string.h> // memset()
#include "pgmspace.h"
#include "ili9341.h"
#include "main.h"

#include "fatfs_sd.h"
#include "fatfs.h"
#include "stdio.h"

#include "ff.h"  // Asegúrate de incluir esto para FRESULT y FATFS

extern const uint8_t smallFont[1140];
extern const uint16_t bigFont[1520];

//uint8_t vaquero[41 * 42 * 2];
uint8_t vaquero[205 * 42 * 2];
uint8_t vaquero2[205 * 42 * 2];
uint8_t bala[13 * 20 * 2];
uint8_t shoot1[4 * 4 * 2];
uint8_t shoot2[4 * 4 * 2];
uint8_t ronda1[15 * 15 * 2];
uint8_t ronda2[15 * 15 * 2];
uint8_t ronda3[15 * 15 * 2];

extern UART_HandleTypeDef huart2;

void transmit_uart(const char *string); // Asegúrate de tener esta función

//***************************************************************************************************************************************
// Función para inicializar LCD
//***************************************************************************************************************************************
void LCD_Init(void) {

	//****************************************
	// Secuencia de Inicialización
	//****************************************
	/*Configure GPIO pin Output Level */
	HAL_GPIO_WritePin(GPIOB, LCD_CS_Pin, GPIO_PIN_SET);
	/*Configure GPIO pin Output Level */
	HAL_GPIO_WritePin(GPIOA, LCD_RD_Pin | LCD_WR_Pin | LCD_RS_Pin,
			GPIO_PIN_SET);

	/*Configure GPIO pin Output Level */
	HAL_GPIO_WritePin(GPIOC, LCD_RST_Pin, GPIO_PIN_SET);
	HAL_Delay(5);
	HAL_GPIO_WritePin(GPIOC, LCD_RST_Pin, GPIO_PIN_RESET);
	HAL_Delay(20);
	HAL_GPIO_WritePin(GPIOC, LCD_RST_Pin, GPIO_PIN_SET);
	HAL_Delay(150);
	HAL_GPIO_WritePin(GPIOB, LCD_CS_Pin, GPIO_PIN_RESET);

	//****************************************
	LCD_CMD(0xE9);  // SETPANELRELATED
	LCD_DATA(0x20);
	//****************************************
	LCD_CMD(0x11); // Exit Sleep SLEEP OUT (SLPOUT)
	HAL_Delay(100);
	//****************************************
	LCD_CMD(0xD1);    // (SETVCOM)
	LCD_DATA(0x00);
	LCD_DATA(0x71);
	LCD_DATA(0x19);
	//****************************************
	LCD_CMD(0xD0);   // (SETPOWER)
	LCD_DATA(0x07);
	LCD_DATA(0x01);
	LCD_DATA(0x08);
	//****************************************
	LCD_CMD(0x36);  // (MEMORYACCESS)
	LCD_DATA(0x40 | 0x80 | 0x20 | 0x08); // LCD_DATA(0x19);
	//****************************************
	LCD_CMD(0x3A); // Set_pixel_format (PIXELFORMAT)
	LCD_DATA(0x05); // color setings, 05h - 16bit pixel, 11h - 3bit pixel
	//****************************************
	LCD_CMD(0xC1);    // (POWERCONTROL2)
	LCD_DATA(0x10);
	LCD_DATA(0x10);
	LCD_DATA(0x02);
	LCD_DATA(0x02);
	//****************************************
	LCD_CMD(0xC0); // Set Default Gamma (POWERCONTROL1)
	LCD_DATA(0x00);
	LCD_DATA(0x35);
	LCD_DATA(0x00);
	LCD_DATA(0x00);
	LCD_DATA(0x01);
	LCD_DATA(0x02);
	//****************************************
	LCD_CMD(0xC5); // Set Frame Rate (VCOMCONTROL1)
	LCD_DATA(0x04); // 72Hz
	//****************************************
	LCD_CMD(0xD2); // Power Settings  (SETPWRNORMAL)
	LCD_DATA(0x01);
	LCD_DATA(0x44);
	//****************************************
	LCD_CMD(0xC8); //Set Gamma  (GAMMASET)
	LCD_DATA(0x04);
	LCD_DATA(0x67);
	LCD_DATA(0x35);
	LCD_DATA(0x04);
	LCD_DATA(0x08);
	LCD_DATA(0x06);
	LCD_DATA(0x24);
	LCD_DATA(0x01);
	LCD_DATA(0x37);
	LCD_DATA(0x40);
	LCD_DATA(0x03);
	LCD_DATA(0x10);
	LCD_DATA(0x08);
	LCD_DATA(0x80);
	LCD_DATA(0x00);
	//****************************************
	LCD_CMD(0x2A); // Set_column_address 320px (CASET)
	LCD_DATA(0x00);
	LCD_DATA(0x00);
	LCD_DATA(0x01);
	LCD_DATA(0x3F);
	//****************************************
	LCD_CMD(0x2B); // Set_page_address 480px (PASET)
	LCD_DATA(0x00);
	LCD_DATA(0x00);
	LCD_DATA(0x01);
	LCD_DATA(0xE0);
	//  LCD_DATA(0x8F);
	LCD_CMD(0x29); //display on
	LCD_CMD(0x2C); //display on

	LCD_CMD(ILI9341_INVOFF); //Invert Off
	HAL_Delay(120);
	LCD_CMD(ILI9341_SLPOUT);    //Exit Sleep
	HAL_Delay(120);
	LCD_CMD(ILI9341_DISPON);    //Display on
	HAL_GPIO_WritePin(GPIOB, LCD_CS_Pin, GPIO_PIN_SET);
}
//***************************************************************************************************************************************
// Función para enviar comandos a la LCD - parámetro (comando)
//***************************************************************************************************************************************
void LCD_CMD(uint8_t cmd) {
	HAL_GPIO_WritePin(LCD_RS_GPIO_Port, LCD_RS_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(LCD_WR_GPIO_Port, LCD_WR_Pin, GPIO_PIN_RESET);

	if ((cmd & (1 << 0)) == 1) {
		HAL_GPIO_WritePin(LCD_D0_GPIO_Port, LCD_D0_Pin, GPIO_PIN_SET);
	} else {
		HAL_GPIO_WritePin(LCD_D0_GPIO_Port, LCD_D0_Pin, GPIO_PIN_RESET);
	}
	if ((cmd & (1 << 1)) == 0x02) {
		HAL_GPIO_WritePin(LCD_D1_GPIO_Port, LCD_D1_Pin, GPIO_PIN_SET);
	} else {
		HAL_GPIO_WritePin(LCD_D1_GPIO_Port, LCD_D1_Pin, GPIO_PIN_RESET);
	}
	if ((cmd & (1 << 2)) == 0x04) {
		HAL_GPIO_WritePin(LCD_D2_GPIO_Port, LCD_D2_Pin, GPIO_PIN_SET);
	} else {
		HAL_GPIO_WritePin(LCD_D2_GPIO_Port, LCD_D2_Pin, GPIO_PIN_RESET);
	}
	if ((cmd & (1 << 3)) == 0x08) {
		HAL_GPIO_WritePin(LCD_D3_GPIO_Port, LCD_D3_Pin, GPIO_PIN_SET);
	} else {
		HAL_GPIO_WritePin(LCD_D3_GPIO_Port, LCD_D3_Pin, GPIO_PIN_RESET);
	}
	if ((cmd & (1 << 4)) == 0x10) {
		HAL_GPIO_WritePin(LCD_D4_GPIO_Port, LCD_D4_Pin, GPIO_PIN_SET);
	} else {
		HAL_GPIO_WritePin(LCD_D4_GPIO_Port, LCD_D4_Pin, GPIO_PIN_RESET);
	}
	if ((cmd & (1 << 5)) == 0x20) {
		HAL_GPIO_WritePin(LCD_D5_GPIO_Port, LCD_D5_Pin, GPIO_PIN_SET);
	} else {
		HAL_GPIO_WritePin(LCD_D5_GPIO_Port, LCD_D5_Pin, GPIO_PIN_RESET);
	}
	if ((cmd & (1 << 6)) == 0x40) {
		HAL_GPIO_WritePin(LCD_D6_GPIO_Port, LCD_D6_Pin, GPIO_PIN_SET);
	} else {
		HAL_GPIO_WritePin(LCD_D6_GPIO_Port, LCD_D6_Pin, GPIO_PIN_RESET);
	}
	if ((cmd & (1 << 7)) == 0x80) {
		HAL_GPIO_WritePin(LCD_D7_GPIO_Port, LCD_D7_Pin, GPIO_PIN_SET);
	} else {
		HAL_GPIO_WritePin(LCD_D7_GPIO_Port, LCD_D7_Pin, GPIO_PIN_RESET);
	}
	//GPIO_PortB_DATA_R = cmd;
	HAL_GPIO_WritePin(LCD_WR_GPIO_Port, LCD_WR_Pin, GPIO_PIN_SET);

}
//***************************************************************************************************************************************
// Función para enviar datos a la LCD - parámetro (dato)
//***************************************************************************************************************************************
void LCD_DATA(uint8_t data) {
	//HAL_GPIO_WritePin(LCD_CS_GPIO_Port, LCD_CS_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(LCD_RS_GPIO_Port, LCD_RS_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(LCD_WR_GPIO_Port, LCD_WR_Pin, GPIO_PIN_RESET);
	if ((data & (1 << 0)) == 1) {
		HAL_GPIO_WritePin(LCD_D0_GPIO_Port, LCD_D0_Pin, GPIO_PIN_SET);
	} else {
		HAL_GPIO_WritePin(LCD_D0_GPIO_Port, LCD_D0_Pin, GPIO_PIN_RESET);
	}
	if ((data & (1 << 1)) == 0x02) {
		HAL_GPIO_WritePin(LCD_D1_GPIO_Port, LCD_D1_Pin, GPIO_PIN_SET);
	} else {
		HAL_GPIO_WritePin(LCD_D1_GPIO_Port, LCD_D1_Pin, GPIO_PIN_RESET);
	}
	if ((data & (1 << 2)) == 0x04) {
		HAL_GPIO_WritePin(LCD_D2_GPIO_Port, LCD_D2_Pin, GPIO_PIN_SET);
	} else {
		HAL_GPIO_WritePin(LCD_D2_GPIO_Port, LCD_D2_Pin, GPIO_PIN_RESET);
	}
	if ((data & (1 << 3)) == 0x08) {
		HAL_GPIO_WritePin(LCD_D3_GPIO_Port, LCD_D3_Pin, GPIO_PIN_SET);
	} else {
		HAL_GPIO_WritePin(LCD_D3_GPIO_Port, LCD_D3_Pin, GPIO_PIN_RESET);
	}
	if ((data & (1 << 4)) == 0x10) {
		HAL_GPIO_WritePin(LCD_D4_GPIO_Port, LCD_D4_Pin, GPIO_PIN_SET);
	} else {
		HAL_GPIO_WritePin(LCD_D4_GPIO_Port, LCD_D4_Pin, GPIO_PIN_RESET);
	}
	if ((data & (1 << 5)) == 0x20) {
		HAL_GPIO_WritePin(LCD_D5_GPIO_Port, LCD_D5_Pin, GPIO_PIN_SET);
	} else {
		HAL_GPIO_WritePin(LCD_D5_GPIO_Port, LCD_D5_Pin, GPIO_PIN_RESET);
	}
	if ((data & (1 << 6)) == 0x40) {
		HAL_GPIO_WritePin(LCD_D6_GPIO_Port, LCD_D6_Pin, GPIO_PIN_SET);
	} else {
		HAL_GPIO_WritePin(LCD_D6_GPIO_Port, LCD_D6_Pin, GPIO_PIN_RESET);
	}
	if ((data & (1 << 7)) == 0x80) {
		HAL_GPIO_WritePin(LCD_D7_GPIO_Port, LCD_D7_Pin, GPIO_PIN_SET);
	} else {
		HAL_GPIO_WritePin(LCD_D7_GPIO_Port, LCD_D7_Pin, GPIO_PIN_RESET);
	}
	HAL_GPIO_WritePin(LCD_WR_GPIO_Port, LCD_WR_Pin, GPIO_PIN_SET);
}
//***************************************************************************************************************************************
// Función para definir rango de direcciones de memoria con las cuales se trabajara (se define una ventana)
//***************************************************************************************************************************************
void SetWindows(unsigned int x1, unsigned int y1, unsigned int x2,
		unsigned int y2) {
	LCD_CMD(0x2a); // Set_column_address 4 parameters
	LCD_DATA(x1 >> 8);
	LCD_DATA(x1);
	LCD_DATA(x2 >> 8);
	LCD_DATA(x2);
	LCD_CMD(0x2b); // Set_page_address 4 parameters
	LCD_DATA(y1 >> 8);
	LCD_DATA(y1);
	LCD_DATA(y2 >> 8);
	LCD_DATA(y2);
	LCD_CMD(0x2c); // Write_memory_start
}
//***************************************************************************************************************************************
// Función para borrar la pantalla - parámetros (color)
//***************************************************************************************************************************************
void LCD_Clear(unsigned int c) {
	unsigned int x, y;
	LCD_CMD(0x02c); // write_memory_start
	HAL_GPIO_WritePin(LCD_RS_GPIO_Port, LCD_RS_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(LCD_CS_GPIO_Port, LCD_CS_Pin, GPIO_PIN_RESET);
	SetWindows(0, 0, 319, 239); // 479, 319);
	for (x = 0; x < 320; x++)
		for (y = 0; y < 240; y++) {
			LCD_DATA(c >> 8);
			LCD_DATA(c);
		}
	HAL_GPIO_WritePin(LCD_CS_GPIO_Port, LCD_CS_Pin, GPIO_PIN_SET);
}
//***************************************************************************************************************************************
// Función para dibujar una línea horizontal - parámetros ( coordenada x, cordenada y, longitud, color)
//***************************************************************************************************************************************
void H_line(unsigned int x, unsigned int y, unsigned int l, unsigned int c) {
	unsigned int i;
	LCD_CMD(0x02c); //write_memory_start
	HAL_GPIO_WritePin(LCD_RS_GPIO_Port, LCD_RS_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(LCD_CS_GPIO_Port, LCD_CS_Pin, GPIO_PIN_RESET);
	l = l + x;
	SetWindows(x, y, l, y);
	//j = l; // * 2;
	for (i = 0; i < l; i++) {
		LCD_DATA(c >> 8);
		LCD_DATA(c);
	}
	HAL_GPIO_WritePin(LCD_CS_GPIO_Port, LCD_CS_Pin, GPIO_PIN_SET);
}
//***************************************************************************************************************************************
// Función para dibujar una línea vertical - parámetros ( coordenada x, cordenada y, longitud, color)
//***************************************************************************************************************************************
void V_line(unsigned int x, unsigned int y, unsigned int l, unsigned int c) {
	unsigned int i;
	LCD_CMD(0x02c); //write_memory_start
	HAL_GPIO_WritePin(LCD_RS_GPIO_Port, LCD_RS_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(LCD_CS_GPIO_Port, LCD_CS_Pin, GPIO_PIN_RESET);
	l = l + y;
	SetWindows(x, y, x, l);
	//j = l; //* 2;
	for (i = 1; i <= l; i++) {
		LCD_DATA(c >> 8);
		LCD_DATA(c);
	}
	HAL_GPIO_WritePin(LCD_CS_GPIO_Port, LCD_CS_Pin, GPIO_PIN_SET);
}
//***************************************************************************************************************************************
// Función para dibujar un rectángulo - parámetros ( coordenada x, cordenada y, ancho, alto, color)
//***************************************************************************************************************************************
void Rect(unsigned int x, unsigned int y, unsigned int w, unsigned int h,
		unsigned int c) {
	H_line(x, y, w, c);
	H_line(x, y + h, w, c);
	V_line(x, y, h, c);
	V_line(x + w, y, h, c);
}
//***************************************************************************************************************************************
// Función para dibujar un rectángulo relleno - parámetros ( coordenada x, cordenada y, ancho, alto, color)
//***************************************************************************************************************************************
/*void FillRect(unsigned int x, unsigned int y, unsigned int w, unsigned int h, unsigned int c) {
 unsigned int i;
 for (i = 0; i < h; i++) {
 H_line(x  , y  , w, c);
 H_line(x  , y+i, w, c);
 }
 }
 */

void FillRect(unsigned int x, unsigned int y, unsigned int w, unsigned int h,
		unsigned int c) {
	LCD_CMD(0x02c); // write_memory_start
	HAL_GPIO_WritePin(LCD_RS_GPIO_Port, LCD_RS_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(LCD_CS_GPIO_Port, LCD_CS_Pin, GPIO_PIN_RESET);

	unsigned int x2, y2;
	x2 = x + w;
	y2 = y + h;
	SetWindows(x, y, x2 - 1, y2 - 1);
	unsigned int k = w * h * 2 - 1;
	for (int i = 0; i < w; i++) {
		for (int j = 0; j < h; j++) {
			LCD_DATA(c >> 8);
			LCD_DATA(c);

			//LCD_DATA(bitmap[k]);
			k = k - 2;
		}
	}
	HAL_GPIO_WritePin(LCD_CS_GPIO_Port, LCD_CS_Pin, GPIO_PIN_SET);
}
//***************************************************************************************************************************************
// Función para dibujar texto - parámetros ( texto, coordenada x, cordenada y, color, background)
//***************************************************************************************************************************************
void LCD_Print(char *text, int x, int y, int fontSize, int color,
		int background) {

	int fontXSize;
	int fontYSize;

	if (fontSize == 1) {
		fontXSize = fontXSizeSmal;
		fontYSize = fontYSizeSmal;
	}
	if (fontSize == 2) {
		fontXSize = fontXSizeBig;
		fontYSize = fontYSizeBig;
	}
	if (fontSize == 3) {
		fontXSize = fontXSizeNum;
		fontYSize = fontYSizeNum;
	}

	char charInput;
	int cLength = strlen(text);
	//Serial.println(cLength, DEC);
	int charDec;
	int c;
	//int charHex;
	char char_array[cLength + 1];
	for (int i = 0; text[i] != '\0'; i++) {
		char_array[i] = text[i];
	}

	//text.toCharArray(char_array, cLength + 1);

	for (int i = 0; i < cLength; i++) {
		charInput = char_array[i];
		//Serial.println(char_array[i]);
		charDec = (int) charInput;
		HAL_GPIO_WritePin(LCD_CS_GPIO_Port, LCD_CS_Pin, GPIO_PIN_RESET);
		SetWindows(x + (i * fontXSize), y, x + (i * fontXSize) + fontXSize - 1,
				y + fontYSize);
		long charHex1;
		for (int n = 0; n < fontYSize; n++) {
			if (fontSize == 1) {
				charHex1 = pgm_read_word_near(
						smallFont + ((charDec - 32) * fontYSize) + n);
			}
			if (fontSize == 2) {
				charHex1 = pgm_read_word_near(
						bigFont + ((charDec - 32) * fontYSize) + n);
			}
			for (int t = 1; t < fontXSize + 1; t++) {
				if ((charHex1 & (1 << (fontXSize - t))) > 0) {
					c = color;
				} else {
					c = background;
				}
				LCD_DATA(c >> 8);
				LCD_DATA(c);
			}
		}
		HAL_GPIO_WritePin(LCD_CS_GPIO_Port, LCD_CS_Pin, GPIO_PIN_SET);
	}
}
//***************************************************************************************************************************************
// Función para dibujar una imagen a partir de un arreglo de colores (Bitmap) Formato (Color 16bit R 5bits G 6bits B 5bits)
//***************************************************************************************************************************************
void LCD_Bitmap(unsigned int x, unsigned int y, unsigned int width,
		unsigned int height, unsigned char bitmap[]) {
	LCD_CMD(0x02c); // write_memory_start
	HAL_GPIO_WritePin(LCD_RS_GPIO_Port, LCD_RS_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(LCD_CS_GPIO_Port, LCD_CS_Pin, GPIO_PIN_RESET);

	unsigned int x2, y2;
	x2 = x + width;
	y2 = y + height;
	SetWindows(x, y, x2 - 1, y2 - 1);
	unsigned int k = 0;

	for (int i = 0; i < width; i++) {
		for (int j = 0; j < height; j++) {
			LCD_DATA(bitmap[k]);
			LCD_DATA(bitmap[k + 1]);
			//LCD_DATA(bitmap[k]);
			k = k + 2;
		}
	}
	HAL_GPIO_WritePin(LCD_CS_GPIO_Port, LCD_CS_Pin, GPIO_PIN_SET);
}
//***************************************************************************************************************************************
// Función para dibujar una imagen sprite - los parámetros columns = número de imagenes en el sprite, index = cual desplegar, flip = darle vuelta
//***************************************************************************************************************************************
void LCD_Sprite(int x, int y, int width, int height, unsigned char bitmap[],
		int columns, int index, char flip, char offset) {
	LCD_CMD(0x02c); // write_memory_start
	HAL_GPIO_WritePin(LCD_RS_GPIO_Port, LCD_RS_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(LCD_CS_GPIO_Port, LCD_CS_Pin, GPIO_PIN_RESET);

	unsigned int x2, y2;
	x2 = x + width;
	y2 = y + height;
	SetWindows(x, y, x2 - 1, y2 - 1);
	int k = 0;
	int ancho = ((width * columns));
	if (flip) {
		for (int j = 0; j < height; j++) {
			k = (j * (ancho) + index * width - 1 - offset) * 2;
			k = k + width * 2;
			for (int i = 0; i < width; i++) {
				LCD_DATA(bitmap[k]);
				LCD_DATA(bitmap[k + 1]);
				k = k - 2;
			}
		}
	} else {
		for (int j = 0; j < height; j++) {
			k = (j * (ancho) + index * width + 1 + offset) * 2;
			for (int i = 0; i < width; i++) {
				LCD_DATA(bitmap[k]);
				LCD_DATA(bitmap[k + 1]);
				k = k + 2;
			}
		}

	}
	HAL_GPIO_WritePin(LCD_CS_GPIO_Port, LCD_CS_Pin, GPIO_PIN_SET);
}



FRESULT LeerSpriteDesdeSD(const char *filename, uint16_t sprite_width, uint16_t sprite_height,
                          uint8_t columns) {
    FIL file;
    UINT br;
    char msg[64];
    uint32_t full_width = sprite_width * columns;

    FRESULT res = f_open(&file, filename, FA_READ);
    if (res != FR_OK) {
        sprintf(msg, "Error abriendo %s (FRES=%d)\r\n", filename, res);
        transmit_uart(msg);
        return res;
    }

    sprintf(msg, "Cargando todos los sprites desde %s...\r\n", filename);
    transmit_uart(msg);

    for (uint16_t row = 0; row < sprite_height; row++) {
        uint32_t offset = row * full_width * 2;
        res = f_lseek(&file, offset);
        if (res != FR_OK) {
            sprintf(msg, "Seek error fila %d (offset=%lu)\r\n", row, offset);
            transmit_uart(msg);
            f_close(&file);
            return res;
        }

        res = f_read(&file, &vaquero[row * full_width * 2], full_width * 2, &br);
        if (res != FR_OK || br != full_width * 2) {
            sprintf(msg, "Error de lectura fila %d (leído=%u)\r\n", row, br);
            transmit_uart(msg);
            f_close(&file);
            return res;
        }
    }

    transmit_uart("Sprites cargados correctamente a RAM.\r\n");
    f_close(&file);
    return FR_OK;
}

FRESULT LeerSpriteDesdeSD2(const char *filename, uint16_t sprite_width, uint16_t sprite_height,
                          uint8_t columns) {
    FIL file;
    UINT br;
    char msg[64];
    uint32_t full_width = sprite_width * columns;

    FRESULT res = f_open(&file, filename, FA_READ);
    if (res != FR_OK) {
        sprintf(msg, "Error abriendo %s (FRES=%d)\r\n", filename, res);
        transmit_uart(msg);
        return res;
    }

    sprintf(msg, "Cargando todos los sprites desde %s...\r\n", filename);
    transmit_uart(msg);

    for (uint16_t row = 0; row < sprite_height; row++) {
        uint32_t offset = row * full_width * 2;
        res = f_lseek(&file, offset);
        if (res != FR_OK) {
            sprintf(msg, "Seek error fila %d (offset=%lu)\r\n", row, offset);
            transmit_uart(msg);
            f_close(&file);
            return res;
        }

        res = f_read(&file, &vaquero2[row * full_width * 2], full_width * 2, &br);
        if (res != FR_OK || br != full_width * 2) {
            sprintf(msg, "Error de lectura fila %d (leído=%u)\r\n", row, br);
            transmit_uart(msg);
            f_close(&file);
            return res;
        }
    }

    transmit_uart("Sprites cargados correctamente a RAM.\r\n");
    f_close(&file);
    return FR_OK;
}


FRESULT LeerSpriteDesdeSDBala(const char *filename, uint16_t width,
		uint16_t height, uint8_t frame_index, uint8_t columns) {
	FIL file;
	UINT br;
	char msg[64];

	// uint32_t frame_size = width * height * 2;
	uint32_t full_width = width * columns;
	//uint32_t offset = frame_index * width * 2;  // horizontal layout
	FRESULT res = f_open(&file, filename, FA_READ);

	if (res != FR_OK) {
		sprintf(msg, "Error abriendo %s (FRES=%d)\r\n", filename, res);
		transmit_uart(msg);
		return res;
	}

	sprintf(msg, "Leyendo frame %d de %s...\r\n", frame_index, filename);
	transmit_uart(msg);

	for (uint16_t row = 0; row < height; row++) {
		uint32_t row_offset = ((row * full_width) + frame_index * width) * 2;

		res = f_lseek(&file, row_offset);
		if (res != FR_OK) {
			sprintf(msg, "Seek error fila %d (offset=%lu)\r\n", row,
					row_offset);
			transmit_uart(msg);
			f_close(&file);
			return res;
		}

		res = f_read(&file, &bala[row * width * 2], width * 2, &br);
		if (res != FR_OK || br != width * 2) {
			sprintf(msg, "Read error fila %d (leído=%u)\r\n", row, br);
			transmit_uart(msg);
			f_close(&file);
			return res;
		}
	}

	transmit_uart("Lectura OK. Sprite cargado a RAM.\r\n");
	f_close(&file);
	return FR_OK;
}

FRESULT LeerSpriteDesdeSDDisparo(const char *filename, uint16_t width,
		uint16_t height, uint8_t frame_index, uint8_t columns) {
	FIL file;
	UINT br;
	char msg[64];

	// uint32_t frame_size = width * height * 2;
	uint32_t full_width = width * columns;
	//uint32_t offset = frame_index * width * 2;  // horizontal layout
	FRESULT res = f_open(&file, filename, FA_READ);

	if (res != FR_OK) {
		sprintf(msg, "Error abriendo %s (FRES=%d)\r\n", filename, res);
		transmit_uart(msg);
		return res;
	}

	sprintf(msg, "Leyendo frame %d de %s...\r\n", frame_index, filename);
	transmit_uart(msg);

	for (uint16_t row = 0; row < height; row++) {
		uint32_t row_offset = ((row * full_width) + frame_index * width) * 2;

		res = f_lseek(&file, row_offset);
		if (res != FR_OK) {
			sprintf(msg, "Seek error fila %d (offset=%lu)\r\n", row,
					row_offset);
			transmit_uart(msg);
			f_close(&file);
			return res;
		}

		res = f_read(&file, &shoot1[row * width * 2], width * 2, &br);
		if (res != FR_OK || br != width * 2) {
			sprintf(msg, "Read error fila %d (leído=%u)\r\n", row, br);
			transmit_uart(msg);
			f_close(&file);
			return res;
		}
	}

	transmit_uart("Lectura OK. Sprite cargado a RAM.\r\n");
	f_close(&file);
	return FR_OK;
}

FRESULT LeerSpriteDesdeSDDisparo2(const char *filename, uint16_t width,
		uint16_t height, uint8_t frame_index, uint8_t columns) {
	FIL file;
	UINT br;
	char msg[64];

	// uint32_t frame_size = width * height * 2;
	uint32_t full_width = width * columns;
	//uint32_t offset = frame_index * width * 2;  // horizontal layout
	FRESULT res = f_open(&file, filename, FA_READ);

	if (res != FR_OK) {
		sprintf(msg, "Error abriendo %s (FRES=%d)\r\n", filename, res);
		transmit_uart(msg);
		return res;
	}

	sprintf(msg, "Leyendo frame %d de %s...\r\n", frame_index, filename);
	transmit_uart(msg);

	for (uint16_t row = 0; row < height; row++) {
		uint32_t row_offset = ((row * full_width) + frame_index * width) * 2;

		res = f_lseek(&file, row_offset);
		if (res != FR_OK) {
			sprintf(msg, "Seek error fila %d (offset=%lu)\r\n", row,
					row_offset);
			transmit_uart(msg);
			f_close(&file);
			return res;
		}

		res = f_read(&file, &shoot2[row * width * 2], width * 2, &br);
		if (res != FR_OK || br != width * 2) {
			sprintf(msg, "Read error fila %d (leído=%u)\r\n", row, br);
			transmit_uart(msg);
			f_close(&file);
			return res;
		}
	}

	transmit_uart("Lectura OK. Sprite cargado a RAM.\r\n");
	f_close(&file);
	return FR_OK;
}
FRESULT LeerSpriteRonda(const char *filename, uint16_t width, uint16_t height, uint8_t frame_index, uint8_t columns, uint8_t *dest_buffer) {
    FIL file;
    UINT br;
    char msg[64];
    uint32_t full_width = width * columns;
    FRESULT res = f_open(&file, filename, FA_READ);
    if (res != FR_OK) {
        sprintf(msg, "Error abriendo %s (FRES=%d)\r\n", filename, res);
        transmit_uart(msg);
        return res;
    }

    for (uint16_t row = 0; row < height; row++) {
        uint32_t row_offset = ((row * full_width) + frame_index * width) * 2;

        res = f_lseek(&file, row_offset);
        if (res != FR_OK) {
            sprintf(msg, "Seek error fila %d (offset=%lu)\r\n", row, row_offset);
            transmit_uart(msg);
            f_close(&file);
            return res;
        }

        res = f_read(&file, &dest_buffer[row * width * 2], width * 2, &br);
        if (res != FR_OK || br != width * 2) {
            sprintf(msg, "Read error fila %d (leído=%u)\r\n", row, br);
            transmit_uart(msg);
            f_close(&file);
            return res;
        }
    }

    f_close(&file);
    transmit_uart("Sprite de ronda leído correctamente.\r\n");
    return FR_OK;
}


void MostrarSpriteDesdeRAM(uint16_t x, uint16_t y, uint16_t width,
				uint16_t height) {
			for (uint16_t row = 0; row < height; row++) {
				SetWindows(x, y + row, x + width - 1, y + row);
				LCD_CMD(0x2C);
				HAL_GPIO_WritePin(LCD_RS_GPIO_Port, LCD_RS_Pin, GPIO_PIN_SET);
				HAL_GPIO_WritePin(LCD_CS_GPIO_Port, LCD_CS_Pin, GPIO_PIN_RESET);

				uint8_t *line = &bala[row * width * 2];
				for (uint16_t i = 0; i < width * 2; i++) {
					LCD_DATA(line[i]);
				}

				HAL_GPIO_WritePin(LCD_CS_GPIO_Port, LCD_CS_Pin, GPIO_PIN_SET);
			}
		}

