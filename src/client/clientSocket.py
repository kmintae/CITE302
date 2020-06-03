from socket import *
from RobotArm import *
from Car import Platform
import yaml
import time

# from C_Motor.Car import Car

# Configurations
config = yaml.load(open("./ClientConfig.yaml", 'r'), Loader=yaml.FullLoader)

def run_client():
    clientSock = socket(AF_INET, SOCK_STREAM)
    clientSock.connect((config["SERVER_IP_ADDR"], config["SERVER_PORT"]))

    car = Platform()
    robotArm = armClient(config["GPIO_ARM_DIRPINS"], config["GPIO_ARM_STPPINS"],config["GPIO_ARM_ENPINS"],
                         config["ROBOTARM_MAX_ANGLES"],config["ROBOTARM_MIN_ANGLES"],config["GPIO_SERVO_PIN"])
    robotArm.setArm()

    #instruction type: HLT, MOV, DCN, GRB, RLZ, CAL
    # Client Flow 1 : Iteration with While Loop, Executing action for robot arm instructions
    while (True):
        recv_inst = clientSock.recv(config["MAX_BUF_SIZE"]).decode()
        inst = recv_inst.split(' ')

        if (recv_inst_tok[0] == 'HLT'):
            car.halt()
            robotArm.halt()
        elif (recv_inst_tok[0] == 'PID'):
            #[cur pos, cur dir],[ tar pos, tar dir]
            car.initialize()
            car.setTarget([float(inst[1]),float(inst[2]),float(inst[3]),float(inst[4])],[float(inst[5]),float(inst[6]),float(inst[7]),float(inst[8])])
            clientSock.sendall("DONE".encode())
        elif (recv_inst_tok[0] == 'MOV'):
            #[cur pos, cur dir]
            car.move([float(inst[1]), float(inst[2]), float(inst[3]), float(inst[4])],False)
        elif (recv_inst_tok[0] == 'MVL'):
            #[cur pos, cur dir]
            car.move([float(inst[1]), float(inst[2]), float(inst[3]), float(inst[4])],True)
        elif (recv_inst_tok[0] == 'GRB'):
            retValue=robotArm.work([recv_inst_tok[1]-config["ROBOT_ARM_GRIPPER_DIST"], recv_inst_tok[2]-config["ROBOT_ARM_GRIPPER_HEIGHT"]], True)
            if(retValue):
                clientSock.sendall("DONE".encode())
            else:
                clientSock.sendall("ERROR".encode())
        elif (recv_inst_tok[0] == 'RLZ'):
            retValue=robotArm.work([recv_inst_tok[1]-config["ROBOT_ARM_GRIPPER_DIST"], recv_inst_tok[2]-config["ROBOT_ARM_GRIPPER_HEIGHT"]], False)
            if(retValue):
                clientSock.sendall("DONE".encode())
            else:
                clientSock.sendall("ERROR".encode())


#if __name__ == "__main__":
    #run_client()
    
# below codes are for tests
#time.sleep(60)
testCar = Platform()
testArm=armClient(config["GPIO_ARM_DIRPINS"], config["GPIO_ARM_STPPINS"], config["GPIO_ARM_ENPINS"], config["ROBOTARM_MIN_ANGLES"],config["GPIO_SERVO_PIN"])
ang = calAngle(config["ROBOTARM_INITIAL_POSITION"][0], config["ROBOTARM_INITIAL_POSITION"][1])
print(ang)
ang = calAngle(200, -83.458)
print(calPos(ang[0], ang[1]))
tar=calPos(ang[0], ang[1])
testArm.setArm()
testCar.initialize()

testArm.work([tar[0], tar[1]], False)
#testCar.offlineMove(720.0,720.0)
time.sleep(1)
#testCar.offlineMove(720.0-360.0,720.0+360.0)
time.sleep(1)
#testCar.offlineMove(720.0-360.0+720,720.0+360.0+720)
time.sleep(1)
#testArm.work([200, -85], False)
#time.sleep(1)
testArm.move(calAngle(config["ROBOTARM_INITIAL_POSITION"][0], config["ROBOTARM_INITIAL_POSITION"][1]))