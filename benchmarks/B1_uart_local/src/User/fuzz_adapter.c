/* fuzz_adapter.c
 * Provide non-weak override for fuzz_target_input so AFL input bytes
 * are injected into the serial processing path.
 */

#include <stdint.h>
#include "Serial.h"

/* This function overrides the weak fallback in main.c.
   It simply injects each fuzzed byte into the Serial module's injection API. */
void fuzz_target_input(uint8_t b)
{
    /* Call the injection helper — this will forward to Serial_HandleByte */
    Serial_InjectByte(b);
}

