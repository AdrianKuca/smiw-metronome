#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

void sda(int pin);
int get_sda(void);
void scl(int pin);
struct {
    volatile enum {
        IC_IDLE,
        IC_START0,
        IC_START1,
        IC_START2,

        IC_STOP0,
        IC_STOP1,
        IC_STOP2,

        IC_TRANSFER_ADDR0,
        IC_TRANSFER_ADDR1,
        IC_TRANSFER_ADDR2,

        IC_GET_ACK0,
        IC_GET_ACK1,
        IC_GET_ACK2,
        
        IC_SEND_ACK0,
        IC_SEND_ACK1,
        IC_SEND_ACK2,

        IC_TRANSFER_BYTE,

        IC_WRITE_BYTE0,
        IC_WRITE_BYTE1,
        IC_WRITE_BYTE2,

        IC_READ_BYTE0,
        IC_READ_BYTE1,
        IC_READ_BYTE2,
    } proc;
    volatile uint8_t addr;
    volatile uint8_t *buff;
    volatile int len;
    volatile int bit_ctr;
    volatile int byte_ctr;
    volatile int ack;

} i2c_state;

void i2c_transfer(uint8_t addr, uint8_t *buf, int len);
int i2c_async(void);
void i2c_isr(void);
