import sys
import RPi.GPIO as gpio  # https://pypi.python.org/pypi/RPi.GPIO more info
import time
import math

# Reference : https://www.youtube.com/watch?v=o-j9TReI1aQ&app=desktop

import EncMotor
import threading
import yaml


# GPIO Pins
## GPIO[0] : Up-LEFT / GPIO[1] : Up-RIGHT
## GPIO[2] : Down-LEFT / GPIO[3] : Down-RIGHT

# Configurations
config = yaml.load(open("./Config.yaml", 'r'), Loader=yaml.FullLoader)


maxDistControl = config["MAX_MOVE_CONTROL_DIST"] * config["MOVE_KP"]
maxAngleControl = config["MAX_ROTATE_CONTROL_ANGLE"] *config["ROTATION_KP"]

class Car:
    # Class Constructor
    def __init__(self):
        self.rightMotor=EncMotor(True,config["RIGHT_WHEEL_PINS"])
        self.LeftMotor = EncMotor(False, config["LEFT_WHEEL_PINS"])
        self.control=[0.0,0.0] # index 0 for right wheel, index 1 for left wheel
        # PID values and allowable error range for moving and rotation
        self.movpid=[config["MOVE_KP"],config["MOVE_KD"],config["MOVE_KI"],config["MOVE_TOLERANCE"]]
        self.rotpid=[config["ROTATION_KP"],config["ROTATION_KD"],config["ROTATION_KI"],config["ROTATION_TOLERANCE"]]
        self.errorAnglePrev=0.0
        self.errorPosPrev = 0.0
        self.timePrev=0.0

    def setTarget (self, target,current):
        self.errorDistPrev=calDist([target[0]-current[0],target[1]-current[1]])
        self.errorAnglePrev=calRotationDegree([current[2],current[3],0],[target[2],target[3],0])
        self.timePrev=time.time()

    def move(self, target, current, delay):
        errorDist = calDist([target[0] - current[0], target[1] - current[1]])
        errorAngle = calRotationDegree([current[2], current[3], 0], [target[2], target[3], 0])
        de_dist=errorDist-self.errorDistPrev
        de_angle=errorAngle-self.errorAnglePrev
        dt=time.time()-self.timePrev
        control_dist=self.movpid[0]*errorDist + self.movpid[1]*de_dist/dt + self.movpid[2]*errorDist*dt;
        control_angle=self.rotpid[0]*errorAngle + self.rotpid[1]*de_angle/dt + self.rotpid[2]*errorAngle*dt;

        #MAXIMUM velocity of robot is 0.5m/s
        # for 1m error PID value becomes about 1000*Kp (=maxControlValue)
        # and we set this as a reference and caculate the ratio (because pwm uses duty cycle( in percentage = max 100%))

        right_control= (control_dist/maxDistControl + control_angle*0.5/maxAngleControl)
        left_control = (control_dist / maxDistControl - control_angle * 0.5 / maxAngleControl)
        self.rightMotor.move(right_control)
        self.leftMotor.move(left_control)

        self.timePrev=time.time()
        self.errorDistPrev=errorDist
        self.errorAnglePrev=errorAngle

    def initialize(self):
        self.rightMotor.set()
        self.leftMotor.set()
        self.errorAnglePrev=0.0
        self.errorPosPrev = 0.0
        self.timePrev=0.0

def calDist(pos):
    return (pos[0]**2+pos[1]**2)**0.5
def calRotationDegree(dir1,dir2):
    #make dir1 & dir2 to unit direction vector
    #if rotation angle is positive than it means that the car must rotate ccw to reach dir2
    dir1= dir1 /(np.linalg.norm(dir1))
    dir2 = (dir2 / (np.linalg.norm(dir2)))
    dotProduct=(dir1*dir2)
    angle =180*(math.acos(dotProduct[0]+dotProduct[1]))/math.pi

    crossProduct=np.cross(dir1,dir2)
    if(crossProduct[2]>0):
        return angle
    else:
        return -angle
