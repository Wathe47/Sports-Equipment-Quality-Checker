#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#define LCD PORTC	//LCD Data port connected to port C
#define EN 7
#define RS 6
#define RW 1


#define US_PORT PORTC
#define	US_PIN	PINC
#define US_DDR 	DDRC
#define US_TRIG_POS	PC0
#define US_ECHO_POS	PC1


#define US_PORT2 PORTD
#define	US_PIN2 PIND
#define US_DDR2 DDRD
#define US_TRIG_POS2	PD2
#define US_ECHO_POS2	PD3

#define US_PORT3 PORTC
#define	US_PIN3	PINC
#define US_DDR3 	DDRC
#define US_TRIG_POS3	PC2
#define US_ECHO_POS3	PC3
#define FOSC 16000000
#define BAUD 9600
#define MYUBRR FOSC/16/BAUD-1


#define US_ERROR	-1
#define	US_NO_OBSTACLE	-2


uint8_t hx711H=0;  //Load Scale High Bits
uint16_t hx711L=0; //Load Scale Low Bits
float loadCellRead();
#define Load_data 4
#define Load_clk 5

uint8_t hx711H2=0;  //Load Scale High Bits
uint16_t hx711L2=0; //Load Scale Low Bits
float loadCellReadStick();
#define Load_data2 0
#define Load_clk2 2

#define ADC_Out PA3
#define OUT_PUT PC3

void HCSR04Init();
void HCSR04Trigger();
uint16_t GetPulseWidth();

void HCSR04Init2();
void HCSR04Trigger2();
uint16_t GetPulseWidth2();

void HCSR04Init3();
void HCSR04Trigger3();
uint16_t GetPulseWidth3();

void LCD_Clear();

unsigned char keypad();
float weight();
float ultrasonic1();
float ultrasonic2();
float ultrasonic_stick();
float loadCellRead();
float hx=0;

void funcBall();
void funcStick();

float itemWeight;
float diameter_ball=0;
float weight_stick;
float height_stick;
float bounce_ball;
char numberString[5];

//Diameter,BouncingHeight, Weight
float ballStandards[][3]={{2.6, 0.55, 0.057},
{8.2, 0.57, 0.270},
{8.2, 0.57, 0.270},
{9.4, 0.65, 0.600},
{9.2, 0.65, 0.600},
{1.6, 0.70, 0.045},
{1.6, 0.65, 0.005},
{2.8, 0.55, 0.160},
{8.5, 0.60, 0.450},
{8.5, 0.65, 0.450},
{7.5, 0.45, 0.450},
{2.9, 0.55, 0.140},
{8.7, 0.65, 0.400},
{8.9, 0.65, 0.400},
{2.8, 0.55, 0.160},
{2.8, 0.40, 0.050}}  ;

//Height, Weight
float stickStandards[][2]={{0.78, 1.20},
{0.81, 1.25},
{0.83, 1.31},
{0.88, 1.40},
{1.14, 0.53},
{1.18, 0.55},
{1.25, 0.56}}  ;
	
	
	
	
void USART_Init( unsigned int ubrr)
{
	
	UBRRH = (unsigned char)(ubrr>>8);
	UBRRL = (unsigned char)ubrr;
	UCSRB = (1<<RXEN) | (1<<TXEN);
	UCSRC = (1<<URSEL)|(1<<USBS)|(3<<UCSZ0);
	
}

void USART_Transmit(unsigned char data)
{
	while( !( UCSRA & (1<<UDRE)) );
	UDR = data;
}


void lcdcmd(unsigned char cmd)
{
	DDRC = 0b00001000;
	PORTD &=~(7<<RS); // RS=0 for command
	PORTD &=~(7<<RW); // RW=0 for Write
	LCD = cmd & 0xF0; // Send upper nibble
	PORTD |=(7<<EN);  // EN=1 for H to L pulse (EN should be given with a pulse)
	_delay_ms(1);
	PORTD &=~(7<<EN);  // EN=1 for H to L pulse
	
	LCD = cmd<<4; // Send low nibble
	PORTD |=(7<<EN);  // EN=1 for H to L pulse (EN should be given with a pulse)
	_delay_ms(1);
	PORTD &=~(7<<EN);  // EN=1 for H to L pulse
}

