
import usb.core

class pwmtest:

    def __init__(self):
        self.TOGGLE_LED1 = 0
        self.SET_DUTY = 1
        self.GET_DUTY = 2
        self.dev = usb.core.find(idVendor = 0x6666, idProduct = 0x0003)
        if self.dev is None:
            raise ValueError('no USB device found matching idVendor = 0x6666 and idProduct = 0x0003')
        self.dev.set_configuration()

    def close(self):
        self.dev = None

    def toggle_led1(self):
        try:
            self.dev.ctrl_transfer(0x40, self.TOGGLE_LED1)
        except usb.core.USBError:
            print "Could not send TOGGLE_LED1 vendor request."

    def set_duty(self, duty):       # duty is expressed as a percentage
        try:
            val = 65536 - int(duty * 655.35)
            self.dev.ctrl_transfer(0x40, self.SET_DUTY, int(val))
        except usb.core.USBError:
            print "Could not send SET_DUTY vendor request."

    def get_duty(self):
        try:
            ret = self.dev.ctrl_transfer(0xC0, self.GET_DUTY, 0, 0, 2)
        except usb.core.USBError:
            print "Could not send GET_DUTY vendor request."
        else:
            val = 65536 - (int(ret[0]) + int(ret[1]) * 256)
            return val / 655.35     # duty cycle is returned as a percentage
