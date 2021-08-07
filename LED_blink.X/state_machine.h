/* 
 * File:   state_machine.h
 * Author: tomek
 *
 * Created on 16 wrze?nia 2020, 09:06
 */

#ifndef STATE_MACHINE_H
#define	STATE_MACHINE_H

#include <stdint.h>
#include <stdbool.h>


struct state
{
    uint8_t ile;
    char buf[2];
    struct
    {
        unsigned FRAME_READY :1;
        unsigned             :7;
    };
};



void state_rx_byte(volatile struct state* st, char ch);
void state_reset(volatile struct state* st);
bool state_is_frame_ready(volatile struct state* st);

#endif	/* STATE_MACHINE_H */

