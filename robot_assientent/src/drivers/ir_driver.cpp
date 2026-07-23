#include "drivers/ir_driver.h"

IrDriver::IrDriver()
    : m_irSend(SystemConfig::Pins::IR_PIN)
{
}

void IrDriver::init() {
    m_irSend.begin();
}

void IrDriver::sendIrCommand(IrCommand cmd) {
    m_irSend.sendNEC(static_cast<uint32_t>(cmd));
}
