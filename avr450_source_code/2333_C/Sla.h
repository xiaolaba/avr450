/****************************************************************************
	
   	Device      : AT90S2333

   	File name   : SLA.h

   	Ver nr.     : 1.0

   	Description : Header file for SLA.c containing the battery 
   				  specific definitions
   
   	Compiler    : IAR icca90
   
   	Author      : Asmund Saetre / Terje Frostad / Dietmar Koenig
 
  	Change log	: 02.02.2000 Changed to fit Battery Charger refrence design 
   				  board   AS
   				  18.05.2000 Final test and review AS  
 
****************************************************************************/

// Include Files
//***************************************************************************
#include <ina90.h>
#include <math.h>
#include "BC.H"
#include "B_Def.H"

// Function prototypes
//***************************************************************************
void FAST_charge(void);
void TRICKLE_charge(void);

// Battery Characteristics: General charge TERMINATION
//***************************************************************************
// Absolute minimum TEMPERATURE = 5C
#define  MIN_TEMP_ABS   0x02C7
// Absolute maximum TEMPERATURE = 40C
#define  MAX_TEMP_ABS   0x0175
     
// Battery Characteristics: FAST charge TERMINATION 
//***************************************************************************
//  Minimum CURRENT threshold = 0.2C
#define  MIN_I_FAST   (unsigned int)((CAPACITY * 0.2)/CURRENT_STEP)  
    
// Battery Characteristics: FAST charge ERROR
//***************************************************************************
// Maximum FAST charge TEMPERATURE = 30C 
#define  MAX_TEMP_FAST   0x01D0
// Maximum FAST Charge Time = 1C = 60min at 1.5 C CURRENT
#define  MAX_TIME_FAST   0x3C
// Maximum FAST Charge CURRENT = 2 C -10mA
#define  MAX_I_FAST   (unsigned int)(((CAPACITY * 2)-10)/ CURRENT_STEP)   
          			
// Battery Characteristics: Charge conditions
//***************************************************************************
// FAST charge CURRENT stop is 10mA under max_I_FAST	
#define	 I_BUFFER    (0xA/CURRENT_STEP)
// Fixed FAST Charge VOLTAGE = CELLS * 2450 mV
#define  VOLT_FAST      (unsigned int)((CELLS * 2450) / VOLTAGE_STEP)   
// Fixed TRICKLE Charge VOLTAGE = CELLS * 2250 mV
#define  VOLT_TRICKLE   (unsigned int)((CELLS * 2250) / VOLTAGE_STEP)