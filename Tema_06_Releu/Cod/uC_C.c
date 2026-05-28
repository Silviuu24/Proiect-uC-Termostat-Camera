#include <REG51.H>

#define ADC_DATA P1
#define LCD P2

sbit RS = P0^0;
sbit RW = P0^1;
sbit E = P0^2;
sbit INTR = P3^0;
sbit BTN_PLUS = P3^2;
sbit BTN_MINUS = P3^3;
sbit RELAY = P3^5;
sbit WR_ADC = P3^6; 
sbit RD_ADC = P3^7; 

unsigned char code string[] = "Temperature:";
unsigned char code set[] = "Set:";
unsigned int set_value = 210;
unsigned int max_value = 300;
unsigned int min_value = 100;
unsigned int temp_value;
bit update_lcd_flag = 0;
	
void delay(int ms);
void command(int hex);
void setup_lcd();
void show_char(unsigned char x);
void show_value(unsigned int val);
void process_data(unsigned char val);
unsigned char read_adc();

void main(){
	ADC_DATA = 0x0FF;
	LCD = 0x00;
	RW = 0;
	RELAY = 0;
	INTR = 1;
	BTN_PLUS = 1;
	BTN_MINUS = 1;
	IT0 = 1;
	IT1 = 1;
	IE = 0x85;
	setup_lcd();

	while(1){
		EA = 0;
		command(0x8C);
		process_data(read_adc());
		EA = 1;
		
		if(update_lcd_flag){
			EA = 0;
			command(0xC4);
			show_value(set_value);
			EA = 1;
			update_lcd_flag = 0;
		}
		
		delay(300);
	}
}


void plus() interrupt 0{
	delay(20);
	if(BTN_PLUS == 0 && set_value < max_value)
	{
		set_value += 5;
		update_lcd_flag = 1;
	}
	while(BTN_PLUS == 0);
	delay(20);
}

void minus() interrupt 2{
	delay(20);
	if(BTN_MINUS == 0 && set_value > min_value){
		set_value -= 5;
		update_lcd_flag = 1;
	}
	while(BTN_MINUS == 0);
	delay(20);
}

void delay(int ms){
	unsigned int j, i;
	for(j=0; j< ms; j++){
	for(i = 0; i< 112; i++){}
	}
}

void command(int hex){
	RS = 0;
	LCD = hex;
	E = 1;
	delay(1);
	E = 0;
}

void setup_lcd(){
	unsigned int i;
	
	command(0x38);
	delay(2);
	
	command(0x0C);
	delay(2);
		
	command(0x01);
	delay(2);	
		
	command(0x06);
	delay(2);
	
	for( i=0; i < 0x0C; i++){
		show_char(string[i]);
	}
	
	command(0xC0);
	delay(2);
	
	for( i=0; i < 0x04; i++){
		show_char(set[i]);
	}
	show_value(set_value);
}

void show_char(unsigned char x){
	RS = 1;
	LCD = x;
	E = 1;
	delay(10);
	E = 0;
}

unsigned char read_adc(){
	unsigned char val;
	
	WR_ADC = 0;
	WR_ADC = 1;
	
	while(INTR == 1);
	
	RD_ADC = 0;
	val = ADC_DATA;
	RD_ADC = 1;
	
	return val;
}

void process_data(unsigned char val){
	
	temp_value = ((unsigned int)val * 80 ) / 51;
	
	if(temp_value < (set_value - 5)){
		RELAY = 1;
	} 
	else if( temp_value > (set_value + 5)){
		RELAY = 0;
	}
	
  show_value(temp_value);
}

void show_value(unsigned int val){
  unsigned char tens, ones, tenth;
	
	tens = val / 100;           
	ones = (val % 100) / 10; 
	tenth = val % 10;              
    
	if (tens > 0){
		show_char(tens + '0'); 
	} 
	else{
		show_char(' '); 
	}
    
	show_char(ones + '0');
    
	show_char('.');
	show_char(tenth + '0');
}