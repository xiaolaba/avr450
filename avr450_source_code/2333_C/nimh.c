/****************************************************************************
	
	Device		: AT90S2333

	File name   : NiMH.c

	Ver nr.		: 1.0

	Description : Source file for NiMH charging algorithme
				    - FAST_charge
					- TRICKLE_charge
								 
	Compiler	: IAR icca90
	 
	Author		: Asmund Saetre	/ Terje	Frostad	/ Dietmar Koenig
 
    Change log  : 02.02.2000 Changed to fit Battery Charger refrence	
                  design board	 AS	 
				  17.04.2000 Debugged by AS	
                  15.05.2000 Final test and review AS
 
****************************************************************************/

#include "NiMH.h"	                        //Battery	specific definitions


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
	int	last_min_temp = 0;
	int	last_min_volt = 0;	 
	int	last_min = 0;
	int	temp = 0;
	int	test = 0;
	unsigned char	fast_finish_hour = 0;
	unsigned char	fast_finish_min = 0;

	OCR1 = 0x05; 
	TCCR1B = 0x01;							//counter1 clock prescale	=	1
	
	#ifdef DEBUG
	term_value1=0;
	#endif
	
	time.sec = 0x00;
  	time.min = 0x00;
  	time.hour = 0x00;
  	time.t_count = 0x3878;
	
	temp = Battery(TEMPERATURE);
	last_min_temp = temp;
	last_min_volt = Battery(VOLTAGE);
	// if TEMPERATURE within absolute limits
	if ((temp	<	MIN_TEMP_ABS)	|| (temp > MAX_TEMP_ABS))		
	{
        // if VOLTAGE lower than absolute VOLTAGE
		if (last_min_volt <=	MAX_VOLT_ABS)
		{   
			// if FAST charge TEMPERATURE high enough	 
			if (temp < MIN_TEMP_FAST)
			{
			    // calculate FAST charge finish	time
				fast_finish_min = (time.min + MAX_TIME_FAST);
				fast_finish_hour = time.hour;
				while (fast_finish_min > 60)
				{
					fast_finish_min = fast_finish_min -	60;
					fast_finish_hour++;
				}
				PORTD	&= 0xDF;			// turn	green	LED	on
				
                // Charge with constant	current	algorithme
				while	((CHKBIT(CHARGE_STATUS,FAST))	&& (!(CHKBIT(CHARGE_STATUS,ERROR))))
				{
					do						// set I_FAST (with	"soft start")
					{
					    //Loop until charge current == I_FAST
						temp = Battery(CURRENT);
						if (temp < I_FAST)
						{
							OCR1++;
						}
					    else if	(temp	>	I_FAST)
						{
						    OCR1--;
						}
					}	while	(temp	!= I_FAST);	// I_FAST	is set now
					
                    #ifdef DEBUG
                    /*Save the min and max charge current for debug 
                    information*/					
					if (temp <= charge_current_min)
						charge_current_min = temp;
					if (temp > charge_current_max)
    	   				charge_current_max = temp;           
    	   			#endif	

                    // Check for error and charge termination conditions
					if ((time.hour ==	fast_finish_hour)	&& (time.min ==	fast_finish_min))
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
					if (temp < MAX_TEMP_ABS)
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
					if (temp > MIN_TEMP_FAST)
					{
						/*Stop the PWM, flag min fast charge temperature 
						charge termination and ERROR. Save the termination
						value and the max limit value for debug information*/
						Stop_PWM();
						SETBIT(TERMINATION,TEMP_MIN);
						SETBIT(CHARGE_STATUS,ERROR);
						#ifdef DEBUG
						term_value1 = temp;
						term_value2 = MIN_TEMP_FAST;
						#endif
					}										 
					temp = Battery(VOLTAGE);
					if (temp > MAX_VOLT_ABS)
					{
    					/*Check	if charge voltage are over max charge voltage
    					the last 3 measurement*/
						test++;																			
						if (test>=4)
						{
    						/*Stop the PWM, flag max charge voltage charge 
	    					termination and ERROR. Save the termination value
	    					and the max limit value for debug information*/
			    			Stop_PWM();
				    		SETBIT(TERMINATION,VOLT_MAX);
					    	SETBIT(CHARGE_STATUS,ERROR);
					    	#ifdef DEBUG
						    term_value1 = temp;
				    		term_value2 =	MAX_VOLT_ABS;
				    		#endif
						}
					}
					else
					{
						test = 0;
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
						    term_value2 = last_min_volt;
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

// Trickle Charge	Algorithme
//************************************************************************************

void TRICKLE_charge(void)
{
	unsigned int temp	=	0;
	unsigned char	trickle_finish_hour	=	0;
	unsigned char	trickle_finish_min	=	0;
	
	OCR1 = 0x05; 
	TCCR1B = 0x01;							//counter1 clock prescale = 1	 

	while	((CHKBIT(CHARGE_STATUS,TRICKLE)) &&	(!(CHKBIT(CHARGE_STATUS,ERROR))))	 
	{		 
		temp = Battery(TEMPERATURE); 
		// if TEMPERATURE within absolute limits
		if ((temp <	MIN_TEMP_ABS) || (temp > MAX_TEMP_ABS))
		{
			temp = Battery(VOLTAGE);
			// if VOLTAGE lower	than absolute VOLTAGE
			if (temp < MAX_VOLT_ABS)
			{
				do							// set I_TRICKLE (with "soft start")
				{
					temp = Battery(CURRENT);
    				if (temp < I_TRICKLE)
						OCR1++;
					else if ((temp > I_TRICKLE)&&(OCR1 > 0))
					    OCR1--;
				}while (temp !=	I_TRICKLE); // I_TRICKLE is	set	now

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
}