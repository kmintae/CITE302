import sys
import RPi.GPIO as gpio  # https://pypi.python.org/pypi/RPi.GPIO more info
import time
import math
import numpy as np

# Reference : https://www.youtube.com/watch?v=o-j9TReI1aQ&app=desktop

from EncMotor import Motor
import threading
import yaml


# GPIO Pins
## GPIO[0] : Up-LEFT / GPIO[1] : Up-RIGHT
## GPIO[2] : Down-LEFT / GPIO[3] : Down-RIGHT

# Configurations
config = yaml.load(open("./ClientConfig.yaml", 'r'), Loader=yaml.FullLoader)


maxDistControl = config["MAX_MOVE_CONTROL_DIST"] * config["MOVE_KP"]
maxAngleControl = config["MAX_ROTATE_CONTROL_ANGLE"] *config["ROTATION_KP"]

class Platform:
    # Class Constructor
    def __init__(self,pwm_r,pwm_l):
        self.rightMotor=Motor(True,config["RIGHT_WHEEL_PINS"],pwm_r)
        self.leftMotor = Motor(False, config["LEFT_WHEEL_PINS"],pwm_l)
        self.control=[0.0,0.0] # index 0 for right wheel, index 1 for left wheel
        # PID values and allowable error range for moving and rotation
        self.movpid=[config["MOVE_KP"],config["MOVE_KD"],config["MOVE_KI"],config["MOVE_TOLERANCE"]]
        self.rotpid=[config["ROTATION_KP"],config["ROTATION_KD"],config["ROTATION_KI"],config["ROTATION_TOLERANCE"]]
        self.errorAnglePrev=0.0
        self.errorDistPrev = 0.0
        self.timePrev=0.0
        self.target=[0.0,0.0,0.0,0.0]
        self.errorDistIntegral=0.0
        self.errorAngleIntegral=0.0
        self.dir_before=[0.0,0.0]

    def setTarget (self,current,target):
        self.target=target
        self.errorDistPrev=calDist([target[0]-current[0],target[1]-current[1]])
        self.errorAnglePrev=calRotationDegree([current[2],current[3]],[target[2],target[3]])
        self.timePrev=time.time()
        self.errorDistIntegral=0.0
        self.errorAngleIntegral=0.0
        self.dir_before=[0.0,0.0]
    def setPID(self,kp,kd,ki,tol,kp_r,kd_r,ki_r,tol_r):
        self.movpid=[kp,kd,ki,tol]
        self.rotpid=[kp_r,kd_r,ki_r,tol_r]
    def offlineWheelMoving(self,right=True,target=0.0):
        ratio = 360./1326
        Kp = 0.2
        Kd = 0.008
        Ki = 0.03
        dt = 0.
        dt_sleep = 0.001
        tolerance = 30.
        start_time = time.time()
            
        if(right):
            motor=self.rightMotor
            target=target
        else:
            motor=self.leftMotor
            target=-target
            
        error_prev = 0.
        time_prev = 0.
        while True:
            motorDeg = motor.getRotationalPos()* ratio
            error = target - motorDeg
            de = error-error_prev
            dt = time.time() - time_prev
            control = Kp*error + Kd*de/dt + Ki*error*dt;

            error_prev = error
            time_prev = time.time()
            motor.move(-control)
         
            if abs(error) <= tolerance:
                motor.pwm.ChangeDutyCycle(0)
                break
            time.sleep(dt_sleep)
    def offlineMove(self,angle1, angle2):
        t1=threading.Thread(target=self.offlineWheelMoving,args=(True,angle1))
        t2=threading.Thread(target=self.offlineWheelMoving,args=(False,angle2))
        t1.start()
        t2.start()
        time.sleep(1)
        return True
    def rotate(self,current):
       
        target=self.target
        errorAngle = calRotationDegree([current[2], current[3]], [target[2], target[3]])
        de_angle=errorAngle-self.errorAnglePrev
        dt=time.time()-self.timePrev
        self.timePrev=time.time()
        self.errorAngleIntegral+=errorAngle*dt
        control_angle=self.rotpid[0]*errorAngle + self.rotpid[1]*de_angle/dt + self.rotpid[2]*self.errorAngleIntegral;
        right_control= ( control_angle*0.5)
        left_control = (- control_angle * 0.5)
        print([errorAngle,dt,de_angle,self.errorAngleIntegral])
        t1 = threading.Thread(target= self.rightMotor.move, args=(right_control,))
        t2 = threading.Thread(target=self.leftMotor.move, args=(left_control,))
        t1.start()
        t2.start()
        t1.join()
        t2.join()

        self.errorAnglePrev=errorAngle
        
    def move(self, current,back):
        control_angle=0.0
        control_dist=0.0
        errorDist=0.0
        errorAngle=0.0
        if(back):
            target=self.target
            target=[self.target[0],self.target[1],target[0] - current[0], target[1] - current[1]]
            errorDist = calDist([target[0] - current[0], target[1] - current[1]])
            
            errorAngle = calRotationDegree([current[2], current[3]], [target[2], target[3]])
            if(abs(calRotationDegree([target[2],target[3]],[current[2], current[3]]))>90.0):
                errorDist=-errorDist
                if(errorAngle>0.0):
                    errorAngle=errorAngle-180.0
                else:
                    errorAngle=180.0-errorAngle
            de_dist=errorDist-self.errorDistPrev
            de_angle=errorAngle-self.errorAnglePrev
            dt=time.time()-self.timePrev
            self.timePrev=time.time()
            self.errorDistIntegral+=errorDist*dt
            self.errorAngleIntegral+=errorAngle*dt
            print([errorDist,errorAngle])
            if(abs(calRotationDegree([target[2],target[3]],self.dir_before))>8.0 ):
                self.dir_before=[target[2],target[3]]
                self.errorAngleIntegral=0
            if(abs(calRotationDegree([target[2],target[3]],[current[2],current[3]]))>8.0 and abs(calRotationDegree([target[2],target[3]],[current[2],current[3]]))<172.0):
                if(abs(errorDist)>100.0 and errorDist>0.0):
                    
                    control_angle=self.rotpid[0]*errorAngle + self.rotpid[1]*de_angle/dt + self.rotpid[2]*self.errorAngleIntegral;
                    print("+")
                    print(control_angle)
                elif(abs(errorDist)>100.0 and errorDist<0.0):
                    control_angle=self.rotpid[0]*errorAngle + self.rotpid[1]*de_angle/dt + self.rotpid[2]*self.errorAngleIntegral;
                    print("-")
                    print(control_angle)
                else:
                    control_dist=self.movpid[0]*errorDist + self.movpid[1]*de_dist/dt + self.movpid[2]*self.errorDistIntegral;
            else:
                if(abs(errorDist)>150.0):
                    control_dist=self.movpid[0]*errorDist + self.movpid[1]*de_dist/dt + self.movpid[2]*self.errorDistIntegral;
                    print("200")
                    print(control_angle)
                else:
                    control_dist=self.movpid[0]*errorDist + self.movpid[1]*de_dist/dt + self.movpid[2]*self.errorDistIntegral;
            
        else:
            target=self.target
            target=[self.target[0],self.target[1],target[0] - current[0], target[1] - current[1]]
            errorDist = calDist([target[0] - current[0], target[1] - current[1]])
            
            errorAngle = calRotationDegree([current[2], current[3]], [target[2], target[3]])
            if(abs(calRotationDegree([target[2],target[3]],[current[2], current[3]]))>90.0):
                errorDist=-errorDist
            de_dist=errorDist-self.errorDistPrev
            de_angle=errorAngle-self.errorAnglePrev
            dt=time.time()-self.timePrev
            self.timePrev=time.time()
            self.errorDistIntegral+=errorDist*dt
            self.errorAngleIntegral+=errorAngle*dt
            print([errorDist,errorAngle])
            if(abs(calRotationDegree([target[2],target[3]],self.dir_before))>8.0 ):
                self.dir_before=[target[2],target[3]]
                self.errorAngleIntegral=0
            if(abs(calRotationDegree([target[2],target[3]],[current[2],current[3]]))>8.0 and abs(calRotationDegree([target[2],target[3]],[current[2],current[3]]))<172.0):
                if(abs(errorDist)>100.0):
                    control_angle=self.rotpid[0]*errorAngle + self.rotpid[1]*de_angle/dt + self.rotpid[2]*self.errorAngleIntegral;
                    print("+")
                    print(control_angle)
                else:
                    control_dist=self.movpid[0]*errorDist + self.movpid[1]*de_dist/dt + self.movpid[2]*self.errorDistIntegral;
            else:
                if(abs(errorDist)>100.0):
                    control_angle=self.rotpid[0]*errorAngle + self.rotpid[1]*de_angle/dt + self.rotpid[2]*self.errorAngleIntegral;
                    control_dist=self.movpid[0]*errorDist + self.movpid[1]*de_dist/dt + self.movpid[2]*self.errorDistIntegral;
                    print("200")
                    print(control_angle)
                else:
                    control_dist=self.movpid[0]*errorDist + self.movpid[1]*de_dist/dt + self.movpid[2]*self.errorDistIntegral;
            

            
            
        #target=self.target
            #MAXIMUM velocity of robot is 0.5m/s
            # for 1m error PID value becomes about 1000*Kp (=maxControlValue)
            # and we set this as a reference and caculate the ratio (because pwm uses duty cycle( in percentage = max 100%))
            #control_dist=self.movpid[0]*errorDist + self.movpid[1]*de_dist/dt + self.movpid[2]*self.errorDistIntegral;
            #control_angle=self.rotpid[0]*errorAngle + self.rotpid[1]*de_angle/dt + self.rotpid[2]*self.errorAngleIntegral;

        print()
        right_control= (control_dist + control_angle*0.5)
        left_control = (control_dist - control_angle * 0.5)
        t1 = threading.Thread(target= self.rightMotor.move, args=(right_control,))
        t2 = threading.Thread(target= self.leftMotor.move, args=(left_control,))
        t1.start()
        t2.start()
        t1.join()
        t2.join()

        self.errorDistPrev=errorDist
        self.errorAnglePrev=errorAngle
    

    def initialize(self):
        self.errorAnglePrev=0.0
        self.errorPosPrev = 0.0
        self.timePrev=0.0
        self.errorDistIntegral=0.0
        self.errorAngleIntegral=0.0
        self.rightMotor.move(0.0)
        self.leftMotor.move(0.0)

    def halt(self):
        self.initialize()
        self.rightMotor.halt()
        self.leftMotor.halt()

def calDist(pos):
    return (pos[0]**2+pos[1]**2)**0.5


def calRotationDegree(dir1,dir2):
    #make dir1 & dir2 to unit direction vector
    #if rotation angle is positive than it means that the car must rotate ccw to reach dir2
    len1=math.sqrt(dir1[0]**2.0+dir1[1]**2.0)
    len2=math.sqrt(dir2[0]**2.0+dir2[1]**2.0)
    if(len1==0 or len2==0):
        return 0
    dir1= [dir1[0]/len1,dir1[1]/len1]
    dir2 = [dir2[0]/len2,dir2[1]/len2]
    dotProduct=dir1[0]*dir2[0]+dir1[1]*dir2[1]
    if(abs(dotProduct)>1.0):
        return 0.0
    angle =180*(math.acos(dotProduct))/math.pi
    crossProduct=-(dir1[0]*dir2[1]-dir1[1]*dir2[0])
    if(crossProduct>0):
        return -angle
    else:
        return angle
