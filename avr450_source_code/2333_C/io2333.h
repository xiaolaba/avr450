/*            - io2333.h -

   This file #defines the internal register addresses for AT90S2333.
   Used with ICCA90 and AA90.

   File version:   $Revision: 1.3 $

*/

#pragma language=extended

#if (((__TID__ >> 8) & 0x7F) !=  90)
#error This file should only be compiled by icca90/aa90
#endif
#if (((__TID__ >> 4) & 0x0F) !=  1)
#error This file should only be included if the processor option is -v0
#endif

/* Include the SFR part if this file has not been included before,
 * OR this file is included by the assembler (SFRs must be defined in
 * each assembler module). AA90 has __TID__ bit 15 = 0. */
#if !defined(__IO2333_INCLUDED) || ((__TID__ >> 15) == 0)

/* =========================*/
/* Predefined SFR Addresses */
/* =========================*/

/* UART Baud Rate Register high */
sfrb UBRRH     = 0x03;

/* ADC Data register */
sfrw ADC       = 0x04; 
sfrb ADCL      = 0x04;
sfrb ADCH      = 0x05;

/* ADC Control and Status Register */
sfrb ADCSR     = 0x06;

/* ADC MUX */
sfrb ADMUX     = 0x07;

/* Analog Comparator Control and Status Register */
sfrb ACSR      = 0x08;

/* UART Baud Rate Register */
sfrb UBRR      = 0x09;
sfrb UBRRL     = 0x09;

/* UART Control Register */
sfrb UCR       = 0x0A;

/* UART Status Register */
sfrb USR       = 0x0B;

/* UART I/O Data Register */
sfrb UDR       = 0x0C;

/* SPI Control Register */
sfrb SPCR      = 0x0D;

/* SPI Status Register */
sfrb SPSR      = 0x0E;

/* SPI I/O Data Register */
sfrb SPDR      = 0x0F;

/* Input Pins, Port D */
sfrb PIND      = 0x10;

/* Data Direction Register, Port D */
sfrb DDRD      = 0x11;

/* Data Register, Port D */
sfrb PORTD     = 0x12;

/* Input Pins, Port C */
sfrb PINC      = 0x13;

/* Data Direction Register, Port C */
sfrb DDRC      = 0x14;

/* Data Register, Port C */
sfrb PORTC     = 0x15;

/* Input Pins, Port B */
sfrb PINB      = 0x16;

/* Data Direction Register, Port B */
sfrb DDRB      = 0x17;

/* Data Register, Port B */
sfrb PORTB     = 0x18;

/* EEPROM Control Register */
sfrb EECR      = 0x1C;

/* EEPROM Data Register */
sfrb EEDR      = 0x1D;

/* EEPROM Address Register */
sfrw EEAR      = 0x1E;
sfrb EEARL     = 0x1E;
sfrb EEARH     = 0x1F;

/* Watchdog Timer Control Register */
sfrb WDTCR     = 0x21;

/* T/C 1 Input Capture Register */
sfrw ICR1      = 0x26;
sfrb ICR1L     = 0x26;
sfrb ICR1H     = 0x27;

/* Timer/Counter1 Output Compare Register A */
sfrw OCR1      = 0x2A;
sfrb OCR1L     = 0x2A;
sfrb OCR1H     = 0x2B;

/* Timer/Counter 1 */
sfrw TCNT1     = 0x2C;
sfrb TCNT1L    = 0x2C;
sfrb TCNT1H    = 0x2D;

/* Timer/Counter 1 Control and Status Register */
sfrb TCCR1B    = 0x2E;

/* Timer/Counter 1 Control Register */
sfrb TCCR1A    = 0x2F;

/* Timer/Counter 0 */
sfrb TCNT0     = 0x32;

/* Timer/Counter 0 Control Register */
sfrb TCCR0     = 0x33;

/* MCU general Status Register */
sfrb MCUSR     = 0x34;

/* MCU general Control Register */
sfrb MCUCR     = 0x35;

/* Timer/Counter Interrupt Flag register */
sfrb TIFR      = 0x38;

/* Timer/Counter Interrupt MaSK register */
sfrb TIMSK     = 0x39;

/* General Interrupt Flag Register */
sfrb GIFR      = 0x3A;

/* General Interrupt MaSK register */
sfrb GIMSK     = 0x3B;
 
/* Stack Pointer */
sfrb SP        = 0x3D;
sfrb SPL       = 0x3D;

/* Status REGister */
sfrb SREG      = 0x3F;


#ifndef __IO2333_INCLUDED
#define __IO2333_INCLUDED

/* SFRs are local in assembler modules (so this file may need to be */
/* included in more than one module in the same source file), */
/* but #defines must only be made once per source file. */

/*                              */
/* Interrupt Vector Definitions */
/*                              */

/* NB! vectors are specified as byte addresses */

#define    RESET_vect           (0x00)
#define    INT0_vect            (0x02)
#define    INT1_vect            (0x04)
#define    TIMER1_CAPT1_vect    (0x06)
#define    TIMER1_COMP_vect     (0x08)
#define    TIMER1_OVF_vect      (0x0A)
#define    TIMER0_OVF_vect      (0x0C)
#define    SPI_STC_vect         (0x0E)
#define    UART_RX_vect         (0x10)
#define    UART_UDRE_vect       (0x12)
#define    UART_TX_vect         (0x14)
#define    ADC_vect             (0x16)
#define    EE_RDY_vect          (0x18)
#define    ANA_COMP_vect        (0x1A)

