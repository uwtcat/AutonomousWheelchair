#include <iostream>
#include <fstream>
#include <dirent.h>
#include <string.h>
#include <vector>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <experimental/filesystem>

namespace fs = std::experimental::filesystem::v1;
using namespace cv;
using namespace std;
using namespace fs;

int main() { 
std::string line,value; //Line stores lines of the file and value stores characters of the line
int i=0; //Iterate through lines
int j=0; //Iterate through characters
int n=0; //Iterate through ,()-...
char v; //Stores variable value as a char to be able to make comparisions easily

std::vector <Rect> anotations; //Stores rectangles for each image
std::vector <int> bbValues; //Bounding box values (xmin,ymin,xmax,ymax)

fs::path anotationsFolder = "/home/csnesummer/Documents/HOG Testing/Test Images_Annotations/annotations/"; //Path of anotations folder
fs::path anotationsParsedFolder = "/home/csnesummer/Documents/HOG Testing/Parsed Test Annotations/"; //Path to store new anotations

fs::recursive_directory_iterator it(anotationsFolder); //Iteradores of files
fs::recursive_directory_iterator endit;

cout<<"Loading anotations from "<<anotationsFolder<<endl;

while((it != endit)) //Until end of folder
{
    if((fs::is_regular_file(*it))) //Good practice
    {
        fs::path imagePath(it->path()); //Complete path of the image

        cout<<"Reading anotations from"<<it->path().filename()<<endl;

        ifstream inputFile; //Declare input file with image path
        inputFile.open(imagePath.string().data(), std::ios_base::in);

        i=0;
        while (! inputFile.eof() ){ //Until end of file

            getline (inputFile,line);//Get lines one by one

            if ((i>=17) && ((i-17)%7==0)){ //In lines numer 17,24,31,38...where bounding boxes coordinates are

                j=69;
                v=line[j]; //Start from character num 69 corresponding to first value of Xmin

                while (j<line.size()){ //Until end of line

                    if (v=='(' || v==',' || v==')' || v==' ' || v=='-'){ //if true, push back acumulated value unless previous value wasn't a symbol also
                        if (n==0){
                            bbValues.push_back(stoi(value)); //stoi converts string in to integer ("567"->567) 
                            value.clear();
                        }
                        n++;
                    }
                    else{
                        value+=v; //Append new number
                        n=0;//Reset in order to know that a number has been read
                    }
                    j++;
                    v=line[j];//Read next character
                }
                Rect rect(bbValues[0],bbValues[1],bbValues[2],bbValues[3]); //Build a rectangle rect(xmin,ymin,xmax-xmin,ymax-ymin)
                anotations.push_back(rect);
                bbValues.clear();
            }
            i++;//Next line
        }
        inputFile.close();            

        cout<<"Writing..."<<endl;

        //Save the anotations to a file
        ofstream outputFile; //Declare file
        fs::path outputPath(anotationsParsedFolder / it->path().filename());// Complete path of the file
        outputFile.open(outputPath.string().data(), ios_base::trunc);

        // Store anotations as x y width heigth
        for (int i=0; i<anotations.size(); i++){
            outputFile<<anotations[i].x<<" ";
            outputFile<<anotations[i].y<<" ";
            outputFile<<anotations[i].width<<" ";
            outputFile<<anotations[i].height<<endl;
        }
        anotations.clear();
        outputFile.close();
    }
    ++it;//Next file in anotations folders
}
}
