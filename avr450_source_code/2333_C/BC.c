/*********************************** MODULE INFO ****************************
	
   Device       : AT90S2333

   File name    : BC.c

   Ver nr.      : 1.0

   Description  : Standalone Battery Charger with AT90S2333 (main program)
                 
   Compiler     : IAR icca90
   
   Author       : Asmund Saetre / Terje Frostad / Dietmar Koenig
 
   Change log   : 02.02.2000 Changed to fit Battery Charger refrence 
                  design board   AS  
                  18.02.2000 Final test and review AS                  
 
****************************************************************************/
  
#include "BC.H"

time_struct time;                         //Globale struct

#ifdef DEBUG 
extern int term_value1;
extern int term_value2;
extern int battery_volt_max;
extern int battery_volt_min;
extern int temperature_max;
extern int temperature_min;
extern int charge_volt_max;
extern int charge_volt_min;
extern int charge_current_max;
extern int charge_current_min;
#endif

void Setup(void)
{
// definition of PWM, Ports, UART and Real-Time-Clock

  TCCR1A  = 0x81;         // define timer1 as 8-bit PWM  
  TIFR    = 0x80;         // clear counter1 overflow flag  
  TIMSK   = 0x80;         // enable counter1 overflow interrupt
  TCNT1   = 0x00;         // set counter1 to zero      
  TCCR1B  = 0x00;         // PWM disabLED

  PORTC   = 0x00;         // Prepare Port C as analog input
  DDRC    = 0x00;         // with no pull-ups
  
  PORTB   = 0xFF;         // Turn of Leds on Port B
  DDRB    = 0xFF;         // Set PORTB as output                    

  PORTD   = 0xFF;         // Turn on internal pull-ups for PORTD 
  
  ADCSR   = 0x96;         // Single A/D conversion, fCK/64 

#ifdef DEBUG 
  Inituart(3);          // initialize uart with baud rate 115200 @ 7.3728MHz
//  inituart(23);          // initialize uart with baud rate 19200 @ 7.3728MHz
#endif
   
  CHARGE_STATUS = 0x00;
  
  time.sec = 0x00;
  time.min = 0x00;
  time.hour = 0x00;
  time.t_count = 0x3878;
  
  _SEI();               // global interrupt enable        
}

void Stop_PWM(void)                         // Stops the PWM in off pos.
{
  if ((TERMINATION == 0x00)&&(TCCR1B & (1<<CS10))&&(OCR1 != 0))
  {
    if (OCR1 == 1)
    {
      while(TCNT1 > 2);                     // Wait for PWM == 1
      while(TCNT1 < 2);                     // Wait for PWM == 0  
    }
    else
    {
      while(TCNT1 > OCR1);                  // Wait for PWM == 1
      while(OCR1 > TCNT1);                  // Wait for PWM == 0  
    }
    TCCR1B = 0x00;                          // Turn PWM off 
  }
}    

void Stable_ADC(void)                     // loop until you have a stable value
{ 
  int V[4];
  char i;
  int Vmax, Vmin;
  
    //Loop until the ADC value is stable. (Vmax <= (Vmin+1))
    for (Vmax=10,Vmin= 0;Vmax > (Vmin+1);)
    {
        V[3] = V[2];
        V[2] = V[1];
        V[1] = V[0];
        ADCSR |= 0x40;                      // Start a new A/D conversion
        while (!(ADCSR & (1<<ADIF)))        // wait until ADC is ready      
            ;
        V[0] = ADC;
        Vmin = V[0];                          // Vmin is the lower VOLTAGE
        Vmax = V[0];                          // Vmax is the higher VOLTAGE  
        /*Save the max and min voltage*/
        for (i=0;i<=3;i++)
        { 
            if (V[i] > Vmax)
                Vmax=V[i];
            if (V[i] < Vmin)
              Vmin=V[i];  
        }
    }
}

int Battery(unsigned char value)
{
  char i;
  int av;

  switch (value)
  {
    case VOLTAGE_WITH_PWM_TURNOFF: 
        /*Stop PWM and select ADMUX ch. VOLTAGE_WITH_PWM_TURNOFF for battery
        voltage measurement. Wait until ADC value is stable*/   
        Stop_PWM();
        ADMUX = VBAT2;
        Stable_ADC();
        break;   
    case TEMPERATURE:
        /*Stop PWM and select ADMUX ch. TEMPERATURE for temperature 
        measurement. Wait until ADC value is stable*/   
        Stop_PWM();
        ADMUX = TBAT2;
        Stable_ADC();
        break; 
    case VOLTAGE: 
        /*Stop PWM and select ADMUX ch. VOLTAGE for charge voltage 
        measurement.*/   
        ADMUX = VBAT2;
        break;   
    case CURRENT: 
        /*Stop PWM and select ADMUX ch. CURRENT for charge current 
        measurement.*/   
        ADMUX = IBAT2;
        break;
   }  

    //Calculate a average out of the next 8 A/D conversions
    for(av=0,i=8;i;--i)
    {
        ADCSR |= 0x40;                      // start new A/D conversion
        while (!(ADCSR & (1<<ADIF)))        // wait until ADC is ready
            ;      
        av = av+ADC;
    }
    av = av/8;

    TCCR1B = 0x01;                          // turn on PWM
    CLRBIT(ADCSR,ADIF);                     // clear ADC interrupt flag

  switch (value)
  {
    case VOLTAGE_WITH_PWM_TURNOFF: 
        //Save min and max battery voltage for debug information        
		if(av <= battery_volt_min)
	        battery_volt_min = av;
		if (av > battery_volt_max)
	        battery_volt_max = av;
        break;   
    case TEMPERATURE:
        //Save min and max battery temperature for debug information        
		if (av <= temperature_min)
		    temperature_min = av;
		if(av > temperature_max)
	      temperature_max = av;
        break; 
    case VOLTAGE: 
        //Save min and max battery charge voltage for debug information        
        if (av <= charge_volt_min)
		    charge_volt_min = av;
		if (av > charge_volt_max)
	        charge_volt_max = av;
        break;   
    case CURRENT:
        //The current logging is placed inside the charge algorithme  
        break;																		
    }  
    return(av);  
}

