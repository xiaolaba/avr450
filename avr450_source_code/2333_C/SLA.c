/****************************************************************************
    	        
	Device     	: AT90S2333

	File name	: SLA.c

	Ver nr.     : 1.0

	Description	: Sourcefile for SLA charging algorithm
				    - FAST_charge
				    - TRICKLE_charge
		  
	Compiler	: IAR icca90
   
	Author		: Asmund Saetre / Terje Frostad / Dietmar Koenig
 
	Change log	: 02.02.2000 Changed to fit Battery Charger Refrence 
	              Design	AS
   				  18.05.2000 Final test and review AS  
 
****************************************************************************/

#include "Sla.h"							//Battery specific definitions

// Extern struct prototypes
extern time_struct time;

#ifdef DEBUG 
extern int term_value1;
extern int term_value2;
extern int charge_current_min;
extern int charge_current_max;
#endif

// Fast	Charge Algorithme
//**************************************************************************

void FAST_charge(void)
{
    unsigned int temp = 0;
    unsigned char fast_finish_hour = 0;
    unsigned char fast_finish_min = 0;
    unsigned int fast_charge_volt = 0;
    unsigned char last_min;

    #ifdef DEBUG 
	term_value1 = 0;
	#endif
	
	time.sec = 0x00;
  	time.min = 0x00;
  	time.hour = 0x00;
  	time.t_count = 0x3878;

    temp = Battery(TEMPERATURE); 
    // if TEMPERATURE within absolute limits
    if ((temp < MIN_TEMP_ABS) && (temp > MAX_TEMP_ABS)&& (temp > MAX_TEMP_FAST))
    {
        // calculate FAST charge finish time
        fast_finish_min = (time.min + MAX_TIME_FAST);
        fast_finish_hour = time.hour;
        while (fast_finish_min	> 60)
        {
            fast_finish_min = fast_finish_min	- 60;
            fast_finish_hour++;
        }
        
        OCR1 = 0x00; 
        TCCR1B = 0x01;					    	//counter1 clock prescale =	1

        if ((CHKBIT(CHARGE_STATUS,FAST)) && (!(CHKBIT(CHARGE_STATUS,ERROR))))
        {
            /*Find the fast charge voltage by setting charge current to 
            MAX_I_FAST - 10mA and read out the charge voltage. Store the
            voltage for later regulating referance*/    
            do
        	{
        	    fast_charge_volt = Battery(CURRENT);
	            if (fast_charge_volt < (MAX_I_FAST-I_BUFFER))
            	    OCR1++;
	            else if (fast_charge_volt > (MAX_I_FAST-I_BUFFER))
                    OCR1--;
	        } while	(fast_charge_volt != (MAX_I_FAST-I_BUFFER));
        }
      
        while ((CHKBIT(CHARGE_STATUS,FAST)) && (!(CHKBIT(CHARGE_STATUS,ERROR))))
        {
            // set fast_charge_volt (with "soft start")
	        do
	        {
        	    temp = Battery(VOLTAGE);
	       	    if (temp < fast_charge_volt)
	                OCR1++;
        	    else if (temp > fast_charge_volt)
	                OCR1--;
	        } while	(temp != fast_charge_volt);

			if ((time.hour	== fast_finish_hour)&&(time.min	== fast_finish_min))
		    {
			    /*Stop the PWM, flag max time charge termination and 
				ERROR. Save the termination value and the max limit 
				value for debug information*/
				Stop_PWM();
				SETBIT(TERMINATION,TIME_MAX);
				SETBIT(CHARGE_STATUS,ERROR);
				#ifdef DEBUG 
				term_value1 = time.min;
				term_value2 = fast_finish_min;
				#endif
			}
            
            temp = Battery(TEMPERATURE);
			if ( temp <	MAX_TEMP_ABS)
			{
				/*Stop the PWM, flag max temperature charge 
				termination and ERROR. Save the termination value and
				the max limit value for debug information*/
				Stop_PWM();
				SETBIT(TERMINATION,TEMP_MAX);
				SETBIT(CHARGE_STATUS,ERROR);
				#ifdef DEBUG 
				term_value1 = temp;
				term_value2 = MAX_TEMP_ABS;
				#endif
			}

            temp = Battery(CURRENT);
			if ( temp <	MAX_I_FAST)
			{
				/*Stop the PWM, flag max charge current charge 
				termination and ERROR. Save the termination value and
				the max limit value for debug information*/
				Stop_PWM();
				SETBIT(TERMINATION,I_MAX);
				SETBIT(CHARGE_STATUS,ERROR);
				#ifdef DEBUG
				term_value1 = temp;
				term_value2 = MAX_I_FAST;
				#endif
			}

            /*Every min check if MIN_I_FAST is reached, if so change to 
            trickle charge*/
            if (time.min != last_min)   
            {
                last_min = time.min;
                temp = Battery(CURRENT);
		    	if ( temp <	MAX_I_FAST)
			    {
    				Stop_PWM();
            	    SETBIT(TERMINATION,I_MIN);
	                CLRBIT(CHARGE_STATUS,FAST);
    	            SETBIT(CHARGE_STATUS,TRICKLE);		
    	            #ifdef DEBUG 
	    			term_value1 = temp;
		    	    term_value2 = MAX_I_FAST;
		    	    #endif
			    }
            }			    
        }
    }
    if(!(CHKBIT(CHARGE_STATUS,ERROR)))
	{
	    if ((temp > MAX_TEMP_ABS)||(temp > MAX_TEMP_FAST))
        {
            //Flag max temperature termination and save the limit value
			SETBIT(CHARGE_STATUS,TEMP_MIN);
			#ifdef DEBUG 
			if (MAX_TEMP_ABS < MAX_TEMP_FAST)
    			term_value2 =	MAX_TEMP_ABS;
    		else	
    			term_value2 =	MAX_TEMP_FAST;
    		#endif	
	    }
	    
		else if (temp < MIN_TEMP_ABS)
		{
		    //Flag min temperature termination and save the limit value
		    #ifdef DEBUG 
		    term_value2 = MIN_TEMP_ABS;
		    #endif
			SETBIT(TERMINATION,TEMP_MIN); 
		}
	    //Flag ERROR and save the measured value causing the error for debug	
	    SETBIT(CHARGE_STATUS,ERROR);        
	    #ifdef DEBUG 
	    term_value1 = temp;
	    #endif
	}
            
} 

