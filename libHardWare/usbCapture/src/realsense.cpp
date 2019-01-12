#include "realsense.h"

RealsenseInterface::RealsenseInterface(){
    isOpen = false;

};
RealsenseInterface::~RealsenseInterface(){};

int RealsenseInterface::init(){

    rs2::config cfg_color;  //新建config类的对象 for color img
    cfg_color.enable_stream(RS2_STREAM_COLOR, color_img_width, color_img_height, RS2_FORMAT_BGR8, 60); //设置color stream 
    rs2::config cfg_depth; //新建config类的对象 for depth img
    cfg_depth.enable_stream(RS2_STREAM_DEPTH, depth_img_width, depth_img_height, RS2_FORMAT_Z16, 60);  //depth stream
    pipe_color.start(cfg_color);
    pipe_depth.start(cfg_depth);
    return 0;
}

int RealsenseInterface::init(int width, int height){
    color_img_height = height;
    color_img_width = width;
    depth_img_height = height;
    depth_img_width = width;
    int statue  = init();
    isOpen = true;
    start();
    return  statue; 
}

int RealsenseInterface::readColorImg(){
    try{
        rs2::frameset color_data = pipe_color.wait_for_frames();
        rs2::frame color = color_data.get_color_frame();
        Mat color_tmp(Size(color_img_width, color_img_height),CV_8UC3,(void*)color.get_data(),Mat::AUTO_STEP);
        pthread_mutex_lock(&imgMutex);
        color_tmp.copyTo(color_img);//写入color_img,加锁
        pthread_mutex_unlock(&imgMutex);
        isColorImgUpdate = true;
        return EXIT_SUCCESS;
    }
    catch (const rs2::error & e)
    {
        std::cerr << "RealSense error calling " << e.get_failed_function() << "(" << e.get_failed_args() << "):\n    " << e.what() << std::endl;
        return EXIT_FAILURE;
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }
}

int RealsenseInterface::readDepthImg(){
    try{
        rs2::frameset depth_data = pipe_depth.wait_for_frames();
        rs2::frame depth = depth_data.get_color_frame();
        Mat depth_tmp(Size(depth_img_width, depth_img_height),CV_16UC1,(void*)depth.get_data(),Mat::AUTO_STEP);
        pthread_mutex_lock(&imgMutex);
        depth_tmp.copyTo(depth_img);//写入color_img,加锁
        pthread_mutex_unlock(&imgMutex);
        return EXIT_SUCCESS;
    }
    catch (const rs2::error & e)
    {
        std::cerr << "RealSense error calling " << e.get_failed_function() << "(" << e.get_failed_args() << "):\n    " << e.what() << std::endl;
        return EXIT_FAILURE;
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }
}

int RealsenseInterface::getColorImg(Mat &img){
    if(!isColorImgUpdate){
        //等待100ms
        int timeCounter=0;
        while(!isColorImgUpdate&&timeCounter<100){
            usleep(1000);//1ms等待
            timeCounter++;
        }
        if(!isColorImgUpdate){
            return -3;//更新超时
        }
    }
    pthread_mutex_lock(&imgMutex);
    color_img.copyTo(img);//读mImg,加锁
    pthread_mutex_unlock(&imgMutex);
    if(!img.empty()&&(img.cols==color_img_width)&&(img.rows==color_img_height)){
        isColorImgUpdate= false;
        return 0;
    } else{
        return -1;
    }

}

int RealsenseInterface::getDepthImg(Mat &img){
    if(!isDepthImgUpdate){
        //等待100ms
        int timeCounter=0;
        while(!isDepthImgUpdate&&timeCounter<100){
            usleep(1000);//1ms等待
            timeCounter++;
        }
        if(!isDepthImgUpdate){
            return -3;//更新超时
        }
    }
    pthread_mutex_lock(&imgMutex);
    depth_img.copyTo(img);//读mImg,加锁
    pthread_mutex_unlock(&imgMutex);
    if(!img.empty()&&(img.cols==depth_img_width)&&(img.rows==depth_img_height)){
        isColorImgUpdate= false;
        return 0;
    } else{
        return -1;
    }

}

void RealsenseInterface::run(){
    while(true){
        readColorImg();
        readDepthImg();
    }
}