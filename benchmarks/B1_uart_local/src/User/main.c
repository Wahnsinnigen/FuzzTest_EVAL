/* main.c - dual build: device (STM32) and host/fuzzable binary
 *
 * - When building for host tests use:   -DHOST_BUILD
 * - When building fuzz harness use:     -DFUZZING
 *
 * This file:
 *  - renames original user main -> real_main()
 *  - provides a weak fuzz_target_input() hook (overridable by fuzz_adapter.c)
 *  - implements FUZZING main() that reads bytes and forwards to fuzz_target_input()
 *
 * Comments and helper code are intentionally minimal and portable.
 */

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

/* Device vs Host includes:
 * - HOST_BUILD: include a tiny host stub (if provided) so host compile doesn't need
 *               the full STM32 SDK headers.
 * - Device build: include real MCU headers and device helpers.
 */
#ifdef HOST_BUILD
  /* Optional tiny stub header for host builds (should live in ./host) */
  #include "stm32f10x_stub.h"   /* provides minimal typedefs/macros used by device code */
  #include "host_stubs.h"       /* prototypes for OLED/Delay on host */
#else
  /* Real device headers (only used when building firmware for the board) */
  #include "stm32f10x.h"
  #include "Delay.h"
  #include "OLED.h"
#endif

/* Shared serial API (both builds) */
#include "Serial.h"
#include <stdint.h>

/* ----------------------------------------------------------------------
 * Keep original behaviour in real_main (this is the original main() you had).
 * We rename user's original main to real_main so the fuzz harness can call it
 * when not in FUZZING mode.
 * -------------------------------------------------------------------- */
uint8_t RxData; /* original global used by the sample project */

int real_main(void)
{
    OLED_Init();
    OLED_ShowString(1, 1, "RxData:");

    Serial_Init();

    while (1)
    {
        if (Serial_GetRxFlag() == 1)
        {
            RxData = Serial_GetRxData();
            Serial_SendByte(RxData);
            OLED_ShowHexNum(1, 8, RxData, 2);
        }
    }

    /* unreachable for embedded loop, keep int signature */
    return 0;
}

/* ----------------------------------------------------------------------
 * Fuzz hook: the fuzz harness calls fuzz_target_input(uint8_t)
 * We provide a weak fallback here which simply mirrors original main loop:
 * send byte back and show on OLED/stdout.
 *
 * You can override this in:
 *  - benchmarks/B1_uart_local/src/User/fuzz_adapter.c  (preferred), or
 *  - by adding a (non-weak) Serial_InjectByte() implementation in Serial.c
 * -------------------------------------------------------------------- */
void __attribute__((weak)) fuzz_target_input(uint8_t b)
{
    /* Default safe behaviour: forward to Serial_SendByte (host prints to stdout).
     * On device the OLED call is available; on host it won't (we guard it).
     */
    Serial_SendByte(b);

#ifdef HOST_BUILD
    /* host: Serial_SendByte prints to stdout (host_stubs.c) */
    (void)b;
#else
    /* device: also display hex on OLED */
    OLED_ShowHexNum(1, 8, b, 2);
#endif
}

/* ----------------------------------------------------------------------
 * Application entry:
 *  - when not fuzzing, call original real_main()
 *  - when fuzzing, read file/stdin and feed bytes to fuzz_target_input()
 * -------------------------------------------------------------------- */
#ifndef FUZZING
int main(void)
{
    return real_main();
}
#else

int main(int argc, char **argv)
{
    FILE *f = stdin;

    if (argc >= 2) {
        f = fopen(argv[1], "rb");
        if (!f) {
            perror("fopen");
            return 1;
        }
    }

    int c;
    while ((c = fgetc(f)) != EOF) {
        fuzz_target_input((uint8_t)c);
    }

    if (f != stdin) fclose(f);
    return 0;
}
#endif /* FUZZING */
