/*********************************** MODULE INFO ************************************
	
   Device      : AT90S2333

   File name   : BC.h

   Ver nr.     : 1.0

   Description : Header file for BC.C
                 
   Compiler    : IAR icca90
   
   Author      : Asmund Saetre / Terje Frostad / Dietmar Koenig
 
   Change log  : 02.02.2000 Changed to fit Battery Charger refrence design board   AS  
 
*************************************************************************************/

#include "io2333.h"
#include <ina90.h>
#include "BC_debug.H"

// Function prototypes
//************************************************************************************
int  Battery(unsigned char);
void Setup(void);
void Stop_PWM(void);
void Stable_ADC(void);
void interrupt[TIMER1_OVF_vect] T1Ovf(void);

// Extern function prototypes
//************************************************************************************
extern void FAST_charge(void);
extern void TRICKLE_charge(void);

// Struct definition
//***************************************************************************
typedef struct
{
    int t_count;
    char sec;                    // global seconds
    char min;                    // global minutes
    char hour;                   // global hour
}time_struct;

// Bit handling macros
//************************************************************************************
#define SETBIT(x,y) (x |= (y))  // Set bit y in byte x
#define CLRBIT(x,y) (x &= (~(y)))// Clear bit y in byte x
#define CHKBIT(x,y) (x & (y))   // Check bit y in byte x


// Charge Status bit mask constants 
//************************************************************************************
#define FAST    0x01   			// bit 0 : FAST charge status bit
#define TRICKLE 0x02   			// bit 1 : TRICKLE charge status bit
#define ERROR   0x04   			// bit 2 : ERROR before or while charging
#define CONST_V 0x08   			// bit 3 : charged with constant VOLTAGE
#define CONST_C 0x10   			// bit 4 : charged with constant CURRENT
#define DELAY   0x20   			// bit 5 : FAST charge DELAY for LiIon after CURRENT threshold detection
#define READY   0x40   			// bit 6 : Trickle charge is terminated and the battery is fully charged
#define FREE2   0x80   			// bit 7 : Not Currently used


// TERMINATION bit mask constants 
//************************************************************************************
#define TEMP_MIN   	0x01   		// bit 0 : minimum TEMPERATURE overflow
#define TEMP_MAX   	0x02   		// bit 1 : maximum TEMPERATURE overflow
#define I_MIN   	0x04   		// bit 2 : minimum CURRENT overflow
#define I_MAX   	0x08   		// bit 3 : maximum CURRENT overflow
#define TIME_MAX   	0x10   		// bit 4 : maximum time overflow
#define VOLT_MAX   	0x20   		// bit 5 : maximum VOLTAGE overflow
#define dV_dt   	0x40   		// bit 6 : dV/dt overflow
#define dT_dt   	0x80   		// bit 7 : dT/dt overflow

// PORT Connections 
//************************************************************************************
#define IBAT2   4     			//CURRENT measurement on ADC channel #4
#define VBAT2   5     			//VOLTAGE measurement on ADC channel #5
#define TBAT2   3     			//TEMPERATURE measurement on ADC channel #3

#define SW0     4     			//Switch0 -> pin 4 on portD
#define SW1     5     			//Switch2 -> pin 5 on portD
#define SW2     6     			//Switch3 -> pin 6 on portD
#define SW3     7     			//Switch4 -> pin 7 on portD

#define LED0    0x01     		//LED0 -> pin 0 on portB
#define LED1    0x04     		//LED1 -> pin 2 on portB
#define LED2    0x08     		//LED2 -> pin 3 on portB
#define LED3    0x10     		//Led3 -> pin 4 on portB

// ADC measurement definitions
//********************************************************************************************

#define TEMPERATURE               5
#define VOLTAGE                   3
#define VOLTAGE_WITH_PWM_TURNOFF  33
#define CURRENT                   1


// Globale Variables
//************************************************************************************
/*Global variables are placed in unused I/O registers to reduce code size and
SRAM requirements, replace with above declaration if I/O registers are used*/

#define TERMINATION EEDR                    
#define CHARGE_STATUS EEAR                  
                         

