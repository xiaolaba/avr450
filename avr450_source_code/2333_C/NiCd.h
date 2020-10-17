/****************************************************************************
	
    Device      : AT90S2333

    File name   : NiCd.h

    Ver nr.     : 1.0

    Description : Header file for NiCd.c containing the battery specific 
                  definitions
                 
    Compiler    : IAR icca90
   
    Author      : Asmund Saetre / Terje Frostad / Dietmar Koenig
 
    Change log  : 02.02.2000 Changed to fit Battery Charger refrence 
                  design board   AS  
                  16.05.2000 Final test and review AS
 
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
// Absolute Maximum Charge VOLTAGE = CELLS * 1600 mV, 
#define  MAX_VOLT_ABS   (int)((CELLS * 1600)/VOLTAGE_STEP)

// Battery Characteristics: FAST charge TERMINATION       
//***************************************************************************
// -dV = 20mV per cell,rounded up to closest one 
#define  NEG_dV      (int)(ceil((CELLS * 0x14)/VOLTAGE_STEP))

// Battery Characteristics: FAST charge ERROR
//***************************************************************************  
// Minimum FAST Charge TEMPERATURE 10C
#define  MIN_TEMP_FAST  0x0296
//Maximum FAST Charge Time = 1.5C = 90min at 1C CURRENT
#define  MAX_TIME_FAST  0x5A
	

// Battery Characteristics: Charge conditions
//***************************************************************************  
//FAST Charge CURRENT = 1C 
#define  I_FAST      (int)(CAPACITY /CURRENT_STEP)           
//TRICKLE Charge CURRENT = 0.025C 	
#define  I_TRICKLE   (int)((CAPACITY * 0.025)/CURRENT_STEP)  


