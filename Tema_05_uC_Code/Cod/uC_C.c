#include <REG51.H>

#define ADC_DATA P1
#define LCD P2

sbit RS = P0^0;
sbit RW = P0^1;
sbit E = P0^2;
sbit WR_ADC = P3^6; 
sbit RD_ADC = P3^7; 
sbit INTR = P3^2;

unsigned char code string[] = "Temperature:";

void delay(int ms);
void command(int hex);
void setup_lcd();
void show_char(unsigned char x);
unsigned char read_adc();
void show_temp(unsigned char val);

void main(){
	ADC_DATA = 0x0FF;
	LCD = 0x00;
	RW = 0;
	INTR = 1;
	
	setup_lcd();

	while(1){
		command(0x8C);
		show_temp(read_adc());
		delay(300);
	}
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
	
	command(0x0E);
	delay(2);
		
	command(0x01);
	delay(2);	
		
	command(0x06);
	delay(2);
	
	for( i=0; i < 0x0C; i++){
		show_char(string[i]);
	}
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

void show_temp(unsigned char val){
	unsigned int temp;
  unsigned char tens, ones, tenth;
    
	temp = ((unsigned int)val * 80 ) / 51;
    
	tens = temp / 100;           
	ones = (temp % 100) / 10; 
	tenth = temp % 10;              
    
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