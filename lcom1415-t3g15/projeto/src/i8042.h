#ifndef _I8042_H_
#define _I8042_H_

#define BIT(n) (0x01<<(n)) //-->Shift a bit to n's position

#define DELAY 20000

#define IRQ_MOUSE 12

#define ESC_CODE 0x81 //-->Break code for ESC key

#define KBD_IRQ 0x01 //-->Keyboard IRQ

//Ports
#define BUF_IN 0x60 //-->IN_BUF
#define BUF_OUT 0x60 //-->OUT_BUF
#define BUF_STAT 0x64 //-->STATUS_BUF
#define BUF_CMD 0x64 //-->STAT_BUF

//Errors
#define ERR_TOUT BIT(6) //-->Timeout
#define ERR_PAR BIT(7) //-->Parity

//Buff
#define ERR_IN_FULL BIT(1) //-->In_buf full
#define ERR_OUT_FULL BIT(0) //-->Out_buf full

#define LEDS 0xED //-->Led's command
#define ACK 0xFA //-->ACK
#define RESEND 0xFE
#define ERROR 0xFC

#define MOUSE_DISABLE 0xA7
#define WRITE_BYTE_MOUSE 0xD4
#define ENABLE_SEND_DATA 0xF4

//Get data from packets
#define LEFT_BTN BIT(0)
#define RIGHT_BTN BIT(1)
#define MID_BTN BIT(2)
#define X_SIGN BIT(4)
#define Y_SIGN BIT(5)
#define X_OVERFLOW BIT(6)
#define Y_OVERFLOW BIT(7)

#define HOOK_KBD 4
#define HOOK_MOUSE	5


#endif

