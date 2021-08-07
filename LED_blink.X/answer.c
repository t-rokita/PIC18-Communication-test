
#include "answer.h"

bool answer_call_handler(const struct cmd_handler* handlers, char cmd, char arg)
{
    while(handlers->cmd)
    {
        if(handlers->cmd  == cmd) {
            (*handlers->handler)(cmd, arg);
        }
        handlers++;
    }
}