import numpy as np
from StepMotor import Motor
import RPi.GPIO as gpio
import math
import operator
import threading
import time
import queue
import yaml

config = yaml.load(open("./ClientConfig.yaml", 'r'), Loader=yaml.FullLoader)

class armClient:

    def __init__(self, dirPins, stpPins,enPins, maxAngles, minAngles, servoPin):
        initPos=config["ROBOTARM_INITIAL_ANGLE"]
        initialPos = calPos(initPos[0], initPos[1])
        self.curAngles = [initPos[0], initPos[1]]
        self.motorList = []
        self.slice = 5000
        self.ratio = 5.0  # ratio of rotation,  second motor rotates robot arm indirect wat
        self.maxAngles=maxAngles
        self.minAngles=minAngles
        self.dirPins=dirPins
        self.stpPins=stpPins
        self.enPins=enPins
        self.stepSize=config["MOTOR_ARM_ANG_PER_STEP"]
        gpio.setmode(gpio.BCM)

        gpio.setup(servoPin, gpio.OUT)
        self.servoPwm = gpio.PWM(servoPin, 50)
        self.servoPwm.start(0)
        gpio.setwarnings(False)

    def setArm(self):
        
        for i in range(0, 2):
            motor = Motor(config["MOTOR_ARM_ANG_PER_STEP"], config["MOTOR_ARM_GEAR"][i], 0.0, 0.0)
            motor.setMotor(self.dirPins[i], self.stpPins[i],self.enPins[i])
            motor.maxAng = self.maxAngles[i]
            motor.minAng = self.minAngles[i]
            motor.curAng = self.curAngles[i]
            self.motorList.append(motor)
        self.motorList[1].curAng = self.curAngles[1] + self.curAngles[0]  

    def move(self,target):
        self.motorList[0].unhalt()
        self.motorList[1].unhalt()
        cur = [self.curAngles[0], self.curAngles[1]]
        print(cur)
        m = self.motorList

        # because the frame of robot arm always makes parallelogram
        mov = [(target[0] - cur[0]), target[1] + target[0] - (cur[1] + cur[0])]
        print(mov)
        stp = [int(mov[0]*m[0].gear/ self.stepSize), int(mov[1]*m[1].gear/ self.stepSize)]
        m[0].curAng += (stp[0] * self.stepSize / m[0].gear)
        m[1].curAng += (stp[1] * self.stepSize / m[1].gear)

        t1 = threading.Thread(target=m[0].move, args=(stp[0] , 5))
        t2 = threading.Thread(target=m[1].move, args=(-stp[1] , 5))
        t1.start()
        t2.start()
        t1.join()
        t2.join()
        self.curAngles = [m[0].curAng, m[1].curAng - m[0].curAng]
        print(calPos(self.curAngles[0],self.curAngles[1]))

    def moveInDirectLine(self, angle_1, angle_2, resol):
        print("direct moving start")
        cur = [self.curAngles[0], self.curAngles[1]]
        print(calPos(cur[0],cur[1]))
        pos1 = calPos(cur[0], cur[1])
        pos2 = calPos(angle_1, angle_2)
        dif = [(pos2[0] - pos1[0]) / resol, (pos2[1] - pos1[1]) / resol]
        m = self.motorList
        before = [cur[0], cur[1]]

        stpArr=[]
        velArr=[]
        len=0
        for i in range(1, resol + 1):
            cur = calAngle(pos1[0] + i * dif[0], pos1[1] + i * dif[1])
            # because the frame of robot arm always makes prallelogram
            mov = [(cur[0] - before[0]), cur[1] + cur[0] - (before[1] + before[0])]
            stp = [int(mov[0]*m[0].gear / self.stepSize), int(mov[1]*m[1].gear/ self.stepSize)]

            if(abs(stp[0])>0 and abs(stp[1])>0):
                len+=1
                stpArr.append(stp)
                v1 = 10
                v2 = 10
                if (abs(stp[0]) > abs(stp[1])):
                    v2 = v1 * abs(stp[1]) / abs(stp[0])
                else:
                    v1 = v2 * abs(stp[0])/ abs(stp[1])
                velArr.append([v1,v2])

            m[0].curAng += (stp[0] * self.stepSize / m[0].gear)
            m[1].curAng += (stp[1] * self.stepSize / m[1].gear)
            before = [m[0].curAng, m[1].curAng - m[0].curAng]
        print("----")
        time.sleep(0.3)
        for i in range(0,len):
            v=velArr[i]
            stp=stpArr[i]
            t1 = threading.Thread(target=m[0].move, args=(stp[0], v[0]))
            t2 = threading.Thread(target=m[1].move, args=(-stp[1], v[1]))
            t1.start()
            t2.start()
            t1.join()
            t2.join()
        self.curAngles = before

    def work(self, target_pos, grap):
        self.motorList[0].unhalt()
        self.motorList[1].unhalt()
        print("workStart")
        cur = [self.curAngles[0], self.curAngles[1]]
        
        #move before work: move to upside of brick, this movement ensures stabillity
        beforeWork=calAngle(target_pos[0],config["ROBOT_ARM_BEFORE_WORK"])
        self.move(beforeWork)

        target = calAngle(target_pos[0], target_pos[1])

        if grap:
            time.sleep(0.5)
            self.servoPwm.ChangeDutyCycle(3)
            time.sleep(0.5)
            self.moveInDirectLine(target[0], target[1], self.slice)
            time.sleep(0.5)
            self.servoPwm.ChangeDutyCycle(12)
            time.sleep(0.5)
            self.moveInDirectLine(beforeWork[0],beforeWork[1],self.slice)
            time.sleep(0.5)
            self.moveToMovingPos()
        else:
            time.sleep(0.5)
            self.moveInDirectLine(target[0], target[1], self.slice)
            time.sleep(0.5)
            self.servoPwm.ChangeDutyCycle(3)
            time.sleep(0.5)
            self.moveInDirectLine(beforeWork[0],beforeWork[1],self.slice)
            time.sleep(0.5)
            self.moveToMovingPos()
        time.sleep(2)

        #print(self.curAngles)

    def moveToMovingPos(self):
        movingPos=config["ROBOT_ARM_MOVING_POSITION"]
        arg=calAngle(movingPos[0],movingPos[1])
        print(arg)
        self.move(arg)

    def halt(self):
        self.motorList[0].halt()
        self.motorList[1].halt()

