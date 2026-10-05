# BeagleY-AI Reaction Timer

An embedded Linux reaction-time game written in C for the BeagleY-AI. The board flashes an LED in a random direction and measures how long the player takes to push an analog joystick the correct way, reading the joystick through an SPI ADC.

Built for ENSC 351 (Embedded and Real-Time System Software) at SFU.

## Hardware

- BeagleY-AI (TI AM67A, quad-core ARM Cortex-A53)
- MCP3208 12-bit SPI ADC, joystick X/Y on channels 0 and 1
- Adafruit analog thumb joystick breakout
- Onboard ACT and PWR LEDs, driven through sysfs

The ADC's VREF shares the joystick's 3.3 V supply, making the measurement
ratiometric: supply variation affects the input and the reference equally and
cancels out.

## Architecture

```
app/ game logic
hal/ hardware access
```

The application layer includes HAL headers only. The game was developed against
a stub joystick implementation before the circuit existed. Replacing the stub
with the SPI driver required no change to `app/src/main.c`.

## Calibration

Direction thresholds in `hal/src/joystick.c` were derived from measurements
taken with `adc_test.c`, which samples both ADC channels at 10 Hz. Resting
centre is approximately 2030 with full 0–4095 travel on both axes. Thresholds
sit at 75% of travel.

## Build

```sh
cmake -S . -B build -DCMAKE_TOOLCHAIN_FILE=toolchain-arm.cmake
cmake --build build
```

Cross-compiled with `aarch64-linux-gnu-gcc` and deployed to the target over NFS.
Built with `-Wall -Werror -Wpedantic -Wextra` and AddressSanitizer.

Run on the target with `sudo ./reaction_timer`; `/dev/spidev0.0` is root-only.

SPI must be enabled on the target before running. On this Debian image that
required a custom device tree overlay, included in `overlays/`.

## Limitations

- Thresholds are fixed at compile time. Runtime auto-calibration would tolerate
  a different joystick without recompiling.

