#define F_CPU 8000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <string.h>
#include <stdlib.h>
	
	
void usart_init(){
	UCSRA = 0x00;
	UCSRB = (1 << RXEN)|(1 << TXEN);
	UBRRH = 0x00;
	UBRRL = 0x33;
	UCSRC = (1 << URSEL)|(3 << UCSZ0);
	return;
}

void usart_transmit(unsigned char byte){
	while(!(UCSRA & 0x20));
	UDR = byte;
	return;
}

unsigned char usart_receive(){
	unsigned char byte;
	while(!(UCSRA & 0x80));
	byte = UDR;
	return byte;
}

void write_2_nibbles_sim(char input)
{
	
	_delay_us(6000);
	char prev = PIND;
	PORTD = (input&0xf0) | (prev&0x0f);
	
	PORTD |= (1<<PD3);
	PORTD &= ~(1<<PD3);
	_delay_us(6000);//remote access
	PORTD = ((input&0x0f)<<4) | (prev&0x0f);
	PORTD |= (1<<PD3);
	PORTD &= ~(1<<PD3);
	
	return;
}

void lcd_data_sim(unsigned char input)
{
	PORTD = (1 << PD2 );
	write_2_nibbles_sim(input);
	_delay_us(43);
	return ;
}

void lcd_command_sim(unsigned char command)
{
	PORTD = (0 << PD2);
	write_2_nibbles_sim(command);
	_delay_us(39);
	return ;
}

void lcd_init_sim()
{
	_delay_ms(40);
	
	PORTD = 0x30;
	PORTD |=(1 << PD3);
	PORTD &=~(1 << PD3);
	_delay_us(39);

	_delay_us(1000);
	PORTD = 0x30;
	PORTD |= (1 << PD3);
	PORTD &=~ (1 << PD3);
	_delay_us(39);

	_delay_us(1000);
	PORTD = 0x20;
	PORTD |= (1 << PD3);
	PORTD &=~ (1 << PD3);
	_delay_us(39);

	_delay_us(1000);
	lcd_command_sim(0x28);
	lcd_command_sim(0x0c);
	lcd_command_sim(0x01);
	_delay_us(1530);
	
	lcd_command_sim(0x06);
	return ;
}

void welcome(){
	lcd_command_sim(0x01);
	_delay_us(1530);
	lcd_data_sim('W');
	lcd_data_sim('E');
	lcd_data_sim('L');
	lcd_data_sim('C');
	lcd_data_sim('O');
	lcd_data_sim('M');
	lcd_data_sim('E');
	return;
}

void check_state(unsigned char num){
	unsigned char state;
	//clear lcd
	lcd_command_sim(0x01);
	_delay_us(1530);
	lcd_data_sim(num);
	lcd_data_sim('.');
	state = usart_receive();
	while(state != '\n'){
		lcd_data_sim(state);
		_delay_us(100);
		state = usart_receive();
		_delay_us(100);
	}
	return;
}

void usart_receive_string(char  *input_buffer)
{
	int i = 0;
	while(1)
	{
		input_buffer[i] = usart_receive();
		if(input_buffer[i] != '\n')
			i++;
		else
			break;
	}
}

void lcd_print_buffer(char *input_buffer, unsigned char num)
{
	lcd_command_sim(0x01);
	_delay_us(1530);
	lcd_data_sim(num);
	lcd_data_sim('.');
	int i = 0;
	while(input_buffer[i] != '\n')
	{
		lcd_data_sim(input_buffer[i++]);
	}
}

void connect(){
	unsigned char command[12] = {'E', 'S', 'P', ':', 'c', 'o', 'n', 'n', 'e', 'c', 't', '\n'};
	
	for(int i=0; i<12; i=i+1)
	{
		usart_transmit(command[i]);
		_delay_us(39);
	}
	//check_state('1');
	return;
}


void addSensor(char *name){
	unsigned char command[16] = {'E', 'S', 'P', ':', 'a', 'd', 'd', 'S', 'e', 'n', 's', 'o', 'r', ':', ' ', '"'};
	for(int i=0; i<16; i++)
	{
		usart_transmit(command[i]);
		_delay_us(39);
	}
	for(int i=0; i<strlen(name); i++)
	{
		usart_transmit(name[i]);
		_delay_us(39);
	}
	usart_transmit('"');
	usart_transmit('\n');
	//check_state('2');
	return;
}


int main(void){
	DDRD=0XFF;
	char input_buffer[20];
	memset(input_buffer,0,sizeof(input_buffer));
	usart_init();
	lcd_init_sim();
	welcome();
	_delay_ms(3000);
	//restart();
	connect();
	usart_receive_string(input_buffer);
	lcd_print_buffer(input_buffer, '1');
	_delay_ms(3000);
	addSensor1("nikodimos");
	usart_receive_string(input_buffer);
	lcd_print_buffer(input_buffer, '2');
	
	
	return 0;
}
