/****************************************************************************
	
	Device		: AT90S2333

	File name   : NiCd.c

	Ver nr.		: 1.0

	Description : Source file for NiCd charging algorithme
				    - FAST_charge
					- TRICKLE_charge
								 
	Compiler	: IAR icca90
	 
	Author		: Asmund Saetre	/ Terje	Frostad	/ Dietmar Koenig
 
    Change log  : 02.02.2000 Changed to fit Battery Charger refrence	
                  design board	 AS	 
				  17.04.2000 Debugged by AS	
                  15.05.2000 Final test and review AS
 
****************************************************************************/

#include "NiCd.h"							//Battery	specific definitions

// Extern struct prototypes
extern time_struct time;

#ifdef DEBUG                  
extern int term_value1;
extern int term_value2;
extern int charge_current_min;
extern int charge_current_max;
#endif

// Fast	Charge Algorithme
//***************************************************************************
void FAST_charge(void)
{	
	char fast_finish_hour	=	0;
	char trickle_finish_min	=	0;
	int	last_min_temp = 0;
	int	last_min_volt = 0;		
	int	temp = 0;
	int temp2 = 0;
	char last_min	=	0;
	char last_sec	=	0;
	int	top_volt	=	0;
	int	new_top_volt	=	0;
	char i = 0;
	int	last_temp	=	0;

    #ifdef DEBUG                  
	term_value1=0;
	#endif
	
	time.sec = 0x00;
  	time.min = 0x00;
  	time.hour = 0x00;
  	time.t_count = 0x3878;
	OCR1 = 0x00; 
    TCCR1B = 0x01;              // Counter 1 clock prescale = 1
	
	temp = Battery(TEMPERATURE); 
	// if	TEMPERATURE	within absolute	limits
	if ((temp	<	MIN_TEMP_ABS) &&	(temp	>	MAX_TEMP_ABS))
	{
		temp2 = Battery(VOLTAGE);
		// if	VOLTAGE	lower than absolute VOLTAGE
		if (temp2 < MAX_VOLT_ABS)           
		{
    		// if	FAST charge	TEMPERATURE	high enough		
			if (temp < MIN_TEMP_FAST)				
			{
				OCR1 = 0x00; 
				TCCR1B = 0x01;              // Counter 1 clock prescale = 1
				last_min_temp = Battery(TEMPERATURE);
				last_min_volt = Battery(VOLTAGE); 
			
				//Calculate FAST charge	finish time
				trickle_finish_min = (time.min + MAX_TIME_FAST);
				fast_finish_hour = time.hour;
				while	(trickle_finish_min	>	60)
				{
					trickle_finish_min = trickle_finish_min	-	60;
					fast_finish_hour++;
				}
				 
				while	((CHKBIT(CHARGE_STATUS,FAST))	&& (!(CHKBIT(CHARGE_STATUS,ERROR))))
				{
					do                      //Set I_FAST with "soft start"
					{	
                        //Loop until charge current == I_FAST
						temp = Battery(CURRENT);
						if ((temp	<	(I_FAST+1))&&(OCR1 < 0xFF))
						{
							OCR1++;
						}
						else if	((temp > (I_FAST-1))&&(OCR1	>	0x00))
						{
							OCR1--;
						}
					}while (temp !=	I_FAST); // I_FAST	is set now

                    #ifdef DEBUG
                    /*Save the min and max charge current for debug 
                    information*/					
					if (temp <= charge_current_min)
						charge_current_min = temp;
					if (temp > charge_current_max)
    	   				charge_current_max = temp;           
    	   			#endif	
	
						
					if ((time.hour	== fast_finish_hour)&&(time.min	== trickle_finish_min))
					{
						/*Stop the PWM, flag max time charge termination and 
						ERROR. Save the termination value and the max limit 
						value for debug information*/
						Stop_PWM();
						SETBIT(TERMINATION,TIME_MAX);
						SETBIT(CHARGE_STATUS,ERROR);
			            #ifdef DEBUG                  
						term_value1 = time.min;
						term_value2 = trickle_finish_min;
						#endif
					}
					temp = Battery(TEMPERATURE);
					if ( temp	<	MAX_TEMP_ABS)
					{
						/*Stop the PWM, flag max temperature charge 
						termination and ERROR. Save the termination value and
						the max limit value for debug information*/
						Stop_PWM();
						SETBIT(TERMINATION,TEMP_MAX);
						SETBIT(CHARGE_STATUS,ERROR);
						#ifdef DEBUG                  
						term_value1 = temp;
						term_value2 =	MAX_TEMP_ABS;
						#endif
					}
					temp = Battery(VOLTAGE);
					if (temp > MAX_VOLT_ABS)
					{
						/*Stop the PWM, flag max charge voltage charge 
						termination and ERROR. Save the termination value and
						the max limit value for debug information*/
						Stop_PWM();
						SETBIT(TERMINATION,VOLT_MAX);
						SETBIT(CHARGE_STATUS,ERROR);
						#ifdef DEBUG                  
						term_value1 = temp;
						term_value2 =	MAX_VOLT_ABS;
						#endif
					}								
					if (time.min !=	last_min)												 
					{
						last_min = time.min;

						//If charge voltage is falling,	change to trickle mode
						temp = Battery(VOLTAGE);
						if ((last_min_volt - temp) > NEG_dV)
						{
	        				/*Stop the PWM, flag FAST charge dV/dt
    	    				termination and change charge mode to 
    	    				"TRICKLE". Save the termination value and the
    	    				max limit value for debug information*/
							Stop_PWM();
							SETBIT(TERMINATION,dV_dt);
							CLRBIT(CHARGE_STATUS,FAST);
							SETBIT(CHARGE_STATUS,TRICKLE);
							#ifdef DEBUG                  
							term_value1 = temp;
					    	term_value2 =	top_volt;
					    	#endif
						}
						//Store	current	charge voltage to next minute test
						last_min_volt = temp;
						
						/*If Battery temperature rises more than max dT/dt
						change to trickle mode*/
						temp = Battery(TEMPERATURE);
						if ((last_min_temp - temp) >= ntc_c[((temp-400)/25)])
						{																						
	    					/*Stop the PWM, flag FAST charge dT/dt
	    			    	termination and change charge mode to "TRICKLE".
	    					Save the termination value and the max limit 
		    				value for debug information*/
							Stop_PWM();
							SETBIT(TERMINATION,dT_dt);
							CLRBIT(CHARGE_STATUS,FAST);
							SETBIT(CHARGE_STATUS,TRICKLE);
							#ifdef DEBUG
							term_value1 = (last_min_temp -temp);
							term_value2 = ntc_c[((temp-400)/25)];
							#endif
						}						
						//Store	current	Battery	temp to next minute test		 
						last_min_temp = temp; 
					}
				}
			}	
    		else if(!(CHKBIT(CHARGE_STATUS,ERROR)))
	    	{
    	    	/*Flag min temperature termination and ERROR. Save 
    		   	the termination value and the max limit value for debug 
    		    information*/
    			SETBIT(TERMINATION,TEMP_MIN);
    			SETBIT(CHARGE_STATUS,ERROR);
                #ifdef DEBUG
		    	term_value1 = temp;
			   	term_value2 = MIN_TEMP_FAST;
				#endif			   	
			}		 

		}
   		else if(!(CHKBIT(CHARGE_STATUS,ERROR)))
		{
    		/*Flag max charge voltage charge termination and ERROR. Save 
    		the termination value and the max limit value for debug 
    		information*/
			SETBIT(TERMINATION,VOLT_MAX);
			SETBIT(CHARGE_STATUS,ERROR);
			#ifdef DEBUG
			term_value1 = temp;
			term_value2 = MAX_VOLT_ABS;
			#endif
		}		 
	}		 
	else if(!(CHKBIT(CHARGE_STATUS,ERROR)))
	{
		if	(temp	<	MIN_TEMP_ABS)
		{
		    //Flag min temperature termination and save the limit value
		    #ifdef DEBUG
		    term_value2 =	MIN_TEMP_ABS;
		    #endif
			SETBIT(TERMINATION,TEMP_MIN); 
		}
		else
		{
		    //Flag max temperature termination and save the limit value
			SETBIT(CHARGE_STATUS,TEMP_MAX);
			#ifdef DEBUG
			term_value2 =	MAX_TEMP_ABS;
			#endif
		}
    	//Flag ERROR and save the measured value causing the error for debug	
    	SETBIT(CHARGE_STATUS,ERROR);        
    	#ifdef DEBUG
	    term_value1 = temp; 				
	    #endif
    }
}
// Trickle Charge	Algorithm
//***************************************************************************
void TRICKLE_charge(void)
{
	int	temp = 0;

	time.sec = 0x00;
  	time.min = 0x00;
  	time.hour = 0x00;
  	time.t_count = 0x3878;

	OCR1 = 0x00; 
    TCCR1B = 0x01;              // Counter 1 clock prescale = 1

    #ifdef DEBUG
	term_value1=0;
	#endif

	while	((CHKBIT(CHARGE_STATUS,TRICKLE)) &&	(!(CHKBIT(CHARGE_STATUS,ERROR))))	 
	{		 
		temp = Battery(TEMPERATURE); 
		// if TEMPERATURE within absolute limits
		if ((temp	<	MIN_TEMP_ABS) ||	(temp	>	MAX_TEMP_ABS))
		{
			temp = Battery(VOLTAGE);
			// if VOLTAGE lower	than absolute VOLTAGE
			if (temp < MAX_VOLT_ABS)
			{
				do	                  // set I_TRICKLE (with "soft start")
				{
				    //Loop until charge current == I_TRICKLE
					temp = Battery(CURRENT);
					if ((temp	<	I_TRICKLE) &&	(OCR1	<	0xFF))
					{
						OCR1++;
					}
					else if	((temp > I_TRICKLE)	&& (OCR1 > 0x00))
					{
						OCR1--;
					}
				}	while (temp	!= I_TRICKLE);

                #ifdef DEBUG
                /*Save the min and max charge current for debug information*/
				if (temp <= charge_current_min)
					charge_current_min = temp; 
				if (temp > charge_current_max)
	   			charge_current_max = temp;
	   			#endif

			}
			else if(!(CHKBIT(CHARGE_STATUS,ERROR)))
			{
    			/*Stop PWM, flag max charge voltage charge termination and 
    			ERROR. Save the termination value and the max limit value for
    			debug information*/
				Stop_PWM();
				SETBIT(TERMINATION,VOLT_MAX);
				SETBIT(CHARGE_STATUS,ERROR);
				#ifdef DEBUG
				term_value1 = temp;
				term_value2 = MAX_VOLT_ABS;
				#endif
			}
		}
		else if(!(CHKBIT(CHARGE_STATUS,ERROR)))
		{
   			/*Stop PWM, flag min max temperature charge termination and ERROR
   			Save the termination value and the limit value for debug 
   			information*/
			Stop_PWM();
			if (temp < MIN_TEMP_ABS)
			{
				SETBIT(TERMINATION,TEMP_MIN);
				#ifdef DEBUG
				term_value2 = MIN_TEMP_ABS;
				#endif
			}
			else
			{
				SETBIT(TERMINATION,TEMP_MAX);
				#ifdef DEBUG
				term_value2 = MAX_TEMP_ABS;
				#endif
			}
			SETBIT(CHARGE_STATUS,ERROR);
			#ifdef DEBUG
			term_value1 = temp;
			#endif
		}
	}
	Stop_PWM();			// stop	PWM
}
