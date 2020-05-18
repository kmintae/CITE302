import RPi.GPIO as IO
import time
import threading

#dir,pwm,stop,,encA,encB
pinR=[1,12,16,20,21]#right motor
pinL=[19,13,6,5,0]#left motor
#DIR=True #true for right
#DIR=False #false for right

time.sleep(80)
ratio = 360./1326
Kp = 0.3
Kd = 0.015
Ki = 1.0
dt = 0.
dt_sleep = 0.001
tolerance = 8.
encoderPosR = 0
encoderPosL = 0
demoR=[720,-270,720,-270]
demoL=[720,270,720,270]

def encoderRightA(channel):
    global encoderPosR
    if IO.input(pinR[3]) == IO.input(pinR[4]):
        encoderPosR += 1
    else:
        encoderPosR -= 1


def encoderRightB(channel):
    global encoderPosR
    if IO.input(pinR[3]) == IO.input(pinR[4]):
        encoderPosR -= 0
    else:
        encoderPosR += 0

def encoderLeftA(channel):
    global encoderPosL
    if IO.input(pinL[3]) == IO.input(pinL[4]):
        encoderPosL += 1
    else:
        encoderPosL -= 1   

def encoderLeftB(channel):
    global encoderPosL
    if IO.input(pinL[3]) == IO.input(pinL[4]):
        encoderPosL -= 0
    else:
        encoderPosL += 0

IO.setmode(IO.BCM)
IO.setwarnings(False)

IO.setup(pinR[3], IO.IN, pull_up_down=IO.PUD_UP)
IO.setup(pinR[4], IO.IN, pull_up_down=IO.PUD_UP)
IO.setup(pinL[3], IO.IN, pull_up_down=IO.PUD_UP)
IO.setup(pinL[4], IO.IN, pull_up_down=IO.PUD_UP)

IO.add_event_detect(pinR[3], IO.BOTH, callback=encoderRightA)
IO.add_event_detect(pinR[4], IO.BOTH, callback=encoderRightB)
IO.add_event_detect(pinL[3], IO.BOTH, callback=encoderLeftA)
IO.add_event_detect(pinL[4], IO.BOTH, callback=encoderLeftB)


for i in range(0,3):
    IO.setup(pinR[i],IO.OUT)
    IO.setup(pinL[i],IO.OUT)

IO.output( pinR[2], True)
p1 = IO.PWM(pinR[1],1000)
p1.start(100)

IO.output( pinL[2], True)
p2 = IO.PWM(pinL[1],1000)
p2.start(100)


def Motor(motor=True,DIR=True,index=0):
    pins=[]
    if(motor):
        pins=pinR
        p=p1
    else:
        pins=pinL
        p=p2
    print(pins)

    #​여기서부터는 PID가 들어간 핵심코드입니다.
    n=0

    start_time = time.time()
    while True:
        temp=0
        if(motor):
            for i in range(index+1):
                temp=temp+demoR[i]
        else:
            for i in range(index+1):
                temp=temp+demoL[i]
        n=n+1
        target=0.0
        if(motor==DIR):
            targetDeg=temp
        else:
            targetDeg=-temp
        error_prev = 0.
        time_prev = 0.

        while True:
            if(motor):
                motorDeg = encoderPosR * ratio
            else:
                motorDeg = encoderPosL * ratio

            error = targetDeg - motorDeg
            de = error-error_prev
            dt = time.time() - time_prev
            control = Kp*error + Kd*de/dt + Ki*error*dt;

            error_prev = error
            time_prev = time.time()
           
            IO.output(pins[0], control <= 0)
            p.ChangeDutyCycle(min(abs(control), 100))

            #print('P-term = %7.1f, D-term = %7.1f, I-term = %7.1f' %(Kp*error, Kd*de/dt, Ki*de*dt))
            #print('time = %6.3f, enc = %d, deg = %5.1f, err = %5.1f, ctrl = %7.1f' %(time.time()-start_time, encoderPos, motorDeg, error, control))
            #print()
            #print('%f, %f' %(de, dt))
         
            if abs(error) <= tolerance:
                IO.output(pins[0], control >= 0)
                p.ChangeDutyCycle(0)
                break
           
            time.sleep(dt_sleep)
        if(n>0):
            break
    print('time = %6.3f, enc = %d, deg = %5.1f, err = %5.1f, ctrl = %7.1f' %(time.time()-start_time, encoderPosL, motorDeg, error, control))
for i in range (4):
    print(i)
    t1=threading.Thread(target=Motor,args=(True,True,i,))
    t2=threading.Thread(target=Motor,args=(False,True,i,))

    t1.start()
    t2.start()
    time.sleep(3)


StepPins = [2,3,4]#motor right
#StepPins = [2,17,27]#motor left


# Set all pins as output
for pin in StepPins:
  IO.setup(pin,IO.OUT)

IO.output(StepPins[1], False)

# Define advanced sequence
# as shown in manufacturers datasheet
IO.output(StepPins[0], True)
# Initialise variables
for i in range(1,200):
    print(i)
    IO.output(StepPins[2], True)
    time.sleep(0.0017)
    IO.output(StepPins[2], False)
IO.output(StepPins[0], False)

StepPins = [2,17,27]
for pin in StepPins:
  IO.setup(pin,IO.OUT)
IO.output(StepPins[1], False)

# Define advanced sequence
# as shown in manufacturers datasheet
IO.output(StepPins[0], True)
# Initialise variables
for i in range(1,300):
    print(i)
    IO.output(StepPins[2], True)
    time.sleep(0.0017)
    IO.output(StepPins[2], False)
IO.output(StepPins[0], False)



