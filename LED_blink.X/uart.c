#include "uart.h"
// Private state
static hal_uart_rx_handler_t* rx_handler = 0;
static struct hal_uart_tx_request* current_tx_request = 0;
// TX state
static uint8_t tx_buffer[10];
static uint8_t tx_buffer_length;
static uint8_t tx_buffer_start;
// RX state
static enum hal_uart_packet_type rx_packet_type;
static uint8_t rx_buffer[10];
static uint8_t rx_buffer_count;
static struct {
    unsigned END :1;
    unsigned     :7;
} rx_flags = {0};

// Private API
static uint8_t crc_update(uint8_t crc, uint8_t data);
static char byte_to_hex(uint8_t byte, uint8_t nibble);
static uint8_t hex_to_byte(char hex);
static uint8_t valid_hex(char hex);
static void fill_buffer_tot(void);
static void fill_buffer_tot2(void);
static void prepare_current_request(void);
static void process_rx_tot(void);
static void process_rx_tot2(void);

static inline uint8_t between(char value, char bottom, char top) 
{
    return value >= bottom && value <= top;
}

// Public API
void hal_uart_init(void)
{
    rx_packet_type = HAL_UART_PACKET_NONE;

    U1CON0bits.MODE = 0; // Async 8-bit mode
    U1BRGL = (1666) & 0xFF;
    U1BRGH = (1666 >> 8) & 0xFF;

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

    // Low priority as a lot of computations is done in these ISRs
    IPR3bits.U1TXIP = 0;
    IPR3bits.U1RXIP = 0;
}

void hal_uart_submit_request(struct hal_uart_tx_request* request)
{
    PIE3bits.U1TXIE = 0;

    request->header.next = 0;

    if (current_tx_request == 0)
    {
        current_tx_request = request;
        prepare_current_request();
    }
    else
    {
        struct hal_uart_tx_request* req = current_tx_request;
        while(req->header.next != 0)
            req = req->header.next;
        
        req->header.next = request;
        request->header.state = HAL_UART_REQUEST_PENDING;
    }

    if (current_tx_request != 0)
        PIE3bits.U1TXIE = 1;
}

void hal_uart_set_rx_mode(enum hal_uart_packet_type packet_type)
{
    PIE3bits.U1RXIE = 0;
    rx_packet_type = packet_type;
    rx_buffer_count = 0;
    rx_flags.END = 0;

    if (packet_type != HAL_UART_PACKET_NONE)
        PIE3bits.U1RXIE = 1;
}

void hal_uart_set_rx_handler(hal_uart_rx_handler_t* handler)
{
    rx_handler = handler;
}

// Private API
static uint8_t crc_update(uint8_t crc, uint8_t data)
{
    data ^= crc;

	for (uint8_t i = 0; i < 8; i++)
    {
        if (( data & 0x80 ) != 0)
        {
            data <<= 1;
            data ^= 0x07;
        }
        else
        {
            data <<= 1;
        }
	}

	return data;
}

static char byte_to_hex(uint8_t byte, uint8_t nibble)
{
    const char values[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};
    
    if (nibble)
        return values[ byte & 0x0F ];
    else
        return values[ (byte & 0xF0) >> 4 ];
}

static uint8_t hex_to_byte(char hex)
{
    if (between(hex, '0', '9'))
        return hex - '0';
    else if (between(hex, 'A', 'F'))
        return hex - 'A' + 10;
    else if (between(hex, 'a', 'f'))
        return hex - 'a' + 10;
    else
        return 0;
}

static uint8_t valid_hex(char hex)
{
    return (between(hex, '0', '9') || between(hex, 'A', 'F') || between(hex, 'a', 'f'));
}

static void fill_buffer_tot(void)
{
    tx_buffer_length = 3;

    tx_buffer[0] = '{';
    tx_buffer[1] = current_tx_request->data.tot.address;
    tx_buffer[2] = '}';

    if (current_tx_request->data.tot.data[0] != '\0')
    {
        for(uint8_t i = 0; i < 5; i++)
            tx_buffer[i + 2] = current_tx_request->data.tot.data[i];
        
        tx_buffer_length = 5 + 3;
        tx_buffer[7] = '}';
    }

    tx_buffer_start = 0;
}

static void fill_buffer_tot2(void)
{
    tx_buffer_length = 3;

    tx_buffer[0] = '{';
    tx_buffer[1] = current_tx_request->data.tot.address;
    tx_buffer[2] = '}';

    if (current_tx_request->data.tot.data[0] != '\0')
    {
        for(uint8_t i = 0; i < 5; i++)
            tx_buffer[i + 2] = current_tx_request->data.tot.data[i];
        
        tx_buffer_length = 5 + 3;
        tx_buffer[7] = '}';
    }

    uint8_t crc = 0xFF;
    for (uint8_t i = 0; i < tx_buffer_length; i++)
        crc = crc_update(crc, tx_buffer[i]);
    
    tx_buffer[tx_buffer_length] = byte_to_hex(crc, 0);
    tx_buffer[tx_buffer_length + 1] = byte_to_hex(crc, 1);
    tx_buffer_length += 2;

    tx_buffer_start = 0;
}

