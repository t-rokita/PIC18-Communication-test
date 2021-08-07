/* 
 * File:   pin_mapping.h
 * Author: tomek
 *
 * Created on 15 wrze?nia 2020, 11:42
 */

#ifndef PIN_MAPPING_H
#define	PIN_MAPPING_H

#include <xc.h>

#define __PIN_ANSEL_(port, pin) (ANSEL ## port ## bits.ANSEL ## port ## pin)
#define ANSEL(args) __PIN_ANSEL_(args)

#define __PIN_WPU_(port, pin) (WPU ## port ## bits.WPU ## port ## pin)
#define WPU(args) __PIN_WPU_(args)

#define __PIN_TRIS_(port, pin) (TRIS ## port ## bits.TRIS ## port ## pin)
#define TRIS(args) __PIN_TRIS_(args)

#define __PIN_PORT_(port, pin) (PORT ## port ## bits.R ## port ## pin)
#define PORT(args) __PIN_PORT_(args)

#define __PIN_LAT_(port, pin) (LAT ## port ## bits.LAT ## port ## pin)
#define LAT(args) __PIN_LAT_(args)

#define __PIN_PPSO_(port, pin) (R ## port ## pin ## PPS)
#define PPSO(args) __PIN_PPSO_(args)

#if defined(__18F24K42) || defined(__18F25K42) || defined(__18F26K42) || defined(__18F27K42)
    #define __PIN_PPSI_A_ (0x00)
    #define __PIN_PPSI_B_ (0x08)
    #define __PIN_PPSI_C_ (0x10)
    #define __PIN_PPSI_BASE_(port) (__PIN_PPSI_ ## port ## _)
    #define __ADC_CH_BASE_(port) (__PIN_PPSI_ ## port ## _)
#else
    #error "MCU not supported"
#endif

#define __PIN_PPSI_(port, pin) ( __PIN_PPSI_BASE_(port) + pin )
#define PPSI(args) __PIN_PPSI_(args)
#define __ADC_CH_(port, pin) ( __ADC_CH_BASE_(port) + pin )
#define ADC_CH(args) __ADC_CH_(args)

#ifdef	__cplusplus
extern "C" {
#endif
 #define PIN_UART_RX  C, 7
 #define PIN_UART_TX  C, 6
 #define PIN_UART_TE  B, 2
#ifdef	__cplusplus
}
#endif

#endif	/* PIN_MAPPING_H */

