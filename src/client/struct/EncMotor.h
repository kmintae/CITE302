/**
 * EncMotor.h
 * Purpose: Class encapsulates 'Encoder DC Motor'
 * @author Mintae Kim
 */

#ifndef CLIENT_STRUCT_ENCMOTOR_H_
#define CLIENT_STRUCT_ENCMOTOR_H_

#include <Windows.h>
// Reference: http://blog.naver.com/PostView.nhn?blogId=nkkh159&logNo=220772495083
// #include <wiringPi.h>

class EncoderMotor
{
private:
    bool isRotatingCW;
    bool isBreak;
    short pwmPin, dirPin, stopPin, encPin1, encPin2;
    
public:
    EncoderMotor (short pwmPin, short dirPin, short stopPin, short encPin1, short encPin2);
    ~EncoderMotor();

    void setVelocity(bool isRotatingCW, float vel);
};

#endif // CLIENT_STRUCT_ENCMOTOR_H_