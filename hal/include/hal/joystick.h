// hardware abstraction layer for joystick

#ifndef _JOYSTICK_H
#define _JOYSTICK_H

typedef enum {
    JOYSTICK_UP,
    JOYSTICK_DOWN,
    JOYSTICK_RIGHT,
    JOYSTICK_LEFT,
    JOYSTICK_NONE
} joystick_direction_t;

// initialization for joystick
void joystick_init(void);

// get firection of joystick
joystick_direction_t joystick_get_direction(void);

// clean up
void joystick_cleanup(void);

#endif
