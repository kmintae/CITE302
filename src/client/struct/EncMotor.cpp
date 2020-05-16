/**
 * EncMotor.cpp
 * Purpose: Class encapsulates 'Encoder DC Motor'
 * @author Mintae Kim, Youngtae Moon
 */

#include "EncMotor.h"

EncoderMotor::EncoderMotor (short pwmPin, short dirPin, short stopPin, short encPin1, short encPin2)
{
    this->pwmPin = pwmPin;
    this->dirPin = dirPin;
    this->stopPin = stopPin;
    this->encPin1 = encPin1;
    this->encPin2 = encPin2;

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