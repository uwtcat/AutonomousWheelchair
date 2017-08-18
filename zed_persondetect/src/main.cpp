// Written using ZED OpenCV tutorials, and the OpenCV peopledetect.cpp

#include <sl/Camera.hpp>
#include <opencv2/opencv.hpp>
#include <sl/Core.hpp>
#include <sl/defines.hpp>
#include <iostream>
#include <iterator>
#include <fstream>
#include <vector>
#include <algorithm>

using namespace sl; 
using namespace std;
using namespace cv;

cv::Mat slMat2cvMat(sl::Mat& input);

static string descriptorVectorFile = "Files/96x160descriptorvector.dat";
static string cvHOGFile = "Files/cvHOGClassifier.yaml";

int main(int argc, char **argv) {
	// Create camera
	Camera zed;
	
	// Configure camera
	InitParameters init_params;
	//init_params.camera_resolution = RESOLUTION_HD720; 
	init_params.camera_resolution = RESOLUTION_VGA;
	init_params.depth_mode = DEPTH_MODE_PERFORMANCE; 
	init_params.coordinate_units = sl::UNIT_METER;
	
	// Open camera
	ERROR_CODE err = zed.open(init_params);
	if(err != SUCCESS) {
		cout << "Camera failed";
		return 1;
	}
	
	// Runtime params
	RuntimeParameters runtime_parameters;
	runtime_parameters.sensing_mode = SENSING_MODE_STANDARD; 
	
	// Create sl and cv Mat to get ZED left image and depth image
   	Resolution image_size = zed.getResolution();
    	sl::Mat image_zed(image_size,sl::MAT_TYPE_8U_C4); // Create a sl::Mat to handle Left image
	cv::Mat image_ocv = slMat2cvMat(image_zed);
	//sl::Mat depth_image_zed(image_size, MAT_TYPE_8U_C4);
	//cv::Mat depth_image_ocv = slMat2cvMat(depth_image_zed);
	
	// Create OpenCV images to display (lower resolution to fit the screen)
    	cv::Size displaySize(720, 404);
	//cv::Size displaySize(672, 376);
	//cv::Size displaySize(336, 188);
	//cv::Size displaySize(168, 94);
    	cv::Mat image_ocv_display(displaySize, CV_8UC4);
    	//cv::Mat depth_image_ocv_display(displaySize, CV_8UC4);
	
	// Execute the calling thread on 2nd core
    	Camera::sticktoCPUCore(2);
	
	HOGDescriptor hog; 

	std::ifstream is(descriptorVectorFile);
	std::istream_iterator<float> start(is), end;
	vector<float> descriptorVector(start, end);
	std::cout << "Read" << descriptorVector.size() << " descriptors" << std::endl;
	
	//std::cout << "numbers read in: \n";
	//std::copy(descriptorVector.begin(), descriptorVector.end(), std::ostream_iterator<double>(std::cout, " "));
	//std::cout << std::endl;

	//SVMLight::SVMClassifier c(descriptorVectorFile);
	//vector<float> descriptorVector = c.getDescriptorVector();
	//hog.setSVMDetector(HOGDescriptor::getDefaultPeopleDetector());
	hog.winSize = Size(96, 160);
	hog.setSVMDetector(descriptorVector);
       // hog.winSize = Size(96, 160);
	//hog.save(cvHOGFile);	
	cv::namedWindow("Image", cv::WINDOW_AUTOSIZE);
	cout << "Pre-loop\n";

	// Capture frames 
		char key = ' ';
		while(key != 'q') {
			if(zed.grab(runtime_parameters) == SUCCESS) {
				// cout << "Looping";
				zed.retrieveImage(image_zed, VIEW_LEFT);
				
				// Person detection
				vector<Rect> found, found_filtered;

				// Data type shenanigans 
				cv::Mat smImage_ocv;
				cvtColor(image_ocv, smImage_ocv, CV_BGRA2BGR);

				 
				
				//TODO: adjust params
				hog.detectMultiScale(smImage_ocv, found, 0, Size(8,8), Size(32,32), 1.08, 2);
				//hog.detectMultiScale(image_ocv, found, 0, Size(8,8), Size(32,32), 1.05, 2);
				//hog.detectMultiScale(image_ocv, found);
				
				size_t i, j; 
				for(i = 0; i < found.size(); i++) {
					Rect r = found[i]; 
					// Do not add small detections inside a bigger detection
					for(j = 0; j < found.size(); j++) {
						if(j != i && (r & found[j]) == r)
							break; 
					}
					if(j == found.size()) 
						found_filtered.push_back(r);
					
				}
				for(i = 0; i < found_filtered.size(); i++) {
					// Shrinking rectangles because HOG over estimates
					Rect r = found_filtered[i];
					r.x += cvRound(r.width*0.1);
					r.width = cvRound(r.width*0.8); 
					r.y += cvRound(r.height*0.06);
					r.height = cvRound(r.height*0.9); 
					rectangle(smImage_ocv, r.tl(), r.br(), cv::Scalar(0,255,0),2);
				}
				
				// Display frames
				//cv::resize(image_ocv, image_ocv_display, displaySize);
				cv::resize(smImage_ocv, image_ocv_display, displaySize);
				imshow("Image", image_ocv_display);
				//imshow("Image", smImage_ocv);
				
				key = cv::waitKey(10);
			}
		}
		
	cout << "Closing camera\n";
	// Close camera
	zed.close();
	return 0;
	
}

cv::Mat slMat2cvMat(sl::Mat& input) {
	//convert MAT_TYPE to CV_TYPE
	int cv_type = -1;
	switch (input.getDataType()) {
		case sl::MAT_TYPE_32F_C1: cv_type = CV_32FC1; break;
		case sl::MAT_TYPE_32F_C2: cv_type = CV_32FC2; break;
		case sl::MAT_TYPE_32F_C3: cv_type = CV_32FC3; break;
		case sl::MAT_TYPE_32F_C4: cv_type = CV_32FC4; break;
		case sl::MAT_TYPE_8U_C1: cv_type = CV_8UC1; break;
		case sl::MAT_TYPE_8U_C2: cv_type = CV_8UC2; break;
		case sl::MAT_TYPE_8U_C3: cv_type = CV_8UC3; break;
		case sl::MAT_TYPE_8U_C4: cv_type = CV_8UC4; break;
		default: break;
	}

	// cv::Mat data requires a uchar* pointer. Therefore, we get the uchar1 pointer from sl::Mat (getPtr<T>())
	//cv::Mat and sl::Mat will share the same memory pointer
	return cv::Mat(input.getHeight(), input.getWidth(), cv_type, input.getPtr<sl::uchar1>(MEM_CPU));
}
