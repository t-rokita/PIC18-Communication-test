#include "state_machine.h"

void state_rx_byte(volatile struct state* st, char ch) {
    if (st->FRAME_READY == 1)
    {
        /* do nothing */
    }
    else if( ch == '%') {
        st->ile = 1;
    }
    else if (st->ile == 1 || st->ile == 2)
    {
        st->buf[st->ile - 1] = ch;
        st->ile++;
    }
    else if (st->ile == 3) {
        if (ch == ' ')
            st->FRAME_READY = 1;
        st->ile = 0;
    }
    else
    {
        st->ile = 0;
    }
}

void state_reset(volatile struct state* st) {
    st->ile = 0;
    st->FRAME_READY = 0;
    
    for(int i = 0; i < 2; i++) {
        st->buf[i] = '\0';
    }
}
bool state_is_frame_ready(volatile struct state* st) { return st->FRAME_READY; } 
