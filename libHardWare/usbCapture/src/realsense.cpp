#include "realsense.h"

RealsenseInterface::RealsenseInterface(){
    isOpen = false;

};
RealsenseInterface::~RealsenseInterface(){};
rs2::align align_to(RS2_STREAM_DEPTH);
int RealsenseInterface::init(){

    rs2::config cfg_color;  //新建config类的对象 for color img
    cfg_color.enable_stream(RS2_STREAM_COLOR, color_img_width, color_img_height, RS2_FORMAT_BGR8, 60); //设置color stream 
    rs2::config cfg_depth; //新建config类的对象 for depth img
    cfg_depth.enable_stream(RS2_STREAM_DEPTH, depth_img_width, depth_img_height, RS2_FORMAT_Z16, 60);  //depth stream
    pipe_color.start(cfg_color);
    pipe_depth.start(cfg_depth);
    
    spat.set_option(RS2_OPTION_HOLES_FILL, 5);
    
    
    // If the demo is too slow, make sure you run in Release (-DCMAKE_BUILD_TYPE=Release)
    // but you can also increase the following parameter to decimate depth more (reducing quality)
    dec.set_option(RS2_OPTION_FILTER_MAGNITUDE, 2);
    // Define transformations from and to Disparity domain

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
        std::cerr << "RealSense color error calling " << e.get_failed_function() << "(" << e.get_failed_args() << "):\n    " << e.what() << std::endl;
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
        // depth_data.apply_filter(align_to)
        //             .apply_filter(dec)
        //             .apply_filter(depth2disparity)
        //             .apply_filter(spat)
                    //depth_data.apply_filter(color_map);
        rs2::frame depth = depth_data.get_depth_frame().apply_filter(spat);
        const int w = depth.as<rs2::video_frame>().get_width();
        const int h = depth.as<rs2::video_frame>().get_height();
        Mat depth_tmp(Size(w, h),CV_16UC1,(void*)depth.get_data(),Mat::AUTO_STEP);
        cv::resize(depth_tmp,depth_tmp,cv::Size(depth_img_width,depth_img_height));
        // cv::cvtColor(depth_tmp,depth_tmp,CV_RGB2GRAY);
        // for (int i = 0; i < depth_tmp.rows; ++i) {
        //     for (int j = 0; j < depth_tmp.cols; ++j){
        //         if(depth_tmp.at<u_char>(i, j)<=50){
        //             depth_tmp.at<u_char>(i, j)=0;
        //         } else{
        //             depth_tmp.at<u_char>(i, j)=255;
        //         }
        //     }
        // }
        // cv::cvtColor(depth_tmp,depth_tmp,CV_GRAY2RGB);
        pthread_mutex_lock(&imgMutex);
        depth_tmp.copyTo(depth_img);//写入color_img,加锁
        pthread_mutex_unlock(&imgMutex);
        isDepthImgUpdate = true;
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
        isDepthImgUpdate= false;
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