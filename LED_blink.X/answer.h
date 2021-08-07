/* 
 * File:   answer.h
 * Author: grabi
 *
 * Created on 16 wrze?nia 2020, 12:11
 */

#ifndef ANSWER_H
#define	ANSWER_H

#include <stdbool.h>

typedef void (*cmd_handler_t)(char, char);

struct cmd_handler {
    char cmd;
    cmd_handler_t handler;
    
}Handler;

bool answer_call_handler(const struct cmd_handler* handlers, char cmd, char arg);

#endif	/* ANSWER_H */

