
#ifndef MAIN_HPP
#define MAIN_HPP

#include <thread>
#include <iostream>
#include <vector>
#include <iostream>
#include <algorithm>
#include <unistd.h>
#include <fstream>
#include <chrono>
#include <pthread.h>
using namespace std;



typedef int LONG;
typedef unsigned short WORD;
typedef unsigned int DWORD;

#pragma pack(push, 1)
typedef struct tagBITMAPFILEHEADER {
    WORD bfType;
    DWORD bfSize;
    WORD bfReserved1;
    WORD bfReserved2;
    DWORD bfOffBits;
} BITMAPFILEHEADER, *PBITMAPFILEHEADER;

typedef struct tagBITMAPINFOHEADER {
    DWORD biSize;
    LONG biWidth;
    LONG biHeight;
    WORD biPlanes;
    WORD biBitCount;
    DWORD biCompression;
    DWORD biSizeImage;
    LONG biXPelsPerMeter;
    LONG biYPelsPerMeter;
    DWORD biClrUsed;
    DWORD biClrImportant;
} BITMAPINFOHEADER, *PBITMAPINFOHEADER;
#pragma pack(pop)

int rows;
int cols;

unsigned char ** red;
unsigned char ** green;
unsigned char ** blue;
const int WHITE = 255;

char* fileBuffer;
int bufferSize;
char* OUTPUTFILE= "out.bmp";
#define NUMBER_OF_THREADS 8
#define NUM_OF_PURPLE_THREADS 2



#endif