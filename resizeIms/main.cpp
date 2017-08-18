#include <iostream>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <dirent.h>
#include <string.h>

using namespace std;
using namespace cv;

int main() {
  string dirName = "/home/csnesummer/Downloads/INRIAPerson/pos_cropped_64x128/";
  DIR *dir;
  dir = opendir(dirName.c_str());
  if(dir == NULL) {
    cout<<"Directory does not exist"<<endl;
    exit(1);
  }
  string imgName;
  struct dirent *ptr;
  
  while((ptr = readdir(dir)) != NULL) {
    if(strcmp(ptr->d_name,".") != 0 && strcmp(ptr->d_name, "..") != 0 
		&& strcmp(ptr->d_name, ".gitignore") != 0) {
    string imgPath(dirName + ptr->d_name);
    Mat img = imread(imgPath);
    if(img.cols == 0) {
      cout << "Error reading file" << imgPath << endl;
      return 1;
    }
    Mat img_resize;
    resize(img, img_resize, Size(64, 128));
    imwrite(imgPath, img_resize);
    }
  }
  closedir(dir);
  
}
