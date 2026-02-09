#include "stm32c0xx_hal.h"
#include <stdio.h>
#include <math.h>
#include <string.h>

/* Handles */
ADC_HandleTypeDef hadc;
UART_HandleTypeDef huart2;

/* Constants */
#define VREF            3.3f
#define ADC_MAX         4095.0f
#define NTC_R0          10000.0f
#define NTC_BETA        3950.0f
#define NTC_T0          298.15f
#define R_REF           10000.0f
#define TEMP_THRESHOLD  40.0f

/* Prototypes */
void GPIO_Init_Custom(void);
void ADC_Init_Custom(void);
void UART2_Init_Custom(void);
float Convert_To_Temperature(uint32_t adc);

/* Main */
int main(void)
{
  HAL_Init();

  GPIO_Init_Custom();
  UART2_Init_Custom();
  ADC_Init_Custom();

  HAL_ADCEx_Calibration_Start(&hadc);

  printf("NTC Temperature Controller Started\r\n");

  while (1)
  {
    HAL_ADC_Start(&hadc);
    HAL_ADC_PollForConversion(&hadc, 100);
    uint32_t adc = HAL_ADC_GetValue(&hadc);
    HAL_ADC_Stop(&hadc);

    float temp = Convert_To_Temperature(adc);

    if (temp >= TEMP_THRESHOLD)
      HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, GPIO_PIN_SET);
    else
      HAL_GPIO_WritePin(GPIOB, GPIO_PIN_6, GPIO_PIN_RESET);

    printf("Temp: %.2f C | LED: %s\r\n",
           temp,
           (temp >= TEMP_THRESHOLD) ? "ON" : "OFF");

    HAL_Delay(500);
  }
}

/* Temperature math */
float Convert_To_Temperature(uint32_t adc)
{
  float voltage = (adc / ADC_MAX) * VREF;
  float resistance = R_REF * (voltage / (VREF - voltage));

  float tempK = 1.0f /
    ((1.0f / NTC_T0) +
     (1.0f / NTC_BETA) * log(resistance / NTC_R0));

  return tempK - 273.15f;
}

/* GPIO Init */
void GPIO_Init_Custom(void)
{
  __HAL_RCC_GPIOB_CLK_ENABLE();

  GPIO_InitTypeDef g = {0};
  g.Pin = GPIO_PIN_6;
  g.Mode = GPIO_MODE_OUTPUT_PP;
  HAL_GPIO_Init(GPIOB, &g);
}

/* ADC Init */
void ADC_Init_Custom(void)
{
  __HAL_RCC_ADC_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();

  GPIO_InitTypeDef g = {0};
  g.Pin = GPIO_PIN_0;
  g.Mode = GPIO_MODE_ANALOG;
  HAL_GPIO_Init(GPIOA, &g);

  hadc.Instance = ADC1;
  hadc.Init.Resolution = ADC_RESOLUTION_12B;
  hadc.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc.Init.ScanConvMode = ADC_SCAN_DISABLE;
  hadc.Init.ContinuousConvMode = DISABLE;
  hadc.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  HAL_ADC_Init(&hadc);

  ADC_ChannelConfTypeDef s = {0};
  s.Channel = ADC_CHANNEL_0;
  s.Rank = ADC_REGULAR_RANK_1;
  HAL_ADC_ConfigChannel(&hadc, &s);
}

/* UART Init */
void UART2_Init_Custom(void)
{
  __HAL_RCC_USART2_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();

  GPIO_InitTypeDef g = {0};
  g.Pin = GPIO_PIN_2 | GPIO_PIN_3;
  g.Mode = GPIO_MODE_AF_PP;
  g.Alternate = GPIO_AF1_USART2;
  HAL_GPIO_Init(GPIOA, &g);

  huart2.Instance = USART2;
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  HAL_UART_Init(&huart2);
}

/* printf redirect */
int _write(int file, char *ptr, int len)
{
  HAL_UART_Transmit(&huart2, (uint8_t*)ptr, len, HAL_MAX_DELAY);
  return len;
}
