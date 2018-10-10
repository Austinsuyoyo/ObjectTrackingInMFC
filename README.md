# ObjectTrackingInMFC
[![GitHub license](https://img.shields.io/badge/license-MIT-blue.svg?style=plastic)](https://raw.githubusercontent.com/Austinsuyoyo/ObjectTrackingInMFC/master/LICENSE)
This is a VS2015 project,implement object tracking using opencv 3.4.3 version.
There are several ways to track the target in the program: 
1.CAMShift
2.MeanShift
3.Template
4.MIL
5.BOOSTING
6.MedeianFlow
7.TLD
8.KCF

# Build Enviroment
- Visual Studio 2015
- [OpenCV 3.4.3](https://opencv.org/releases.html)
- [CMAKE 3.13](https://cmake.org/download/)
# Usage
1. Download this repository
2. Add New Environment Variable  
`Variable Name : OPENCV_DIR`  
`Variable Value: "you own opencv library"`  
>You should compile OpenCV library from source code, because **tracking** module is not in central OpenCV repository.[Make own OpenCV Library](https://github.com/Austinsuyoyo/ObjectTrackingInMFC/wiki/Make-own-OpenCV-with-CMAKE)

![step11](https://raw.githubusercontent.com/Austinsuyoyo/ObjectTrackingInMFC/master/image/step11.png)

3. Edit environment variable **Path**  
`...;C:\opencv\build\x64\vc14\bin`   
![step12](https://raw.githubusercontent.com/Austinsuyoyo/ObjectTrackingInMFC/master/image/step12.png)
4. Need a camera
5. Run

# Demo

![Demo](https://raw.githubusercontent.com/Austinsuyoyo/ObjectTrackingInMFC/master/image/demo.png)
![Demo](https://raw.githubusercontent.com/Austinsuyoyo/ObjectTrackingInMFC/master/image/demo2.png)

# Referance

[OpenCV Contrib](https://github.com/opencv/opencv_contrib) - OpenCV official extra module  
[OpenCV 3.2 above Tracker example](https://github.com/lenlen/opencv/blob/tracking_api/samples/cpp/tracker.cpp)

