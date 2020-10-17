/*********************************** MODULE INFO ****************************
	
   Device       : AT90S2333

   File name    : BC_debug.c

   Ver nr.      : 1.0

   Description  : UART report functions for monitoring of the charging
                 
   Compiler     : IAR icca90
   
   Author       : Asmund Saetre
 
   Change log   : 16.05.2000 Made by AS
 
****************************************************************************/
#ifdef DEBUG
  
#include "BC_debug.H"
#include "BC.h"

// Extern struct prototypes
extern time_struct time;

int term_value1;
int term_value2;

int battery_volt_max = 0;
int battery_volt_min = 0;
int temperature_max = 0;
int temperature_min = 0;
int charge_volt_max = 0;
int charge_volt_min = 0;
int charge_current_max = 0;
int charge_current_min = 0;

// UART routines are only included for debug purpose
void Inituart(unsigned char baudrate)
{
    UBRR = baudrate;
    UCR = (1<<TXEN);                        // enable UART transmitter
}

void Send_int(int uart_data)
{
    int temp,middle;
    temp = uart_data;
    while (!(USR & (1<<UDRE)))              // wait for empty transmit buffer
        ;
    middle = (temp/1000);                   // first digit of int uart_data
    UDR = middle + 0x30;
    temp = (temp-(middle*1000));
    while (!(USR & (1<<UDRE)))              // wait for empty transmit buffer
        ;
    middle = (temp/100);                    // second digit of int uart_data
    UDR = middle + 0x30;
    temp = (temp-(middle*100));
    while (!(USR & (1<<UDRE)))              // wait for empty transmit buffer
        ;
    middle = (temp/10);                     // third digit of int uart_data
    UDR = middle + 0x30;
    temp = (temp-(middle*10));
    while (!(USR & (1<<UDRE)))              // wait for empty transmit buffer
        ;
    middle = temp;                          
    UDR = middle + 0x30;                    // fourth digit of int uart_data
}

void Send_char(char uart_data)
{
    char temp,middle;
    temp = uart_data;
    while (!(USR & (1<<UDRE)))              // wait for empty transmit buffer
        ;
    middle = (temp/10);                     // first digit of char uart_data
    UDR = middle + 0x30;
    temp = (temp-(middle*10));
    while (!(USR & (1<<UDRE)))              // wait for empty transmit buffer
        ;
    middle = temp;                          // second digit of char uart_data
    UDR = middle + 0x30;
}

void Send_state_change(void)
{
    while (!(USR & (1<<UDRE)))              // wait for empty transmit buffer
        ;
    UDR = 'S';                            
    while (!(USR & (1<<UDRE)))              // wait for empty transmit buffer
        ;
    UDR = 't';                            
    while (!(USR & (1<<UDRE)))              // wait for empty transmit buffer
        ;
    UDR = 'a';                            
    while (!(USR & (1<<UDRE)))              // wait for empty transmit buffer
        ;
    UDR = 't';                            
    while (!(USR & (1<<UDRE)))              // wait for empty transmit buffer
        ;
    UDR = 'e';                            
    while (!(USR & (1<<UDRE)))              // wait for empty transmit buffer
        ;
    UDR = ':';                            
    while (!(USR & (1<<UDRE)))              // wait for empty transmit buffer
        ;
    UDR = ' ';                            
    while (!(USR & (1<<UDRE)))              // wait for empty transmit buffer
        ;
    Send_char(CHARGE_STATUS);                            
    while (!(USR & (1<<UDRE)))              // wait for empty transmit buffer
        ;
    UDR = ' ';                            
    while (!(USR & (1<<UDRE)))              // wait for empty transmit buffer
        ;
    UDR = 'T';                            
    while (!(USR & (1<<UDRE)))              // wait for empty transmit buffer
        ;
    UDR = 'e';                            
    while (!(USR & (1<<UDRE)))              // wait for empty transmit buffer
        ;
    UDR = 'r';                            
    while (!(USR & (1<<UDRE)))              // wait for empty transmit buffer
        ;
    UDR = 'm';                            
    while (!(USR & (1<<UDRE)))              // wait for empty transmit buffer
        ;
    UDR = ':';                            
    while (!(USR & (1<<UDRE)))              // wait for empty transmit buffer
        ;
    UDR = ' ';                            
    while (!(USR & (1<<UDRE)))              // wait for empty transmit buffer
        ;
    Send_char(TERMINATION);				    //Send termination byte
    while (!(USR & (1<<UDRE)))              // wait for empty transmit buffer
        ;
    UDR = ' ';                            
    while (!(USR & (1<<UDRE)))              // wait for empty transmit buffer
        ;
    Send_int(term_value1);                            
    while (!(USR & (1<<UDRE)))              // wait for empty transmit buffer
        ;
    UDR = ' ';                            
    while (!(USR & (1<<UDRE)))              // wait for empty transmit buffer
        ;
	Send_int(term_value2);                            
    while (!(USR & (1<<UDRE)))              // wait for empty transmit buffer
        ;
}

