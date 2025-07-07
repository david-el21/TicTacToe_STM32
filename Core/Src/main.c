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
#include "usb_device.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>      // add for printf
#include <string.h>
#include <stdbool.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define BOARD 3
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
TIM_HandleTypeDef htim2;
UART_HandleTypeDef huart1;

/* USER CODE BEGIN PV */
uint8_t uart_buf[1];
// initializare tabla 3x3
char board[BOARD][BOARD];
// verificare ca ledul se aprinde pentru pinul corect
//ROSU
int ledpinr[9] = {0,1,2,3,4,5,6,7,13};
//VERDE
int ledpinv[9] = {5,6,7,8,9,10,11,12,13}; 
//Initializare vectori pentru utilizarea mai usoara si eficienta a pinilor
uint16_t greenPins[] = {GPIO_PIN_2, GPIO_PIN_3, GPIO_PIN_4, GPIO_PIN_5, GPIO_PIN_6, GPIO_PIN_7, GPIO_PIN_8, GPIO_PIN_9, GPIO_PIN_10}; //VERZI
uint16_t redPins[] = {GPIO_PIN_0, GPIO_PIN_1, GPIO_PIN_2, GPIO_PIN_3, GPIO_PIN_4, GPIO_PIN_5, GPIO_PIN_6, GPIO_PIN_7, GPIO_PIN_13}; // ROSII
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART1_UART_Init(void);
static void MX_TIM2_Init(void);
void initializeBoard();
void printBoard();
void ResetLEDS();

/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

#ifdef __GNUC__
#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif

/**
  * @brief  Retargets the C library printf function to the USART.
  * @param  None
  * @retval None
  */
PUTCHAR_PROTOTYPE
{
  HAL_UART_Transmit(&huart1, (uint8_t *)&ch, 1, 0xFFFF);
  return ch;
}

/* also redefine _write for printf*/
int _write(int fd, char * ptr, int len)
{
  HAL_UART_Transmit(&huart1, (uint8_t *) ptr, len, HAL_MAX_DELAY);
  return len;
}
// Functie pentru trimiterea mesajelor prin UART
void sendMessage(const char* message) {
    HAL_UART_Transmit(&huart1, (uint8_t*)message, strlen(message), HAL_MAX_DELAY);
}

// Functie pentru citirea unui numar de la UART
int readSerialNumber(void) {
    uint8_t character;
    while(1) {
        HAL_UART_Receive(&huart1, &character, 1, HAL_MAX_DELAY);
        if(character >= '1' && character <= '9') {
            return (character - '0');
        }
    }
}

// Initializeaza tabla cu spatii goale 
void initializeBoard() {
    for (int i = 0; i < BOARD; i++) {
        for (int j = 0; j < BOARD; j++) {
            board[i][j] = ' ';
        }
    }
}

// reseteaza led-urile la starea 0
void resetLEDS() {
  for (int i = 0; i < 0; i++){
    HAL_GPIO_WritePin(GPIOA, redPins[i], GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOB, greenPins[i], GPIO_PIN_RESET);
  }
}

// Printeaza tabla pe consola
void printBoard() {
  char buffer[100];

  for (int i = 0; i < BOARD; i++) {
        for (int j = 0; j < BOARD; j++) {
            sprintf(buffer, " %c ", board[i][j]);
            sendMessage(buffer);
            if (j < BOARD - 1) sendMessage("|");
        }
        printf("\n");
        if (i < BOARD - 1) sendMessage("---+---+---\n");
    }
}

int checkWin() {
    // Linii, coloane si diagonale 'X' SAU '0'
    for (int i = 0; i < 3; i++) {
        if (board[i][0] != ' ' && board[i][0] == board[i][1] && board[i][1] == board[i][2]) {
            return 1;
        }
        if (board[0][i] != ' ' && board[0][i] == board[1][i] && board[1][i] == board[2][i]) {
            return 1;
        }
    }

    if (board[0][0] != ' ' && board[0][0] == board[1][1] && board[1][1] == board[2][2]) {
        return 1;
    }
    if (board[0][2] != ' ' && board[0][2] == board[1][1] && board[1][1] == board[2][0]) {
        return 1;
    }
    // Verifica remiza
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            if (board[i][j] == ' ') {
                return 0; // Inca merge jocul
            }
        }
    }

    return -1; // remiza
}

// Efect de victorie pentru X
void victoryX(void) {
    // Stingem ledurile
    for(int i = 0; i < 9; i++) {
        HAL_GPIO_WritePin(GPIOA, redPins[i], GPIO_PIN_RESET);
        HAL_GPIO_WritePin(GPIOB, greenPins[i], GPIO_PIN_RESET);
    }
    
    // Efect de clipire pentru victoria lui X
    for(int i = 0; i < 9; i++) {
        // Aprinde toate LED-urile
        for(int j = 0; j < 9; j++) {
            HAL_GPIO_WritePin(GPIOA, redPins[i], GPIO_PIN_SET);
        }
        HAL_Delay(200);
        
        // Stinge toate LED-urile
        for(int j = 0; j < 9; j++) {
            HAL_GPIO_WritePin(GPIOA, redPins[i], GPIO_PIN_RESET);
        }
        HAL_Delay(200);
    }
}

// Efect de victorie pentru 0
void victory0(void) {
    // Stingem ledurile
    for(int i = 0; i < 9; i++) {
        HAL_GPIO_WritePin(GPIOA, redPins[i], GPIO_PIN_RESET);
        HAL_GPIO_WritePin(GPIOB, greenPins[i], GPIO_PIN_RESET);
    }
    
    // Efect de clipire pentru victoria lui 0
    for(int i = 0; i < 9; i++) {
        // Aprinde toate LED-urile
        for(int j = 0; j < 9; j++) {
            HAL_GPIO_WritePin(GPIOB, greenPins[i], GPIO_PIN_SET);
        }
        HAL_Delay(200);
        
        // Stinge toate LED-urile
        for(int j = 0; j < 9; j++) {
            HAL_GPIO_WritePin(GPIOB, greenPins[i], GPIO_PIN_RESET);
        }
        HAL_Delay(200);
    }
}