void lcddata(unsigned char data)
{
	PORTD |=(7<<RS); //RS=1 for data
	PORTD &=~(7<<RW); //RW=0 for write
	LCD = data & 0xF0; //Send upper nibble
	PORTD |=(7<<EN); // EN=1 for H to L pulse
	_delay_ms(1);
	PORTD &=~(7<<EN); // EN=0 for H to L pulse
	
	LCD = data <<4; // send low nibble
	PORTD |=(7<<EN); // EN=1 for H to L pulse
	_delay_ms(1);
	PORTD &=~(7<<EN); // EN=0 for H to L pulse
}

void lcd_int()
{
	
	
	DDRC = 0xF0;	//Define output port
	DDRD |= (7<<EN);	//Define EN pin as output
	DDRD =0xFF;       // Define RS and RW pin as output
	PORTD &=~(7<<EN);	// Initialize EN=0
	lcdcmd(0x33);
	lcdcmd(0x32);
	lcdcmd(0x28);	//LCD in 4bit mode
	lcdcmd(0x0E);	//Display on cursor on
	lcdcmd(0x01);	//Clear LCD
	_delay_ms(2);
	
}

void lcd_Print (char *str)		/*Send string to LCD function */
{
	int i;
	for(i=0;str[i]!=0;i++)		/* Send each char of string till the NULL */
	{
		lcddata (str[i]);
	}
}

unsigned char lcd_write(){
	
	unsigned char x='a';
	while(1)
	{
		PORTB =0xF0; //11110000 Make all 4 columns 1 and rows 0
		if (PINB != 0xF0) //if port A not similar to F0 means key is press
		{
			x = keypad();
			lcddata(x);
			
			if(x!='a'){
				break;
			}
		}
	}
	return x;
}


int main()
{
	DDRA = 0; //port A as input
	USART_Init(MYUBRR);
	DDRD|=(1<<Load_clk); //Load cell clock pin
	PORTD&=~(1<<Load_clk);//Clock pin low
	
	
	unsigned char typeChoice;
	unsigned char enter;
	
	do {
		DDRB =0x0F; // make PA0-PA3 (1)-->output and PA4-PA7 (0)-->input
		lcd_int();
		
		lcd_Print("(1-Ball 2-Stick)");
		lcdcmd(0xC0);
		lcd_Print("Select: ");
		typeChoice =lcd_write();
		
		do {
			enter =lcd_write();
		} while (enter !='-');


		if (typeChoice =='1' || typeChoice =='2')
		{

			if (typeChoice=='1')
			{
				funcBall();
				}else{
				funcStick();
			}
		}
		else
		{
			LCD_Clear();
			lcd_Print("Wrong Input!");
			_delay_ms(50);
		}
	} while (typeChoice !='1' && typeChoice !='2');
	

	
	return 0;
}

unsigned char keypad()
{
	int time = 12;
	int colValues[] = {0b11111110,0b11111101,0b11111011,0b11110111};
	unsigned char arr[][4]= {{'7','8','9','/'},
	{'4','5','6','x'},
	{'1','2','3','-'},
	{' ','0','=','+'}};
	
	for (int i=0;i<4;i++)
	{
		PORTB = colValues[i];
		
		if((PINB & (1<<PINB4))==0)
		{
			_delay_ms(time);
			
			if (i==3)
			{
				LCD_Clear();
				break;
				}else{
				return arr[i][0];
			}

		}
		else if((PINB & (1<<PINB5))==0)
		{
			_delay_ms(time);
			
			return arr[i][1];
		}
		else if((PINB & (1<<PINB6))==0)
		{
			_delay_ms(time);
			
			if (i==3)
			{
				lcdcmd(0x10);
				break;
				}else{
				return arr[i][2];
			}
			
		}
		else if((PINB & (1<<PINB7))==0)
		{
			_delay_ms(time);
			
			if (i==0)
			{
				lcdcmd(0x80);
				break;
			}else if (i==1)
			{
				lcdcmd(0xC0);
				break;
			} else if (i==2)
			{
				return arr[i][3];
				break;
			}else if (i==3)
			{
				lcdcmd(0x14);
				break;
			}

		}
		
	}
	
}

void LCD_Clear()
{
	lcdcmd(0x01);		/* Clear display */
	_delay_ms(2);
	lcdcmd(0x80);		/* Cursor at home position */
	
}

