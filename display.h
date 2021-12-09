#include <stdint.h>
#include <avr/io.h>

void prepare_display(void);
void display_digit(uint8_t digit, uint8_t current_segment);
uint8_t* number_to_digits(uint8_t number, uint8_t *ret);
void display_number(uint8_t number);
uint8_t display_isr(void);
