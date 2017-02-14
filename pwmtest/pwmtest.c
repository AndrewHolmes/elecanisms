#include <p24FJ128GB206.h>
#include "config.h"
#include "common.h"
#include "usb.h"
#include "pin.h"
#include "ui.h"
#include "timer.h"
#include "oc.h"

#define TOGGLE_LED1     0
#define SET_DUTY        1
#define GET_DUTY        2

uint16_t    current;

void __attribute__((interrupt, auto_psv)) _OC1Interrupt (void) {
    IFS0bits.OC1IF = 0;
    pin_set(&D[12]);
    current = pin_read(&A[0]);
    pin_clear(&D[12]);
}

//void ClassRequests(void) {
//    switch (USB_setup.bRequest) {
//        default:
//            USB_error_flags |= 0x01;                    // set Request Error Flag
//    }
//}

void VendorRequests(void) {
    WORD temp;

    switch (USB_setup.bRequest) {
        case TOGGLE_LED1:
            led_toggle(&led1);
            BD[EP0IN].bytecount = 0;    // set EP0 IN byte count to 0 
            BD[EP0IN].status = 0xC8;    // send packet as DATA1, set UOWN bit
            break;
        case SET_DUTY:
            pin_write(&D[13], USB_setup.wValue.w);
            BD[EP0IN].bytecount = 0;    // set EP0 IN byte count to 0 
            BD[EP0IN].status = 0xC8;    // send packet as DATA1, set UOWN bit
            break;
        case GET_DUTY:
            temp.w = pin_read(&D[13]);
            BD[EP0IN].address[0] = temp.b[0];
            BD[EP0IN].address[1] = temp.b[1];
            BD[EP0IN].bytecount = 2;    // set EP0 IN byte count to 2
            BD[EP0IN].status = 0xC8;    // send packet as DATA1, set UOWN bit
            break;
        default:
            USB_error_flags |= 0x01;    // set Request Error Flag
    }
}

void VendorRequestsIn(void) {
    switch (USB_request.setup.bRequest) {
        default:
            USB_error_flags |= 0x01;                    // set Request Error Flag
    }
}

void VendorRequestsOut(void) {
    switch (USB_request.setup.bRequest) {
        default:
            USB_error_flags |= 0x01;                    // set Request Error Flag
    }
}

int16_t main(void) {
    init_clock();
    init_ui();
    init_pin();
    init_timer();
    init_oc();

    AD1CON3 = 0x0201;                       // set auto-sample time to 2*TAD and
                                            // TAD to 2*TCY

    oc_pwm(&oc1, &D[13], NULL, 10e3, 0x8000);
    OC1CON2bits.OCINV = 1;                  // reconfigure OC1 to have an inverted output

    pin_digitalOut(&D[12]);
    pin_clear(&D[12]);

    IFS0bits.OC1IF = 0;                     // clear the OC1 interrupt flag
    IEC0bits.OC1IE = 1;                     // enable the OC1 interrupt

    InitUSB();                              // initialize the USB registers and serial interface engine
    while (USB_USWSTAT!=CONFIG_STATE) {     // while the peripheral is not configured...
        ServiceUSB();                       // ...service USB requests
    }
    while (1) {
        ServiceUSB();                       // service any pending USB requests
    }
}

