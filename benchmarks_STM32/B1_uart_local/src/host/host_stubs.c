/*
 * host_stubs.c
 *
 * Simple host-side replacements for OLED/Delay/etc used when building
 * the firmware as a host binary (for AFL++ / host testing).
 *
 * These implementations are intentionally tiny and print to stdout.
 */

#ifdef HOST_BUILD

#define _POSIX_C_SOURCE 199309L   /* make nanosleep visible on some systems */
#include <stdio.h>
#include <unistd.h>     /* for usleep */
#include <sys/types.h>  /* for useconds_t if needed */
#include <time.h>     /* nanosleep */
#include <unistd.h>
#include <stdint.h>

/* Initialize "OLED" on host: print a banner */
void OLED_Init(void) {
    printf("[HOST] OLED_Init()\n");
    fflush(stdout);
}

/* Show a string on OLED - on host we just print coordinates + string */
void OLED_ShowString(int x, int y, const char *s) {
    if (s)
        printf("[HOST] OLED_ShowString(%d,%d): %s\n", x, y, s);
    else
        printf("[HOST] OLED_ShowString(%d,%d): (null)\n", x, y);
    fflush(stdout);
}

/* Show a hex number - print on host */
void OLED_ShowHexNum(int x, int y, unsigned int v, int d) {
    (void)x; (void)y; (void)d;
    printf("[HOST] OLED_ShowHexNum: 0x%X\n", v);
    fflush(stdout);
}

/* Simple millisecond delay - shorten actual sleep so fuzzing is fast.
 * Adjust divisor if you need slower/faster behavior.
 */
void Delay_ms(int ms) {
    if (ms <= 0) return;
    /* shorten actual sleep to speed fuzzing, adjust divisor if needed */
    long usec = (long)ms * 1000L / 10L; /* same scaling as original host stub */
    struct timespec ts;
    ts.tv_sec = usec / 1000000L;
    ts.tv_nsec = (usec % 1000000L) * 1000L;
    /* ignore return value; best-effort sleep */
    nanosleep(&ts, NULL);
}

#endif /* HOST_BUILD */
