#ifndef __MCAL_GPIO_H__
#define __MCAL_GPIO_H__

#include "pin_mapping.h"

#define MCAL_GPIO_AS_OUTPUT(port) do { \
        __PIN_LAT_(port) = 0;           \
        __PIN_TRIS_(port) = 0;          \
        __PIN_ANSEL_(port) = 0;         \
    } while(0)

#define MCAL_GPIO_AS_DIGITAL_INPUT(port) do { \
        __PIN_TRIS_(port) = 1;          \
        __PIN_ANSEL_(port) = 0;         \
    } while(0)

#define MCAL_GPIO_WRITE(port, value) __PIN_LAT_(port) = (value)
#define MCAL_GPIO_READ(port) (__PIN_PORT_(port))
#define MCAL_GPIO_VALUE(port) (__PIN_LAT_(port))

#define MCAL_GPIO_ALT_OUT(port, alt) __PIN_PPSO_(port) = MCAL_PPSO_##alt
#define MCAL_GPIO_ALT_IN(port, alt) MCAL_PPSI_##alt = __PIN_PPSI_(port)

#endif // __MCAL_GPIO_H__