#include <stdint.h>
#include <avr/io.h>
#include "display.h"
void prepare_display(){
	PORTB = 0xff;
	PORTD = PORTD & ~(PORTD & 0b1111000); // Disable all four segments
}

void display_digit(uint8_t digit, uint8_t segment){
    PORTB = digits[digit];
    PORTD = PORTD | !(PORTD & (1 << segment+3)) // Enable single segment 0 1 2 3 (from right)
}

void display_number(uint8_t number){
    
}