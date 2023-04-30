### Histogram Matcher
### Author: Jon Eman
### Date: 10/23/2022

### Build
##### To build, install OpenCV on your computer, change the OpenCV path in the CMAKE file, and use the standard CMAKE commands for building.

### Usage

##### Run the program with the following command structure:
    histo <image file> -m=<method> [<matching file>]

##### Where -m is the histogram equalizaiton or matching method. (1 for equalization, 2 for matching to an image, and 3 for matching to a normalized histogram txt file)
##### and <matching file> is an image you wish to histogram match, or a normalized histogram in a txt file.
##### Images must be 8 bit greyscale. Histogram must be 256 lines long, with all values adding up to 1.