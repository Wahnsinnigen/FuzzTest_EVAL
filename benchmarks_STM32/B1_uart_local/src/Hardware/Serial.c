#include "Serial.h"

/* Common includes used in both variant (stdarg/stdio required by Printf) */
#include <stddef.h>

#ifdef HOST_BUILD
  /* Host build: include stdio/strings etc. for stubs */
  #include <stdio.h>
  #include <stdarg.h>
  #include <string.h>

  /* Host-only rx buffer & flags */
  volatile uint8_t rx_flag = 0;
  volatile uint8_t rx_buffer[RX_BUF_SIZE];
  volatile uint32_t rx_head = 0;
  volatile uint32_t rx_tail = 0;

  /* Forward declaration */
  void Serial_HandleByte(uint8_t b);

  void Serial_Init(void) {
      rx_head = rx_tail = 0;
      rx_flag = 0;
  }

  void Serial_SendByte(uint8_t b) {
      putchar((int)b);
      fflush(stdout);
  }

  void Serial_SendArray(uint8_t *Array, uint16_t Length) {
      if (!Array) return;
      for (uint16_t i = 0; i < Length; i++) {
          Serial_SendByte(Array[i]);
      }
  }

  void Serial_SendString(char *String) {
      if (!String) return;
      for (size_t i = 0; String[i] != '\0'; i++) {
          Serial_SendByte((uint8_t)String[i]);
      }
  }

  uint32_t Serial_Pow(uint32_t X, uint32_t Y) {
      uint32_t Result = 1;
      while (Y--) Result *= X;
      return Result;
  }

  void Serial_SendNumber(uint32_t Number, uint8_t Length) {
      for (uint8_t i = 0; i < Length; i++) {
          uint32_t d = (Number / Serial_Pow(10, Length - i - 1)) % 10;
          Serial_SendByte((uint8_t)(d + '0'));
      }
  }

  int fputc(int ch, FILE *f) {
      Serial_SendByte((uint8_t)ch);
      return ch;
  }

  void Serial_Printf(char *format, ...) {
      char buf[256];
      va_list arg;
      va_start(arg, format);
      vsnprintf(buf, sizeof(buf), format, arg);
      va_end(arg);
      Serial_SendString(buf);
  }

  uint8_t Serial_GetRxFlag(void) {
      return (uint8_t)rx_flag;
  }

  uint8_t Serial_GetRxData(void) {
      /* If you want to emulate ISR+main loop, read from buffer */
      uint8_t v = 0;
      if (rx_head != rx_tail) {
          v = rx_buffer[rx_tail++];
          if (rx_tail >= RX_BUF_SIZE) rx_tail = 0;
          if (rx_tail == rx_head) rx_flag = 0;
      }
      return v;
  }

  void Serial_InjectByte(uint8_t b) {
      /* Put into circular buffer and set flag */
      rx_buffer[rx_head++] = b;
      if (rx_head >= RX_BUF_SIZE) rx_head = 0;
      rx_flag = 1;

      /* Also call handler synchronously for deterministic fuzzing */
      Serial_HandleByte(b);
  }

  /* Example single-byte handler used by both host and device */
  void Serial_HandleByte(uint8_t b) {
      /* Echo back */
      Serial_SendByte(b);

      /* Print debug line on host so we see activity */
      Serial_Printf("[HOST-OLED] byte=0x%02X\n", b);

      /* If you want to emulate OLED_ShowHexNum, you can do it here as print */
  }

#else  /* device (Keil) build: original MCU implementation */

  /* For device side we need MCU headers and stdarg (used in Printf) */
#include "stm32f10x.h"
#include "OLED.h"
#include <stdarg.h>
#include <stdio.h>


uint8_t Serial_RxData;
uint8_t Serial_RxFlag;


void Serial_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	USART_InitTypeDef USART_InitStructure;
	USART_InitStructure.USART_BaudRate = 9600;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_Init(USART1, &USART_InitStructure);
	
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_Init(&NVIC_InitStructure);
	
	USART_Cmd(USART1, ENABLE);
}

void Serial_SendByte(uint8_t Byte)
{
	USART_SendData(USART1, Byte);
	while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
}

void Serial_SendArray(uint8_t *Array, uint16_t Length)
{
	uint16_t i;
	for (i = 0; i < Length; i ++)
	{
		Serial_SendByte(Array[i]);
	}
}

void Serial_SendString(char *String)
{
	uint8_t i;
	for (i = 0; String[i] != '\0'; i ++)
	{
		Serial_SendByte(String[i]);
	}
}

uint32_t Serial_Pow(uint32_t X, uint32_t Y)
{
	uint32_t Result = 1;
	while (Y --)
	{
		Result *= X;
	}
	return Result;
}

void Serial_SendNumber(uint32_t Number, uint8_t Length)
{
	uint8_t i;
	for (i = 0; i < Length; i ++)
	{
		Serial_SendByte(Number / Serial_Pow(10, Length - i - 1) % 10 + '0');
	}
}

int fputc(int ch, FILE *f)
{
	Serial_SendByte(ch);
	return ch;
}

void Serial_Printf(char *format, ...)
{
	char String[100];
	va_list arg;
	va_start(arg, format);
	vsprintf(String, format, arg);
	va_end(arg);
	Serial_SendString(String);
}

uint8_t Serial_GetRxFlag(void)
{
	if (Serial_RxFlag == 1)
	{
		Serial_RxFlag = 0;
		return 1;
	}
	return 0;
}

uint8_t Serial_GetRxData(void)
{
	return Serial_RxData;
}

void USART1_IRQHandler(void)
{
	if (USART_GetITStatus(USART1, USART_IT_RXNE) == SET)
	{
		Serial_RxData = USART_ReceiveData(USART1);
		Serial_RxFlag = 1;
		USART_ClearITPendingBit(USART1, USART_IT_RXNE);
	}
}

#endif /* HOST_BUILD */