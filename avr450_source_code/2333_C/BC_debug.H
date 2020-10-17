/****************************************************************************
	
   Device      : AT90S2333

   File name   : BC_debug.h

   Ver nr.     : 1.0

   Description : Header file for BC_debug.C
                 
   Compiler    : IAR icca90
   
   Author      : Asmund Saetre
 
   Change log  : 16.05.2000 Made by  AS  
 
****************************************************************************/

#include "io2333.h"
#include <ina90.h>

// Function prototypes
//***************************************************************************
void Inituart(unsigned char);
void Send_int(int);
void Send_char(char);
void Send_state_change(void);
void Send_charge_state(void);
void Send_CR_LF (void);