void alternatingEffect(void) {
    // Repetam efectul de 3 ori
    for(int repeat = 0; repeat < 3; repeat++) {
        // Mergem de la stanga la dreapta
        for(int i = 0; i < 9; i++) {
            // Aprinde LED-ul curent (rosu cat si verde)
            HAL_GPIO_WritePin(GPIOA, redPins[i] , GPIO_PIN_SET);
            HAL_GPIO_WritePin(GPIOB, greenPins[i] , GPIO_PIN_SET);
            
            HAL_Delay(100);  // Asteapta
            
            // Stinge LED-ul curent
            HAL_GPIO_WritePin(GPIOA, redPins[i] , GPIO_PIN_RESET);
            HAL_GPIO_WritePin(GPIOB, greenPins[i] , GPIO_PIN_RESET);
        }
        
        // Mergem de la dreapta la stânga
        for(int i = 8; i >= 0; i--) {
            // Aprinde LED-ul curent (rosu cat si verde)
            HAL_GPIO_WritePin(GPIOA, redPins[i] , GPIO_PIN_SET);
            HAL_GPIO_WritePin(GPIOB, greenPins[i] , GPIO_PIN_SET);
            
            HAL_Delay(100);  // Asteapta 
            
            // Stinge LED-ul curent
            HAL_GPIO_WritePin(GPIOA, redPins[i] , GPIO_PIN_RESET);
            HAL_GPIO_WritePin(GPIOB, greenPins[i] , GPIO_PIN_RESET);
        }
    }
}

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  int player = 1; //Player 1 starts
  int decizie;
  char buffer[80];
  int position;
  int row, col;

  HAL_Init();

  SystemClock_Config();

  MX_GPIO_Init();
  MX_USART1_UART_Init();
  MX_USB_DEVICE_Init();
  MX_TIM2_Init();
  
  HAL_UART_Receive_IT(&huart1, uart_buf, 1); 
  HAL_TIM_Base_Start_IT(&htim2);  

  if (HAL_UART_Init(&huart1) != HAL_OK) {
        Error_Handler();
    }
    resetLEDS();
    initializeBoard();
    while(1) {
    printBoard();
    sprintf(buffer, "Player %d, enter a number ( 1 - 9 ):\r\n ", player);
    sendMessage(buffer);
    position = readSerialNumber();
    if( player == 1 ) {
      sprintf(buffer, "Pinul pentru ledul rosu PA%d", ledpinr[position - 1]);
      sendMessage(buffer);
      HAL_GPIO_WritePin(GPIOA, redPins[position - 1], GPIO_PIN_SET);
    } else {
      sprintf(buffer, "Pinul pentru ledul verde PB%d", ledpinv[position - 1]);
      sendMessage(buffer);
      HAL_GPIO_WritePin(GPIOB, greenPins[position - 1], GPIO_PIN_SET);
    }
    sendMessage("\r\n");
    row = (position - 1) / 3;
    col = (position - 1) % 3;

    if (position < 1 || position > 9 || board[row][col] != ' ') {
          printf("Invalid input. Try again.\n");
        continue;
    }

    board[row][col] = (player == 1) ? 'X' : 'O';

    int result = checkWin();
    if (result == 1) {
        printBoard();
        sprintf(buffer, "Player %d wins!\n", player);
        sendMessage(buffer);
        if(player == 1) {
          victoryX();
        } else {
        victory0();
        }
    } else if (result == -1) {
        printBoard();
        sendMessage("It's a draw!\n");
        alternatingEffect();
     }

      player = (player == 1) ? 2 : 1;
      
      if ( result == 1 || result == -1) {
        sprintf(buffer, " Daca doriti un nou joc introduceti tasta '1' sau '2' pentru a opri jocul: ");
        sendMessage(buffer);
        decizie = readSerialNumber();
          if ( decizie == 1){
            resetLEDS();
            initializeBoard();
            continue;
          }
            else {
              sprintf(buffer,"Jocul a fost oprit! Apasati reset pentru un nou joc.");
              sendMessage(buffer);
              break;
            }
      }
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
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USB;
  PeriphClkInit.UsbClockSelection = RCC_USBCLKSOURCE_PLL_DIV1_5;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
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
  TIM_OC_InitTypeDef sConfigOC = {0};

  /* USER CODE BEGIN TIM2_Init  1 */

  /* USER CODE END TIM2_Init 1 */
  htim2.Instance = TIM2;
  htim2.Init.Prescaler = 7199;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = 100;
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
  if (HAL_TIM_OC_Init(&htim2) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_TIMING;
  sConfigOC.Pulse = 0;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  if (HAL_TIM_OC_ConfigChannel(&htim2, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM2_Init 2 */

  /* USER CODE END TIM2_Init 2 */

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
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA,GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7|GPIO_PIN_13, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB,GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7|GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_10|GPIO_PIN_11, GPIO_PIN_RESET);

  /*Configure GPIO pin : PC13 */
  GPIO_InitStruct.Pin = GPIO_PIN_13;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : PB2 - PB10*/
  GPIO_InitStruct.Pin = GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7|GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_10;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pin : PA0 - PA8 precum si PA13*/ 
  GPIO_InitStruct.Pin = GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_0|GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7|GPIO_PIN_13;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

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