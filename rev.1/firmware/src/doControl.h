#ifndef DOCONTROL_H
#define DOCONTROL_H

#include <Arduino.h>

#include "thermostat.h"
#include "pins.h"

uint8_t doControl(struct ControlState* controlState, struct SensorState* sensorState, struct OutputState* outputState);

#endif
