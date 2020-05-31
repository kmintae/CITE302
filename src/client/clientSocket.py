from socket import *
from RobotArm import armClient
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
            car.move([float(inst[1]), float(inst[2]), float(inst[3]), float(inst[4])])
        elif (recv_inst_tok[0] == 'GRB'):
            retValue=robotArm.work([recv_inst_tok[1], recv_inst_tok[2]], True)
            if(retValue):
                clientSock.sendall("DONE".encode())
            else:
                clientSock.sendall("ERROR".encode())
        elif (recv_inst_tok[0] == 'RLZ'):
            retValue=robotArm.work([recv_inst_tok[1], recv_inst_tok[2]], False)
            if(retValue):
                clientSock.sendall("DONE".encode())
            else:
                clientSock.sendall("ERROR".encode())


if __name__ == "__main__":
    run_client()