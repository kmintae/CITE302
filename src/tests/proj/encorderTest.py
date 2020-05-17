import RPi.GPIO as IO
import time

pwmPin = 19
dirPin = 13
encPinA = 23
encPinB = 24
stopPin = 6

IO.setmode(IO.BCM)
IO.setwarnings(False)
IO.setup(encPinA, IO.IN, pull_up_down=IO.PUD_UP)
IO.setup(encPinB, IO.IN, pull_up_down=IO.PUD_UP)
IO.setup(pwmPin,IO.OUT)
IO.setup(dirPin,IO.OUT)
IO.setup(stopPin,IO.OUT)

IO.output(stopPin,False)
p = IO.PWM(19,100)
p.start(0)

encoderPos = 0

def encoderA(channel):
    global encoderPos
    if IO.input(encPinA) == IO.input(encPinB):
        encoderPos += 1
    else:
        encoderPos -= 1
   
def encoderB(channel):
    global encoderPos
    if IO.input(encPinA) == IO.input(encPinB):
        encoderPos -= 1
    else:
        encoderPos += 1

IO.add_event_detect(encPinA, IO.BOTH, callback=encoderA)
IO.add_event_detect(encPinB, IO.BOTH, callback=encoderB)

#​여기서부터는 PID가 들어간 핵심코드입니다.


targetDeg= 1080000.
ratio = 360./2644
Kp = 100.
Kd = 5.
Ki = 0.
dt = 0.
dt_sleep = 0.001
tolerance = 1.





