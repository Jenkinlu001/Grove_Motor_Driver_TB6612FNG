#include "Grove_Motor_Driver_TB6612FNG.h"

MotorDriver::MotorDriver()
{
}

void MotorDriver::init(uint8_t addr)
{
    _addr = addr;
    standby();
}

void MotorDriver::standby()
{
    I2Cdev::writeByte(_addr, GROVE_MOTOR_DRIVER_I2C_CMD_STANDBY, 0);
}

void MotorDriver::notStandby()
{
    I2Cdev::writeByte(_addr, GROVE_MOTOR_DRIVER_I2C_CMD_NOT_STANDBY, 0);
}

void MotorDriver::setI2cAddr(uint8_t addr)
{
    if (addr == 0x00) return;
    else if (addr >= 0x80) return;
    I2Cdev::writeByte(_addr, GROVE_MOTOR_DRIVER_I2C_CMD_SET_ADDR, addr);
    _addr = addr;
    delay(100);
}

void MotorDriver::dcMotorRun(motor_channel_type_t chl, int16_t speed)
{
    if (speed > 255) speed = 255;
    else if (speed < -255) speed = -255;

    if (speed >= 0) _buffer[0] = GROVE_MOTOR_DRIVER_I2C_CMD_CW;
    else _buffer[0] = GROVE_MOTOR_DRIVER_I2C_CMD_CCW;

    _buffer[1] = chl;
    _buffer[2] = abs(speed);

    I2Cdev::writeBytes(_addr, _buffer[0], 2, _buffer+1);
}

void MotorDriver::dcMotorBrake(motor_channel_type_t chl)
{
    I2Cdev::writeByte(_addr, GROVE_MOTOR_DRIVER_I2C_CMD_BRAKE, chl);
}

void MotorDriver::dcMotorStop(motor_channel_type_t chl)
{
    I2Cdev::writeByte(_addr, GROVE_MOTOR_DRIVER_I2C_CMD_STOP, chl);
}

void MotorDriver::stepperRun(stepper_mode_type_t mode, int16_t steps, uint16_t rpm)
{
    uint8_t cw = 0;
    // 0.1ms_per_step
    uint16_t ms_per_step = 0;
    
    if (steps > 0) cw = 1;
    else if (steps == 0) cw = 2;
    else if (steps == -32768) steps=32767;
    else steps = -steps;

    if (rpm < 1) rpm = 1;
    else if (rpm > 300) rpm = 300;

    ms_per_step = (uint16_t)(3000.0 / (float)rpm);
    _buffer[0] = mode;
    _buffer[1] = cw;    //(cw=1) => cw; (cw=0) => ccw
    _buffer[2] = steps;
    _buffer[3] = (steps >> 8);
    _buffer[4] = ms_per_step;
    _buffer[5] = (ms_per_step >> 8);

    I2Cdev::writeBytes(_addr, GROVE_MOTOR_DRIVER_I2C_CMD_STEPPER_RUN, 6, _buffer);
}

void MotorDriver::stepperStop()
{
    I2Cdev::writeByte(_addr, GROVE_MOTOR_DRIVER_I2C_CMD_STEPPER_STOP, 0);
}
