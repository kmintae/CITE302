import numpy as np
import RPi.GPIO as gpio
import math
import operator
import threading
import time
import queue
import yaml
servoPin=22
gpio.setwarnings(False)
gpio.setmode(gpio.BCM)
gpio.setup(servoPin, gpio.OUT)
servoPwm = gpio.PWM(servoPin, 100)
servoPwm.start(0)
servoPwm.ChangeDutyCycle(9.5)