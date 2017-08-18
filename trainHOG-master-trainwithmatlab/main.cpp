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
#ifdef __MINGW32__
#include <sys/stat.h>
#endif
#include <ios>
#include <fstream>
#include <stdexcept>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/ml/ml.hpp>

#define SVMLIGHT 1
#define LIBSVM 2

//#define TRAINHOG_USEDSVM SVMLIGHT
#define TRAINHOG_USEDSVM SVMLIGHT

#if TRAINHOG_USEDSVM == SVMLIGHT
    #include "svmlight/svmlight.h"
    #define TRAINHOG_SVM_TO_TRAIN SVMlight
#elif TRAINHOG_USEDSVM == LIBSVM
    #include "libsvm/libsvm.h"
    #define TRAINHOG_SVM_TO_TRAIN libSVM
#endif

using namespace std;
using namespace cv;


// Directory containing positive sample images
static string posSamplesDir = "pos/";
// Directory containing negative sample images
static string negSamplesDir = "neg/";
// Set the file to write the features to
static string featuresFile = "genfiles/features.dat";
// Set the file to write the SVM model to
static string svmModelFile = "genfiles/svmlightmodel.dat";
// Set the file to write the resulting detecting descriptor vector to
static string descriptorVectorFile = "genfiles/96x160descriptorvectorIteration10.dat";
// Set the file to write the resulting opencv hog classifier as YAML file
static string cvHOGFile = "genfiles/cvHOGClassifier.yaml";
// File to read from
static string matlabFile = "/home/csnesummer/Documents/TrainTestIterations/96x160matlabfeaturesIteration10.dat";

// HOG parameters for training that for some reason are not included in the HOG class
static const Size trainingPadding = Size(0, 0);
static const Size winStride = Size(8, 8);
// </editor-fold>

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
 * Saves the given descriptor vector to a file
 * @param descriptorVector the descriptor vector to save
 * @param _vectorIndices contains indices for the corresponding vector values (e.g. descriptorVector(0)=3.5f may have index 1)
 * @param fileName
 * @TODO Use _vectorIndices to write correct indices
 */
