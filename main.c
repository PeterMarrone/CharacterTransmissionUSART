/*
 *
 * Name: Peter Marrone
 * Lab3
 * Date: 9/24/2021
 *
 */ 

#define F_CPU 16000000UL	// 16 MHz clock from the debug processor
#include <avr/io.h>
#include <util/delay.h>

#define BAUD 9600
#define BAUDRATE ((F_CPU)/(BAUD*16UL)-1)	// Set baud rate value for UBRR

void USART_init();							// Prototypes
void transmit(unsigned char data);
void scan(unsigned char pad[4][4]);

int main(void)
{
	unsigned char keypad[4][4] = {{'1','2','3','A'},	// Creates 2D char array for 4x4 keypad
								  {'4','5','6','B'}, 
								  {'7','8','9','C'}, 
								  {'*','0','#','D'}};
							 
	USART_init();	// Initializes USART
	DDRD |= (1 << DDD4) | (1 << DDD5) | (1 << DDD6) | (1 << DDD7);		// Sets the rows to output
	DDRB &= ~(1 << DDB0) & ~(1 << DDB1) & ~(1 << DDB2) & ~(1 << DDB3);	// Sets the columns to input
	PORTB |= (1 << DDB0) | (1 << DDB1) | (1 << DDB2) | (1 << DDB3);		// Enables the Pull-Up resistors
	PORTD |= (1 << DDD4) | (1 << DDD5) | (1 << DDD6) | (1 << DDD7);		// Sets rows to HIGH
	
    while (1) 
    {
		scan(keypad);	// Calls scan() function
    }
}

void USART_init()
{
	UBRR0H = (BAUDRATE >> 8);	// Shifts 8 bits to the right
	UBRR0L = BAUDRATE;		// Sets the Baud rate
	UCSR0B |= (1 << TXEN0);		// Enables transmitter
	UCSR0C |= (1 << UCSZ00) | (1 << UCSZ01);	// Sets data frame to 8-bits and 1 stop bit
}

void transmit(unsigned char data)
{
	while (!(UCSR0A & (1 << UDRE0)));	// Waits while transmit register is free
	UDR0 = data;	// Load data into register
}

void scan(unsigned char pad[4][4])
{
	for(unsigned int i = 0; i < 4; i++)
	{
		PORTD &= ~(1 << (i+4));		// Sets rows to LOW one at a time
		for(unsigned int j = 0; j < 4; j++)
		{
			if(!(PINB & (1 << j)))		// Check each column to see which is pulled low
			{
				transmit(pad[i][j]);	// Transmit data at intersection
				_delay_ms(300);		// Delay 300ms to counter button bounce
			}
		}
		PORTD |= (1 << DDD4) | (1 << DDD5) | (1 << DDD6) | (1 << DDD7);		// Sets rows to HIGH
	}
}

