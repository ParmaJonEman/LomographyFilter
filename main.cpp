/* Author: Jon Eman
 * Date: 10/23/2022
 * Title: Histo
 * Description: Histogram equalization and matching algorithm.
 *
 */

#include "main.h"

int main(int argc, char **argv) {
    string imageFile;
    string matchFile;

    int* histogram;
    float* normalizedHistogram;
    int* equalizedHistogram;

    int * goalHistogram;
    float* goalNormalizedHistogram;
    int* goalEqualizedHistogram;

    int* specifiedHistogram;

    int option;

    parseParameters(argc, argv, &imageFile, &option, &matchFile);
    cout << "option is: " << option << endl;
    cout << "image file is: " <<  imageFile << endl;
    cout << "match file is: " <<  matchFile << endl;

    try {
        Mat image = imread(imageFile, 1);
        if (image.empty() || isImageColor(image))
            throw (string("Cannot open input image ") + imageFile);
        imshow("Original Image", image);
        histogram = createTheHistogram(image);
        normalizedHistogram = normalizeTheHistogram(histogram, image);
        equalizedHistogram = equalizeTheHistogram(normalizedHistogram);

        switch(option) {
            case 1: {
                image = applyTheHistogram(equalizedHistogram, image);
                break;
            }
            case 2: {
                cout << matchFile << endl;
                Mat goalImage = imread(matchFile, 1);
                if (goalImage.empty() ||  isImageColor(goalImage))
                    throw (string("Cannot open input image ") + matchFile);
                imshow("Goal Image", goalImage);
                goalHistogram = createTheHistogram(goalImage);
                goalNormalizedHistogram = normalizeTheHistogram(goalHistogram, goalImage);
                goalEqualizedHistogram = equalizeTheHistogram(goalNormalizedHistogram);
                specifiedHistogram = specifyTheHistogram(equalizedHistogram,goalEqualizedHistogram);
                image = applyTheHistogram(specifiedHistogram, image);
                break;
            }
            case 3: {
                goalNormalizedHistogram = getNormalizedHistogramFromFile(matchFile);
                goalEqualizedHistogram = equalizeTheHistogram(goalNormalizedHistogram);
                specifiedHistogram = specifyTheHistogram(equalizedHistogram,goalEqualizedHistogram);
                image = applyTheHistogram(specifiedHistogram, image);
                break;
            }
            default: {
                cerr << "Invalid Option" << endl;
            }
        }
        imshow("Final Image", image);
        waitKey();
    }
    catch (string &str) {
        cerr << "Error: " << argv[0] << ": " << str << endl;
        return (1);
    }
    catch (Exception &e) {
        cerr << "Error: " << argv[0] << ": " << e.msg << endl;
        return (1);
    }
    return (0);
}

static int* createTheHistogram(Mat image){
    static int histogram[256];

    for(int i = 0; i < 256; i++){
        histogram[i] = 0;
    }

    for(int i=0; i < image.cols; i++){
        for(int j=0; j < image.rows; j++) {
            for(int k=0; k < image.channels(); k++) {
                histogram[image.at<Vec3b>(j, i)[k]]++;
            }
        }
    }

    return histogram;

}

static int isImageColor(Mat image){
    for(int i = 0; i < image.cols; i++){
        for(int j = 0; j < image.rows; j++){
            if((image.at<Vec3b>(j, i)[0] != image.at<Vec3b>(j, i)[1]) || (image.at<Vec3b>(j, i)[0] != image.at<Vec3b>(j, i)[2])){
                return 1;
            }
        }
    }
    return 0;
}

static float* normalizeTheHistogram(int histogram[256], Mat image){
    static float normalizedHistogram[256];
    for(int i = 0; i < 256; i++){
        normalizedHistogram[i] = 0.0;
    }
    for(int i = 0; i < 256; i++){
        normalizedHistogram[i] = static_cast<float>(histogram[i]) / static_cast<float>(image.total()*image.channels());
    }

    return normalizedHistogram;
}

static float* getNormalizedHistogramFromFile(string fileName){
    static float goalNormalizedHistogram[256];
    for(int i = 0; i < 256; i++){
        goalNormalizedHistogram[i] = 0.0;
    }
    ifstream myfile(fileName);
    string buffer;
    if(!myfile){
        cout << "Error opening histogram file" << endl;
    }
    for(int i = 0; i < 256; i++){
        getline(myfile, buffer, '\n');
        float probability = stof(buffer);
        goalNormalizedHistogram[i] = probability;
    }
    return goalNormalizedHistogram;
}

static int* equalizeTheHistogram(float normalizedHistogram[256]){
    static float tempArray[256];
    static int equalizedHistogram[256];

    for(int i = 0; i < 256; i++){
        tempArray[i] = 0.0;
        equalizedHistogram[i] = 0;
    }

    for(int i = 0; i < 256; i++){
        for(int j = 0; j < i + 1; j++) {
            tempArray[i] = tempArray[i] + normalizedHistogram[j];
        }
        equalizedHistogram[i] = round(tempArray[i] * (255));
    }
    return equalizedHistogram;
}

static int* specifyTheHistogram(int equalizedHistogram[256], int goalEqualizedHistogram[256]){
    static int specifiedHistogram[256];
    for(int i = 0; i < 256; i++){
        specifiedHistogram[i] = i;
    }
    for(int i = 0; i < 256; i++){
        for(int j = 0; j < 256; j++){
            if(goalEqualizedHistogram[j] == equalizedHistogram[i]){
                specifiedHistogram[i] = j;
                break;
            }
        }
    }
    return specifiedHistogram;
}

static Mat applyTheHistogram(int histogram[256], Mat image){

    for(int i=0; i < image.cols; i++){
        for(int j=0; j < image.rows; j++){
            for(int k=0; k < image.channels(); k++) {
                image.at<Vec3b>(j, i)[k] = histogram[image.at<Vec3b>(j, i)[k]];
            }
        }
    }

    return image;
}

static int parseParameters(int argc, char** argv, string* imageFile, int* option, string* matchFile)
{
    String keys =
            {
                    "{help h usage ? |                            | print this message   }"
                    "{@imagefile|| image you want to use	}"
                    "{m|1| histogram option	}"
                    "{@matchfile|| File to match to	}"
            };

    // Get required parameters. If any are left blank, defaults are set based on the above table
    // If no directory is passed in, or if the user passes in a help param, usage info is printed
    CommandLineParser parser(argc, argv, keys);
    parser.about("Histo v1.0");

    if (!parser.has("@imagefile") || parser.has("help"))
    {
        parser.printMessage();
        return(0);
    }

    if (parser.has("m"))
    {
        *option = parser.get<int>("m");
    }

    if (parser.has("@matchfile"))
    {
/*        cout << matchFile->size() << endl;
        matchFile->resize(parser.get<string>("@matchfile").size());
        cout << matchFile->size() << endl;*/
        *matchFile = parser.get<string>("@matchfile");
        // matchFile->append(parser.get<string>("@matchfile"));
/*        cout << *matchFile << endl;*/
    }

    // cout << imageFile->size() << endl;
    *imageFile = parser.get<string>("@imagefile");
    // cout << imageFile->size() << endl;
    return(1);
}