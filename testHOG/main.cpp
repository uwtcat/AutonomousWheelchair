/**
 * @file:   main.cpp
 * @author: Jan Hendriks (dahoc3150 [at] gmail.com)
 * @date:   Created on 2. Dezember 2012
 * @brief:  Example program on how to train your custom HOG detecting vector
 * for use with openCV <code>hog.setSVMDetector(_descriptor)</code>;
 * 
 * Copyright 2015 Jan Hendriks
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 *  
 * For the paper regarding Histograms of Oriented Gradients (HOG), @see http://lear.inrialpes.fr/pubs/2005/DT05/
 * You can populate the positive samples dir with files from the INRIA person detection dataset, @see http://pascal.inrialpes.fr/data/human/
 * This program uses SVMlight as machine learning algorithm (@see http://svmlight.joachims.org/), but is not restricted to it
 * Tested in Ubuntu Linux 64bit 12.04 "Precise Pangolin" with openCV 2.3.1, SVMlight 6.02, g++ 4.6.3
 * and standard HOG settings, training images of size 64x128px.
 * 
 * What this program basically does:
 * 1. Read positive and negative training sample image files from specified directories
 * 2. Calculate their HOG features and keep track of their classes (pos, neg)
 * 3. Save the feature map (vector of vectors/matrix) to file system
 * 4. Read in and pass the features and their classes to a machine learning algorithm, e.g. SVMlight
 * 5. Train the machine learning algorithm using the specified parameters
 * 6. Use the calculated support vectors and SVM model to calculate a single detecting descriptor vector
 * 7. Dry-run the newly trained custom HOG descriptor against training set and against camera images, if available
 * 
 * Build by issuing:
 * g++ `pkg-config --cflags opencv` -c -g -MMD -MP -MF main.o.d -o main.o main.cpp
 * gcc -c -g `pkg-config --cflags opencv` -MMD -MP -MF svmlight/svm_learn.o.d -o svmlight/svm_learn.o svmlight/svm_learn.c
 * gcc -c -g `pkg-config --cflags opencv` -MMD -MP -MF svmlight/svm_hideo.o.d -o svmlight/svm_hideo.o svmlight/svm_hideo.c
 * gcc -c -g `pkg-config --cflags opencv` -MMD -MP -MF svmlight/svm_common.o.d -o svmlight/svm_common.o svmlight/svm_common.c
 * g++ `pkg-config --cflags opencv` -o trainhog main.o svmlight/svm_learn.o svmlight/svm_hideo.o svmlight/svm_common.o `pkg-config --libs opencv`
 * 
 * Warning:
 * Be aware that the program may consume a considerable amount of main memory, hard disk memory and time, dependent on the amount of training samples.
 * Also be aware that (esp. for 32bit systems), there are limitations for the maximum file size which may take effect when writing the features file.
 * 
 * Terms of use:
 * This program is to be used as an example and is provided on an "as-is" basis without any warranties of any kind, either express or implied.
 * Use at your own risk.
 * For used third-party software, refer to their respective terms of use and licensing.
 */

// <editor-fold defaultstate="collapsed" desc="Definitions">
/* Parameter and macro definitions */

#include <stdio.h>
#include <dirent.h>
#include <iostream>
#include <iterator>
#include <vector>
#include <algorithm>
#include <ios>
#include <fstream>
#include <stdexcept>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/ml/ml.hpp>
#include <opencv2/objdetect/objdetect.hpp>
#ifdef __MINGW32__
#include <sys/stat.h>
#endif



using namespace std;
using namespace cv;


