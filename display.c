#include "display.h"
#include "util/delay.h"
struct {
    volatile enum {
        D_FIRST_DIGIT,
        D_SECOND_DIGIT,
        D_THIRD_DIGIT,
        D_IDLE,
    } proc;
    volatile uint8_t raw;
    volatile uint8_t current_segment;
    volatile uint8_t digits[3];
} display_state;

uint8_t segment_digits[] = {
	192,
	249,
	164,
	176,
	153,
	146,
	130,
	248,
	128,
	144
};

void prepare_display(){
    DDRB=0xFF;
    PORTB = 0xff;
    display_state.proc = D_FIRST_DIGIT;
}

void display_digit(uint8_t segmented_digit, uint8_t current_segment){
    PORTB = segmented_digit;
    PORTD = ~(0b10000111) & ~(1 << (current_segment + 3)); // Enable single segment 0 1 2 3 (from right)
}

uint8_t* number_to_digits(uint8_t number, uint8_t *digits){
    digits[0] = number % 10;
    digits[1] = (number / 10) % 10;
    digits[2] = (number / 100) % 10;
    return digits;
}

void display_number(uint8_t number){
    uint8_t digits[3] = {0,0,0};
    number_to_digits(number, digits);
    for(uint8_t i = 0; i < 3; i++){
        display_state.digits[i] = segment_digits[digits[i]];
    }
}

uint8_t display_isr()
{
    switch(display_state.proc){ // First digit is the least significant
        case D_FIRST_DIGIT:
            DDRD = 0xff;
            display_digit(display_state.digits[0],0);
            _delay_ms(10);
            display_state.proc = D_SECOND_DIGIT;
        break;  
        case D_SECOND_DIGIT:
            display_digit(display_state.digits[1],1);
            _delay_ms(10);
            display_state.proc = D_THIRD_DIGIT;
        break;
        case D_THIRD_DIGIT:
            display_digit(display_state.digits[2],2);
            _delay_ms(10);
            display_state.proc = D_IDLE;
        break;
        case D_IDLE:
            PORTB = 0xff;
            PORTD = 0x00;
            DDRD = 0x00;
            display_state.proc = D_FIRST_DIGIT;
        break;
    }
    return(display_state.proc == D_FIRST_DIGIT);
}