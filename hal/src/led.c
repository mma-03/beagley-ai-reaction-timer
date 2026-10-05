// hal/src/led.c
// implementation for led.h
#include "hal/led.h"
#include "hal/timing.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>

// file-scope constants

#define MAX_LED_PATH 64

static const char* const LED_PATHS[NUM_LEDS] = {
    "/sys/class/leds/ACT", // LED_GREEN
    "/sys/class/leds/PWR"  // LED_RED
};

static bool is_initialized = false;

// private helper
// takes which LED, which file, and what to write
static void writeLedFile(led_colour_t led, const char* filename, const char* value) {
    // reserve bytes
    char path[MAX_LED_PATH];

    // writes to path
    // snprintf(where to write, how many bytes available, format, values for the two %s)
    snprintf(path, MAX_LED_PATH, "%s/%s", LED_PATHS[led], filename);

    // open the file
    FILE* pFile = fopen(path, "w");
    // check for NULL
    if(pFile == NULL) {
        fprintf(stderr, "Error opening the LED file: %s\n", path);
        perror("Reason");
        exit(EXIT_FAILURE);
    }

    // write to the file
    int charsWritten = fprintf(pFile, "%s", value);
    // write check
    if (charsWritten <= 0) {
        fprintf(stderr, "Error writing to the LED file: %s\n", path);
        perror("Reason");
        exit(EXIT_FAILURE);
    }

    // close the file
    // close check
    if (fclose(pFile) != 0) {
        fprintf(stderr, "Error closing the LED file: %s\n", path);
        perror("Reason");
        exit(EXIT_FAILURE);
    }
}

// public functions implementing led.h

void led_init(void) {
    assert(!is_initialized);
    for (led_colour_t i = 0; i < NUM_LEDS; i++) {
        // set both triggers
        writeLedFile(i, "trigger", "none");

        // 100 ms delay
        sleepForMs(100);

        // set both brightness
        writeLedFile(i, "brightness", "0");
    }
    // flag
    is_initialized = true;
}

void led_on(led_colour_t led) {
    assert(is_initialized);
    writeLedFile(led, "brightness", "1");
}

void led_off(led_colour_t led) {
    assert(is_initialized);
    writeLedFile(led, "brightness", "0");
}

void led_flash(led_colour_t led, int count, int on_ms, int off_ms) {
    assert(is_initialized);
    for(int i = 0; i < count; i++) {
        led_on(led);
        sleepForMs(on_ms);
        led_off(led);
        sleepForMs(off_ms);
    }
}

void led_cleanup(void) {
    assert(is_initialized);
    for(led_colour_t i = 0; i < NUM_LEDS; i++) {
        writeLedFile(i, "brightness", "0");
    }
    is_initialized = false;
}
