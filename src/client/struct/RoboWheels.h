/**
 * RoboWheels.h
 * Purpose: Direction Control with 4 'Enc-DC Motor's.
 * @author Mintae Kim
 */

#ifndef CLIENT_STRUCT_ROBOWHEELS_H_
#define CLIENT_STRUCT_ROBOWHEELS_H_

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
public:
    RobotWheels();
    ~RobotWheels();

    void movFront(float vel);
    void movBack(float vel);
    void rotRight(float angVel);
    void rotLeft(float angVel);
    void hiatus(); // For Real-time Instruction Fetching

    void *commit(void *ptr);

};

#endif // CLIENT_STRUCT_ROBOWHEELS_H_