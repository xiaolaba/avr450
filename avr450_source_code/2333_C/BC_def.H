/****************************************************************************
	
    Device      : AT90S2333

    File name   : BC_def.h

    Ver nr.		: 1.0

    Description : This file	has	to be edited before	compiling and contain
                  the battery specifications.
                  - Battery Type : Define the battery type to be charged
            	  - CELLS : Number of	cells in the battery pack
    	          - VOLTAGE_STEP : ADC step VOLTAGE according to ADC input
    	          resistors	at voltage measurment
            	  - CURRENT_STEP : ADC step VOLTAGE according to resistors	
				  at current measurment
                  - CAPACITY : Define the battery capasity
            	  - Cell_VOLTAGE : Define the cell voltage for LiIon 
            	  battery
        		  - NTC table : Number of	ADC	steps according	to 0.5 
        		  celcius at the NTC measurement. Starts at	step 400 and
        		  ends at step 675 Se further details in the documentation
								 
    Compiler    : IAR icca90
	 
	 Author	    : Asmund Saetre / Terje Frostad / Dietmar Koenig
 
	 Change	log	: 02.02.2000	Changed	to fit Battery Charger refrence	
	              design board	 AS	 
				  17.04.2000	Debugged by	AS
 
****************************************************************************/

// Battery Type
//***************************************************************************

#define	SLA					    // Sealed	Lead-Acid battery
//#define	NiCd    			  // Nickel-Cadmium	battery
//#define	NiMH				  // Nickel-Metal-Hydride battery
//#define	LiIon				  // Litium-Ion	battery	

// Number	of CELLS 
//***************************************************************************

#define	CELLS			1       	// Number of cells in the battery pack


// ADC step
//***************************************************************************
// ADC voltage step	according to resistors at ADC voltage measurement input
#define	VOLTAGE_STEP	11.8	 
// ADC current step	according to resistors on ADC current measurment input   
#define	CURRENT_STEP	1.95		
					 
//CAPACITY of	battery	pack
//***************************************************************************
#ifdef NiCd
#define	CAPACITY		750	        // Battery pack	capasity in	mAh	 (NiCd)
#endif

#ifdef NiMH
#define	CAPACITY		1100		// Battery pack	capasity in	mAh (NiMh)
#endif

#ifdef LiIon
#define	CAPACITY		1350	    // Battery pack	capasity in	mAh	 (LiIon)
#endif

#ifdef SLA
#define	CAPACITY		1200	    // Battery pack	capasity in	mAh	 (SLA)
#endif

// Cell	VOLTAGE	for	LiIon	Battery
//***************************************************************************

//#define	LiIon_CELL_VOLT		4100	// Change	cell voltage between 4100	mV 
//#define	LiIon_CELL_VOLT		4200	// and 4200	mV according to	the	battery	
#define	LiIon_CELL_VOLT		3600	// manufacturer's specifications

// NTC table
//***************************************************************************

flash	char ntc_c[12]={5,4,6,4,5,5,5,5,5,6,4,5};
/*Number	of ADC steps according to 0.5 celcius at the NTC measurement. 
Starts at step 400 and ends at step 675. Se further details in the 
documentation*/