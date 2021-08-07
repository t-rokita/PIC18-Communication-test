#ifndef __MCAL_SPI1_H__
#define __MCAL_SPI1_H__

#include <xc.h>

#define MCAL_PPSO_SPI1_SDO (0b011111)
#define MCAL_PPSO_SPI1_SCK (0b011110)
#define MCAL_PPSO_SPI1_SS  (0b100000)
#define MCAL_PPSI_SPI1_SCK SPI1SCKPPS
#define MCAL_PPSI_SPI1_SDI SPI1SDIPPS
#define MCAL_PPSI_SPI1_SS  SPI1SSPPS

/**
 * @brief Clock source for SPI1 clock
 */
enum mcal_spi1_clk {
    MCAL_SPI1_CLK_FOSC            = 0b0000,
    MCAL_SPI1_CLK_HFINTOSC        = 0b0001,
    MCAL_SPI1_CLK_MFINTOSC        = 0b0010,
    MCAL_SPI1_CLK_CLKREF          = 0b0011,
    MCAL_SPI1_CLK_TMR0_OVF        = 0b0100,
    MCAL_SPI1_CLK_TMR2_POSTSCALED = 0b0101,
    MCAL_SPI1_CLK_TMR4_POSTSCALED = 0b0110,
    MCAL_SPI1_CLK_TMR6_POSTSCALED = 0b0111,
    MCAL_SPI1_CLK_SMT_MATCH       = 0b1000
};

/**
 * @brief Sets clock source for SPI1 module
 * 
 * @param clock selected clock source
 */
void MCAL_SPI1_SELECT_CLOCK(enum mcal_spi1_clk clock);
#define MCAL_SPI1_SELECT_CLOCK(clk) SPI1CLK = clk

/**
 * @brief Sets clock divider
 * 
 * Clock divider is set according to formula: $div = 2*(baud + 1)$
 * 
 * @param baud baud value
 */
void MCAL_SPI1_SET_BAUD(uint8_t baud);
#define MCAL_SPI1_SET_BAUD(baud) SPI1BAUD = baud

/**
 * @brief Sets spi mode for SPI1 module
 * 
 * @param ckp clock polarity select: 0 - low is idle, 1 - high is idle
 * @param cke clock edge select: 0 - idle->active, 1 - active->idle
 */
void MCAL_SPI1_SET_MODE(uint8_t ckp, uint8_t cke);
#define MCAL_SPI1_SET_MODE(p, e) do { \
    SPI1CON1bits.CKP = p; \
    SPI1CON1bits.CKE = e; \
} while(0)

/**
 * @brief Sets data output polarity
 * 
 * @param polarity output polarity: 0 - active-high, 1 - active-low
 */
void MCAL_SPI1_SET_OUT_DATA_POLARITY(uint8_t polarity);
#define MCAL_SPI1_SET_OUT_DATA_POLARITY(polarity) SPI1CON1bits.SDOP = polarity

/**
 * @brief Sets data input polarity
 * 
 * @param polarity output polarity: 0 - active-high, 1 - active-low
 */
void MCAL_SPI1_SET_IN_DATA_POLARITY(uint8_t polarity);
#define MCAL_SPI1_SET_IN_DATA_POLARITY(polarity) SPI1CON1bits.SDIP = polarity

/**
 * @brief Sets slave select i/o polarity
 * 
 * @param polarity slave select polarity: 0 - active-high, 1 - active-low
 */
void MCAL_SPI1_SET_SS_POLARITY(uint8_t polarity);
#define MCAL_SPI1_SET_SS_POLARITY(polarity) SPI1CON1bits.SSP = polarity

/**
 * @brief Sets master mode
 */
void MCAL_SPI1_SET_MASTER();
#define MCAL_SPI1_SET_MASTER() SPI1CON0bits.MST = 1

/**
 * @brief Sets slave mode
 */
void MCAL_SPI1_SET_SLAVE();
#define MCAL_SPI1_SET_SLAVE() SPI1CON0bits.MST = 1

/**
 * @brief Enables SPI1 module
 */
void MCAL_SPI1_ENABLE();
#define MCAL_SPI1_ENABLE() SPI1CON0bits.EN = 1

/**
 * @brief Disables SPI1 modules
 */
void MCAL_SPI1_DISABLE();
#define MCAL_SPI1_DISABLE() SPI1CON0bits.EN = 0

#endif // __MCAL_SPI1_H__