void funcBall(){
	

	unsigned char enter;
	unsigned char sportNo1;
	unsigned char sportNo2;
	int intSportNo1;
	int intSportNo2;
	int sportNoFull;
	unsigned char cArray[10]={'0','1','2','3','4','5','6','7','8','9'};
	int intArray[10]={0,1,2,3,4,5,6,7,8,9};
	

	do {
		LCD_Clear();
		lcd_Print("Enter sport");
		lcdcmd(0xC0);
		lcd_Print("Number:");
		sportNo1= lcd_write();
		sportNo2= lcd_write();
		
		for (int i=0; i<10;i++)
		{
			if (sportNo1==cArray[i])
			{
				intSportNo1= intArray[i];
			}
			if (sportNo2==cArray[i])
			{
				intSportNo2= intArray[i];
			}
			
		}
		
		sportNoFull = intSportNo1*10 + intSportNo2;

		do{
			enter =lcd_write();
		} while (enter !='-');
		
	} while (sportNoFull > 16);
	

	LCD_Clear();
	lcd_Print("Insert and Press");
	lcdcmd(0xC0);
	lcd_Print("Enter !");

	do{
		enter =lcd_write();
	} while (enter !='-');

	LCD_Clear();
	lcd_Print("Processing...");
	_delay_ms(10);
	
	
	USART_Transmit('A'); //pass to the second chip
	_delay_ms(250);	
	
	itemWeight = loadCellRead();
	LCD_Clear();
	if( itemWeight<= (ballStandards[sportNoFull-1][2]) - 0.010 ||  itemWeight>= (ballStandards[sportNoFull-1][2]) + 0.010  )
	{
		lcd_Print("Weight test");
		lcdcmd(0xC0);
		lcd_Print("Fail!");
		
		} else {
		lcd_Print("Weight test");
		lcdcmd(0xC0);
		lcd_Print("Pass!");
	}	
	
	_delay_ms(100);
	LCD_Clear();
	

	USART_Transmit('B'); //pass to the second chip
	_delay_ms(200);

	//diameter_ball =  ultrasonic1();----------------------------------------------
	//_delay_ms(100);----------------------------------------------------
	
	itoa(diameter_ball, numberString, 10);
	//lcd_Print(numberString);
	//_delay_ms(100);
	//LCD_Clear();
			
	if( diameter_ball<= (ballStandards[sportNoFull-1][0]) - 0.010 ||  diameter_ball>= (ballStandards[sportNoFull-1][0]) + 0.010  )
	{
		lcd_Print(" diameter test");
		lcdcmd(0xC0);
		lcd_Print("Fail!");
		
		} else {
		lcd_Print(" diameter test");
		lcdcmd(0xC0);
		lcd_Print("Pass!");
	}
	
		_delay_ms(100);
		LCD_Clear();
		
	USART_Transmit('C'); //pass to the second chip
	_delay_ms(1200);
	
	DDRC=0b00001000; // set PC3 as output to control the LED
	DDRA|=(1<<ADC_Out);         //Make PA0 as input
	

	if(1<<ADC_Out)
	{
		
		bounce_ball = ultrasonic2();
		if( bounce_ball<= (stickStandards[intSportNo1-1][0]) - 0.010 ||  bounce_ball >= (stickStandards[intSportNo1-1][0]) + 0.010  )
		{
			lcd_Print("Bounce test");
			lcdcmd(0xC0);
			lcd_Print("Fail!");
			
			} else {
			lcd_Print("Bounce test");
			lcdcmd(0xC0);
			lcd_Print("Pass!");
		}
		
			_delay_ms(100);
			LCD_Clear();
			
	}
	

	
}


