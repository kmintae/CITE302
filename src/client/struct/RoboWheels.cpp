/**
 * RoboWheels.cpp
 * Purpose: Direction Control with 4 'Enc-DC Motor's.
 * @author Mintae Kim
 */

#include "RoboWheels.h"

RobotWheels::RobotWheels()
{
    this->motorL = new EncoderMotorParameter;
    this->motorR = new EncoderMotorParameter;
    this->motorL->isRotatingCW = this->motorR->isRotatingCW = false;
    this->motorL->vel = this->motorR->vel = 0.0;

    // TODO: Fetch .ini file
    this->motorL->encMotor = new EncoderMotor();
    this->motorR->encMotor = new EncoderMotor();

    update();
}

RobotWheels::~RobotWheels()
{
    this->motorL->vel = this->motorR->vel = 0;
    update();
    pthread_kill(threadL, 0);
    pthread_kill(threadR, 0);

    free(this->motorL->encMotor);
    free(this->motorR->encMotor);
    free(this->motorL);
    free(this->motorR);
}

void RobotWheels::movFront(float vel)
{
    this->motorL->vel = this->motorR->vel = vel;
    this->motorL->isRotatingCW = this->motorR->isRotatingCW = false;
    update();
}
void RobotWheels::movBack(float vel)
{
    this->motorL->vel = this->motorR->vel = vel;
    this->motorL->isRotatingCW = this->motorR->isRotatingCW = true;
    update();
}
void RobotWheels::rotRight(float angVel)
{
    this->motorL->vel = this->motorR->vel = angVel;
    this->motorL->isRotatingCW = true;
    this->motorR->isRotatingCW = false;
    update();
}
void RobotWheels::rotLeft(float angVel)
{
    this->motorL->vel = this->motorR->vel = angVel;
    this->motorL->isRotatingCW = false;
    this->motorR->isRotatingCW = true;
    update();
}
// For Real-time Instruction Fetching
void RobotWheels::hiatus()
{
    this->motorL->vel = this->motorR->vel = 0;
    update();
}

void RobotWheels::update()
{
    this->threadL_id = pthread_create(&threadL, NULL, this->commit, (void *)(this->motorL));
    this->threadR_id = pthread_create(&threadR, NULL, this->commit, (void *)(this->motorR));
}
void *RobotWheels::commit(void *ptr)
{
    EncoderMotorParameter *ptr_arg = (EncoderMotorParameter *)ptr;
    ptr_arg->encMotor->setVelocity(ptr_arg->isRotatingCW, ptr_arg->vel);
}