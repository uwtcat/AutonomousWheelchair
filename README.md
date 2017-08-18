# AutonomousWheelchair - Leader Following 

## Setting up ZED Camera on New Computer
-	Go to the ZED documentation for instructions: https://www.stereolabs.com/documentation/ 
    - Summary of minimum requirements: 
    - Dual-core 2,3GHz 
    - 4GB RAM
    - NVIDIA GPU (must have CUDA compatible GPU to use all functions)
    - USB 2.0 (3.0 highly recommended; when using 2.0, I was unable to establish a connection)
    - Ubuntu 16.04 (for Linux; hard requirement, Ubuntu 14.04 is not sufficient. I had some issues with corrupted 16.04 ISOs, so installed 14.04 and upgraded)
- Download most recent version of CUDA (currently 8.0 is being used)
    - If the computer is not CUDA-capable, some basic functionality of the ZED camera is still possible. The ZED Explorer, which takes RGB images, can still operate. No depth operations are enabled without CUDA. 
- Download OpenCV. I could get OpenCV 3.1 to work; OpenCV 3.2 did not. On the TX2, OpenCV 2.4.13 is installed.  
    -   Ensure that previous versions of OpenCV are completely uninstalled. 
    -	When I installed OpenCV 3.1, I ran into some linking errors. Some of these were due to using Ubuntu 14.04, but others had some responses online. From the nvidia help site: go to opencv-3.1.0/modules/cudalegacy/src/graphcuts.cpp (in OpenCV downloaded folder) and alter the #if !defined line at the top such that it reads 
    -	#if !defined (HAVE_CUDA) || (CUDA_DISABLER) || (CUDART_VERSION >= 8000) 
-	Download SDK from the website or from the ZED flash drive. 
    -	http://stereolabs.com/developers/release/2.0
    -	Note that there are different options for windows, linux, TX1 and TX2. 
-	On the MacBook, ZED SDK installed in /usr/local/zed/ 
-	ZED should be functional immediately.
    -	Prebuilt utilities located in zed/tools/. ZED Explorer is good for checking basic functionality. Run these tools from the terminal so error messages are displayed. 
    - If camera failed errors occur, I encountered them for the following reasons: 
    -	Incompatible GPU / CUDA not installed correctly 
-	Check computer minimum requirements & CUDA installation.
    -	Calibration file is missing – see calibration section below.

## ZED Camera Calibration 
-	Use the ZED Calibration tool for personalized calibration. This process works best with a large monitor. The room must be as dark as possible. The process involves moving the ZED to match with circles on the screen. The calibration tool has an instruction panel for reference. 
-	If not desiring the personalized calibration, a calibration file can be downloaded from the website. https://stereolabs.com/developers/calib/13802 
    -	13802 is the serial number of this lab’s ZED camera. 

## ZED Programming
-	The documentation page for ZED linked under the first heading has information about ZED SDK functions, tools, and samples. 
-	Samples are located in the /zed/sample/ directory. The /zed/sample/interfaces/ directory has samples specific to OpenCV. 
-	Stereolabs also has a github account with these examples and some other tutorials. http://github.com/stereolabs

## Summer 2017 Project Files 
### HOG+LSVM Training
Two code bases were considered for the HOG descriptor training process: 
-	http://github.com/DaHoC/trainHOG
    -	All C++ code used for training was adapted from this github repository, which uses OpenCV’s HOG computation function and calls SVMLight. There’s a known issue flagged in this repository regarding excessive false positives; this issue was encountered, but not resolved. It’s possible that adjusting parameters on the computation or detector functions could resolve the issue, but this was not tested during the semester, and the computation of HOG descriptors was shifted to a MATLAB script. 
-	http://opendetection.com 
    -	This is an open source project, but it was ultimately not used because it requires many dependencies that were difficult to install without linking errors. 

#### MATLAB
-	hogfeatures.m 
    -	Insert positive and negative target folder into posTargetFolder and negTargetFolder variables. 
    -	Variable filename is the file where HOG features will be saved. 
    -	Feature calculations occur for all images in the positive and negative target folders using the extractHOGFeatures function. 

#### C++
trainHOG-master-trainwithmatlab directory
-	Code from the DaHoC repository, mostly commented out so that it only does the following: 
-	Loads MATLAB HOG features.
-	Calls SVMLight functions through a wrapper class in order to train an LSVM and get a single detecting vector. 
-	Save detecting vector to file. 

### HOG+LSVM Testing
testHOG directory. Code is also adapted from the DaHoC repository. Loads a file of thresholds for the LSVM (generated in MATLAB with makeThresholds.m), and loads images from positive and negative directories. Uses the OpenCV hog.detect() function, where the thresholds are varied to determine how that effects hit ratio. A file is written with the threshold, and the hit rates. 
Note that due to time constraints, the ability to iterate with and/or without replacement (to preserve test sets) was not automated. 
An ROC curve can be plotted with the makeROC.m file; it’s programmed to read the file format outputted by testHOG. 
Person detection with ZED
-	Based on the OpenCV samples. Uses OpenCV hog.detectMultiScale() for person detection. Note that a trained descriptor vector can be loaded instead of using the OpenCV default. 
-	Note that performance would be better if using the GPU; right now, CPU is largely being used (as per the github tutorial on OpenCV). 
Other Code
-	INRIA Person dataset annotation parser 
    -	Code from a stack overflow post; see README for the URL and the make instructions. 
    -	Set folder for annotations, and folder for storing the parsed annotations. 
    -	The parsed annotations can be loaded into a program for easy checking for box overlaps (if testing detector on full-size images or images with multiple people). 
-	resizeIms and cropIms 
    -	Using OpenCV functions for image editing; resize / crop all images in a directory. 
    
### ZED Person Detector
Written using OpenCV ZED interface sample and code from https://github.com/opencv/opencv/blob/master/samples/cpp/peopledetect.cpp
Connects to the ZED camera, and runs person detection code on the feed. Note that presently the feed lags behind real-time, as optimization was not performed (i.e. using GPU, etc). 
