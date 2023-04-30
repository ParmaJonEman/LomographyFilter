// main.h : Include file for standard system include files,
// or project specific include files.

#pragma once

#include <conio.h>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <errno.h>
#include <locale.h>
#include <vector>
#include <string>
#include <fstream>
#include <iomanip>
#include <opencv2/core/utility.hpp>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgcodecs.hpp>
#include "opencv2/imgproc.hpp"
#include <algorithm>
#include <iterator>

using namespace cv;
using namespace std;

static int parseParameters(int argc, char** argv, string* imageFile, int* option, string* matchFile);

static int* createTheHistogram(Mat image);
static float* normalizeTheHistogram(int histogram[256], Mat image);
static int* equalizeTheHistogram(float normalizedHistogram[256]);
static Mat applyTheHistogram(int histogram[256], Mat image);
static int* specifyTheHistogram(int equalizedHistogram[256], int goalEqualizedHistogram[256]);
static float* getNormalizedHistogramFromFile(string fileName);
static int isImageColor(Mat image);