
#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#define FOSC 16000000
#define BAUD 9600
#define MYUBRR FOSC/16/BAUD-1
#define ADC_Out PA0

void stepper1();
void stepper2();
void stepper3();
void stepper4();
void stepper5();
void stepper6();

int phase;
int back_count;
int rounds; // Declared a variable as "rounds" to make how many rounds we need to rotate



void USART_Init( unsigned int ubrr)
{
	
	UBRRH = (unsigned char)(ubrr>>8);
	UBRRL = (unsigned char)ubrr;
	UCSRB = (1<<RXEN) | (1<<TXEN);
	UCSRC = (1<<URSEL)|(1<<USBS)|(3<<UCSZ0);
	
}

unsigned char USART_Receive( void )
{
	while( !( UCSRA & (1<<RXC)) );
	return UDR;
}









void stepper1(){

	int sDelay=25; // Declared a variable as "sDelay" to control the speed of the rotation
	
	rounds = 2;
	while (rounds != 0) // Clock Wise Rotation
	{
		PORTB = 0x03;
		_delay_ms(sDelay);
		PORTB = 0x02;
		_delay_ms(sDelay);
		PORTB = 0x01;
		_delay_ms(sDelay);
		PORTB = 0x00;
		_delay_ms(sDelay);
		
		rounds -=1;
	}
}

void stepper2(){
	
	int tValue =0;
	int phase =  7;
	back_count = 2;
	
	while (tValue == 0)
	{
		
		if (phase==7)
		{
			PORTB = 0x07;
			_delay_ms(25);
			--phase;
		}
		
		if (phase==6)
		{
			PORTB = 0x06;
			_delay_ms(25);
			--phase;
		}
		
		if (phase==5)
		{
			PORTB = 0x05;
			_delay_ms(25);
			--phase;
		}
		
		if (phase==4)
		{
			PORTB = 0x04;
			_delay_ms(25);
			--phase;
		}
		else
		{
			phase = 7;
		}
		back_count++;
		
		if (((PINA,7)||(PINA,1))&&((PINA,5)||(PINA,6)))
		{
			tValue = 1;
		}
		
	}

}



void stepper3(){
	
	while (back_count != 0)
	{
		if (phase==4)
		{
			PORTB = 0x04;
			_delay_ms(25);
			phase++;
		}
		
		if (phase==5)
		{
			PORTB = 0x05;
			_delay_ms(25);
			phase++;
		}
		
		if (phase==6)
		{
			PORTB = 0x06;
			_delay_ms(25);
			phase++;
		}
		
		if (phase==7)
		{
			PORTB = 0x07;
			_delay_ms(25);
			phase++;
		}
		
		else
		{
			phase = 4;
		}
		
		back_count--;
		
	}
}


void stepper4(){
	int sDelay=25; // Declared a variable as "sDelay" to control the speed of the rotation
	
	rounds = 2;
	while (rounds != 0) // Clock Wise Rotation
	{
		PORTB = 0x00;
		_delay_ms(sDelay);
		PORTB = 0x01;
		_delay_ms(sDelay);
		PORTB = 0x02;
		_delay_ms(sDelay);
		PORTB = 0x03;
		_delay_ms(sDelay);
		
		rounds -=1;
	}
	
	rounds = 2;
	while (rounds != 0) // Clock Wise Rotation
	{
		PORTB = 0x0B;
		_delay_ms(sDelay);
		PORTB = 0x0A;
		_delay_ms(sDelay);
		PORTB = 0x09;
		_delay_ms(sDelay);
		PORTB = 0x08;
		_delay_ms(sDelay);
		
		rounds -=1;
	}
	
	rounds = 2;
	while (rounds != 0) // Clock Wise Rotation
	{
		PORTB = 0x08;
		_delay_ms(sDelay);
		PORTB = 0x09;
		_delay_ms(sDelay);
		PORTB = 0x0A;
		_delay_ms(sDelay);
		PORTB = 0x0B;
		_delay_ms(sDelay);
		
		rounds -=1;
	}
}


void stepper5()
{
	int sDelay=20; // Declared a variable as "sDelay" to control the speed of the rotation
	
	rounds = 2;
	while (rounds != 0) // Clock Wise Rotation
	{
		PORTB = 0x0F;
		_delay_ms(sDelay);
		PORTB = 0x0E;
		_delay_ms(sDelay);
		PORTB = 0x0D;
		_delay_ms(sDelay);
		PORTB = 0x0C;
		_delay_ms(sDelay);
		
		rounds -=1;
	}
}


 void stepper6(){
	 	int delay=10; // Declared a variable as "delay" to control the speed of the rotation
	 	
	 	rounds = 2;
	 	while (rounds != 0) // Clock Wise Rotation
	 	{
		 	PORTB = 0x80;
		 	_delay_ms(delay);
		 	PORTB = 0xC0;
		 	_delay_ms(delay);
		 	PORTB = 0x40;
		 	_delay_ms(delay);
		 	PORTB = 0x60;
		 	_delay_ms(delay);
		 	PORTB = 0x20;
		 	_delay_ms(delay);
		 	PORTB = 0x30;
		 	_delay_ms(delay);
		 	PORTB = 0x10;
		 	_delay_ms(delay);
		 	PORTB = 0x90;
		 	_delay_ms(delay);
			 		 	
		 	rounds -=1;
	 	}
		 
		 rounds = 2;
		 while (rounds != 0) // Clock Wise Rotation
		 {
			 PORTB = 0x10;
			 _delay_ms(delay);
			 PORTB = 0x30;
			 _delay_ms(delay);
			 PORTB = 0x20;
			 _delay_ms(delay);
			 PORTB = 0x60;
			 _delay_ms(delay);
			 PORTB = 0x40;
			 _delay_ms(delay);
			 PORTB = 0xC0;
			 _delay_ms(delay);
			 PORTB = 0x80;
			 _delay_ms(delay);
			 PORTB = 0x90;
			 _delay_ms(delay);
			 
			 rounds -=1;
		 }
 } 



int main(){
	DDRB = 0xFF;
	DDRA = 0x00;
	USART_Init(MYUBRR);
	int h = USART_Receive();
	
	while(1)
	{
		if(h=='A')
		{
			stepper1();
			break;
		}
	}
	h = USART_Receive();
	
	while(1)
	{
		if(h=='B')
		{
			stepper2();
			break;
		}
	}
	h = USART_Receive();
	
	
	while(1)
	{
		if(h=='C')
		{
			stepper3();
			_delay_ms(10);
			stepper4();
			_delay_ms(10);
			stepper5();
			_delay_ms(10);
			stepper6();
			break;

		}
	}
	return 0;
}










