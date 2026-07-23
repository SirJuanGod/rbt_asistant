#ifndef IR_DRIVER_H
#define IR_DRIVER_H

#include <Arduino.h>
#include <IRremoteESP8266.h>
#include <IRsend.h>
#include "config/system_config.h"
#include "core/system_types.h"

class IrDriver {
public:
    IrDriver();
    ~IrDriver() = default;

    void init();
    void sendIrCommand(IrCommand cmd);

private:
    IRsend m_irSend;
};

#endif // IR_DRIVER_H
