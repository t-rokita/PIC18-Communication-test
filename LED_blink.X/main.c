/* 
 * File:   main.c
 * Author: tomek
 *
 * Created on 15 wrze?nia 2020, 10:23
 */
// CONFIG1L
#pragma config FEXTOSC = OFF    // External Oscillator Selection (Oscillator not enabled)
#pragma config RSTOSC = HFINTOSC_64MHZ// Reset Oscillator Selection (HFINTOSC with HFFRQ = 64 MHz and CDIV = 1:1)

// CONFIG1H
#pragma config CLKOUTEN = OFF   // Clock out Enable bit (CLKOUT function is disabled)
#pragma config PR1WAY = ON      // PRLOCKED One-Way Set Enable bit (PRLOCK bit can be cleared and set only once)
#pragma config CSWEN = ON       // Clock Switch Enable bit (Writing to NOSC and NDIV is allowed)
#pragma config FCMEN = ON       // Fail-Safe Clock Monitor Enable bit (Fail-Safe Clock Monitor enabled)

// CONFIG2L
#pragma config MCLRE = INTMCLR  // MCLR Enable bit (If LVP = 0, MCLR pin function is port defined function; If LVP =1, RE3 pin fuction is MCLR)
#pragma config PWRTS = PWRT_1   // Power-up timer selection bits (PWRT set at 1ms)
#pragma config MVECEN = ON      // Multi-vector enable bit (Multi-vector enabled, Vector table used for interrupts)
#pragma config IVT1WAY = ON     // IVTLOCK bit One-way set enable bit (IVTLOCK bit can be cleared and set only once)
#pragma config LPBOREN = OFF    // Low Power BOR Enable bit (ULPBOR disabled)
#pragma config BOREN = SBORDIS  // Brown-out Reset Enable bits (Brown-out Reset enabled , SBOREN bit is ignored)

// CONFIG2H
#pragma config BORV = VBOR_2P45 // Brown-out Reset Voltage Selection bits (Brown-out Reset Voltage (VBOR) set to 2.45V)
#pragma config ZCD = OFF        // ZCD Disable bit (ZCD disabled. ZCD can be enabled by setting the ZCDSEN bit of ZCDCON)
#pragma config PPS1WAY = ON     // PPSLOCK bit One-Way Set Enable bit (PPSLOCK bit can be cleared and set only once; PPS registers remain locked after one clear/set cycle)
#pragma config STVREN = ON      // Stack Full/Underflow Reset Enable bit (Stack full/underflow will cause Reset)
#pragma config DEBUG = OFF      // Debugger Enable bit (Background debugger disabled)
#pragma config XINST = OFF      // Extended Instruction Set Enable bit (Extended Instruction Set and Indexed Addressing Mode disabled)

// CONFIG3L
#pragma config WDTCPS = WDTCPS_31// WDT Period selection bits (Divider ratio 1:65536; software control of WDTPS)
#pragma config WDTE = OFF       // WDT operating mode (WDT Disabled; SWDTEN is ignored)

// CONFIG3H
#pragma config WDTCWS = WDTCWS_7// WDT Window Select bits (window always open (100%); software control; keyed access not required)
#pragma config WDTCCS = SC      // WDT input clock selector (Software Control)

// CONFIG4L
#pragma config BBSIZE = BBSIZE_512// Boot Block Size selection bits (Boot Block size is 512 words)
#pragma config BBEN = OFF       // Boot Block enable bit (Boot block disabled)
#pragma config SAFEN = OFF      // Storage Area Flash enable bit (SAF disabled)
#pragma config WRTAPP = OFF     // Application Block write protection bit (Application Block not write protected)

// CONFIG4H
#pragma config WRTB = OFF       // Configuration Register Write Protection bit (Configuration registers (300000-30000Bh) not write-protected)
#pragma config WRTC = OFF       // Boot Block Write Protection bit (Boot Block (000000-0007FFh) not write-protected)
#pragma config WRTD = OFF       // Data EEPROM Write Protection bit (Data EEPROM not write-protected)
#pragma config WRTSAF = OFF     // SAF Write protection bit (SAF not Write Protected)
#pragma config LVP = OFF        // Low Voltage Programming Enable bit (HV on MCLR/VPP must be used for programming)

// CONFIG5L
#pragma config CP = OFF         // PFM and Data EEPROM Code Protection bit (PFM and Data EEPROM code protection disabled)

// CONFIG5H

