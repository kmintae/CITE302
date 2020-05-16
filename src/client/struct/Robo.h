/**
 * Robo.h
 * Purpose: Declaration of struct Robot
 * @author Mintae Kim
 */

#ifndef CLIENT_STRUCT_ROBO_H_
#define CLIENT_STRUCT_ROBO_H_

#include "RoboWheels.h"
#include "RoboArm.h"

struct Robot
{
    RobotWheels robotWheels;
    RobotArm robotArm;
};

#endif // CLIENT_STRUCT_ROBO_H_