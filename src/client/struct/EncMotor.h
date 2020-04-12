/**
 * EncMotor.h
 * Purpose: Class encapsulates 'Encoder DC Motor'
 * @author Mintae Kim
 */

#ifndef ENCMOTOR_H_
#define ENCMOTOR_H_

#include <Windows.h>
// Reference: http://blog.naver.com/PostView.nhn?blogId=nkkh159&logNo=220772495083
// #include <wiringPi.h>

class EncoderMotor
{
private:
    bool rotCW;
    bool isBreak;
    short dirPin, stpPin;
    
public:
    EncoderMotor (short dirPin, short stpPin);
    ~EncoderMotor();

    void rotate(float vel);
};

#endif // ENCMOTOR_H_