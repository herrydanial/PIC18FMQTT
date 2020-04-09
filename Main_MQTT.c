#include <p18f4580.h>
#include <delays.h>
#include <stdio.h>
#pragma config OSC=HS,LVP=OFF,PWRT=ON,DEBUG=OFF,WDT=OFF,MCLRE=OFF
#define rs PORTBbits.RB4
#define en PORTBbits.RB5
void tx(unsigned char);
unsigned char rx(void);
void dtw(unsigned char);
void cmw(unsigned char);
void wrt(unsigned char*,char);
unsigned int adc(unsigned char);
enum Adc_Channel {an0,an1,an2,an3,an4,an5,an7,an8,an9,an10,an11};
enum wrt_packet {LCD,ESP};
void main(void)
{
	unsigned int ADC;
	unsigned char*DATA;
	ADCON1=0x0a;
	ADCON2=0xbc;
	TXSTA=0x20;
	SPBRG=0x02;
	RCSTA=0x90;
	TRISD=0x00;
	TRISBbits.TRISB4=TRISBbits.TRISB5=0;
	Delay10KTCYx(100);
	cmw(0x38);
	cmw(0x01);
	cmw(0x0c);
	while(1)
	{
		cmw(0x80);
		ADC=adc(an1);
		dtw(ADC/1000|48);
		dtw(ADC/100%10|48);
		dtw(ADC/10%10|48);
		dtw(ADC%10|48);
		wrt(DATA,LCD);
	}
}
void tx(unsigned char D)
{
	while(!PIR1bits.TXIF);
	TXREG=D;
}
unsigned char rx(void)
{
	while(!PIR1bits.RCIF)
	if(RCSTAbits.OERR)
	{
		RCSTAbits.CREN=0;
		RCSTAbits.CREN=1;
	}
	return RCREG;
}
void dtw(unsigned char D)
{
	PORTD=D;
	rs=1;
	en=1;
	Delay100TCYx(50);
	en=0;
	Delay100TCYx(100);
}
void cmw(unsigned char D)
{
	PORTD=D;
	rs=0;
	en=1;
	Delay100TCYx(50);
	en=0;
	Delay100TCYx(100);
}
unsigned int adc(unsigned char D)
{
	unsigned int ADC;
	Delay10KTCYx(1);
	ADCON0=D<<2|1;
	ADCON0bits.GO=1;
	while(ADCON0bits.DONE);
	ADC=ADRESH;
	return ADC<<8|ADRESL;
}
void wrt(unsigned char*D,char ESP_or_LCD)
{
	while(*D)
	switch(ESP_or_LCD)
	{
		case LCD:
			dtw(*D++);
			break;
		case ESP:
			tx(*D++);
			break;
		default:
			break;
	}
}