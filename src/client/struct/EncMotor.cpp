/**
 * EncMotor.cpp
 * Purpose: Class encapsulates 'Encoder DC Motor'
 * @author Mintae Kim
 */

#include "EncMotor.h"

EncoderMotor::EncoderMotor (short dirPin, short stpPin)
{
    this->dirPin = dirPin;
    this->stpPin = stpPin;

    // gpio.setmode(gpio.BCM)
    // gpio.setup(dirPin, gpio.OUT)  # setting gpio, dirPin controls direction of motor
    // gpio.setup(stpPin, gpio.OUT)  # stpPin controls step number of motor, you must set connect pin on rasberry Pi 3B
    // gpio.output(dirPin, True)
    // gpio.output(stpPin, False)

}

EncoderMotor::~EncoderMotor()
{
    // gpio.cleanup();
}

void EncoderMotor::setVelocity(bool isRotatingCW, float vel)
{
    // TODO: isRotatingCW, velocity -> change velocity
}