#ifndef INPUT_H
#define INPUT_H

typedef enum {
    Unknown = 0,
    Up = 1,
    Down = 2,
    Enter = 3,
    Back = 4
} InputValue;

int input_init();
InputValue input_get();
void input_wait_enter();
void input_stop();
#endif