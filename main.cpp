#include "main.h"

int lomoSliderLevel = 0;
const int lomoSliderMax = 20;
const int lomoSliderMin = 0;
int haloSliderLevel = 100;
const int haloSliderMax = 100;
const int haloSliderMin = 0;

Mat originalImage;
Mat haloMask;
Mat mergedImage;

int main(int argc, char **argv) {
    string imageFile;
    parseParameters(argc, argv, &imageFile);
    cout << "Selected Image: " << imageFile << endl;

    try {
        originalImage = imread(imageFile, 1);
        if (originalImage.empty())
            throw (string("Cannot open input originalImage ") + imageFile);

        haloMask = createHaloFilter(100);

        namedWindow("Lomo Window", WINDOW_AUTOSIZE);

        createTrackbar("Lomo", "Lomo Window", nullptr, lomoSliderMax, onTrackbarLomo);
        setTrackbarPos("Lomo", "Lomo Window", lomoSliderLevel);
        setTrackbarMin("Lomo", "Lomo Window", lomoSliderMin);

        createTrackbar("Halo", "Lomo Window", nullptr, haloSliderMax, onTrackbarHalo);
        setTrackbarPos("Halo", "Lomo Window", haloSliderLevel);
        setTrackbarMin("Halo", "Lomo Window", haloSliderMin);
        onTrackbarLomo(lomoSliderLevel, nullptr);
        waitForKeyCommand();
    }
    catch (string &str) {
        cerr << "Error: " << argv[0] << ": " << str << endl;
        return (1);
    }
    catch (Exception &e) {
        cerr << "Error: " << argv[0] << ": " << e.msg << endl;
        return (1);
    }
    return 0;
}

static int waitForKeyCommand() {
    //	Waiting for one of the following keys:
    // Keycode	Keyboard Key	Functionality
    //	81			q				Quit
    //	113			Q				Quit
    //	83			s				Save and Quit
    //	115			S				Save and Quit

    int key = 0;
    while (key != 81 && key != 113 && key != 83 && key != 115) {
        key = waitKey();
    }
    switch (key) {
        case 81:
        case 113:
            return (1); // Quitting application gracefully by returning 1
        case 83:
        case 115: // Save image and quit
            imwrite("Output.jpg", mergedImage);
            break;
        default:
            cerr
                    << "There should not be a way to access this case, please report scenario to developer."; //The while loop should prevent this, but just in case, we quit gracefully
            return (1);
    }
    return 0;
}

static void onTrackbarHalo(int percent, void *) {
    int radius = (min(originalImage.cols, originalImage.rows) / 100) * percent;
    if (radius == 0) {
        radius = 1;
    }
    haloMask = createHaloFilter(radius);
    onTrackbarLomo(lomoSliderLevel, nullptr);
}

static void onTrackbarLomo(int level, void *) {
    lomoSliderLevel = level;

    if (lomoSliderLevel >= 8) {
        Mat lookupMat = createLUT(lomoSliderLevel);
        Mat Bands[3];
        split(originalImage, Bands);
        for (int i = 0; i < originalImage.cols; i++) {
            for (int j = 0; j < originalImage.rows; j++) {
                Bands[2].at<uchar>(j, i) = lookupMat.at<uchar>(0, int(Bands[2].at<uchar>(j, i)));
            }
        }
        vector<Mat> channels = {Bands[0], Bands[1], Bands[2]};
        merge(channels, mergedImage);
    } else {
        originalImage.copyTo(mergedImage);
    }
    mergedImage.convertTo(mergedImage, CV_32FC3);
    mergedImage = haloMask.mul(mergedImage);
    mergedImage.convertTo(mergedImage, CV_8UC3);
    imshow("Lomo Window", mergedImage);
}

static Mat createLUT(float param) {
    const float e = 2.718;
    param = param / 100.0f;
    try {
        Mat lutMat(1, 256, CV_8UC1);
        for (int i = 0; i < 256; i++) {
            int value = 256.0f / (1.0f + pow(e, -(((i / 256.0f) - .5f) / param)));
            lutMat.at<uchar>(0, i) = value;
        }
        return lutMat;
    }
    catch (Exception &e) {
        cerr << "Error: " << ": " << e.msg << endl;
    }
}

static Mat createHaloFilter(int radius) {
    Mat haloFilter(originalImage.rows, originalImage.cols, CV_32FC3, Scalar(0.25, 0.25, 0.25));
    circle(haloFilter, Point(originalImage.cols / 2, originalImage.rows / 2), radius, Scalar(1, 1, 1), -1);
    blur(haloFilter, haloFilter, Size(radius, radius), Point(radius / 2, radius / 2));
    return haloFilter;
}

static int parseParameters(int argc, char **argv, string *imageFile) {
    String keys =
            {
                    "{help h usage ? |                            | print this message   }"
                    "{@imagefile|| originalImage you want to use	}"
            };

    // Get required parameters.
    // If no image is passed in, or if the user passes in a help param, usage info is printed
    CommandLineParser parser(argc, argv, keys);
    parser.about("Lomo v1.0");

    if (!parser.has("@imagefile") || parser.has("help")) {
        parser.printMessage();
        return (0);
    }

    *imageFile = parser.get<string>("@imagefile");
    return (1);
}