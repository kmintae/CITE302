/**
 * RoboWheels.h
 * Purpose: Direction Control with 4 'Enc-DC Motor's.
 * @author Mintae Kim
 */

#ifndef ROBOWHEELS_H_
#define ROBOWHEELS_H_

#include <pthread.h>
#include <Windows.h>

#include "EncMotor.h"

struct EncoderMotorParameter
{
    EncoderMotor *encMotor;
    bool isRotatingCW;
    float vel;
};

class RobotWheels
{
private:
    EncoderMotorParameter *motorL, *motorR;
    pthread_t threadL, threadR;
    int threadL_id, threadR_id;

    void update();
    void *commit(void *ptr);

public:
    RobotWheels();
    ~RobotWheels();

    void movFront(float vel);
    void movBack(float vel);
    void rotRight(float angVel);
    void rotLeft(float angVel);
    void hiatus(); // For Real-time Instruction Fetching
};

#endif // ROBOWHEELS_H_