l1 = config["ROBOTARM_L1"]
l2 = config["ROBOTARM_L2"]

def calPos(a, b):
    # 3 cases
    return [l1 * math.cos(a * math.pi / 180) + l2 * math.cos((a + b - 180) * math.pi / 180),
            l1 * math.sin(a * math.pi / 180) + l2 * math.sin((a + b - 180) * math.pi / 180)]


def calAngle(x, y):
    l3 = math.sqrt(x ** 2 + y ** 2)
    tan = math.atan(y / x)
    # cosine second law,  a3,a2
    a3 = math.acos((l3 ** 2 - l1 ** 2 - l2 ** 2) / (-2.0 * l1 * l2))
    a2 = math.acos((l2 ** 2 - l3 ** 2 - l1 ** 2) / (-2.0 * l1 * l3))
    return [(tan + a2) * 180 / math.pi, a3 * 180 / math.pi]

#test _ drawing square

test = armClient(config["GPIO_ARM_DIRPINS"], config["GPIO_ARM_STPPINS"], config["GPIO_ARM_ENPINS"],
                         config["ROBOTARM_MAX_ANGLES"], config["ROBOTARM_MIN_ANGLES"],config["GPIO_SERVO_PIN"])
ang = calAngle(150.0, -85.0)
print(ang)
print(calPos(ang[0], ang[1]))
tar=calPos(ang[0], ang[1])
test.setArm()
test.move([90.0,90.0])
test.work([tar[0], tar[1]], True)

test.work([200, -85], False)
ang = calAngle(170.0, 0.0)
#test.work([tar[0], tar[1]], False)
