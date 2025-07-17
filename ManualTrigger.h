#ifndef MANUAL_TRIGGER_H
#define MANUAL_TRIGGER_H

#include <string>
#include <gpiod.h>

class ManualTrigger {
private:
    int gpioPin;
    struct gpiod_chip *chip;
    struct gpiod_line *line;

public:
    ManualTrigger(int pin);
    ~ManualTrigger();
    bool checkTrigger();
};

#endif // MANUAL_TRIGGER_H