/* Bit definitions for use with the IAR Assembler   
   The Register Bit names are represented by their bit number (0-7).
*/    
 
/* MCU general Status Register */    
#define    WDRF        3
#define    BORF        2
#define    EXTRF       1
#define    PORF        0
 
/* General Interrupt MaSK register */
#define    INT1        7
#define    INT0        6
 
/* General Interrupt Flag Register */
#define    INTF1       7
#define    INTF0       6                   
 
/* Timer/Counter Interrupt MaSK register */
#define    TOIE1       7
#define    OCIE1       6 
#define    TICIE1      3
#define    TOIE0       1
 
/* Timer/Counter Interrupt Flag register */
#define    TOV1         7
#define    OCF1         6
#define    ICF1         3
#define    TOV0         1
 
/* MCU general Control Register */ 
#define    SE           5
#define    SM           4
#define    ISC11        3
#define    ISC10        2
#define    ISC01        1
#define    ISC00        0
 
/* Timer/Counter 0 Control Register */
#define    CS02         2
#define    CS01         1
#define    CS00         0
 
/* Timer/Counter 1 Control Register */
#define    COM11        7
#define    COM10        6
#define    PWM11        1
#define    PWM10        0
 
/* Timer/Counter 1 Control and Status Register */
#define    ICNC1        7
#define    ICES1        6
#define    CTC1         3
#define    CS12         2
#define    CS11         1
#define    CS10         0
 
/* Watchdog Timer Control Register */                         
#define    WDTOE        4
#define    WDE          3
#define    WDP2         2
#define    WDP1         1
#define    WDP0         0    
 
/* EEPROM Control Register */
#define    EERIE        3
#define    EEMWE        2
#define    EEWE         1
#define    EERE         0
 
/* SPI Control Register */
#define    SPIE         7       
#define    SPE          6
#define    DORD         5
#define    MSTR         4
#define    CPOL         3
#define    CPHA         2
#define    SPR1         1
#define    SPR0         0

/* SPI Status Register */
#define    SPIF         7
#define    WCOL         6
 
/* UART Status Register */
#define    RXC          7
#define    TXC          6
#define    UDRE         5
#define    FE           4
#define    OVR          3
#define    MPCM         0
 
/* UART Control Register */
#define    RXCIE        7
#define    TXCIE        6
#define    UDRIE        5
#define    RXEN         4
#define    TXEN         3
#define    CHR9         2
#define    RXB8         1
#define    TXB8         0
 
/* Analog Comparator Control and Status Register */
#define    ACD          7
#define    AINBG        6
#define    ACO          5
#define    ACI          4
#define    ACIE         3
#define    ACIC         2
#define    ACIS1        1
#define    ACIS0        0

/* ADC MUX */
#define    ACDBG        6
#define    MUX2         2
#define    MUX1         1
#define    MUX0         0

/* ADC Control and Status Register */
#define    ADEN         7
#define    ADSC         6
#define    ADFR         5
#define    ADIF         4
#define    ADIE         3
#define    ADPS2        2
#define    ADPS1        1
#define    ADPS0        0  
 
/* Data Register, Port B */  
#define    PB5      5
#define    PB4      4
#define    PB3      3
#define    PB2      2
#define    PB1      1
#define    PB0      0
 
/* Data Direction Register, Port B */
#define    DDB5     5
#define    DDB4     4
#define    DDB3     3
#define    DDB2     2
#define    DDB1     1
#define    DDB0     0
 
/* Input Pins, Port B */
#define    PINB5    5
#define    PINB4    4
#define    PINB3    3
#define    PINB2    2
#define    PINB1    1
#define    PINB0    0
 
/* Data Register, Port C */
#define    PC5      5
#define    PC4      4
#define    PC3      3
#define    PC2      2
#define    PC1      1
#define    PC0      0
 
/* Data Direction Register, Port C */
#define    DDC5     5
#define    DDC4     4
#define    DDC3     3
#define    DDC2     2
#define    DDC1     1
#define    DDC0     0
 
/* Input Pins, Port C */
#define    PINC5    5
#define    PINC4    4
#define    PINC3    3
#define    PINC2    2
#define    PINC1    1
#define    PINC0    0
 
/* Data Register, Port D */
#define    PD7      7
#define    PD6      6
#define    PD5      5
#define    PD4      4
#define    PD3      3
#define    PD2      2
#define    PD1      1
#define    PD0      0
 
/* Data Direction Register, Port D */
#define    DDD7     7
#define    DDD6     6
#define    DDD5     5
#define    DDD4     4
#define    DDD3     3
#define    DDD2     2
#define    DDD1     1
#define    DDD0     0
 
/* Input Pins, Port D */
#define    PIND7     7
#define    PIND6     6
#define    PIND5     5
#define    PIND4     4
#define    PIND3     3
#define    PIND2     2
#define    PIND1     1
#define    PIND0     0
 
/* Pointer definition   */
#define    XL     R26
#define    XH     R27
#define    YL     R28
#define    YH     R29
#define    ZL     R30
#define    ZH     R31
 
/* Contants */
#define    RAMEND   $DF    /*Last On-Chip SRAM location*/
#define    XRAMEND  $DF
#define    E2END    $7F
#define    FLASHEND $7FF

#endif /* __IO2333_INCLUDED (define part) */
#endif /* __IO2333_INCLUDED (SFR part) */
