#include <reg52.h>

void send_serial_digit (unsigned char );
void send_serial_full (unsigned char);
void digit_clock (void);
void full_clock (void);
void display_digit (void);
void display_full (void);
void delay(void);
void initialize_display (void);
void spi_digit_init (void);
void spi_full_init (void);
void clear_digit (void);
void clear_full(void);

sbit DS_DIGIT = P1^3;
sbit DIGIT_SH_CLK = P1^4;
sbit DIGIT_ST_CLK = P1^0;

sbit DS_FULL = P1^1;
sbit FULL_SH_CLK = P0^7;
sbit FULL_ST_CLK = P0^6;

sbit RESET = P1^2;

sbit STATUS_LED = P0^2;
sbit DIRECTION_RED = P0^0;
sbit DIRECTION_GREEN = P0^1;
 

//char sdata[3];
//char i=0;

unsigned int index;
unsigned int index_array[5];
static char k;
unsigned int i,digit_count;
const char displaydata[10]={0xEE,0x06,0xBA,0x3E,0x56,0x7C,0xFC,0x0E,0xFE,0x7E};
static int status;

void serial_int(void) interrupt 4
{
	int i,temp_digit_count;
	static char chr='\0';
	char dat;
	
	if(RI==1)
	{
		if(SBUF!='-'&&'0'<=SBUF<='9')
		{
			digit_count++;
		dat = SBUF;		/* read the character into our local buffer */
		chr=dat;
		dat=dat;
		index=0;
		for(k=1;k<=4;k++)
		{
			if(dat&1)
			{
			if (k==1)index=index+1;
			if (k==2)index=index+2;
			if (k==3)index=index+4;
			if (k==4)index=index+8;
			}
			dat=dat>>1;
		}
		
		//P0=displaydata[index];
		//send_serial(displaydata[index]);
	index_array[digit_count]=index;	
	
		
		
		RI = 0;        /* clear the received interrupt flag */
              TI = 1;
	}	
	if (SBUF=='-')
	{
		chr=SBUF;
		//P0=chr;
	    //cl1-ear_digit();
		clear_full();
		
		/*for(i = 0;i<(4-digit_count);i++)
		{
			send_serial_digit(0x00);
		} */
		temp_digit_count=digit_count;
		while(digit_count!=0)
		{
			index=index_array[digit_count];
			send_serial_digit(displaydata[index]);
			digit_count--;
		}

		for(i = 0;i<(4-temp_digit_count);i++)
		{
			send_serial_digit(0x00);
		}
		display_digit();
		status=1;
		DIRECTION_GREEN=1;
		DIRECTION_RED=0;
		RI=0;
		TI=1;
		
	}
	if (SBUF=='F')
	{
		chr=SBUF;
		//P0=chr;
		clear_digit();
		digit_count=0;
		delay();
		send_serial_full(0xE0);   //send "L" 
		send_serial_full(0xE0);	  //send "L"
		send_serial_full(0xE6);	  //send "U"
		send_serial_full(0xD8);	  //send "F"
		display_full();
		status=0;
		DIRECTION_GREEN=0;
		DIRECTION_RED=1;
		//FULL=1;
	}
	

}
	else if(TI==1)
	{
		 TI = 0;        /* clear the transmit interrupt flag */
              if (chr != '\0')        /* if there's something in the local buffer... */
              {
                      if (chr == '\r') chr = '\n';        /* convert  to  */
                      SBUF = chr;        /* put the character into the transmit buffer */
                      chr = '\0';
              }
		}
}

void send_serial_digit (unsigned char sdata)
{
	int count;
	for (count=0;count<8;count++)
	{
		if(sdata&0x80)
		{
			DS_DIGIT=1;
		}
		else
		{
			DS_DIGIT=0;
		}
		digit_clock();
		sdata=sdata<<1;
	}
}

void send_serial_full (unsigned char sdata)
{
	int count;
	for(count=0;count<8;count++)
	{
		if (sdata&0x80)
		{
		 	DS_FULL=1;
		}
		else
		{
			DS_FULL=0;
		}
		full_clock();
		sdata=sdata<<1;
	}

	
}

void digit_clock (void)
{
	DIGIT_SH_CLK=0;
	delay();
	DIGIT_SH_CLK=1;
	delay();
	DIGIT_SH_CLK=0;
}

void full_clock (void)
{
	FULL_SH_CLK=0;
	delay();
	FULL_SH_CLK=1;
	delay();
	FULL_SH_CLK=0;
}



void display_digit (void)
{
	DIGIT_ST_CLK=0;
//	STATUS_LED=0;
	delay();
//	STATUS_LED=1;
	DIGIT_ST_CLK=1;
	delay();
//	STATUS_LED=0;
	DIGIT_ST_CLK=0;
	
}

void display_full (void)
{
	FULL_ST_CLK=0;
	delay();
	FULL_ST_CLK=1;
	delay();
	FULL_ST_CLK=0;
}



void serial_init()
{
	TMOD = 0x20;
	SCON = 0x50;
	TH1 = 0xFD;
	ET0=0;
	TR1 = 1;
	TI = 1;
	
}

void enable_interrupts()
{
	ES=1;	  //allow serial interrupts
	EA=1;	  //enable interrupts
}

void spi_digit_init (void)
{
	DIGIT_SH_CLK=0;
	DS_DIGIT=0;
	DIGIT_ST_CLK=0;
	RESET=1;
}


void spi_full_init (void)
{
	FULL_SH_CLK=0;
	DS_FULL=0;
	FULL_ST_CLK=0;
	RESET=1;
}



void delay(void)
{
	int k;
	for(k=0;k<100;k++) {;}
}

void initialize_display (void)
{		
		clear_digit();
		clear_full();
		DIRECTION_RED=0;
		DIRECTION_GREEN=0;
	    send_serial_digit(0x10);
		send_serial_digit(0x10);
		send_serial_digit(0x10);
		send_serial_digit(0x10);
		display_digit();

	/*	send_serial_full(0x10);
		send_serial_full(0x10);
		send_serial_full(0x10);
		send_serial_full(0x10);
		display_full();	 */
	
}

void clear_digit (void)
{
	send_serial_digit(0x00);
	send_serial_digit(0x00);
	send_serial_digit(0x00);
	send_serial_digit(0x00);
	display_digit();
}

void clear_full (void)
{
	send_serial_full(0x00);
	send_serial_full(0x00);
	send_serial_full(0x00);
	send_serial_full(0x00);
	display_full();
}

void main (void)
{
	for(i=0;i<60000;i++){;}
	serial_init();
	enable_interrupts();
	spi_digit_init();
	spi_full_init();
	initialize_display();
	//FULL=0;
	while(1)
	{
		unsigned int j;
    for (j = 0; j < 32758; j++) {;}        /* delay */
   {
   if(status==1)
   DIRECTION_GREEN=~DIRECTION_GREEN;
   STATUS_LED=~STATUS_LED;
	}			
	}
}