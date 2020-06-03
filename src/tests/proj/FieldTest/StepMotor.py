# This code is a basic code for stpper motor.
# Cotrolling with A4988 and Rasberry Pi 3 B

import sys
import RPi.GPIO as gpio  # https://pypi.python.org/pypi/RPi.GPIO more info
import time
import math


class Motor:

    def __init__(self, angPerSt, gear, maxAng=0.0, minAng=0.0):
        self.dir = 'right'  # direction of rotate, right=clockwise
        self.curAng = 0.00  # current angle, For robot arm positioning & coorection error of movility
        self.angPerSt = angPerSt  # stepper motor configuration
        self.gear = gear  # geared stpper motor configuration, when it 2 times slow down, then value is 2.00
        self.maxAng = maxAng  # maximun Angle that stepper can rotate, it's for robotArm
        self.minAng = minAng
        self.dirPin = 0
        self.stpPin = 0
        self.enPin=0

    def setMotor(self, dirPin, stpPin,enPin):
        gpio.setmode(gpio.BCM)
        gpio.setup(dirPin, gpio.OUT)  # setting gpio, dirPin controls direction of motor
        gpio.setup(stpPin, gpio.OUT)  # stpPin controls step number of motor, you must set connect pin on rasberry Pi 3B
        gpio.setup(enPin, gpio.OUT)
        self.enPin=enPin

        gpio.output(dirPin, True)
        gpio.output(stpPin, False)
        gpio.output(enPin, True)

        # Updating dir/stp Pins
        self.dirPin = dirPin
        self.stpPin = stpPin

    def getDelay(self, vel):
        # rotation per sec => step/s
        totalStep = 360.0 / self.angPerSt
        t = 1.000 / vel
        return t / totalStep

    def getSmoothVel(self, maxVel, target, cur):
        return 20 * maxVel / (19 + abs(math.sqrt(cur / target)))

    def move(self, stp=0, vel=0.0):
        #if (angle + self.curAng < self.minAng or angle + self.curAng > self.maxAng) and self.maxAng != 0:
         #   print("False")
          #  return False
        delay = self.getDelay(abs(vel) * self.gear)
        counter = 0
        if (stp == 0):
            return False

        if stp > 0:
            gpio.output(self.dirPin, True)
        else:
            gpio.output(self.dirPin, False)


        while counter < abs(stp):
            gpio.output(self.stpPin, True)
            time.sleep(delay)
            gpio.output(self.stpPin, False)
            time.sleep(delay)
            counter += 1
        gpio.output(self.stpPin, False)
        return True

    def halt(self):
        gpio.output(self.enPin,False)

    def unhalt(self):
        gpio.output(self.enPin, True)

    def __del__(self):
        print()
        # gpio.cleanup()