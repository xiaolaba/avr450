/****************************************************************************
	
    Device      : AT90S2333

    File name   : LiIon.h

    Ver nr.     : 1.0

    Description : Header file for LiIon.c containing the battery specific 
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
#include "BC_def.H"

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
// Absolute Maximum Charge VOLTAGE = CELLS * cell VOLTAGE * scale factor
#define  MAX_VOLT_ABS    (unsigned int)((CELLS * LiIon_CELL_VOLT)/ VOLTAGE_STEP)  
     
// Battery Characteristics:FAST charge TERMINATION     
//***************************************************************************
// Minimum CURRENT threshold = 50mA per cell
#define  MIN_I_FAST  (unsigned int)((CELLS * 50)/ CURRENT_STEP)
// time DELAY after "MIN_I_FAST" is reached = 30min
#define  FAST_TIME_DELAY     0x1E

// Battery Characteristics:TRICKLE charge TERMINATION     
//***************************************************************************
// Maximum TRICKLE Charge Time = 1.5C = 90min    					
#define  MAX_TIME_TRICKLE  0x5A
  
// Battery Characteristics: FAST charge ERROR
//***************************************************************************
// Minimum FAST Charge TEMPERATURE 10C
#define  MIN_TEMP_FAST  0x0296          	
// Maximum FAST Charge Time = 1.5C = 90 min at 1C CURRENT  
#define  MAX_TIME_FAST  0x5A               
        
// Battery Characteristics: General Charge conditions
//***************************************************************************
// VOLTAGE tolerance = CELLS * 50mV * scale factor
#define  VOLT_TOLERANCE (unsigned int)((CELLS * 50)/ VOLTAGE_STEP)            
// FAST Charge CURRENT = 1C (in mA, with 1.966mA per bit)
#define  I_FAST      (unsigned int)(CAPACITY / CURRENT_STEP)               
// TRICKLE Charge CURRENT = 0.025C (in mA, with 1.966mA per bit)
#define  I_TRICKLE   (unsigned int)((CAPACITY * 0.025)/ CURRENT_STEP)
//FAST Charge voltage is defined in bc_def.h
#define  VOLT_FAST LiIon_CELL_VOLT
/* TRICKLE Charge voltage is defined in bc_def.h 
CURRENT = 0.025C (in mA, with 1.966mA per bit)*/
#define  VOLT_TRICKLE  LiIon_CELL_VOLT