static void saveDescriptorVectorToFile(vector<float>& descriptorVector, vector<unsigned int>& _vectorIndices, string fileName) {
    printf("Saving descriptor vector to file '%s'\n", fileName.c_str());
    string separator = " "; // Use blank as default separator between single features
    fstream File;
    float percent;
    File.open(fileName.c_str(), ios::out);
    if (File.good() && File.is_open()) {
        printf("Saving %lu descriptor vector features:\t", descriptorVector.size());
        storeCursor();
        for (int feature = 0; feature < descriptorVector.size(); ++feature) {
            if ((feature % 10 == 0) || (feature == (descriptorVector.size()-1)) ) {
                percent = ((1 + feature) * 100 / descriptorVector.size());
                printf("%4u (%3.0f%%)", feature, percent);
                fflush(stdout);
                resetCursor();
            }
            File << descriptorVector.at(feature) << separator;
        }
        printf("\n");
        File << endl;
        File.flush();
        File.close();
    }
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
 * This is the actual calculation from the (input) image data to the HOG descriptor/feature vector using the hog.compute() function
 * @param imageFilename file path of the image file to read and calculate feature vector from
 * @param descriptorVector the returned calculated feature vector<float> , 
 *      I can't comprehend why openCV implementation returns std::vector<float> instead of cv::MatExpr_<float> (e.g. Mat<float>)
 * @param hog HOGDescriptor containin HOG settings
 */
static void calculateFeaturesFromInput(const string& imageFilename, vector<float>& featureVector, HOGDescriptor& hog) {
    /** for imread flags from openCV documentation, 
     * @see http://docs.opencv.org/modules/highgui/doc/reading_and_writing_images_and_video.html?highlight=imread#Mat imread(const string& filename, int flags)
     * @note If you get a compile-time error complaining about following line (esp. imread),
     * you either do not have a current openCV version (>2.0) 
     * or the linking order is incorrect, try g++ -o openCVHogTrainer main.cpp `pkg-config --cflags --libs opencv`
     */
    //Mat imageData = imread(imageFilename, IMREAD_GRAYSCALE);
    //Mat imageData = imread(imageFilename, IMREAD_COLOR);
    Mat imageData = imread(imageFilename);
    if (imageData.empty()) {
        featureVector.clear();
        printf("Error: HOG image '%s' is empty, features calculation skipped!\n", imageFilename.c_str());
        return;
    }
    // Check for mismatching dimensions
    if (imageData.cols != hog.winSize.width || imageData.rows != hog.winSize.height) {
        featureVector.clear();
        printf("Error: Image '%s' dimensions (%u x %u) do not match HOG window size (%u x %u)!\n", imageFilename.c_str(), imageData.cols, imageData.rows, hog.winSize.width, hog.winSize.height);
        return;
    }
    vector<Point> locations;
    hog.compute(imageData, featureVector, winStride, trainingPadding, locations);
    imageData.release(); // Release the image again after features are extracted
}

/**
 * Shows the detections in the image
 * @param found vector containing valid detection rectangles
 * @param imageData the image in which the detections are drawn
 */
static void showDetections(const vector<Point>& found, Mat& imageData) {
    size_t i, j;
    for (i = 0; i < found.size(); ++i) {
        Point r = found[i];
        // Rect_(_Tp _x, _Tp _y, _Tp _width, _Tp _height);
        rectangle(imageData, Rect(r.x-16, r.y-32, 32, 64), Scalar(64, 255, 64), 3);
    }
}

/**
 * Shows the detections in the image
 * @param found vector containing valid detection rectangles
 * @param imageData the image in which the detections are drawn
 */
static void showDetections(const vector<Rect>& found, Mat& imageData) {
    vector<Rect> found_filtered;
    size_t i, j;
    for (i = 0; i < found.size(); ++i) {
        Rect r = found[i];
        for (j = 0; j < found.size(); ++j)
            if (j != i && (r & found[j]) == r)
                break;
        if (j == found.size())
            found_filtered.push_back(r);
    }
    for (i = 0; i < found_filtered.size(); i++) {
        Rect r = found_filtered[i];
        rectangle(imageData, r.tl(), r.br(), Scalar(64, 255, 64), 3);
    }
}

/**
 * Test the trained detector against the same training set to get an approximate idea of the detector.
 * Warning: This does not allow any statement about detection quality, as the detector might be overfitting.
 * Detector quality must be determined using an independent test set.
 * @param hog
 */
static void detectTrainingSetTest(const HOGDescriptor& hog, const double hitThreshold, const vector<string>& posFileNames, const vector<string>& negFileNames) {
    unsigned int truePositives = 0;
    unsigned int trueNegatives = 0;
    unsigned int falsePositives = 0;
    unsigned int falseNegatives = 0;
    vector<Point> foundDetection;
    // Walk over positive training samples, generate images and detect
    for (vector<string>::const_iterator posTrainingIterator = posFileNames.begin(); posTrainingIterator != posFileNames.end(); ++posTrainingIterator) {
        const Mat imageData = imread(*posTrainingIterator);
        hog.detect(imageData, foundDetection, hitThreshold, winStride, trainingPadding);
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
        hog.detect(imageData, foundDetection, hitThreshold, winStride, trainingPadding);
        if (foundDetection.size() > 0) {
            falsePositives += foundDetection.size();
        } else {
            ++trueNegatives;
        }        
    }
    
    printf("Results:\n\tTrue Positives: %u\n\tTrue Negatives: %u\n\tFalse Positives: %u\n\tFalse Negatives: %u\n", truePositives, trueNegatives, falsePositives, falseNegatives);
}

/**
 * Test detection with custom HOG description vector
 * @param hog
 * @param hitThreshold threshold value for detection
 * @param imageData
 */
static void detectTest(const HOGDescriptor& hog, const double hitThreshold, Mat& imageData) {
    vector<Rect> found;
    Size padding(Size(32, 32));
    Size winStride(Size(8, 8));
    hog.detectMultiScale(imageData, found, hitThreshold, winStride, padding, 1.05, 2);
    showDetections(found, imageData);
}
// </editor-fold>

/**
 * Main program entry point
 * @param argc unused
 * @param argv unused
 * @return EXIT_SUCCESS (0) or EXIT_FAILURE (1)
 */
int main(int argc, char** argv) {

    // <editor-fold defaultstate="collapsed" desc="Init">
    HOGDescriptor hog; // Use standard parameters here
    //hog.winSize = Size(64, 128); // Default training images size as used in paper
    hog.winSize = Size(96, 160);
    // Get the files to train from somewhere
    static vector<string> positiveTrainingImages;
    static vector<string> negativeTrainingImages;
    static vector<string> validExtensions;
    validExtensions.push_back("jpg");
    validExtensions.push_back("png");
    validExtensions.push_back("ppm");
    // </editor-fold>

    /// @WARNING: This is really important, some libraries (e.g. ROS) seems to set the system locale which takes decimal commata instead of points which causes the file input parsing to fail
    setlocale(LC_ALL, "C"); // Do not use the system locale
    setlocale(LC_NUMERIC,"C");
    setlocale(LC_ALL, "POSIX");

    float percent;
   /// Read in and train the calculated feature vectors
   
    printf("Calling %s\n", TRAINHOG_SVM_TO_TRAIN::getInstance()->getSVMName());
    //TRAINHOG_SVM_TO_TRAIN::getInstance()->read_problem(const_cast<char*> (featuresFile.c_str()));
    TRAINHOG_SVM_TO_TRAIN::getInstance()->read_problem(const_cast<char*>(matlabFile.c_str()));
    TRAINHOG_SVM_TO_TRAIN::getInstance()->train(); // Call the core libsvm training procedure
    printf("Training done, saving model file!\n");
    TRAINHOG_SVM_TO_TRAIN::getInstance()->saveModelToFile(svmModelFile);
    // </editor-fold>

    // <editor-fold defaultstate="collapsed" desc="Generate single detecting feature vector from calculated SVM support vectors and SVM model">
    printf("Generating representative single HOG feature vector using svmlight!\n");
    vector<float> descriptorVector;
    vector<unsigned int> descriptorVectorIndices;
    // Generate a single detecting feature vector (v1 | b) from the trained support vectors, for use e.g. with the HOG algorithm
    
    TRAINHOG_SVM_TO_TRAIN::getInstance()->getSingleDetectingVector(descriptorVector, descriptorVectorIndices);
    // And save the precious to file system
    //descriptorVectorIndices = descriptorVector.size();
    printf("Vector size: %lu Indices size: %lu:", descriptorVector.size(), descriptorVectorIndices.size());
    saveDescriptorVectorToFile(descriptorVector, descriptorVectorIndices, descriptorVectorFile);
    printf("File saved.");
    // </editor-fold>

    // <editor-fold defaultstate="collapsed" desc="Test detecting vector">
    // Detector detection tolerance threshold
    const double hitThreshold = TRAINHOG_SVM_TO_TRAIN::getInstance()->getThreshold();
    printf("\nThreshold: %f", hitThreshold);
 
    return EXIT_SUCCESS;
}