void funcStick(){
	
	unsigned char enter;
	unsigned char sportNo1;
	int intSportNo1;
	unsigned char cArray[10]={'0','1','2','3','4','5','6','7','8','9'};
	int intArray[10]={0,1,2,3,4,5,6,7,8,9};


	do {
		LCD_Clear();
		lcd_Print("Enter sport");
		lcdcmd(0xC0);
		lcd_Print("Number:");
		sportNo1= lcd_write();
		
		for (int i=0; i<10;i++)
		{
			if (sportNo1==cArray[i])
			{
				intSportNo1= intArray[i];
			}
		}

		
		do{
			enter =lcd_write();
		} while (enter !='-');
		
	} while (intSportNo1 > 7);
	

	LCD_Clear();
	lcd_Print("Insert and Press");
	lcdcmd(0xC0);
	lcd_Print("Enter !");

	do{
		enter =lcd_write();
	} while (enter !='-');
	LCD_Clear();
	lcd_Print("Processing...");
	_delay_ms(100);
	weight_stick = loadCellReadStick();
	_delay_ms(100);
	
	if( weight_stick<= (stickStandards[intSportNo1-1][1]) - 0.010 ||  weight_stick >= (stickStandards[intSportNo1-1][1]) + 0.010  )
	{
		lcd_Print("Weight test");
		lcdcmd(0xC0);
		lcd_Print("Fail!");
		
		} else {
		lcd_Print("Weight test");
		lcdcmd(0xC0);
		lcd_Print("Pass!");
	}
	_delay_ms(100);
	LCD_Clear();
	
	
	height_stick = ultrasonic_stick();
	_delay_ms(100);
	
	if( height_stick<= (stickStandards[intSportNo1-1][0]) - 0.010 ||  height_stick >= (stickStandards[intSportNo1-1][0]) + 0.010  )
	{
		lcd_Print("Height test");
		lcdcmd(0xC0);
		lcd_Print("Fail!");
		
		} else {
		lcd_Print("Height test");
		lcdcmd(0xC0);
		lcd_Print("Pass!");
	}
		_delay_ms(100);
		LCD_Clear();
		
}


float loadCellRead(){
	
	hx711H=0;hx711L=0;  //clear variables
	for(uint8_t i=0;i<8;i++){  // Load cell data high 8 bits
		PORTD|=(1<<Load_clk); //Clock pin high
		_delay_us(10);
		if ((PIND&(1<<Load_data))>>Load_data)  //read data pin
		{hx711H|=(1<<(7-i));//set hx 711 variable
		}
		else
		{hx711H&=~(1<<(7-i));
		}
		PORTD&=~(1<<Load_clk); //Clock pin low
		_delay_us(5);
	}
	
	for(uint8_t i=0;i<16;i++){ // Load cell data low 16 bits
		PORTD|=(1<<Load_clk); //Clock pin high
		_delay_us(10);
		if ((PIND&(1<<Load_data))>>Load_data) //read data pin
		{hx711L|=(1<<(15-i));
		}
		else
		{hx711L&=~(1<<(15-i));
		}
		PORTD&=~(1<<Load_clk); //Clock pin low
		_delay_us(5);
	}
	
	
	hx711L=hx711L>>1; //shift bits
	
	if (hx711H&1)  //bit setup
	{hx711L|=(1<<15);
	}
	else
	{hx711L&=~(1<<15);
	}
	hx711H=hx711H>>1;
	
	return (hx711H*(65536/18029.6))+hx711L/18029.6; //load cell calibration
}


float loadCellReadStick(){
	
	
	hx711H2=0;hx711L2=0;  //clear variables
	for(uint8_t i=0;i<8;i++){  // Load cell data high 8 bits
		PORTA|=(1<<Load_clk2); //Clock pin high
		_delay_us(10);
		if ((PINA&(1<<Load_data2))>>Load_data2)  //read data pin
		{hx711H2|=(1<<(7-i));//set hx 711 variable
		}
		else
		{hx711H2&=~(1<<(7-i));
		}
		PORTA&=~(1<<Load_clk2); //Clock pin low
		_delay_us(5);
	}
	
	for(uint8_t i=0;i<16;i++){ // Load cell data low 16 bits
		PORTA|=(1<<Load_clk2); //Clock pin high
		_delay_us(10);
		if ((PINA&(1<<Load_data2))>>Load_data2) //read data pin
		{hx711L2|=(1<<(15-i));
		}
		else
		{hx711L2&=~(1<<(15-i));
		}
		PORTA&=~(1<<Load_clk2); //Clock pin low
		_delay_us(5);
	}
	
	
	hx711L2=hx711L2>>1; //shift bits
	
	if (hx711H2&1)  //bit setup
	{hx711L2|=(1<<15);
	}
	else
	{hx711L2&=~(1<<15);
	}
	hx711H2=hx711H2>>1;
	
	return (hx711H2*(65536/18029.6))+hx711L2/18029.6; //load cell calibration
}



float ultrasonic1()
{
	int diameter ;

	while(1)
	{
		uint16_t r;
		_delay_ms(100);
		HCSR04Init();
		
		while(1)
		{
			HCSR04Trigger();
			r=GetPulseWidth();
			diameter =((r*0.034/2.0)+1);
			_delay_ms(30);
			
			return diameter;
		}
	}
}

