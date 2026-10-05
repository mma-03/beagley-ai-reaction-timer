// main.c: implementation of game logic

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>
#include <limits.h>
#include "hal/led.h"
#include "hal/timing.h"
#include "hal/joystick.h"

// local helper functions

// welcome message and instructions
static void showWelcome(void) {
    printf("Hello embedded world, from Maaz!\n\n");
    printf("When the LEDs light up, press the joystick in that direction!\n");
    printf("(Press left or right to exit)\n");
}

// ready message and flash
static void getReadyFlash(void) {
    printf("Get ready...\n");
    
    // flash 4 times
    for (int i  = 0; i < 4; i++) {
        led_on(LED_GREEN);
        sleepForMs(250);
        led_off(LED_GREEN);
        led_on(LED_RED);
        sleepForMs(250);
        led_off(LED_RED);
    }
}

// block until joystick is not held up or down, warns the user
static void waitForRelease(void) {
    joystick_direction_t direction = joystick_get_direction();
    if(direction == JOYSTICK_UP || direction == JOYSTICK_DOWN){
        printf("Please let go of the joystick\n");
        while(direction == JOYSTICK_UP || direction == JOYSTICK_DOWN) {
            sleepForMs(10);
            direction = joystick_get_direction(); // refresh for next test loop
        }
    }
}

// wait for any joystick press, up to timeoutMS
// returns true and fills pDirection/pElapsedMs on press
// returns false on timeout
static bool waitForPress(long long timeoutMS, joystick_direction_t* pDirection, long long* pElapsedMs){
    long long startTime = getTimeInMs();
    while(true) {
        joystick_direction_t direction = joystick_get_direction();
        if(direction != JOYSTICK_NONE) {
            *pDirection = direction;
            *pElapsedMs = getTimeInMs() - startTime;
            return true;
        } else if (getTimeInMs() - startTime >= timeoutMS) {
            return false;
        }
        sleepForMs(10);
    }
}

int main(void) {
    // startup: runs once
    led_init(); // initialize led
    joystick_init(); // initialize joystick
    srand(time(NULL)); // initialize random generator
    showWelcome();
    const long long TIMEOUT_MS = 5000;

    // very large value so the first correct answer can beat it
    long long bestTimeMs = LLONG_MAX;

    bool running = true;
    // each loop is one round of the game
    while(running) {
        getReadyFlash();
        waitForRelease();
        
        // sleep a random 500-3000ms
        int randDelayMs = 500 + rand() % (2501);
        sleepForMs(randDelayMs);

        // read joystick once, if up/down => too soon
        joystick_direction_t direction = joystick_get_direction();
        if (direction == JOYSTICK_UP || direction == JOYSTICK_DOWN) {
            printf("Too soon\n");
            continue;
        }

        // pick a random direction (up or down)
        joystick_direction_t target;
        if (rand() % 2 == 0) {
            target = JOYSTICK_UP;
            printf("Press UP now!\n");
            led_on(LED_GREEN);
        } else {
            target = JOYSTICK_DOWN;
            printf("Press DOWN now!\n");
            led_on(LED_RED);
        }

        joystick_direction_t pressed;
        long long elapsedMs;
        bool gotPress = waitForPress(TIMEOUT_MS, &pressed, &elapsedMs);
        led_off(LED_GREEN);
        led_off(LED_RED);

        
        if(!gotPress) {
            // no response within 5 seconds
            printf("No input within %lld ms; quitting!\n", TIMEOUT_MS);
            running = false;
        } else if (pressed == JOYSTICK_RIGHT || pressed == JOYSTICK_LEFT) {
            // pressed left or right to quit
            printf("User selected to quit.\n");
            running = false;
        } else if (pressed == target) {
            // correct response
            printf("Correct!\n");
            if (elapsedMs < bestTimeMs) {
                printf("New best time!\n");
                bestTimeMs = elapsedMs;
            }
            printf("Your reaction time was %5lld ms; best so far in game is %5lld ms.\n", elapsedMs, bestTimeMs);
            led_flash(LED_GREEN, 5, 100, 100);
        } else {
            // incorrect response
            printf("Incorrect.\n");
            led_flash(LED_RED, 5, 100, 100);
        }
    }
    // shutdown: runs once
    led_cleanup();
    joystick_cleanup();
    return 0;
}