# Face_recognize_on_EAIDK
基于边缘计算的人脸识别项目，C++实现，边缘设备是EAIDK310

## 文件结构介绍
build-eaidk_visual_embeded-Desktop-Debug：编译目录和运行目录

eaidk_visual_embeded:主要代码文件

libs:Vision.Face SDK

## 运行说明

1.配置socket

修改socket.txt文件可选择是否与服务端进行通信

如果需要进行socket连接，在socket.txt第一行填y，第二行填服务端IP，第三行填端口号；
否则，第一行填n，第二、三行无所谓


2.运行代码
在build目录下运行执行可执行文件

````
./eaidk_visual_embeded
````

注：如无法执行，执行下属命令添加执行权限
````
chmod +x eaidk_visual_embeded
````

## 编译说明
1.	开发板不带RTC电池，如果不修改时间，makefile文件的生成时间晚于当前系统时间，编译会提示错误。需手动设置当前系统时间，参考命令如下：
````      
sudo date -s "2020-04-20 09:16:30"
````      

2.	进入build-eaidk_visual_embeded-Desktop-Debug，直接进行编译
````
make
````
      
注：当修改mainwindow.ui时，重新在build-eaidk_visual_embeded-Desktop-Debug目录输入如下命令
````      
qmake-qt5 ../eaidk_visual_embeded/eaidk_visual_embeded.pro -o Makefile
````
然后再进行编译
````
make
````