void Send_charge_state(void)
{
    while (!(USR & (1<<UDRE)))              // wait for empty transmit buffer
        ;
    UDR = 't';                            
    while (!(USR & (1<<UDRE)))              // wait for empty transmit buffer
        ;
    UDR = ':';                            
    while (!(USR & (1<<UDRE)))              // wait for empty transmit buffer
        ;
	Send_char(time.min);                            
    while (!(USR & (1<<UDRE)))              // wait for empty transmit buffer
        ;
    UDR = ':';                            
    while (!(USR & (1<<UDRE)))              // wait for empty transmit buffer
        ;
	Send_char(time.sec);                            
    while (!(USR & (1<<UDRE)))              // wait for empty transmit buffer
        ;
    UDR = ' ';                            
    while (!(USR & (1<<UDRE)))              // wait for empty transmit buffer
        ;
    UDR = 'B';                            
    while (!(USR & (1<<UDRE)))              // wait for empty transmit buffer
        ;
    UDR = 'V';                            
    while (!(USR & (1<<UDRE)))              // wait for empty transmit buffer
        ;
    UDR = ':';                            
    while (!(USR & (1<<UDRE)))              // wait for empty transmit buffer
        ;
	Send_int(battery_volt_min);                            
    while (!(USR & (1<<UDRE)))              // wait for empty transmit buffer
        ;
    UDR = '-';                                             
    while (!(USR & (1<<UDRE)))              // wait for empty transmit buffer
        ;
	Send_int(battery_volt_max);                            
    while (!(USR & (1<<UDRE)))              // wait for empty transmit buffer
        ;
    UDR = ' ';                            
    while (!(USR & (1<<UDRE)))              // wait for empty transmit buffer
        ;
    UDR = 'C';                            
    while (!(USR & (1<<UDRE)))              // wait for empty transmit buffer
        ;
    UDR = 'V';                            
    while (!(USR & (1<<UDRE)))              // wait for empty transmit buffer
        ;
    UDR = ':';                            
    while (!(USR & (1<<UDRE)))              // wait for empty transmit buffer
        ;
    UDR = ' ';                            
    while (!(USR & (1<<UDRE)))              // wait for empty transmit buffer
        ;
	Send_int(charge_volt_min);                            
    while (!(USR & (1<<UDRE)))              // wait for empty transmit buffer
        ;
    UDR = '-';             
    while (!(USR & (1<<UDRE)))              // wait for empty transmit buffer
        ;
	Send_int(charge_volt_max);                            
    while (!(USR & (1<<UDRE)))              // wait for empty transmit buffer
        ;
    UDR = ' ';                            
    while (!(USR & (1<<UDRE)))              // wait for empty transmit buffer
        ;
    UDR = 'C';                            
    while (!(USR & (1<<UDRE)))              // wait for empty transmit buffer
        ;
    UDR = ':';                            
    while (!(USR & (1<<UDRE)))              // wait for empty transmit buffer
        ;
		Send_int(charge_current_min);                            
    while (!(USR & (1<<UDRE)))              // wait for empty transmit buffer
        ;
    UDR = '-';             
    while (!(USR & (1<<UDRE)))              // wait for empty transmit buffer
        ;
		Send_int(charge_current_max);                            
    while (!(USR & (1<<UDRE)))              // wait for empty transmit buffer
        ;
    UDR = ' ';                            
    while (!(USR & (1<<UDRE)))              // wait for empty transmit buffer
        ;
    UDR = 'T';                            
    while (!(USR & (1<<UDRE)))              // wait for empty transmit buffer
        ;
    UDR = ':';                            
    while (!(USR & (1<<UDRE)))              // wait for empty transmit buffer
        ;
		Send_int(temperature_min);
    while (!(USR & (1<<UDRE)))              // wait for empty transmit buffer
        ;
    UDR = '-';             
    while (!(USR & (1<<UDRE)))              // wait for empty transmit buffer
        ;
		Send_int(temperature_max);
    while (!(USR & (1<<UDRE)))              // wait for empty transmit buffer
        ;
    UDR = ' ';                            
    while (!(USR & (1<<UDRE)))              // wait for empty transmit buffer
        ;
		
	battery_volt_max 	= 0x00;
	battery_volt_min	= 0x03FF;
	temperature_max 	= 0x00;
	temperature_min 	= 0x03FF;
	charge_volt_max 	= 0x00;
	charge_volt_min 	= 0x03FF;
	charge_current_max 	= 0x00;
	charge_current_min 	= 0x03FF;
}

void Send_CR_LF (void)
{
    while (!(USR & (1<<UDRE)))              // wait for empty transmit buffer
        ;
    UDR = 0x0D;                             // send carriage return
    while (!(USR & (1<<UDRE)))              // wait for empty transmit buffer
        ;
    UDR = 0x0A;                             // send line feed               
}
#endif

