#include <p24FJ128GB206.h>
#include <stdint.h>
#include "config.h"
#include "common.h"
#include "ui.h"
#include "usb.h"
#include "pin.h"
#include "int.h"
#include "i2c.h"
#include "uart.h"

void red() {
    led_toggle(&led1);
}

void green() {
    led_toggle(&led2);
}

void blue() {
    led_toggle(&led3);
}

int16_t main(void) {
    init_clock();
    init_ui();
    init_pin();
    init_int();
    init_i2c();
    init_uart();

    pin_digitalIn(&D[0]);
    int_attach(&int1, &D[0], 1, red);

    pin_digitalIn(&D[1]);
    int_attach(&int2, &D[1], 0, green);

    pin_digitalIn(&D[2]);
    int_attach(&int3, &D[2], 0, blue);

    pin_digitalIn(&D[3]);
    int_attach(&int4, &D[3], 0, blue);

    led_on(&led1);

    while(1) {}
}
