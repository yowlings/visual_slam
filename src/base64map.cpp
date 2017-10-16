#include<ros/ros.h> //ros标准库头文件
#include<iostream> //C++标准输入输出库
/*
  cv_bridge中包含CvBridge库
*/
#include<cv_bridge/cv_bridge.h>
/*
  ROS图象类型的编码函数
*/
#include<sensor_msgs/image_encodings.h>
/*
   image_transport 头文件用来在ROS系统中的话题上发布和订阅图象消息
*/
#include<image_transport/image_transport.h>

#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <string>
#include <vector>
#include "std_msgs/String.h"
#include "nav_msgs/OccupancyGrid.h"
using namespace cv;
using namespace std;

//dark gray  100 100 100
//white   240 240 240
//green  121  213  117
//dark red  0 0 204
string baseStr="AA%%";
string encode(const unsigned char* Data,int DataByte)  ;

void mapCB(const nav_msgs::OccupancyGrid& msg);

int main(int argc,char** argv){
  ros::init(argc, argv, "base64map");
  ros::NodeHandle nh;
  ros::Publisher bas64_pub = nh.advertise<std_msgs::String>("/base64map/img", 1);
  ros::Subscriber map_sub = nh.subscribe("/map", 1 ,mapCB);

  ros::spin();
  ros::Rate loop_rate(1);
  while(ros::ok()){
    std_msgs::String mapstr;
    mapstr.data = baseStr;
    bas64_pub.publish(mapstr);
    loop_rate.sleep();

  }

  return 0;

}
void mapCB(const nav_msgs::OccupancyGrid& msg){
  Mat mat(msg.info.width,msg.info.height,CV_8UC3);

  for(int i=0;i<mat.rows;i++){
    for(int j=0;j<mat.cols;j++){
      if(msg.data[i*msg.info.width+j]==-1){
        mat.at<Vec3b>(i,j)[0] = 0;
        mat.at<Vec3b>(i,j)[1] = 0;
        mat.at<Vec3b>(i,j)[2] = 200;
      }else{
        mat.at<Vec3b>(i,j)[0] = msg.data[i*msg.info.width+j];
        mat.at<Vec3b>(i,j)[1] = msg.data[i*msg.info.width+j];
        mat.at<Vec3b>(i,j)[2] = msg.data[i*msg.info.width+j];


      }
    }
//    cout<<"yes"<<endl;

  }
//	cout<<"rows: "<<mat.rows<<endl;
//	cout<<"cols: "<<mat.cols<<endl;
//	cout<<"channels: "<<mat.channels()<<endl;

//	imwrite("ggg.png",mat);
//  cout<<"yes"<<endl;
//  imshow("cv",mat);
//  waitKey(2000);


	vector<uchar> vecImg;                               //Mat 图片数据转换为vector<uchar>  
  vector<int> params;
//vecCompression_params.push_back(CV_IMWRITE_JPEG_QUALITY);
  params.push_back(CV_IMWRITE_PNG_COMPRESSION);
  params.push_back(90);
  imencode(".png", mat, vecImg, params);
  baseStr = encode(vecImg.data(), vecImg.size());
  cout<<"base64 String: "<<baseStr<<endl;

//	waitKey(3000);

}


string encode(const unsigned char* Data,int DataByte)  
{  
    //编码表  
    const char EncodeTable[]="ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";  
    //返回值  
    string strEncode;  
    unsigned char Tmp[4]={0};  
    int LineLength=0;  
    for(int i=0;i<(int)(DataByte / 3);i++)  
    {  
        Tmp[1] = *Data++;  
        Tmp[2] = *Data++;  
        Tmp[3] = *Data++;  
        strEncode+= EncodeTable[Tmp[1] >> 2];  
        strEncode+= EncodeTable[((Tmp[1] << 4) | (Tmp[2] >> 4)) & 0x3F];  
        strEncode+= EncodeTable[((Tmp[2] << 2) | (Tmp[3] >> 6)) & 0x3F];  
        strEncode+= EncodeTable[Tmp[3] & 0x3F];  
        if(LineLength+=4,LineLength==76) {strEncode+="\r\n";LineLength=0;}  
    }  
    //对剩余数据进行编码  
    int Mod=DataByte % 3;  
    if(Mod==1)  
    {  
        Tmp[1] = *Data++;  
        strEncode+= EncodeTable[(Tmp[1] & 0xFC) >> 2];  
        strEncode+= EncodeTable[((Tmp[1] & 0x03) << 4)];  
        strEncode+= "==";  
    }  
    else if(Mod==2)  
    {  
        Tmp[1] = *Data++;  
        Tmp[2] = *Data++;  
        strEncode+= EncodeTable[(Tmp[1] & 0xFC) >> 2];  
        strEncode+= EncodeTable[((Tmp[1] & 0x03) << 4) | ((Tmp[2] & 0xF0) >> 4)];  
        strEncode+= EncodeTable[((Tmp[2] & 0x0F) << 2)];  
        strEncode+= "=";  
    }  
      
    return strEncode;  
}  
