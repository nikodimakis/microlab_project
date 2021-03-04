#define F_CPU 8000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <string.h>
#include <stdlib.h>

#define NumberOfSensors  6
char input_buffer[30];


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
	lcd_data_sim('A');
	lcd_data_sim('C');
	lcd_data_sim('C');
	lcd_data_sim('E');
	lcd_data_sim('S');
	lcd_data_sim('S');
	lcd_data_sim(' ');
	lcd_data_sim('P');
	lcd_data_sim('O');
	lcd_data_sim('I');
	lcd_data_sim('N');
	lcd_data_sim('T');
	return;
}

void printm(char* temp){
	lcd_command_sim(0x01);
	_delay_us(1530);
	for (int i=0; i<strlen(temp); i++)
	lcd_data_sim(temp[i]);
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
	//lcd_data_sim(num);		// Printing num for debugging
	//lcd_data_sim('.');
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
	return;
}

void restart(){
	printm("Restarting");
	_delay_ms(1000);
	unsigned char command[12] = {'E', 'S', 'P', ':', 'r', 'e', 's', 't', 'a', 'r', 't', '\n'};
	
	for(int i=0; i<12; i=i+1)
	{
		usart_transmit(command[i]);
		_delay_us(39);
	}
	usart_receive_string(input_buffer);
	memset(input_buffer,0,sizeof(input_buffer));
	usart_receive_string(input_buffer);
	memset(input_buffer,0,sizeof(input_buffer));
	_delay_ms(3000);
	return;
}

void accessPoint(char *ip, char *ssid, char *pass){
	printm("Starting AP");
	_delay_ms(2000);
	/*unsigned char command1[13] = {'E', 'S', 'P', ':', 'h', 'o', 's', 't', 'I', 'P', ':', ' ', '"'};
	for(int i=0; i<13; i=i+1)
	{
		usart_transmit(command1[i]);
		_delay_us(39);
	}
	for(int i=0; i<strlen(ip); i++)
	{
		usart_transmit(ip[i]);
		_delay_us(39);
	}
	usart_transmit('"');
	usart_transmit('\n');
	usart_receive_string(input_buffer);
	memset(input_buffer,0,sizeof(input_buffer));
	_delay_ms(1000);
	*/
	unsigned char command2[11] = {'E', 'S', 'P', ':', 's', 's', 'i', 'd', ':', ' ', '"'};
	for(int i=0; i<11; i=i+1)
	{
		usart_transmit(command2[i]);
		_delay_us(39);
	}
	for(int i=0; i<strlen(ssid); i++)
	{
		usart_transmit(ssid[i]);
		_delay_us(39);
	}
	usart_transmit('"');
	usart_transmit('\n');
	usart_receive_string(input_buffer);
	memset(input_buffer,0,sizeof(input_buffer));
	_delay_ms(1000);
	unsigned char command3[15] = {'E', 'S', 'P', ':', 'p', 'a', 's', 's', 'w', 'o', 'r', 'd', ':', ' ', '"'};
	for(int i=0; i<15; i=i+1)
	{
		usart_transmit(command3[i]);
		_delay_us(39);
	}
	for(int i=0; i<strlen(pass); i++)
	{
		usart_transmit(pass[i]);
		_delay_us(39);
	}
	usart_transmit('"');
	usart_transmit('\n');
	usart_receive_string(input_buffer);
	memset(input_buffer,0,sizeof(input_buffer));
	_delay_ms(1000);
	
	unsigned char command[12] = {'E', 'S', 'P', ':', 'A', 'P', 'S', 't', 'a', 'r', 't', '\n'};
		
	for(int i=0; i<12; i=i+1)
	{
		usart_transmit(command[i]);
		_delay_us(39);
	}
	usart_receive_string(input_buffer);
	lcd_print_buffer(input_buffer, 'w');
	_delay_ms(2000);
	return;
}



void addSensor(char *name){
	printm("Adding sensor");
	_delay_ms(2000);
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
	
	usart_receive_string(input_buffer);
	lcd_print_buffer(input_buffer, 'a');
	_delay_ms(2000);
	return;
}



void sensorValue(char *name, char* temp){
	printm("setting Value");
	_delay_ms(2000);
	unsigned char command[17] = {'E', 'S', 'P', ':', 's', 'e', 'n', 's', 'o', 'r', 'V', 'a', 'l', 'u', 'e', ':', '"'};
	for(int i=0; i<17; i++)
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
	usart_transmit('[');
	for(int i=0; i<strlen(temp); i++)
	{
		usart_transmit(temp[i]);
		_delay_us(39);
	}
	usart_transmit(']');
	usart_transmit('\n');
	
	usart_receive_string(input_buffer);
	lcd_print_buffer(input_buffer, 's');
	_delay_ms(2000);
	
	return;
}


void getValue(char *name){
	printm("getting Value");
	_delay_ms(2000);
	unsigned char command[14] = {'E', 'S', 'P', ':', 'g', 'e', 't', 'V', 'a', 'l', 'u', 'e', ':', '"'};
	for(int i=0; i<14; i++)
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
	
	usart_receive_string(input_buffer);
	lcd_print_buffer(input_buffer, 'g');
	_delay_ms(2000);
	return;
}



int main(void){
	DDRD=0XFF;
	memset(input_buffer,0,sizeof(input_buffer));
	usart_init();
	lcd_init_sim();
	welcome();
	_delay_ms(3000);
	
	restart();
	
	// creating the sensors
	//char AllSensor[NumberOfSensors][3] = {"A_1", "A_2", "A_3", "B_1", "B_2", "B_3"};
	//for (int i=0; i<NumberOfSensors; i++)
	//	addSensor(AllSensor[i]);

	// adding all possible sensors 
	addSensor("A_1");
	addSensor("A_2");
	addSensor("B_1");
	addSensor("B_2");

	// lets assume that we already know the value some of them (A_i)
	sensorValue("A_1", "13");
	sensorValue("A_2", "101");
	
	// just show the value of them
	printm("Printing known values");
	getValue("A_1");
	getValue("A_2");

	// to make the access point we need ip ssid and password
	accessPoint("192.168.4.2", "marketos", "marketo1");


	while(1){
		// just serving all client that may connect to the AP
		printm("waiting . . .");
		usart_receive_string(input_buffer);
		lcd_print_buffer(input_buffer, 'P');
		_delay_ms(2000);
		memset(input_buffer,0,sizeof(input_buffer));
	}
	
	
	return 0;
}

