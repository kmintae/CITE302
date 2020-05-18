import sys
import time
import RPi.GPIO as GPIO

GPIO.setwarnings(False)
# Use BCM GPIO references
# instead of physical pin numbers
GPIO.setmode(GPIO.BCM)
 
# Define GPIO signals to use
# Physical pins 11,15,16,18
# GPIO17,GPIO22,GPIO23,GPIO24

# EN, DIR, STP
StepPins = [2,3,4]#motor right
#StepPins = [2,17,27]#motor left


# Set all pins as output
for pin in StepPins:
  GPIO.setup(pin,GPIO.OUT)

GPIO.output(StepPins[1], True)

# Define advanced sequence
# as shown in manufacturers datasheet
GPIO.output(StepPins[0], True)
# Initialise variables
for i in range(1,200):
    print(i)
    GPIO.output(StepPins[2], True)
    time.sleep(0.0017)
    GPIO.output(StepPins[2], False)
GPIO.output(StepPins[0], False)

StepPins = [2,17,27]
for pin in StepPins:
  GPIO.setup(pin,GPIO.OUT)
GPIO.output(StepPins[1], True)

# Define advanced sequence
# as shown in manufacturers datasheet
GPIO.output(StepPins[0], True)
# Initialise variables
for i in range(1,300):
    print(i)
    GPIO.output(StepPins[2], True)
    time.sleep(0.0017)
    GPIO.output(StepPins[2], False)
GPIO.output(StepPins[0], False)


