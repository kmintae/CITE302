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

    def __init__(self, dirPins, stpPins,enPins, minAngles, servoPin):
        initPos=config["ROBOTARM_INITIAL_POSITION"]
        self.curAngles = calAngle(initPos[0], initPos[1])
        self.motorList = []
        self.slice = 5000
        self.ratio = 5.0  # ratio of rotation,  second motor rotates robot arm indirect wat
        self.maxAngles=self.curAngles
        self.minAngles=minAngles
        self.dirPins=dirPins
        self.stpPins=stpPins
        self.enPins=enPins
        self.stepSize=config["MOTOR_ARM_ANG_PER_STEP"]
        gpio.setmode(gpio.BCM)

        gpio.setup(servoPin, gpio.OUT)
        self.servoPwm = gpio.PWM(servoPin,100 )
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
        m = self.motorList

        # because the frame of robot arm always makes parallelogram
        mov = [(target[0] - cur[0]), target[1] + target[0] - (cur[1] + cur[0])]
        stp = [int(mov[0]*m[0].gear/ self.stepSize), int(mov[1]*m[1].gear/ self.stepSize)]
        m[0].curAng += (stp[0] * self.stepSize / m[0].gear)
        m[1].curAng += (stp[1] * self.stepSize / m[1].gear)

        t1 = threading.Thread(target=m[0].move, args=(stp[0] , 4))
        t2 = threading.Thread(target=m[1].move, args=(-stp[1] , 4))
        t1.start()
        t2.start()
        t1.join()
        t2.join()
        self.curAngles = [m[0].curAng, m[1].curAng - m[0].curAng]

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
            if(not self.inAngle(cur)):
                return False
            if(abs(stp[0])>0 and abs(stp[1])>0):
                len+=1
                stpArr.append(stp)
                v1 = 60
                v2 = 60
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
        return True

    def work(self, target_pos, grap):
        self.motorList[0].unhalt()
        self.motorList[1].unhalt()
        print("workStart")
        cur = [self.curAngles[0], self.curAngles[1]]
        
        #move before work: move to upside of brick, this movement ensures stabillity
        beforeWork=calAngle(target_pos[0],config["ROBOT_ARM_BEFORE_WORK"])
        self.move(beforeWork)
        target = calAngle(target_pos[0], target_pos[1])
        if(not self.inPos(target_pos) or not self.inAngle(target)):
            return False
        
        if grap:
            time.sleep(0.3)
            self.servoPwm.ChangeDutyCycle(3)
            time.sleep(0.3)
            ret=self.moveInDirectLine(target[0], target[1], self.slice)
            if(not ret):
                return False
            time.sleep(0.3)
            self.servoPwm.ChangeDutyCycle(9.8)
            time.sleep(0.3)
            self.moveInDirectLine(beforeWork[0],beforeWork[1],self.slice)
            time.sleep(0.3)
            self.moveToMovingPos()
        else:
            time.sleep(0.3)
            ret=self.moveInDirectLine(target[0], target[1], self.slice)
            if(not ret):
                return False
            time.sleep(0.3)
            self.servoPwm.ChangeDutyCycle(3)
            time.sleep(0.3)
            self.moveInDirectLine(beforeWork[0],beforeWork[1],self.slice)
            time.sleep(0.3)
            self.moveToMovingPos()
            time.sleep(0.3)
            self.servoPwm.ChangeDutyCycle(9.8)
        time.sleep(1)
        return True
        #print(self.curAngles)

    def moveToMovingPos(self):
        movingPos=config["ROBOT_ARM_MOVING_POSITION"]
        arg=calAngle(movingPos[0],movingPos[1])
        print(arg)
        self.move(arg)
        
    def inAngle(self,angles):
        if(angles[0]>self.maxAngles[0] or angles[1]>self.maxAngles[1]):
            return False
        return True
    
    def inPos(self,pos):
        if(math.sqrt(pos[0]**2+pos[1]**2)>config["ROBOTARM_MAX_WORKING_DIST"]):
            return False
        return True
        
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

ang = calAngle(170.0, 0.0)
#test.work([tar[0], tar[1]], False)
