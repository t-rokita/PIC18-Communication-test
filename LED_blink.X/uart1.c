//#include "uart.h"
//#include "mcal.h"
//// Private state
//static hal_uart_rx_handler_t* rx_handler = 0;
//static struct hal_uart_tx_request* current_tx_request = 0;
//// TX state
//static uint8_t tx_buffer[10];
//static uint8_t tx_buffer_length;
//static uint8_t tx_buffer_start;
//// RX state
//static enum hal_uart_packet_type rx_packet_type;
//static uint8_t rx_buffer[10];
//static uint8_t rx_buffer_count;
//
//static struct {
//    unsigned END :1;
//    unsigned     :7;
//} rx_flags = {0};
//

//void hal_uart_init(void)
//{
//    rx_packet_type = HAL_UART_PACKET_NONE;
//
//    U1CON0bits.MODE = 0; // Async 8-bit mode
//    U1BRGL = (1666) & 0xFF;
//    U1BRGH = (1666 >> 8) & 0xFF;
//
//    MCAL_GPIO_AS_OUTPUT(PIN_UART_TX);
//    MCAL_GPIO_AS_DIGITAL_INPUT(PIN_UART_RX);
//    MCAL_GPIO_AS_OUTPUT(PIN_UART_TE);
//
//    MCAL_GPIO_ALT_OUT(PIN_UART_TX, UART1_TX); 
//    MCAL_GPIO_ALT_OUT(PIN_UART_TE, UART1_TXDE); 
//    MCAL_GPIO_ALT_IN(PIN_UART_RX, UART1_RX);
//    MCAL_PPSI_UART1_CTS = 0xFF; // Use some unimplemented pin
//
//    U1CON2bits.FLO = 0b10; // rts/cts and TXDE
//    U1CON0bits.TXEN = 1;
//    U1CON0bits.RXEN = 1;
//    U1CON1bits.ON = 1;
//
//    // Low priority as a lot of computations is done in these ISRs
//    IPR3bits.U1TXIP = 0;
//    IPR3bits.U1RXIP = 0;
//}