// Directory containing positive sample images
static string posSamplesDir = "/home/csnesummer/Documents/ZED_cropped/pos/";
// Directory containing negative sample images
static string negSamplesDir = "/home/csnesummer/Documents/ZED_cropped/neg/";
// File for parsed annotations -- not used presently
static string parsedAnnotationsDir = "/home/nvidia/Downloads/HOG Testing/Parsed Test Annotations/";
// Set the file to read the detecting descriptor vector from
static string descriptorVectorFile = "/home/csnesummer/Downloads/trainHOG-master-trainwithmatlab/genfiles/96x160descriptorvectorIteration10.dat";
// Set the file to write the resulting opencv hog classifier as YAML file
static string cvHOGFile = "genfiles/cvHOGClassifier.yaml";
// Set the file to write results for plotting
static string outputForML = "ZEDDataIteration10Output_FullSet.txt";
// Set file for reading threshold values
static string thresholdFile = "thresholds.dat";


// <editor-fold defaultstate="collapsed" desc="Helper functions">
/* Helper functions */

static string toLowerCase(const string& in) {
    string t;
    for (string::const_iterator i = in.begin(); i != in.end(); ++i) {
        t += tolower(*i);
    }
    return t;
}

static void storeCursor(void) {
    printf("\033[s");
}

static void resetCursor(void) {
    printf("\033[u");
}


/**
 * For unixoid systems only: Lists all files in a given directory and returns a vector of path+name in string format
 * @param dirName
 * @param fileNames found file names in specified directory
 * @param validExtensions containing the valid file extensions for collection in lower case
 */
static void getFilesInDirectory(const string& dirName, vector<string>& fileNames, const vector<string>& validExtensions) {
    printf("Opening directory %s\n", dirName.c_str());
#ifdef __MINGW32__
	struct stat s;
#endif
    struct dirent* ep;
    size_t extensionLocation;
    DIR* dp = opendir(dirName.c_str());
    if (dp != NULL) {
        while ((ep = readdir(dp))) {
            // Ignore (sub-)directories like . , .. , .svn, etc.
#ifdef __MINGW32__	
			stat(ep->d_name, &s);
			if (s.st_mode & S_IFDIR) {
				continue;
			}
#else
            if (ep->d_type & DT_DIR) {
                continue;
            }
#endif
            extensionLocation = string(ep->d_name).find_last_of("."); // Assume the last point marks beginning of extension like file.ext
            // Check if extension is matching the wanted ones
            string tempExt = toLowerCase(string(ep->d_name).substr(extensionLocation + 1));
            if (find(validExtensions.begin(), validExtensions.end(), tempExt) != validExtensions.end()) {
                printf("Found matching data file '%s'\n", ep->d_name);
                fileNames.push_back((string) dirName + ep->d_name);
            } else {
                printf("Found file does not match required file type, skipping: '%s'\n", ep->d_name);
            }
        }
        (void) closedir(dp);
    } else {
        printf("Error opening directory '%s'!\n", dirName.c_str());
    }
    return;
}


/**
 * Test the trained detector against the same training set to get an approximate idea of the detector.
 * Warning: This does not allow any statement about detection quality, as the detector might be overfitting.
 * Detector quality must be determined using an independent test set.
 * @param hog
 */