void C_task main(void)
{
    Setup();
	Send_CR_LF();
    for (;;)
    {
        /*Makes the charger start in FAST mode every time and clear 
        TERMINATION indication before starting new charge*/
        SETBIT(CHARGE_STATUS,FAST);
        TERMINATION = 0x00;
        
        SETBIT(PORTB,(LED0+LED1+LED2+LED3));// Turn all LEDs off    
        while ((PIND & (1<<PD4)));          // Wait until SW0 pushed
    
        while (!(CHKBIT(CHARGE_STATUS,ERROR)))
        {
            if ((CHKBIT(CHARGE_STATUS,FAST)) && (!(CHKBIT(CHARGE_STATUS,ERROR))))
            {
                /*Turn only LED0 on, LED0 indicates "FAST Charge" 
                Enter FAST_charge function which handles the fast  
                charge algorithme. When returning send a state change message 
                throug the UART*/
                SETBIT(PORTB,(LED0+LED1+LED2+LED3));
                CLRBIT(PORTB,(LED0));
                TERMINATION = 0x00;
                CHARGE_STATUS = 0x00;
                    FAST_charge();
                #ifdef DEBUG 

                    Send_state_change();
				    Send_CR_LF();
                #endif
            }                   
            if ((CHKBIT(CHARGE_STATUS,TRICKLE)) && (!(CHKBIT(CHARGE_STATUS,ERROR))))
            {
                /*Turn only LED1 on, LED1 indicates "TRICKLE Charge" 
                Enter TRICKLE_charge function which handles the trickle 
                charge algorithme. When returning send a state change message 
                throug the UART*/
                SETBIT(PORTB,(LED0+LED1+LED2+LED3));
                CLRBIT(PORTB,(LED1));
                TERMINATION = 0x00;
                CHARGE_STATUS = 0x00;
                TRICKLE_charge();
                FAST_charge();
                #ifdef DEBUG 
                    Send_state_change();
				    Send_CR_LF();
                #endif
            }
        }
        
        if (CHKBIT(CHARGE_STATUS,ERROR))
        {
            /*Turn only LED3 on, LED3 indicates "ERROR". 
            Enter a eternal loop and no ERROR recovery except "reset-button"
            is possible*/
            SETBIT(PORTB,(LED0+LED1+LED2+LED3));
            CLRBIT(PORTB,LED3);
            for(;;);
        }   
    } 
}

void interrupt[TIMER1_OVF_vect] T1Ovf(void)
{  
  if (0x0000 == --time.t_count)
  {     
		#ifdef DEBUG				
			if (!(time.sec % 10))
			{
			Send_charge_state();			//Write state every	10 second
			Send_CR_LF();
			}
		#endif				 
    if ( 60 == ++time.sec )
    {
      if ( 60 == ++time.min )
      {
        if ( 24 == ++time.hour )
        {
            time.hour = 0x00;
        }
        time.min = 0x00;
        }
        time.sec = 0x00;       
        if ((CHKBIT(CHARGE_STATUS,TRICKLE)) && (!(CHKBIT(CHARGE_STATUS,ERROR))))
        {   /*Blink LED1 with 1 Hz if in trickle charge*/
		    if (time.sec % 2)
       		    CLRBIT(PORTB,(LED1));		// red LED1 on every odd sec
            else 
    		    SETBIT(PORTB,(LED1));		// red LED1 off every even sec
        }    		
    }
    // This is 7,372,800Hz/510 = 14,456Hz (Max PWM frequency)    
    time.t_count = 0x3878;          
  }
}

/*
void interrupt[TIMER1_CAPT1_vect] T1Capt(void)
{
// Not used in this application
}

void interrupt[TIMER1_COMP_vect] T1Comp(void)
{
// Not used in this application
}

void interrupt[TIMER0_OVF_vect] T0Ovf(void)
{
// Not used in this application
}

void interrupt[SPI_STC_vect] SPITranceiveComplete(void)
{
// Not used in this application
}

void interrupt[UART_RX_vect] UARTReceive(void)
{
// Not used in this application
}

void interrupt[UART_UDRE_vect] UARTDregEmpty(void)
{
// Not used in this application
}

void interrupt[UART_TX_vect] UARTTransmit(void)
{
// Not used in this application
}

void interrupt[ADC_vect] ADCConvert(void)
{
// Not used in this application
}

void interrupt[EE_RDY_vect] EEPROMReady(void)
{
// Not used in this application
}

void interrupt[ANA_COMP_vect] ACOMP(void)
{
// Not used in this application
}
*/
// ************************[ End of BC.C ]*******************************