static void prepare_current_request(void)
{
    while(current_tx_request != 0)
    {
        current_tx_request->header.state = HAL_UART_REQUEST_BUSY;
        if (current_tx_request->packet_type == HAL_UART_PACKET_TOT)
        {
            fill_buffer_tot();
            PIE3bits.U1TXIE = 1;
            return;
        }
        else if (current_tx_request->packet_type == HAL_UART_PACKET_TOT2)
        {
            fill_buffer_tot2();
            PIE3bits.U1TXIE = 1;
            return;
        }
        else
        {
            current_tx_request->header.state = HAL_UART_REQUEST_DONE;
            current_tx_request = current_tx_request->header.next;
        }
    }
    
    PIE3bits.U1TXIE = 0;
    return;
}

static void process_rx_tot(void)
{
    union hal_uart_packet packet = {0};

    if (rx_handler == 0)
        goto cleanup;

    if (rx_buffer[0] != '{')
        goto cleanup;

    if (rx_buffer[2] == '}')
    {
        packet.tot.address = rx_buffer[1];
        rx_handler(&packet, HAL_UART_PACKET_TOT);
        goto cleanup;
    }

    if (rx_buffer[7] == '}')
    {
        packet.tot.address = rx_buffer[1];
        for(uint8_t i = 0; i < 5; i++)
            packet.tot.data[i] = rx_buffer[i + 2];
        
        rx_handler(&packet, HAL_UART_PACKET_TOT);
        goto cleanup;
    }
cleanup:
    rx_buffer_count = 0;
    rx_flags.END = 0;
}

static void process_rx_tot2(void)
{
    union hal_uart_packet packet = {0};

    if (rx_handler == 0)
        goto cleanup;

    if (rx_buffer_count != 10)
        goto cleanup;

    if (rx_buffer[0] != '{')
        goto cleanup;

    if (! valid_hex(rx_buffer[8]))
        goto cleanup;

    if (! valid_hex(rx_buffer[9]))
        goto cleanup;

    if (rx_buffer[7] != '}')
        goto cleanup;
    
    uint8_t theirCrc = 
                (hex_to_byte(rx_buffer[8]) << 4)
                | hex_to_byte(rx_buffer[9]);

    uint8_t myCrc = 0xFF;
    for (uint8_t i = 0; i < 8; i++)
        myCrc = crc_update(myCrc, rx_buffer[i]);

    if (myCrc != theirCrc)
        goto cleanup;

    packet.tot.address = rx_buffer[1];
    for(uint8_t i = 0; i < 5; i++)
        packet.tot.data[i] = rx_buffer[i + 2];
    
    rx_handler(&packet, HAL_UART_PACKET_TOT2);
    
cleanup:
    rx_buffer_count = 0;
    rx_flags.END = 0;
}

// Interrupts
MCAL_ISR(U1TX)
{
    U1TXB = tx_buffer[tx_buffer_start];
    tx_buffer_start ++;
    if (tx_buffer_start >= tx_buffer_length)
    {
        current_tx_request->header.state = HAL_UART_REQUEST_DONE;
        current_tx_request = current_tx_request->header.next;
        prepare_current_request();
    }
}

MCAL_ISR(U1RX)
{
    uint8_t value = U1RXB;
    if (rx_packet_type == HAL_UART_PACKET_NONE)
        return;

    if (current_tx_request != 0)
        return;
    
    if (rx_packet_type == HAL_UART_PACKET_TOT)
    {
        if (value == '{')
        {
            rx_buffer[0] = value;
            rx_buffer_count = 1;
        }
        else
        {
            rx_buffer[rx_buffer_count] = value;
            rx_buffer_count++;

            if (rx_buffer_count > 8)
                rx_buffer_count = 0;
        }
        
        if (value == '}')
            process_rx_tot();
    }
    else if (rx_packet_type == HAL_UART_PACKET_TOT2)
    {
        if (value == '{')
        {
            rx_buffer[0] = value;
            rx_buffer_count = 1;
            rx_flags.END = 0;
        }
        else
        {
            rx_buffer[rx_buffer_count] = value;
            rx_buffer_count++;

            if (rx_buffer_count > 8 && ! rx_flags.END)
                rx_buffer_count = 0;
            else if (rx_buffer_count == 10 && rx_flags.END)
                process_rx_tot2();
        }
        
        if (value == '}')
        {
            rx_flags.END = 1;
            if (rx_buffer_count != 8)
                process_rx_tot();
        }
    }
}
