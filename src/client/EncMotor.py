# This code is a basic code for stpper motor.
# Cotrolling with A4988 and Rasberry Pi 3 B

import sys
import RPi.GPIO as gpio  # https://pypi.python.org/pypi/RPi.GPIO more info
import time
import math
import yaml

config = yaml.load(open("./ClientConfig.yaml", 'r'), Loader=yaml.FullLoader)

gpio.setmode(gpio.BCM)
'''
encoderPosR=0
encoderPosL=0
pinR=[config["RIGHT_WHEEL_PINS"][3],config["RIGHT_WHEEL_PINS"][4]]
pinL=[config["LEFT_WHEEL_PINS"][3],config["LEFT_WHEEL_PINS"][4]]

gpio.setup( pinR[0], gpio.IN, pull_up_down=gpio.PUD_UP)
gpio.setup( pinR[1], gpio.IN, pull_up_down=gpio.PUD_UP)
gpio.setup( pinL[0], gpio.IN, pull_up_down=gpio.PUD_UP)
gpio.setup( pinL[1], gpio.IN, pull_up_down=gpio.PUD_UP)

def encoderR(channel):
    global encoderPosR
    if gpio.input(pinR[0]) == gpio.input(pinR[1]):
        encoderPosR += 1
    else:
        encoderPosR -= 1
        
def encoderL(channel):
   
    global encoderPosL
    if gpio.input(pinL[0]) == gpio.input(pinL[1]):
        encoderPosL += 1
    else:
        encoderPosL -= 1
gpio.add_event_detect(pinR[0], gpio.BOTH, callback=encoderR)
gpio.add_event_detect(pinL[0], gpio.BOTH, callback=encoderL)
'''
class Motor:

    def __init__(self,right,pins,pwm):
        gpio.setmode(gpio.BCM)
        self.right = right  # motor posision => True for the right which is determined based on the direction from the rear of the robot to the front.
        self.gear = config["MOTOR_WHEEL_GEAR"]  # gear ratio => torque becomes 'gear' times bigger
        self.encPulsePerCycle = config["ENC_PULSE_PER_CYCLE"] # number of encorder pulse per cycle
        self.wheelSize = config["WHEEL_SIZE"]
        self.pins=pins
        gpio.setmode(gpio.BCM)
        for i in range (0,3):
            gpio.setup(self.pins[i], gpio.OUT) # setting gpio, dirPin, pwmPin, stpPin as output

        gpio.output(self.pins[0], True)
        gpio.output(self.pins[2], True)
        self.pwm=pwm
        self.pwm.start(0)
        self.dt_sleep=0.1 #delay for measuring actual angular velocity of wheel
        

    def getActualVel(self):
        if(self.right):
            before=encoderPosR
        else:
            before=encoderPosL
        time.sleep(self.dt_sleep)
        if(self.right):
            after=encoderPosR
        else:
            after=encoderPosL
        angularVelocity=(360.0*(after-before)/self.encPulsePerCycle)/self.gear/self.dt_sleep
        return angularVelocity
    
    def getRotationalPos(self):
        if(self.right):
            return encoderPosR
        else:
            return encoderPosL

    def move(self, control):
        gpio.output(self.pins[2], True)
        print(control)
        if(self.right):
            control=-control
        gpio.setmode(gpio.BCM)
        gpio.setup(self.pins[0], gpio.OUT)
        gpio.output(self.pins[0],control>=0)
        pwmWriteValue=min(abs(control), 100)
        self.pwm.ChangeDutyCycle(pwmWriteValue)
        return True

    def halt(self):
        self.pwm.ChangeDutyCycle(0)
        gpio.output(self.pins[2], False)


    def __del__(self):
        gpio.cleanup()