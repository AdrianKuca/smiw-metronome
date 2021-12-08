#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

void sda(int pin)
{
    if(pin)
        DDRD &= ~(1<<6);
    else
        DDRD |= (1<<6);
}

int get_sda(){
    return PIND & (1<<6);
}
void scl(int pin)
{
    if(pin)
        DDRD &= ~(1<<5);
    else
        DDRD |= (1<<5);
}

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

void i2c_transfer(uint8_t addr, uint8_t *buf, int len)
{
    i2c_state.addr = addr;
    i2c_state.buff = buf;
    i2c_state.len = len;
    i2c_state.byte_ctr = 0;
    i2c_state.bit_ctr = 0;
    i2c_state.ack = 0;
    i2c_state.proc = IC_START0;
    return;
}

int i2c_async()
{
    return i2c_state.proc == IC_IDLE;
}

ISR(TIMER0_OVF_vect)
{
    PORTB += 1;

    switch (i2c_state.proc)
    {
        // Start
        
        case IC_START0:
            scl(1);
            i2c_state.proc = IC_START1;
        break;

        case IC_START1:
            sda(0); 
            i2c_state.proc = IC_START2;
        break;

        case IC_START2:
            scl(0); 
            i2c_state.proc = IC_TRANSFER_ADDR0;
        break;

        // Transfer addr

        case IC_TRANSFER_ADDR0:
            if (i2c_state.addr & (1 << (7-i2c_state.bit_ctr)))
            {
                sda(1); 
            }
            else
            { 
                sda(0);
            }
            i2c_state.proc = IC_TRANSFER_ADDR1;
        break;

        case IC_TRANSFER_ADDR1:
            scl(1);
            i2c_state.proc = IC_TRANSFER_ADDR2;
        break;

        case IC_TRANSFER_ADDR2:
            scl(0);
            if(i2c_state.bit_ctr++ < 7)
                i2c_state.proc = IC_TRANSFER_ADDR0;
            else{
                i2c_state.proc = IC_GET_ACK0;
                i2c_state.byte_ctr = -1;
                i2c_state.bit_ctr = 0;
            }

        break;

        // GET ACK

        case IC_GET_ACK0:
            sda(1);
            i2c_state.proc = IC_GET_ACK1;
        break;

        case IC_GET_ACK1:
            scl(1);
            i2c_state.proc = IC_GET_ACK2;
        break;

        case IC_GET_ACK2:
            i2c_state.ack = get_sda();
            scl(0);
            if(i2c_state.byte_ctr++ !=  i2c_state.len - 1)
                i2c_state.proc = IC_TRANSFER_BYTE;
            else
                i2c_state.proc = IC_STOP0;
        break;

        // SEND ACK

        case IC_SEND_ACK0:
            if(i2c_state.byte_ctr == i2c_state.len-1)
                sda(1);
            else
                sda(0);
            scl(1);
            i2c_state.proc = IC_SEND_ACK1;
        break;

        case IC_SEND_ACK1:
            scl(0);
            i2c_state.proc = IC_SEND_ACK2;
        break;

        case IC_SEND_ACK2:
            sda(0);
            if(i2c_state.byte_ctr++ !=  i2c_state.len - 1)
                i2c_state.proc = IC_TRANSFER_BYTE;
            else
                i2c_state.proc = IC_STOP0;
        break;

        // Transfer Byte

        case IC_TRANSFER_BYTE:
            if (i2c_state.addr & 0x01)
            {
                i2c_state.proc = IC_READ_BYTE0;

            }
            else
            {
                i2c_state.proc = IC_WRITE_BYTE0;
            }
        break;

        // Write byte 

        case IC_WRITE_BYTE0:
            if (i2c_state.buff[i2c_state.byte_ctr] & (1 << (7-i2c_state.bit_ctr)))
            {
                sda(1); 
            }
            else
            { 
                sda(0);
            }
            i2c_state.proc = IC_WRITE_BYTE1;
        break;

        case IC_WRITE_BYTE1:
            scl(1);
            i2c_state.proc = IC_WRITE_BYTE2;
        break;

        case IC_WRITE_BYTE2:
            scl(0);
            if(i2c_state.bit_ctr++ < 7)
                i2c_state.proc = IC_WRITE_BYTE0;
            else{
                i2c_state.bit_ctr = 0;
                i2c_state.proc = IC_GET_ACK0;
            }
        break;

        // Read byte 

        case IC_READ_BYTE0:
            sda(1);
            i2c_state.proc = IC_READ_BYTE1;
        break;

        case IC_READ_BYTE1:
            scl(1);
            i2c_state.proc = IC_READ_BYTE2;
        break;

        case IC_READ_BYTE2:
            *i2c_state.buff = *i2c_state.buff << 1;
            *i2c_state.buff |= !!(get_sda());
            scl(0);
            if(i2c_state.bit_ctr++ < 7)
                i2c_state.proc = IC_READ_BYTE0;
            else{
                i2c_state.bit_ctr = 0;
                i2c_state.proc = IC_SEND_ACK0;
            }
        break;

        // Stop

        case IC_STOP0:
            sda(0);
            i2c_state.proc = IC_STOP1;
        break;

        case IC_STOP1:
            scl(1); 
            i2c_state.proc = IC_STOP2;
        break;

        case IC_STOP2:
            sda(1); 
            i2c_state.proc = IC_IDLE;
        break;

        case IC_IDLE:
        default:
        break;
    }
}

int main(void)
{
    PORTD = 0x00;
    DDRD = 0x10;

    DDRB = 0xFF;
    PORTB = 0x00;

    TCCR0A = 0;
    TCCR0B = (1<<CS01);
    TIMSK = (1<< TOIE0);

    i2c_state.proc = IC_IDLE;

    uint8_t buf[] = {0x40,0x00};
    uint8_t buf2[] = {0x00};

    enum {
        S_READ,
        S_WRITE,
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
            }
        }
    }

	return 0;
}
