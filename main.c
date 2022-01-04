#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <util/atomic.h>
#include "i2c.h"
#include "display.h"

volatile uint32_t ticks_16khz = 0;

int main(void)
{
    PORTD = 0x00;
    DDRD = 0x14;

    DDRB = 0xFF;
    PORTB = 0x00;

    TCCR0A =  (1 << WGM01); //   CTC, clear on compare
    TCCR0B = (1 << CS00); //no prescaler
    TIMSK = (1 << OCIE0A); // output compare A
    OCR0A = 250-1; //16kHz 250 ticks

    i2c_state.proc = IC_IDLE;
    prepare_display();

    uint8_t adc_raw[] = {0x01};
    
    sei();

    i2c_transfer(0x91,adc_raw,1);

    uint32_t duration_ticks = 0;
    uint32_t frequency_bpm = adc_raw[0];
    uint32_t clack = 1000;
    while(1)
    {
        display_number(frequency_bpm);

        if (i2c_async()){
            if(display_isr())
            {
                frequency_bpm = adc_raw[0];
                i2c_transfer(0x91,adc_raw,1);
            }
        }
        
        duration_ticks = (int32_t)16000*60/frequency_bpm;
        ATOMIC_BLOCK(ATOMIC_FORCEON)
        {
            if (((int32_t)(ticks_16khz - clack - duration_ticks) >= 0) && (frequency_bpm !=0)){
                clack += duration_ticks;
                PIND = (1<<2);
            }
        }
    }
	return 0;
}

ISR(TIMER0_COMPA_vect)
{
    i2c_isr();
    ticks_16khz++;

}
