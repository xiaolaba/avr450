/****************************************************************************
    
    Device      : AT90S2333

    File name   : LiIon.c
    
    Ver nr.     : 1.0

    Description : Source file for LiIon charging algorithme
                    - FAST_charge
                    - TRICKLE_charge
                  
    Compiler    : IAR icca90
   
    Author      : Asmund Saetre / Terje Frostad / Dietmar Koenig
 
    Change log  : 02.02.2000 Changed to fit Battery Charger refrence    
                  design board   AS  
                  17.04.2000 Debugged by AS 
                  16.05.2000 Final test and review AS
 
*************************************************************************************/
#include "LiIon.h"                      //Battery specific definitions

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
    unsigned int temp = 0;
    unsigned char fast_finish_hour = 0;
    unsigned char fast_finish_min = 0;
    unsigned char delay_hour = 0;  
    unsigned char delay_min = 0;
    unsigned char last_min = 0;

    #ifdef DEBUG 
	term_value1=0;
	#endif
	
	time.sec = 0x00;
  	time.min = 0x00;
  	time.hour = 0x00;
  	time.t_count = 0x3878;


    SETBIT(CHARGE_STATUS,CONST_C);
    CLRBIT(CHARGE_STATUS,DELAY); 
    
    // if TEMPERATURE within absolute limits
    temp = Battery(TEMPERATURE); 
    if ((temp < MIN_TEMP_ABS) && (temp > MAX_TEMP_ABS))
    {
        // if VOLTAGE lower than absolute VOLTAGE
        temp = Battery(VOLTAGE);    
        if (temp <= (MAX_VOLT_ABS + VOLT_TOLERANCE))
        {
            // if FAST charge TEMPERATURE high enough
            temp = Battery(TEMPERATURE); 
            if (temp < MIN_TEMP_FAST)
            {
                OCR1 = 0x0; 
                TCCR1B = 0x01;              //counter1 clock prescale = 1

                // calculate FAST charge finish time
                fast_finish_min = (time.min + MAX_TIME_FAST);
                fast_finish_hour = time.hour;
                while (fast_finish_min > 60)
                {
                    fast_finish_min = fast_finish_min - 60;
                    fast_finish_hour++;
                }
                while ((CHKBIT(CHARGE_STATUS,FAST)) && (!(CHKBIT(CHARGE_STATUS,ERROR))))
                {
                    // Charge with constant current algorithme
                    if (CHKBIT(CHARGE_STATUS,CONST_C))
                    {
                        // set I_FAST (with "soft start")
                        do
                        {
                            temp = Battery(CURRENT);
                            if ((temp < I_FAST)&&(OCR1 < 0xFF))
                                OCR1++;
                            if ((temp > I_FAST)&&(OCR1 > 0x00))
                                OCR1--;
                        }while (temp != I_FAST);        // I_FAST is set now

                        #ifdef DEBUG
                        /*Save the min and max charge current for debug 
                        information*/					
			    		if (temp <= charge_current_min)
				    		charge_current_min = temp;
					    if (temp > charge_current_max)
        	   				charge_current_max = temp;           
        	   			#endif	
                 
                        /*if VOLTAGE within range change from constant 
                        CURRENT charge mode to constant VOLTAGE charge mode*/
                        temp = Battery(VOLTAGE_WITH_PWM_TURNOFF);            
                        if ((temp >= (VOLT_TRICKLE - VOLT_TOLERANCE)) && (temp <= (VOLT_TRICKLE + VOLT_TOLERANCE)))
                        {
                            CLRBIT(CHARGE_STATUS,CONST_C);
                            SETBIT(CHARGE_STATUS,CONST_V);
                        }            
                    }    

                    // Charge with constant voltage algorithme
                    else if (CHKBIT(CHARGE_STATUS,CONST_V))
                    {       
                        // set VOLT_FAST (with "soft start")
                        do                                              
                        {
                            temp = Battery(VOLTAGE);
                            if ((temp < VOLT_FAST)&&(OCR1 < 0xFF))
                                OCR1++;
                            else if ((temp > VOLT_FAST)&&(OCR1 > 0x00))
                                OCR1--;
                        }while ((temp <= (VOLT_FAST -(VOLT_TOLERANCE/4)))||(temp >= (VOLT_FAST+(VOLT_TOLERANCE/4))));                       
                        // VOLT_TRICKLE is set now
                    }

                    // Check for error and charge termination conditions
                    
                    //If above max charge time, flag error    
                    if ((time.hour == fast_finish_hour) && (time.min == fast_finish_min))                               
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

                    //If above max charge temperature, flag error    
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
                    //If beyond min charge temperature, flag error    					
                    if (temp > MIN_TEMP_FAST)
					{
						/*Stop the PWM, flag min temperature charge 
						termination and ERROR. Save the termination value and
						the max limit value for debug information*/
						Stop_PWM();
						SETBIT(TERMINATION,TEMP_MIN);
						SETBIT(CHARGE_STATUS,ERROR);
						#ifdef DEBUG 
						term_value1 = temp;
						term_value2 =	MIN_TEMP_ABS;
						#endif
					}
                    /*Every min check if MIN_I_FAST is reached, if so 
                    calculate the delay time. 
                    Check every 60 seconds if delay time after reached 
                    MIN_I_FAST is over, if so change to trickle charge*/
                    if (time.min != last_min)   
                    {
                        last_min = time.min;
                        if (((CHKBIT(CHARGE_STATUS,CONST_V)) && (!(CHKBIT(CHARGE_STATUS,DELAY))) && (Battery(CURRENT) <= MIN_I_FAST)))
                        {
                            // calculate DELAY finish time
                            delay_min = (time.min + FAST_TIME_DELAY);
                            delay_hour = time.hour;
                            while (delay_min > 60)
                            {
                                delay_min = delay_min - 60;
                                delay_hour++;
                            }            
                            SETBIT(CHARGE_STATUS,DELAY);  
                        }

                        // Check if delay time after min_I_FAST is done, if so change to trickle charge
                        if ((time.hour == delay_hour)&&(time.min == delay_min)&&(CHKBIT(CHARGE_STATUS,DELAY)))
                        {
                            /*Stop PWM and change from constant VOLTAGE charge 
                            mode back to constant CURRENT charge mode. Change 
                            charge mode from "FAST" to "TRICKLE" Save the 
                            termination value and the max limit value for debug
                            information*/
						    Stop_PWM();
                            SETBIT(TERMINATION,DELAY);
                            CLRBIT(CHARGE_STATUS,CONST_V);
                            SETBIT(CHARGE_STATUS,CONST_C);
                            CLRBIT(CHARGE_STATUS,FAST);               
                            SETBIT(CHARGE_STATUS,TRICKLE);            
                            #ifdef DEBUG 
  						    term_value1 = time.min;
						    term_value2 = delay_min;
						    #endif
                        }                         
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
	    if (temp < MIN_TEMP_ABS)
		{
		    //Flag min temperature termination and save the limit value
		    #ifdef DEBUG 
		    term_value2 = MIN_TEMP_ABS;
		    #endif
			SETBIT(TERMINATION,TEMP_MIN); 
		}
		else
		{
		    //Flag max temperature termination and save the limit value
			SETBIT(CHARGE_STATUS,TEMP_MAX);
			#ifdef DEBUG 
			term_value2 = MAX_TEMP_ABS;
			#endif
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
    unsigned char trickle_finish_min = 0;
    unsigned char trickle_finish_hour = 0;
      
	time.sec = 0x00;
  	time.min = 0x00;
  	time.hour = 0x00;
  	time.t_count = 0x3878;


    OCR1 = 0x00; 
    TCCR1B = 0x01;                            //counter1 clock prescale = 1  
  
    #ifdef DEBUG 
    term_value1=0;
    #endif   
     
    while ((CHKBIT(CHARGE_STATUS,TRICKLE)) && (!(CHKBIT(CHARGE_STATUS,ERROR))))  
    {    
        // if TEMPERATURE within absolute limits
        temp = Battery(TEMPERATURE); 
        if ((temp < MIN_TEMP_ABS) || (temp > MAX_TEMP_ABS))
        {
             // if charge voltage lower than absolute max charge voltage
            if (Battery(VOLTAGE) <= (VOLT_TRICKLE + VOLT_TOLERANCE))
            {
                //Charge with constant current algorithme
                if (CHKBIT(CHARGE_STATUS,CONST_C))
                {
                    // set I_TRICKLE (with "soft start")
                    do
                    {
                        temp = Battery(CURRENT);
                        if ((temp < I_TRICKLE)&&(OCR1 < 0xFF))
                        {
                            OCR1++;
                        }
                        if ((temp > I_TRICKLE)&&(OCR1 > 0x00))
                        {
                            OCR1--;
                        }
                    }while (temp != I_TRICKLE); // I_TRICKLE is set now
               
                    #ifdef DEBUG
                    /*Save the min and max charge current for debug 
                    information*/					
					if (temp <= charge_current_min)
						charge_current_min = temp;
					if (temp > charge_current_max)
    	   				charge_current_max = temp;           
    	   			#endif	
                    
                    /*if VOLTAGE within range change from constant 
                    CURRENT charge mode to constant VOLTAGE charge mode*/
                    temp = Battery(VOLTAGE_WITH_PWM_TURNOFF);            
                    if ((temp >= (VOLT_TRICKLE - VOLT_TOLERANCE)) && (temp <= (VOLT_TRICKLE + VOLT_TOLERANCE)))
                    {
                        CLRBIT(CHARGE_STATUS,CONST_C);
                        SETBIT(CHARGE_STATUS,CONST_V);
                    }            
                }
                
                //Charge with constant current algorithme                 
                if (CHKBIT(CHARGE_STATUS,CONST_V))
                {  
                    // set VOLT_TRICKLE (with "soft start")    
                    do                      // set VOLT_TRICKLE
                    {
                        temp = Battery(VOLTAGE);
                        if ((temp < VOLT_TRICKLE)&&(OCR1 < 0xFF))
                        {
                            OCR1++;
                        }
                        if ((temp > VOLT_TRICKLE)&&(OCR1 > 0x00))
                        {
                            OCR1--;
                        }
                    }while ((temp <= (VOLT_TRICKLE-(VOLT_TOLERANCE/4)))||(temp >= (VOLT_TRICKLE+(VOLT_TOLERANCE/4))));                       
                    // VOLT_TRICKLE is set now          }
                }


                // Check for error and charge termination conditions
                if ((time.hour == trickle_finish_hour) && (time.min == trickle_finish_min))                               
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
					term_value2 = MAX_TEMP_ABS;
					#endif
				}
                if (temp > MIN_TEMP_FAST)
				{
				    /*Stop the PWM, flag min temperature charge 
					termination and ERROR. Save the termination value and
					the max limit value for debug information*/
					Stop_PWM();
					SETBIT(TERMINATION,TEMP_MIN);
					SETBIT(CHARGE_STATUS,ERROR);
					#ifdef DEBUG 
					term_value1 = temp;
					term_value2 =	MIN_TEMP_ABS;
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
	        if (temp < MIN_TEMP_ABS)
    		{
	    	    //Flag min temperature termination and save the limit value
	    	    #ifdef DEBUG 
		        term_value2 = MIN_TEMP_ABS;
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
}