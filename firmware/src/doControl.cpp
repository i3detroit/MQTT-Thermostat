#include "doControl.h"

#define FAN_DELAY 30*1000

uint8_t change_bit(uint8_t val, uint8_t num, bool bitval) {
  return (val & ~(1<<num)) | (bitval << num);
}

uint8_t setHeat(uint8_t toWrite, struct OutputState* outputState) {
  if(outputState->mode != HEAT) {
    outputState->mqttOutputDirty = true;
    Serial.println("heat changed");
    outputState->fanDelayEnd = millis() + FAN_DELAY;
  }
  outputState->mode = HEAT;
  toWrite = change_bit(toWrite, 7-ENABLE_CONTROL, 1);
  toWrite = change_bit(toWrite, 7-SELECT_CONTROL, 0);
  return toWrite;
}

uint8_t setCool(uint8_t toWrite, struct OutputState* outputState) {
  if(outputState->mode != COOL) {
    Serial.println("cool changed");
    outputState->mqttOutputDirty = true;
    outputState->fanDelayEnd = millis() + FAN_DELAY;
  }
  outputState->mode = COOL;
  toWrite = change_bit(toWrite, 7-ENABLE_CONTROL, 1);
  toWrite = change_bit(toWrite, 7-SELECT_CONTROL, 1);
  return toWrite;
}
uint8_t setOff(uint8_t toWrite, struct OutputState* outputState) {
  if(outputState->mode != OFF) {
    Serial.println("off changed");
    outputState->mqttOutputDirty = true;
  }
  outputState->mode = OFF;
  return change_bit(toWrite, 7-ENABLE_CONTROL, 0);
}

uint8_t doControl(struct ControlState* controlState, struct SensorState* sensorState, struct OutputState* outputState) {
  uint8_t toWrite = 0;

  //status lights
  toWrite = change_bit(toWrite, 7-LED_FAN_ON, controlState->fan);
  toWrite = change_bit(toWrite, 7-LED_FAN_AUTO, !controlState->fan);
  toWrite = change_bit(toWrite, 7-LED_HEAT, controlState->mode == HEAT);
  toWrite = change_bit(toWrite, 7-LED_COOL, controlState->mode == COOL);
  toWrite = change_bit(toWrite, 7-LED_OFF, controlState->mode == OFF);

  // heating logic
  /*
   * if heat
   *
   *    if(current + swing < target)
   *        below temp, heat
   *        start heat
   *    if(current - swing > target)
   *        above
   *        stop
   *    if(current > target-swing && current < target+swing && enabled)
   *        heating through swing
   *        heat
   *    if(current > target-swing && current < target+swing && !enabled)
   *        //cooling through swing
   *        stop
   *    else
   *        there is no else
   *
   * Fan table
   * fan state      fan last state      enabled     force fan   fandelay long enough
   * 0              0                   0           0           0
   * 0              0                   0           0           1
   * 1t             0                   0           1           0
   * 1t             0                   0           1           1
   * 0              0                   1           0           0
   * 1t             0                   1           0           1
   * 0              0                   1           1           0
   * 1t             0                   1           1           1
   * 0t             1                   0           0           0
   * 0t             1                   0           0           1
   * 1              1                   0           1           0
   * 1              1                   0           1           1
   * 1              1                   1           0           0
   * 1              1                   1           0           1
   * 1              1                   1           1           0
   * 1              1                   1           1           1
   * s/   \* \([01t]*\) *\([01]\) *\([01]\) *\([01]\) *\([01]\)/case 0b\2\3\4\5: \1/
   *
   *
   */
  if(sensorState->temp < MIN_TEMP) {
    Serial.println("under min temp");
    toWrite = setHeat(toWrite, outputState);
  } else if(controlState->mode == HEAT) {
    if(sensorState->temp < controlState->target - controlState->swing) {
      toWrite = setHeat(toWrite, outputState);
      Serial.println("heat on");
    } else  if(sensorState->temp >= controlState->target + controlState->swing) {
      toWrite = setOff(toWrite, outputState);
      Serial.println("heat off");
    } else {
      Serial.println("HEAT inside swing; continue");
      if(outputState->mode == HEAT) {
        toWrite = setHeat(toWrite, outputState);
      } else {
        toWrite = setOff(toWrite, outputState);
      }
    }
#ifndef HEAT_ONLY
  } else if(controlState->mode == COOL) {
    //Serial.println("COOLING");
    //Serial.println(sensorState->temp);
    //Serial.println(controlState->target);

    //Serial.println(sensorState->temp + controlState->swing <  controlState->target);
    //Serial.println(abs(sensorState->temp-controlState->target) <= controlState->swing);
    //Serial.println(outputState->mode != COOL);
    if(sensorState->temp >= controlState->target + controlState->swing) {
      toWrite = setCool(toWrite, outputState);
      Serial.println("cool on");
    } else if(sensorState->temp < controlState->target - controlState->swing ) {
      Serial.println("cool off");
      toWrite = setOff(toWrite, outputState);
    } else {
      Serial.println("COOL inside swing; continue");
      if(outputState->mode == COOL) {
        toWrite = setCool(toWrite, outputState);
      } else {
        toWrite = setOff(toWrite, outputState);
      }
    }
#endif
  } else if(controlState->mode == OFF) {
    Serial.println("off");
    //off
    toWrite = setOff(toWrite, outputState);
  } else {
    Serial.println("holy fuck mode is broken");
    //TODO: error to mqtt
  }

  uint8_t fanControl = ((0x01 & outputState->fan == true) << 3) | ((0x01 & outputState->mode != OFF) << 2) | ((0x01 & controlState->fan) << 1) | (0x01 & (int32_t)(millis() - outputState->fanDelayEnd) >= 0);
  //Serial.println("FAN");
  //Serial.println(outputState->fan);
  //Serial.println(outputState->mode != OFF);
  //Serial.println(controlState->fan);
  //Serial.println((int32_t)(millis() - outputState->fanDelayEnd) >= 0);
  //Serial.println("FAN VAR");
  //Serial.println(fanControl >> 3 & 0x01);
  //Serial.println(fanControl >> 2 & 0x01);
  //Serial.println(fanControl >> 1 & 0x01);
  //Serial.println(fanControl >> 0 & 0x01);


  //fan last state      enabled     force fan   done with delay
  switch(fanControl) {
    case 0b1000:
    case 0b1001:
      //write 0, transition
      Serial.println("Fan transition off");
      outputState->mqttOutputDirty = true;
    case 0b0000:
    case 0b0001:
    case 0b0100:
    case 0b0110:
      //write 0
      outputState->fan = 0;
      break;
    case 0b0101:
    case 0b0010:
    case 0b0011:
    case 0b0111:
      //write 1; transition
      Serial.println("Fan transition on");
      outputState->mqttOutputDirty = true;
    case 0b1010:
    case 0b1011:
    case 0b1100:
    case 0b1101:
    case 0b1110:
    case 0b1111:
      //write 1
      outputState->fan = 1;
      break;
    default:
      //fuck?
      break;
  }
  toWrite = change_bit(toWrite, 7-FAN_CONTROL, outputState->fan);

  // //write control
  //Serial.println("control update");
  //Serial.println(fanForced);
  //Serial.println(heat);
  //Serial.println(cool);
  //Serial.println(toWrite, BIN);

  /*
   * EN  SEL | H  C
   * 0   0   | 0  1
   * 0   1   | 1  0
   * 1   0   | 0  0
   * 1   1   | 0  0
   */

  //writeLow(toWrite);
  //Serial.println(writeLow(toWrite), BIN);
  return toWrite;
}