// Trickle Charge Algorithme
//************************************************************************************

void TRICKLE_charge(void)
{
    unsigned int temp = 0;

	time.sec = 0x00;
  	time.min = 0x00;
  	time.hour = 0x00;
  	time.t_count = 0x3878;
  
    OCR1 = 0x00; 
    TCCR1B = 0x01;					    //counter1 clock prescale =	1  
  
    while	((CHKBIT(CHARGE_STATUS,TRICKLE)) && (!(CHKBIT(CHARGE_STATUS,ERROR))))  
    {    
   	    // if TEMPERATURE within absolute limits 
        temp = Battery(TEMPERATURE); 
        if ((temp <	MIN_TEMP_ABS) || (temp > MAX_TEMP_ABS))
        {
            do						    // set VOLT_TRICKLE (with "soft start")
            {
	            temp = Battery(VOLTAGE);
            	if (temp < VOLT_TRICKLE)
	 	            OCR1++;
                else if (temp > VOLT_TRICKLE)
            	    OCR1--;
        	}while (temp != VOLT_TRICKLE);			    // VOLT_TRICKLE is set	now
        }

	    if (temp > MAX_TEMP_ABS)
		{
		    //Flag max temperature termination and save the limit value
			SETBIT(CHARGE_STATUS,TEMP_MAX);
			#ifdef DEBUG 
  			term_value2 =	MAX_TEMP_ABS;
  			#endif
		}
    	else if (temp < MIN_TEMP_ABS)
		{
		    //Flag min temperature termination and save the limit value
			SETBIT(TERMINATION,TEMP_MIN); 
			#ifdef DEBUG 
		    term_value2 = MIN_TEMP_ABS;
		    #endif
		}

	    //Flag ERROR and save the measured value causing the error for debug	
	    SETBIT(CHARGE_STATUS,ERROR);        
	    #ifdef DEBUG 
	    term_value1 = temp; 				
	    #endif
	}
}