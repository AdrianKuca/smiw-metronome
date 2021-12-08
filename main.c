#include <avr/io.h>
#include <util/delay.h>

void sda(int pin)
{
    if(pin)
        DDRD &= ~(1<<6);
    else
        DDRD |= (1<<6);
}

void scl(int pin)
{
    if(pin)
        DDRD &= ~(1<<5);
    else
        DDRD |= (1<<5);
}


void i2c_start()
{
    scl(1);
    _delay_ms(10);
    sda(0); 
    _delay_ms(10);
    scl(0);
    _delay_ms(10);

}
void i2c_stop()
{  
    sda(0); 
    _delay_ms(10);
    scl(1); 
    _delay_ms(10);
    sda(1); 
    _delay_ms(10);
}

int i2c_byte(uint8_t byte)
{
    for(int i=7;i>=0;i--)
    {
        if (byte & (1<<i))
        {
            sda(1); 
        }
        else
        { 
            sda(0);
        }
        _delay_ms(10); // data set up time
        scl(1);
        _delay_ms(10); // thigh time
        scl(0);
    }
    int ret;

    sda(1);
    _delay_ms(10);

    scl(1); 
    _delay_ms(10);
    ret = (PIND & (1<<6));
    scl(0); 

    return ret;
}


int main(void)
{
    PORTD = 0x00;
    DDRD = 0x00;

    while(1)
    {
        i2c_start();
        i2c_byte(0x90);
        i2c_byte(0x40);
        i2c_byte(0xf0);

        i2c_stop();
    }

	return 0;
}
