//Various LCD functions in 4-bit mode

#ifndef LCD_H
#define	LCD_H

void lcd_command(char);
void lcd_char(char);
void lcd_string(char *str);

void lcd_init(){
	DDRD |= 0xCF;					//Change DDRA --> DDRB and PORTA --> PORTB if LCD attached to port B and so forth
	lcd_command(0x33);				//Initialize LCD Driver
	lcd_command(0x32);				//Four bit mode
	lcd_command(0x2C);				//2 Line Mode
	lcd_command(0x0C);				//Display On, Cursor Off, Blink Off  Change to 0x0F if cursor is desired
	lcd_command(0x01);				//Clear Screen, Cursor Home
}

void lcd_command(char cmd){
	char temp = cmd;
	PORTD=0;
	_delay_ms(5);
	cmd = ( (cmd & 0xF0) >> 4) | 0x80;		//Write Upper Nibble (RS=0) E --> 1
	PORTD = cmd;
	_delay_ms(5);
	cmd ^= 0x80;							//E --> 0
	PORTD = cmd;
	_delay_ms(5);
	cmd = temp;
	cmd = ( (cmd & 0x0F) ) | 0x80;			//Write Lower Nibble (RS=0) E --> 1
	PORTD = cmd;
	_delay_ms(5);
	cmd ^= 0x80;							//E --> 0
	PORTD = cmd;
	_delay_ms(7);
}

void lcd_char(char data){
	char temp = data;
	PORTD = 0x40;
	_delay_ms(5);
	data = ( (data & 0xF0) >> 4) | 0xC0;	//Write Upper Nibble (RS=1) E --> 1
	PORTD = data;
	_delay_ms(5);
	data ^= 0x80;							//  E --> 0
	PORTD = data;
	_delay_ms(5);
	data = temp;
	data = ( (data & 0x0F) ) | 0xC0;		//Write Lower Nibble (RS=1) E --> 1
	PORTD = data;
	_delay_ms(5);
	data ^= 0x80;							//E --> 0
	PORTD = data;
	_delay_ms(7);
}

void lcd_string(char *str){
    while(*str > 0)
        lcd_char(*str++);
}

#endif