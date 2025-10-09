/*
 * stm32f10x_stub.h
 *
 * Minimal stubs so host build can include files that normally require STM32
 * vendor headers. Define only the symbols used by the firmware so host
 * compilation succeeds. All implementations are no-ops or simple macros.
 *
 * Put this file in src/host and compile with -I./src/host -DHOST_BUILD
 */

#ifndef STM32F10X_STUB_H
#define STM32F10X_STUB_H

/* Minimal integer types */
#include <stdint.h>
typedef uint32_t  uint32_t_t; /* not used but ok */
typedef uint32_t  u32;
typedef unsigned short uint16_t;
typedef unsigned char  uint8_t;

/* Provide common fixed-width types for rest of code */
typedef uint32_t  uint32_t;
typedef uint16_t  uint16_t;
typedef uint8_t   uint8_t;

/* ENABLE/DISABLE */
#ifndef ENABLE
#define ENABLE  1
#endif
#ifndef DISABLE
#define DISABLE 0
#endif

/* Dummy GPIO/USART/NVIC "types" that match member names used by Serial.c */
typedef struct {
    /* Only fields referenced by Serial.c are included */
    int GPIO_Mode;
    int GPIO_Pin;
    int GPIO_Speed;
} GPIO_InitTypeDef;

typedef struct {
    int NVIC_IRQChannel;
    int NVIC_IRQChannelCmd;
    int NVIC_IRQChannelPreemptionPriority;
    int NVIC_IRQChannelSubPriority;
} NVIC_InitTypeDef;

typedef struct {
    int USART_BaudRate;
    int USART_HardwareFlowControl;
    int USART_Mode;
    int USART_Parity;
    int USART_StopBits;
    int USART_WordLength;
} USART_InitTypeDef;

/* Dummy defines for pins/speeds */
#define GPIO_Pin_9         9
#define GPIO_Pin_10        10
#define GPIO_Speed_50MHz   50

/* Dummy GPIO mode constants used in the code */
#define GPIO_Mode_AF_PP  1
#define GPIO_Mode_IPU    2

/* Dummy USART constants */
#define USART_Mode_Tx    0x01
#define USART_Mode_Rx    0x02
#define USART_HardwareFlowControl_None 0
#define USART_Parity_No 0
#define USART_StopBits_1 1
#define USART_WordLength_8b 8

/* Dummy IT/flag values */
#define USART_IT_RXNE  1
#define USART_FLAG_TXE 1

/* Generic SET/RESET */
#define SET   1
#define RESET 0

/* Dummy peripheral identifiers and IRQn */
#define USART1 ((void*)1)
#define USART1_IRQn  37   /* arbitrary */

/* NVIC priority group used in original code */
#define NVIC_PriorityGroup_2 0

/* Minimal stub functions used by Serial.c or start files:
   these are no-op for host builds */
static inline void RCC_APB2PeriphClockCmd(int a, int b) { (void)a; (void)b; }
static inline void GPIO_Init(void* a, GPIO_InitTypeDef* b) { (void)a; (void)b; }
static inline void USART_Init(void* a, USART_InitTypeDef* b) { (void)a; (void)b; }
static inline void NVIC_Init(NVIC_InitTypeDef* n) { (void)n; }
static inline void USART_ITConfig(void* a, int it, int en) { (void)a; (void)it; (void)en; }
static inline int USART_GetITStatus(void* a, int it) { (void)a; (void)it; return 0; }
static inline uint8_t USART_ReceiveData(void* a) { (void)a; return 0; }
static inline void USART_ClearITPendingBit(void* a, int it) { (void)a; (void)it; }

/* Simple GPIO/USART "objects" placeholders */
#define GPIOA ((void*)1)

/* fputc / printf related: none here; Serial.c will provide fputc/Serial_SendByte */

#endif /* STM32F10X_STUB_H */
