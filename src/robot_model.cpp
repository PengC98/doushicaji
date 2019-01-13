////////////////////////////////////////////////////////////////////////////////
///Copyright(c)      UESTC MBZIRC     Model Code for UAV
///ALL RIGHTS RESERVED
///@file:control_model.cpp
///@brief: UAV 模型，包含对所有消息的管理，负责与无人机进行信息交互
/// 
///@vesion 1.0
///@author: PC
///@email: 694977655@qq.com
///@date: 18-11-6
///修订历史：
////////////////////////////////////////////////////////////////////////////////
#include <unistd.h>
#include <robot_model.h>
#include <basic_tool.h>

using namespace cv;
using namespace std;

RobotModel::RobotModel() {

}
RobotModel::~RobotModel() {

}

int RobotModel::init(Vehicle* vehicle){
    mRvehicle = vehicle;
    string serialPath,videoPath;
    //配置文件
    cv::FileStorage f("../res/main_config.yaml", cv::FileStorage::READ);
    f["robot_id"] >> mRobotId;//机器人id
    f["serial_path"] >> serialPath;//机器人串口路径
    f["capture_path"] >> videoPath;//机器人摄像头路径
    f.release();
    //初始化串口
    
    if(mSerialInterface.init(mRvehicle)==0) {
        cout<<"[robot model init ]: RobotSerialInterface init successed!"<<endl;
        //mSerialPort.ShowParam();
    } else{
        cout<<"[robot model init ]: RobotSerialInterface init failed!"<<endl;
    }
    if(mRealsense.init(640,480) == 0){
        cout << "[robot model init ]:RealsenseCapture init successed!" <<endl;
        usleep(1000000);
    }
     //初始化摄像头
    // if(mUsbCapture.init("/dev/Video",1280,720)==0){
    //     cout << "[robot model init ]:UsbCapture init successed!" <<endl;
    //     usleep(1000000);//等待1s
    //     //mUsbCapture.infoPrint();
    // }else{
    //     cout << "[robot model init ]:UsbCapture init failed! No UsbCapture" <<endl;
    // }
    mCurrentMode=ROBOT_MODE_EMPTY;

}

unsigned char RobotModel::getRobotId(){
    return mRobotId;
};


UsbCaptureWithThread* RobotModel::getpUsbCapture() {
    return &mUsbCapture;
}

RealsenseInterface* RobotModel::getRealsenseCpature(){
    return &mRealsense;
}

void RobotModel::setCurrentMode(RobotMode robotMode) {
    mCurrentMode=robotMode;
}
SerialInterface* RobotModel::getpSerialInterface(){//add define
    return &mSerialInterface;
}


RobotMode RobotModel::getCurrentMode() {
    return mCurrentMode;
}


//机器人数据接口
void RobotModel::DataUpdate(SerialPacket recv_packet){
    pthread_mutex_lock(&dataMutex);
    mFlightStatus = recv_packet.flightStatus;
    mLatLon = recv_packet.latLon;
    mAltitude = recv_packet.altitude;
    mRc = recv_packet.rc;
    mVelocity = recv_packet.velocity;
    mQuaternion = recv_packet.quaternion;
    pthread_mutex_unlock(&dataMutex);
}
Point3f RobotModel::getCurrentVelocity(){
    Point3f tmp;
    pthread_mutex_lock(&dataMutex);//加锁
    tmp.x = mVelocity.data.x;
    tmp.y = mVelocity.data.y;
    tmp.z = mVelocity.data.z;
    pthread_mutex_unlock(&dataMutex);
}
Point3f RobotModel::getCurrentAngle(){
    Telemetry::Vector3f tmp_;
    Point3f tmp;
    pthread_mutex_lock(&dataMutex);
    tmp_ = basictool.toEulerAngle(mQuaternion);
    tmp.x = tmp_.x;
    tmp.y = tmp_.y;
    tmp.z = tmp_.z;
    pthread_mutex_unlock(&dataMutex);
}
