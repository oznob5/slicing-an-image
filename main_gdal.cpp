#include <iostream>
#include "gdal.h"
#include "gdal_priv.h"
#include "cpl_conv.h"
#include "cpl_string.h"
#include <ctime>
using namespace std;

bool existDir(const char* name);
void createDir();
int sliceImage(const char* imgPath);

int main() {
	clock_t start_time = clock();

	const char* imgPath = "fr_0041_0308_22184_1_02298_02_G_S_21.tiff";
	sliceImage(imgPath);

	clock_t end_time = clock();
  	double seconds = (double) (end_time - start_time) / CLOCKS_PER_SEC;
  	cout << seconds << endl;

	return 0;
}

bool existDir(const char * name) {
  struct stat s;
  if (stat(name, &s)) return false;
  return S_ISDIR(s.st_mode);
};

void createDir() {
  if (existDir("path_2"))
    {
      system("rm -rf path_2/");
    }
  int check = mkdir("path_2", S_IRUSR | S_IWUSR | S_IXUSR | S_IXGRP | S_IXOTH);
}

int sliceImage(const char* imgPath) {
	createDir();
	GDALAllRegister();
	CPLSetConfigOption("GDAL_FILENAME_IS_UTF8", "NO");
	
	GDALDataset* img = (GDALDataset *)GDALOpen(imgPath, GA_ReadOnly);
	if (img == nullptr) {
	    cout << "Can't Open Image!" << endl;
	    return 1;
	}
	int imgWidth = img->GetRasterXSize();   // Ширина изображения
	int imgHeight = img->GetRasterYSize();  // Высота изображения
	int bandNum = img->GetRasterCount();    // Количество полос
	int depth = GDALGetDataTypeSize(img->GetRasterBand(1)->GetRasterDataType());  // Глубина 

	GDALDriver *pDriver = GetGDALDriverManager()->GetDriverByName("GTIFF"); // Драйвер изображения
	char** ppszOptions = NULL;
	ppszOptions = CSLSetNameValue(ppszOptions, "BIGTIFF", "IF_NEEDED"); // Настройка информации об изображении

	int bufWidth = 700;
	int bufHeight = 700;
	string dstPath_string;
	size_t imgBufNum = (size_t) bufWidth * bufHeight * bandNum * depth;
	size_t imgBufOffset = (size_t) bufWidth * (bufHeight-1) * bandNum * depth;
	

	for (int i = 0; i < imgWidth - bufWidth; i+= bufWidth) {
		for (int j = 0; j < imgHeight - bufHeight; j+= bufHeight) {
			dstPath_string = "path_2//IMG_" + to_string(j/bufHeight) + "_" + to_string(i/bufWidth) + ".tiff";
			const char* dstPath = dstPath_string.c_str();
			GDALDataset* dst = pDriver->Create(dstPath, bufWidth, bufHeight, bandNum, GDT_Byte, ppszOptions);
			if (dst == nullptr) {
			    printf("Can't Write Image!");
			    return 1;
			}
			GByte *imgBuf = new GByte[imgBufNum];
			img->RasterIO(GF_Read, i, j, bufWidth, bufHeight, imgBuf + imgBufOffset, bufWidth, bufHeight,
			    GDT_Byte, bandNum, nullptr, bandNum*depth, -bufWidth*bandNum*depth, depth);
			dst->RasterIO(GF_Write, 0, 0, bufWidth, bufHeight, imgBuf + imgBufOffset, bufWidth, bufHeight,
			    GDT_Byte, bandNum, nullptr, bandNum*depth, -bufWidth*bandNum*depth, depth);
			delete[] imgBuf;
			imgBuf = nullptr;
			GDALClose(dst);
		}
	}
	GDALClose(img);
	return 0;
}
