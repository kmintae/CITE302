import RPi.GPIO as IO
import time
import threading

#dir,pwm,stop,,encA,encB
pinR=[1,12,16,20,21]#right motor
pinL=[19,13,6,5,0]#left motor
#DIR=True #true for right
#DIR=False #false for right

ratio = 360./1326
Kp = 0.2
Kd = 0.015
Ki = 0.2
dt = 0.
dt_sleep = 0.001
tolerance = 2.

encoderPosR = 0
encoderPosL = 0

def encoderRightA(channel):
    global encoderPosR
    if IO.input(pinR[3]) == IO.input(pinR[4]):
        encoderPosR += 1
    else:
        encoderPosR -= 1

def encoderLeftA(channel):
    global encoderPosL
    if IO.input(pinL[3]) == IO.input(pinL[4]):
        encoderPosL += 1
    else:
        encoderPosL -= 1   


IO.setmode(IO.BCM)
IO.setwarnings(False)

IO.setup(pinR[3], IO.IN, pull_up_down=IO.PUD_UP)
IO.setup(pinR[4], IO.IN, pull_up_down=IO.PUD_UP)
IO.setup(pinL[3], IO.IN, pull_up_down=IO.PUD_UP)
IO.setup(pinL[4], IO.IN, pull_up_down=IO.PUD_UP)

IO.add_event_detect(pinR[3], IO.BOTH, callback=encoderRightA)
IO.add_event_detect(pinL[3], IO.BOTH, callback=encoderLeftA)


def Motor(DIR=True):
    pins=[]
    if(DIR):
        pins=pinR
    else:
        pins=pinL
    print(pins)

    for i in range(0,3):
        IO.setup(pins[i],IO.OUT)


    IO.output( pins[2], True)
    p = IO.PWM(pins[1],1000)
    p.start(100)

    #​여기서부터는 PID가 들어간 핵심코드입니다.
    n=0
    target=  0.0
    start_time = time.time()
    while True:
        n=n+1
        target=  480.+target
        if(DIR):
            targetDeg=target
        else:
            targetDeg=-target
        error_prev = 0.
        time_prev = 0.

        while True:
            if(DIR):
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
        time.sleep(1)
        if(n>0):
            break
    IO.output(pins[2] , False)

t1=threading.Thread(target=Motor,args=(True,))
t2=threading.Thread(target=Motor,args=(False,))

t1.start()
t2.start()


