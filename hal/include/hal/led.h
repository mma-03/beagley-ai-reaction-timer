// Hardware abstraction layer : LED
#ifndef _LED_H
#define _LED_H

typedef enum {
    LED_GREEN, 
    LED_RED,
    NUM_LEDS
} led_colour_t;

// initialize LED
// manual control of both LEDS by setting trigger to none
void led_init(void); 

// flash the LED on for (on_ms), then off for (off_ms), (count) times
void led_flash(led_colour_t led, int count, int on_ms, int off_ms); 

// turn the LED on
void led_on(led_colour_t led);

// turn off
void led_off(led_colour_t led);

// clean up
// turns both LEDs off
void led_cleanup(void);

#endif