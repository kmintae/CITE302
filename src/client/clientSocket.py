from socket import *
from RobotArm import *
from Car import Platform
import yaml
import time
import RPi.GPIO as gpio 

# from C_Motor.Car import Car

# Configurations
config = yaml.load(open("./ClientConfig.yaml", 'r'), Loader=yaml.FullLoader)
gpio.setmode(gpio.BCM)
pin1=config["RIGHT_WHEEL_PINS"][1]
pin2=config["LEFT_WHEEL_PINS"][1]
servo=config["GPIO_SERVO_PIN"]

gpio.setmode(gpio.BCM)
gpio.setup(pin1, gpio.OUT)
gpio.setup(pin2, gpio.OUT)
gpio.setup(servo, gpio.OUT)
pwm_r=gpio.PWM(pin1,1000)
pwm_l=gpio.PWM(pin2,1000)
servopwm=gpio.PWM(servo,100 )

car = Platform(pwm_r,pwm_l)
robotArm = armClient(config["GPIO_ARM_DIRPINS"], config["GPIO_ARM_STPPINS"],config["GPIO_ARM_ENPINS"],
                            config["ROBOTARM_MIN_ANGLES"],servopwm)
robotArm.setArm()
robotArm.moveToMovingPos()
def run_client():
    phase=0
    back=False
    while(True):
        try:
            
            clientSock = socket(AF_INET, SOCK_STREAM)
            print("start tcp connection")
            clientSock.connect((config["SERVER_IP_ADDR"], config["SERVER_PORT"]))
            print("server connected")
            clientSock.sendall(config["ROBOT_NUMBER"].encode())


            #instruction type: HLT, MOV, DCN, GRB, RLZ, CAL
            # Client Flow 1 : Iteration with While Loop, Executing action for robot arm instructions
            while (True):
                recv_inst = clientSock.recv(config["MAX_BUF_SIZE"]).decode("UTF-8")
                inst = recv_inst.split(' ')

                if (inst[0] == 'HLT'):
                    print("hlt")
                    car.halt()
                    #robotArm.halt()
                    clientSock.sendall("DONE".encode())
                    print("hlt")
                elif (inst[0] == 'SET'):
                    print("set")
                    print(inst)
                    car.setPID(float(inst[1]),float(inst[2]),float(inst[3]),float(inst[4]),float(inst[5]),float(inst[6]),float(inst[7]),float(inst[8]))
                    clientSock.sendall("DONE".encode())
                elif (inst[0] == 'PID'):
                    if(phase==1):
                        back=True
                        phase=0
                    else:
                        back=False
                    print("pid")
                    print(inst)
                    #[cur pos, cur dir],[ tar pos, tar dir]
                    car.initialize()
                    car.setTarget([float(inst[1]),float(inst[2]),float(inst[3]),float(inst[4])],[float(inst[5]),float(inst[6]),float(inst[7]),float(inst[8])])
                    clientSock.sendall("DONE".encode())
                elif (inst[0] == 'MOV'):
                    
                    #cur pos, cur dir]
                    print("mov")
                    print(inst)
                    car.move([float(inst[1]), float(inst[2]), float(inst[3]), float(inst[4])],back)
                    clientSock.sendall("DONE".encode())
                elif (inst[0] == 'MVL'):
                    print("mvl")
                    #[cur pos, cur dir]
                    print(inst)
                    car.rotate([float(inst[1]), float(inst[2]), float(inst[3]), float(inst[4])])
                    clientSock.sendall("DONE".encode())
                    
                elif (inst[0] == 'GRB'):
                    phase=1
                    print("grb")
                    print(inst)
                    retValue=robotArm.work([float(inst[1])-config["ROBOT_ARM_GRIPPER_DIST"], float(inst[2])+config["ROBOT_ARM_GRIPPER_HEIGHT"]-config["ROBOT_ARM_CENTER_HEIGHT"]], True)
                    if(retValue):
                        clientSock.sendall("DONE".encode())
                    else:
                        clientSock.sendall("ERROR".encode())
                elif (inst[0] == 'RLZ'):
                    phase=1
                    print("rlz")
                    retValue=robotArm.work([float(inst[1])-config["ROBOT_ARM_GRIPPER_DIST"], float(inst[2])+config["ROBOT_ARM_GRIPPER_HEIGHT"]-config["ROBOT_ARM_CENTER_HEIGHT"]], False)
                    if(retValue):
                        clientSock.sendall("DONE".encode())
                    else:
                        clientSock.sendall("ERROR".encode())
        except error:
            car.rightMotor.pwm.ChangeDutyCycle(0)
            car.leftMotor.pwm.ChangeDutyCycle(0)
            print("try again")
            time.sleep(0.5)
            continue
        


if __name__ == "__main__":
    run_client()
    
# below codes are for tests
#time.sleep(60)
'''
robotArm = armClient(config["GPIO_ARM_DIRPINS"], config["GPIO_ARM_STPPINS"],config["GPIO_ARM_ENPINS"],
                            config["ROBOTARM_MIN_ANGLES"],servopwm)
robotArm.setArm()
ang = calAngle(config["ROBOTARM_INITIAL_POSITION"][0], config["ROBOTARM_INITIAL_POSITION"][1])
print(ang)
ang = calAngle(200,-83.458)
print(calPos(ang[0], ang[1]))
tar=calPos(ang[0], ang[1])
robotArm.setArm()


robotArm.work([tar[0], tar[1]], False)
#testCar.offlineMove(720.0,720.0)
time.sleep(1)
#testCar.offlineMove(720.0-360.0,720.0+360.0)
time.sleep(1)
#testCar.offlineMove(720.0-360.0+720,720.0+360.0+720)
time.sleep(1)
#testArm.work([200, -85], False)
#time.sleep(1)
robotArm.move(calAngle(config["ROBOTARM_INITIAL_POSITION"][0], config["ROBOTARM_INITIAL_POSITION"][1]))

'''