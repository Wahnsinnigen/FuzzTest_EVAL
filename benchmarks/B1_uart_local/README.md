# B1_uart_local

**Short description**  
Minimal UART example (original Keil project adapted) — benchmark target for AFL++ fuzzing experiments.

## Location / Revision
- Branch: `fuzzing`
- Commit: (fill commit SHA after push)

## Contents
- `src/` : (future) minimal C sources for fuzzing build
- `scripts/` : run scripts (qemu/gdb wrappers)
- `seeds/` : AFL input seeds

## Build & run (high-level, fill when ready)
Requirements: arm-none-eabi-gcc, qemu-system-arm, arm-none-eabi-gdb, AFL++.

Example (when `Makefile` and scripts present):
```bash
cd benchmarks/B1_uart_local
make
./scripts/run_qemu.sh      # start qemu (paused)
afl-fuzz -i seeds -o outputs -- ./scripts/run_target_via_mem.sh @@
