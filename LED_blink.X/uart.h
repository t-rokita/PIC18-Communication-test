/* 
 * File:   uart.h
 * Author: tomek
 *
 * Created on 15 wrze?nia 2020, 11:13
 */

#ifndef UART_H
#define	UART_H

#ifdef	__cplusplus
extern "C" {
#endif

#include <stdint.h>

#pragma pack (1)

union hal_uart_packet
{
    struct
    {
        char address;
        char data[5];
    } tot __attribute__((__packed__));
};

enum hal_uart_packet_type
{
    HAL_UART_PACKET_NONE = 0,
    HAL_UART_PACKET_TOT = 1,
    HAL_UART_PACKET_TOT2 = 2,
} __attribute__((__packed__));

enum hal_uart_request_state
{
    HAL_UART_REQUEST_NEW = 0,
    HAL_UART_REQUEST_PENDING = 1,
    HAL_UART_REQUEST_BUSY = 2,
    HAL_UART_REQUEST_DONE = 3,
} __attribute__((__packed__));

struct hal_uart_tx_request
{
    struct {
        enum hal_uart_request_state state;
        struct hal_uart_tx_request* next;
    } header;
    enum hal_uart_packet_type packet_type;
    union hal_uart_packet data;
};

typedef void (hal_uart_rx_handler_t)(union hal_uart_packet *data, enum hal_uart_packet_type type);

void hal_uart_init(void);

void hal_uart_submit_request(struct hal_uart_tx_request* request);

void hal_uart_set_rx_mode(enum hal_uart_packet_type packet_type);

void hal_uart_set_rx_handler(hal_uart_rx_handler_t* handler);


#ifdef	__cplusplus
}
#endif

#endif	/* UART_H */

