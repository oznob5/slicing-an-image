#include <sys/types.h>
#include <sys/stat.h>
#include <opencv2/highgui.hpp>
#include <opencv2/opencv.hpp>
#include <iostream>
#include <unistd.h>
#include <ctime>

bool existDir(const char * name);
void createDir();
void sliceImage(cv::Mat image);

int main() {
  clock_t start_time = clock();
  cv::Mat image = cv::imread("fr_0041_0102_22658_1_02777_08_G_S.tiff");
  if (image.empty()) {
      std::cout << "!!! imread() failed to open target image" << std::endl;
        return -1;        
  }
  createDir();
  sliceImage(image);
  clock_t end_time = clock();
  double seconds = (double) (end_time - start_time) / CLOCKS_PER_SEC;
  std::cout << seconds << std::endl;
  return 0;
}

bool existDir(const char * name) {
  struct stat s;
  if (stat(name,&s)) return false;
  return S_ISDIR(s.st_mode);
};

void createDir() {
  if (existDir("path")) {
      system("rm -rf path/");   // only linux
    }
  int check = mkdir("path", S_IRUSR | S_IWUSR | S_IXUSR | S_IXGRP | S_IXOTH);
}

void sliceImage(cv::Mat image) {
  char outString[50];
  int step = 200;
  cv::Mat img;
  int i, j;
  int x = image.cols;
  int y = image.rows;
  for(i = 0; i < x; i += step) {
    if (i + step > x) {
      i = x - step;
    }
    for(j = 0; j < y; j += step) {
      if (j + step > y) {
        j = y - step;
      }
      cv::Rect crop_region(i, j, step, step);
      img = image(crop_region);
      sprintf(outString, "path//IMG_%d_%d.jpg", j/step, i/step);
      cv::imwrite(outString,img);
    }
  }
}
