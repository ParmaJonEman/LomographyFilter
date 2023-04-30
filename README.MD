# Lomo
### Setup

- [Install OpenCV](https://opencv.org/releases/), and replace the path in the CMAKE file on the find_package line with your new path
- If you're on Windows, [download dirent.h](https://github.com/tronkko/dirent) and put in your linker's include folder
- Build the project with the standard CMAKE build commands

### Run

- Run the application via the command line with the following usage patten:
    - lomo.exe <image>
- Example:
    - lomo.exe cat.jpg
- Key commands:
    - s or S to save the image and exit (Image is aved as Output.jpg)
    - q or Q to quit the application without saving the image