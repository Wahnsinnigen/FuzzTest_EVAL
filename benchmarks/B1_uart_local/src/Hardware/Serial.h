#ifndef __SERIAL_H
#define __SERIAL_H

#ifdef __cplusplus
extern "C" {
#endif

/* common integer types */
#include <stdint.h>

/*
 * Shared serial interface for both device (STM32) and host (fuzzable) builds.
 *
 * - Put only public prototypes, config macros and shared externs here.
 * - Do NOT include real MCU headers (stm32f10x.h) in this header; include
 *   those only inside device source files or use a tiny stub in host builds.
 */

/* --- Config: RX buffer size for host harness (override before include if needed) --- */
#ifndef RX_BUF_SIZE
#define RX_BUF_SIZE 256
#endif

/* --- Host-only shared externs (only exported for HOST_BUILD) --- */
#if defined(HOST_BUILD)
/* Circular rx buffer and indices (volatile because host harness may update). */
extern volatile uint8_t rx_flag;               /* non-zero when data available */
extern volatile uint8_t rx_buffer[RX_BUF_SIZE];/* circular RX buffer */
extern volatile uint32_t rx_head;              /* next write index */
extern volatile uint32_t rx_tail;              /* next read index */
#endif /* HOST_BUILD */

/* --- Public API (functions implemented in Serial.c) --- */

/* Initialize serial subsystem
 * - device: configure hardware
 * - host:   reset indices / prepare harness
 */
void Serial_Init(void);

/* Send helpers */
void Serial_SendByte(uint8_t byte);
void Serial_SendArray(uint8_t *array, uint16_t length);
void Serial_SendString(char *str);
void Serial_SendNumber(uint32_t number, uint8_t length);

/* printf-like helper (may be thin wrapper to vprintf / Serial_SendString) */
void Serial_Printf(char *format, ...);

/* RX helpers (polling-style API)
 * Return non-zero if an RX byte is available (clears the flag if applicable).
 */
uint8_t Serial_GetRxFlag(void);

/* Retrieve a single received byte. Some code uses GetRxData, others use
 * ReceiveData. We export both for convenience. */
uint8_t Serial_GetRxData(void);
uint8_t Serial_ReceiveData(void);

/* --- Fuzzing helpers (new) --- */
/* Inject a single byte into the device's receive path. The fuzz adapter calls this. */
void Serial_InjectByte(uint8_t b);

/* Central single-byte handler: move the logic that processes an incoming byte here.
 * This allows both the real main loop and the fuzz adapter to call the same handler.
 */
void Serial_HandleByte(uint8_t b);

#ifdef __cplusplus
}
#endif

#endif /* __SERIAL_H */
