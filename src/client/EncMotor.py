# This code is a basic code for stpper motor.
# Cotrolling with A4988 and Rasberry Pi 3 B

import sys
import RPi.GPIO as gpio  # https://pypi.python.org/pypi/RPi.GPIO more info
import time
import math

config = yaml.load(open("./ClientConfig.yaml", 'r'), Loader=yaml.FullLoader)

class Motor:

    def __init__(self,right,pins):
        self.right = True  # motor posision => True for the right which is determined based on the direction from the rear of the robot to the front.
        self.gear = config["MOTOR_WHEEL_GEAR"]  # gear ratio => torque becomes 'gear' times bigger
        self.encPulsePerCycle = config["ENC_PULSE_PER_CYCLE"] # number of encorder pulse per cycle
        self.wheelSize = config["WHEEL_SIZE"]
        self.encoderPos = 0
        self.pins=pins
        self.pwm
        self.dt_sleep=0.1 #delay for measuring actual angular velocity of wheel

    # set dir, pwm, start, encA, encB pins
    def setMotor(self):
        gpio.setmode(gpio.BCM)
        for i in range (0,3):
            gpio.setup(self.pins[i], gpio.OUT) # setting gpio, dirPin, pwmPin, stpPin as output

        gpio.output(self.pins[1], True)
        gpio.output(self.pins[2], True)
        self.pwm=gpio.PWM(pins[1],1000)
        self.pwm=pwm.start(0)

        gpio.setup( pins[3], gpio.IN, pull_up_down=gpio.PUD_UP) # setting encorder input pin. we get pulse of encoder through this pin
        gpio.setup( pins[4], gpio.IN, pull_up_down=gpio.PUD_UP)
        gpio.add_event_detect(pins[3], gpio.BOTH, callback=self.encoderA())

    def encoderA(self,channel):
        if IO.input(self.pins[3]) == IO.input(self.pins[4]):
            selg.encoderPos += 1
        else:
            self.encoderPos -= 1

    def getActualVel(self):
        before=self.encoderPos
        time.sleep(self.dt_sleep)
        after=self.encoderPos
        angularVelocity=(360.0*(after-before)/self.encPulsePerCycle)/self.gear/self.dt_sleep
        return angularVelocity

    def move(self, control):
        if(not self.right):
            control=-control

        gpio.output(self.pins[0],control>=0)
        pwmWriteValue=min(abs(control), 100)
        pwm.ChangeDutyCycle(pwmWriteValue)
        return True

    def halt(self):
        pwm.ChangeDutyCycle(0)
        gpio.output(self.pins[2], False)


    def __del__(self):
        # gpio.cleanup()