static void detectTrainingSetTest(const HOGDescriptor& hog, const vector<string>& posFileNames, const vector<string>& negFileNames) {

     // Threshold vector 
    printf("\nLoading descriptor vector.\n");
    std::ifstream is(thresholdFile);
    std::istream_iterator<double> start(is), end;
    vector<double> hitThreshold(start, end);

    unsigned int truePositives = 0;
    unsigned int trueNegatives = 0;
    unsigned int falsePositives = 0;
    unsigned int falseNegatives = 0;
    //vector<Rect> foundDetection;
    vector<Point> foundDetection;


    // Set up output file
    ofstream outputFile;
    outputFile.open(outputForML);
    
    // Walk over positive training samples, generate images and detect
for(vector<double>::const_iterator hitThresholdIterator = hitThreshold.begin(); hitThresholdIterator != hitThreshold.end(); ++hitThresholdIterator) {
    for (vector<string>::const_iterator posTrainingIterator = posFileNames.begin(); posTrainingIterator != posFileNames.end(); ++posTrainingIterator) {
        const Mat imageData = imread(*posTrainingIterator);
        //hog.detectMultiScale(imageData, foundDetection, 0, Size(8,8), Size(32,32), 1.08, 2);
        hog.detect(imageData, foundDetection, *hitThresholdIterator, Size(8,8), Size(32,32));
        if (foundDetection.size() > 0) {
            ++truePositives;
            falseNegatives += foundDetection.size() - 1;
        } else {
            ++falseNegatives;
        }
    }
    // Walk over negative training samples, generate images and detect
    for (vector<string>::const_iterator negTrainingIterator = negFileNames.begin(); negTrainingIterator != negFileNames.end(); ++negTrainingIterator) {
        const Mat imageData = imread(*negTrainingIterator);
        //hog.detectMultiScale(imageData, foundDetection, 0, Size(8,8), Size(32,32), 1.08, 2);
        hog.detect(imageData, foundDetection, *hitThresholdIterator, Size(8,8), Size(32,32));
        if (foundDetection.size() > 0) {
            falsePositives += foundDetection.size();
        } else {
            ++trueNegatives;
        }        
    }

    // write results to file
    outputFile << *hitThresholdIterator << " " << truePositives << " " << trueNegatives << " " << falsePositives << " " << falseNegatives << "\n";

    printf("Results for Threshold %f:\n\tTrue Positives: %u\n\tTrue Negatives: %u\n\tFalse Positives: %u\n\tFalse Negatives: %u\n", *hitThresholdIterator, truePositives, trueNegatives, falsePositives, falseNegatives);
    // clear variables 
    truePositives = 0;
    trueNegatives = 0; 
    falsePositives = 0; 
    falseNegatives = 0; 
}

outputFile.close();    
    
}


/**
 * Main program entry point
 * @param argc unused
 * @param argv unused
 * @return EXIT_SUCCESS (0) or EXIT_FAILURE (1)
 */
int main(int argc, char** argv) {


    HOGDescriptor hog; // Use standard parameters here
    //hog.winSize = Size(64, 128); // Default training images size as used in paper
    hog.winSize = Size(96, 160);
    //hog.winSize = Size(192, 320);
    // Get the files to train from somewhere
    static vector<string> positiveTestingImages;
    static vector<string> negativeTestingImages;
    static vector<string> validExtensions;
    validExtensions.push_back("jpg");
    validExtensions.push_back("png");
    validExtensions.push_back("ppm");
    // </editor-fold>

    // <editor-fold defaultstate="collapsed" desc="Read image files">
    getFilesInDirectory(posSamplesDir, positiveTestingImages, validExtensions);
    getFilesInDirectory(negSamplesDir, negativeTestingImages, validExtensions);
    /// Retrieve the descriptor vectors from the samples
    unsigned long overallSamples = positiveTestingImages.size() + negativeTestingImages.size();
  
  
    // Make sure there are actually samples to train
    if (overallSamples == 0) {
        printf("No testing sample files found, nothing to do!\n");
        return EXIT_SUCCESS;
    }

    /// @WARNING: This is really important, some libraries (e.g. ROS) seems to set the system locale which takes decimal commata instead of points which causes the file input parsing to fail
    setlocale(LC_ALL, "C"); // Do not use the system locale
    setlocale(LC_NUMERIC,"C");
    setlocale(LC_ALL, "POSIX");

   printf("\nLoading descriptor vector.\n");
   std::ifstream is(descriptorVectorFile);
   std::istream_iterator<float> start(is), end;
   vector<float> descriptorVector(start, end);
   

   printf("Setting hog SVM detector.\n");   
    // Set our custom detecting vector
    if(!descriptorVector.empty()) {
       hog.setSVMDetector(descriptorVector);
    } else {
       printf("Error in descriptor vector file.\n");
    }
  
	
    printf("Testing training phase using training set as test set (just to check if training is ok - no detection quality conclusion with this!)\n");
    detectTrainingSetTest(hog, positiveTestingImages, negativeTestingImages);

   
    return EXIT_SUCCESS;
}