float ultrasonic2()
{
	DDRB = 0x00;
	int diameter ;
	
	while(1)
	{
		uint16_t r;
		_delay_ms(100);
		HCSR04Init2();
		
		while(1)
		{
			HCSR04Trigger2();
			r=GetPulseWidth2();
			diameter =((r*0.034/2.0)+1);
			_delay_ms(30);
			
			return diameter;
		}
	}
}

float ultrasonic_stick()
{
	DDRB = 0x00;
	int diameter ;

	while(1)
	{
		uint16_t r;
		_delay_ms(100);
		HCSR04Init3();
		
		while(1)
		{
			HCSR04Trigger3();
			r=GetPulseWidth3();
			diameter =((r*0.034/2.0)+1);
			_delay_ms(30);
			
			return diameter;
		}
	}
}


void HCSR04Init()
{
	
	US_DDR|=(1<<US_TRIG_POS); //PC0
}

void HCSR04Trigger()
{
	
	US_PORT|=(1<<US_TRIG_POS); //PC0   //high
	
	_delay_us(15);
	
	US_PORT&=~(1<<US_TRIG_POS);	//PC0	//low
}

uint16_t GetPulseWidth()
{
	
	uint32_t i,result;

	for(i=0;i<600000;i++)
	{
		if(!(US_PIN & (1<<US_ECHO_POS)))	//PC1
		continue;
		else
		break;
	}

	if(i==600000)
	return US_ERROR;
	
	TCCR1A=0X00;
	TCCR1B=(1<<CS11);
	TCNT1=0x00;

	for(i=0;i<600000;i++)
	{
		if(US_PIN & (1<<US_ECHO_POS))
		{
			if(TCNT1 > 60000) break; else continue;
		}
		else
		break;
	}

	if(i==600000)
	return US_NO_OBSTACLE;

	result=TCNT1;

	TCCR1B=0x00;

	if(result > 60000)
	return US_NO_OBSTACLE;
	else
	return (result>>1);
}


void HCSR04Init2()
{
	
	US_DDR2|=(1<<US_TRIG_POS2); //PC0
}

void HCSR04Trigger2()
{
	
	US_PORT2|=(1<<US_TRIG_POS2); //PC0   //high
	
	_delay_us(15);
	
	US_PORT2&=~(1<<US_TRIG_POS2);	//PC0	//low
}

uint16_t GetPulseWidth2()
{
	
	uint32_t i,result;

	for(i=0;i<600000;i++)
	{
		if(!(US_PIN2 & (1<<US_ECHO_POS2)))	//PC1
		continue;
		else
		break;
	}

	if(i==600000)
	return US_ERROR;
	
	TCCR1A=0X00;
	TCCR1B=(1<<CS11);
	TCNT1=0x00;

	for(i=0;i<600000;i++)
	{
		if(US_PIN2 & (1<<US_ECHO_POS2))
		{
			if(TCNT1 > 60000) break; else continue;
		}
		else
		break;
	}

	if(i==600000)
	return US_NO_OBSTACLE;

	result=TCNT1;

	TCCR1B=0x00;

	if(result > 60000)
	return US_NO_OBSTACLE;
	else
	return (result>>1);
}

void HCSR04Init3()
{
	
	US_DDR3|=(1<<US_TRIG_POS3); //PC0
}

void HCSR04Trigger3()
{
	
	US_PORT3|=(1<<US_TRIG_POS3); //PC0   //high
	
	_delay_us(15);
	
	US_PORT3&=~(1<<US_TRIG_POS3);	//PC0	//low
}

uint16_t GetPulseWidth3()
{
	
	uint32_t i,result;

	for(i=0;i<600000;i++)
	{
		if(!(US_PIN3 & (1<<US_ECHO_POS3)))	//PC1
		continue;
		else
		break;
	}

	if(i==600000)
	return US_ERROR;
	
	TCCR1A=0X00;
	TCCR1B=(1<<CS11);
	TCNT1=0x00;

	for(i=0;i<600000;i++)
	{
		if(US_PIN3 & (1<<US_ECHO_POS3))
		{
			if(TCNT1 > 60000) break; else continue;
		}
		else
		break;
	}

	if(i==600000)
	return US_NO_OBSTACLE;

	result=TCNT1;

	TCCR1B=0x00;

	if(result > 60000)
	return US_NO_OBSTACLE;
	else
	return (result>>1);
}


