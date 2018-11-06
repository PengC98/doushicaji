#ifndef RMDEMO_CONTROL_MODEL_H
#define RMDEMO_CONTROL_MODEL_H
#include "opencv2/opencv.hpp"
#include "robot_model.h"
#include "auto_aim.h"

class ControlModel{
public:
    ControlModel();
    ~ControlModel();

public:
    void init(RobotModel* pRobotModel);
    //串口监听数据处理函数接口
    void serialListenDataProcess(SerialPacket recvPacket);
    void processFSM();

private:
    //机器人临时模式变量
    RobotMode mSetMode;

    AutoAim *autoAim;
private:
    RobotModel* pRobotModel;
    //相关临时变量
private:

};

#endif //RMDEMO_CONTROL_MODEL_H