#include <xc.h>
#include <pic18f27k42.h>
#include <pic18.h>
//#include "uart.h"
#include <stdint.h>
#include "mcal.h"
#include "state_machine.h"
#include "answer.h"

void prints(const char* str)
{
    while(*str)
    {
        while(U1TXIF == 0) ;
        
        U1TXB = *str++;
    }
}

void Led_blinking(int how_many_times) {
    for(int i = 0; i < how_many_times;i++) {
        LATBbits.LATB5 = ~LATBbits.LATB5;
        __delay_ms(100);
        LATBbits.LATB5 = ~LATBbits.LATB5;
        __delay_ms(100);
    }
}

char readb(void)
{
    while(U1RXIF == 0);
    return U1RXB;
}

void scanfs() {
    char buffer[4];
    for(size_t i = 0;i < 4 ;i++) {
        buffer[i] = 0;
    }
    char ch;
    int i = 0;
    if(readb() == '%') {
       while( (ch = readb()) != ' ') {
           buffer[i] = ch;
           i++;
        }
       for(size_t i = 0;i < 4; i++) {
           while(U1TXIF == 0);
           U1TXB = buffer[i];
        }
        if(buffer[0] == 's') {
            LATBbits.LATB5 = ~LATBbits.LATB5;
        }
        else if (buffer[0] == 'a') {
            LATBbits.LATB5 = ~LATBbits.LATB5;
        }
        else if (buffer[0] == 'b') {
            Led_blinking(buffer[1] - '0');   
        }
      for(size_t i = 0;i < 6;i++) {
        buffer[i] = 0;
      }
    }
//     while(U1IF == 0) {
//        LATBbits.LATB5 = ~LATBbits.LATB5;
//        __delay_ms(100);
//        LATBbits.LATB5 = ~LATBbits.LATB5;
//        __delay_ms(100);
//    } 
}

volatile struct state rx_state;

MCAL_ISR(U1RX)
{
    char znak = U1RXB;
    state_rx_byte(&rx_state, znak);
//    if (znak == '1')
//        LATBbits.LATB5 = 1;
//    else
//        LATBbits.LATB5 = 0;
}

void func_hundler(char cmd, char arg)
{
    if (cmd == 'a')
         LATBbits.LATB5 = ~LATBbits.LATB5;
    else if (cmd == 's')
        LATBbits.LATB5 = 1;
    else if (cmd == 'd')
        LATBbits.LATB5 = 0;
    else if (cmd == 'b')
        Led_blinking(arg - '0'); 
}

const struct cmd_handler handlers[5] = {
    {
        .cmd = 'a',
        .handler = func_hundler,
    },
    {
        .cmd = 'b',
        .handler = func_hundler,
    },
    {
        .cmd = 's',
        .handler = func_hundler,
    },
    {
        .cmd = 'd',
        .handler = func_hundler,
    },
    {0}
};

void main(void) {
    TRISBbits.TRISB5 = 0;
    ANSELBbits.ANSELB5 = 0;
    LATBbits.LATB5 = 1;
    
    U1CON0bits.MODE = 0; // Async 8-bit mode
    U1BRGL = (415) & 0xFF;
    U1BRGH = (415 >> 8) & 0xFF;
    
    MCAL_GPIO_AS_OUTPUT(PIN_UART_TX);
    MCAL_GPIO_AS_DIGITAL_INPUT(PIN_UART_RX);
    MCAL_GPIO_AS_OUTPUT(PIN_UART_TE);

    MCAL_GPIO_ALT_OUT(PIN_UART_TX, UART1_TX); 
    MCAL_GPIO_ALT_OUT(PIN_UART_TE, UART1_TXDE); 
    MCAL_GPIO_ALT_IN(PIN_UART_RX, UART1_RX);
    MCAL_PPSI_UART1_CTS = 0xFF; // Use some unimplemented pin
    
    U1CON2bits.FLO = 0b10; // rts/cts and TXDE
    U1CON0bits.TXEN = 1;
    U1CON0bits.RXEN = 1;
    U1CON1bits.ON = 1;
    
    PIE3bits.U1RXIE = 1;
    INTCON0bits.GIEH = 1;
    
    //const char * ala = "Ala ma kota\n";
    
    while(1) 
    {
        if (state_is_frame_ready(&rx_state))
        {
            if(! answer_call_handler(handlers, rx_state.buf[0], rx_state.buf[1]))
            {
                /* error */
            }
            
            state_reset(&rx_state);
        }

    }
}

