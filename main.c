#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "i2c.h"

int main(void)
{
    PORTD = 0x00;
    DDRD = 0x10;

    DDRB = 0xFF;
    PORTB = 0x00;

    TCCR0A = 0;
    TCCR0B = (1 << CS01);
    TIMSK = (1 << TOIE0);

    i2c_state.proc = IC_IDLE;

    uint8_t buf[] = {0x40,0x00};
    uint8_t buf2[] = {0x00};

    enum {
        S_READ,
        S_WRITE,
        S_DISPLAY,
    } state;

    state = S_WRITE;
    
    sei();

    while(1)
    {
        if (i2c_async())
        {
            switch(state)
            {
                case S_WRITE:
                    buf[1] = buf2[0];
                    i2c_transfer(0x90,buf,2);
                    state = S_READ;
                break;
                case S_READ:
                    i2c_transfer(0x91,buf2,1);
                    state = S_WRITE;
                break;
                case S_DISPLAY:

                break;
            }
        }
    }

	return 0;
}
