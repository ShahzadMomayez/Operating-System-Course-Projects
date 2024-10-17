#include <fstream>
#include <iostream>
#include <algorithm>
# include "bmp.hpp"


using namespace std;

void RGB_Allocate(unsigned char **&dude)
{  
    
    dude = new unsigned char *[rows];
    for (int i = 0; i <= rows; i++)
        dude[i] = new unsigned char[cols];
}

bool fillAndAllocate(char*& buffer, const char* fileName, int& rows, int& cols, int& bufferSize) {
    
    ifstream file(fileName);
    if (!file) {
        std::cout << "File" << fileName << " doesn't exist!" << endl;
        return false;
    }

    file.seekg(0, std::ios::end);
    streampos length = file.tellg();
    file.seekg(0, std::ios::beg);

    buffer = new char[length];
    file.read(&buffer[0], length);

    PBITMAPFILEHEADER file_header;
    PBITMAPINFOHEADER info_header;

    file_header = (PBITMAPFILEHEADER)(&buffer[0]);
    info_header = (PBITMAPINFOHEADER)(&buffer[0] + sizeof(BITMAPFILEHEADER));
    rows = info_header->biHeight;
    cols = info_header->biWidth;
    bufferSize = file_header->bfSize;
    return true;
}

void getPixelsFromBMP24(int end, int rows, int cols, char* fileReadBuffer) {
    auto startTime= chrono::high_resolution_clock::now();
    int count = 1;
    int extra = cols % 4;


    for (int i = 0; i < rows; i++) {
        count += extra;
        for (int j = cols - 1; j >= 0; j--) {
            for (int k = 0; k < 3; k++) {
                
                switch (k) {               
                case 0:
                    red[i][j] = fileReadBuffer[end - count];                    
                    break;
                case 1:
                    green[i][j]= fileReadBuffer[end - count];
                    break;
                case 2:
                    blue[i][j] = fileReadBuffer[end - count];
                    break;
                }
                count ++;

            }
        }
    }
    auto endTime= chrono::high_resolution_clock::now();
    cout << "Read: " << chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count() << " milliseconds" << endl;
}

void writeOutBmp24(char* fileBuffer, const char* nameOfFileToCreate, int bufferSize) {
    auto startTime= chrono::high_resolution_clock::now();
    std::ofstream write(nameOfFileToCreate);
    if (!write) {
        std::cout << "Failed to write " << nameOfFileToCreate << std::endl;
        return;
    }

    int count = 1;
    int extra = cols % 4;
    for (int i = 0; i < rows; i++) {
        count += extra;
        for (int j = cols - 1; j >= 0; j--) {
            for (int k = 0; k < 3; k++) {
                switch (k) {
                case 0:
                    fileBuffer[bufferSize - count] = red[i][j];
                    break;
                case 1:
                    fileBuffer[bufferSize - count] = green[i][j];
                    break;
                case 2:
                    fileBuffer[bufferSize - count] = blue[i][j];
                    break;
                }
                count ++;
            }
        }
    }
    write.write(fileBuffer, bufferSize);
     auto endTime= chrono::high_resolution_clock::now();
    cout << "Write: " << chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count() << " milliseconds" << endl;
}

void flipVertical() {     
    auto startTime= chrono::high_resolution_clock::now();
    unsigned char temp;      
    for (int i = 0; i < rows/2; ++i)
        for (int j = 0; j < cols ; ++j){
                    temp =  red[rows-i-1][j];
                    red[rows-i-1][j] = red[i][j];
                    red[i][j] = temp;

                    temp =  green[rows-i-1][j];
                    green[rows-i-1][j] = green[i][j];
                    green[i][j] = temp;

                    temp =  blue[rows-i-1][j];
                    blue[rows-i-1][j] = blue[i][j];
                    blue[i][j] = temp;                  
        }        
         auto endTime= chrono::high_resolution_clock::now();
    cout << "Flip: " << chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count() << " milliseconds" << endl;       
}

void filterBlured(){
    auto startTime= chrono::high_resolution_clock::now();
    unsigned int convFilter[3][3] = {
        {1,2,1},
        {2,4,2},
        {1,2,1}
    };   

    for (int x = 0; x < rows; x++) {
        for (int y = 0; y < cols; y++) {
            int outputPixelRed = 0;
            int outputPixelBlue = 0;
            int outputPixelGreen = 0;
            for (int i = -1; i <= 1; i++) {
                for (int j = -1; j <= 1; j++) {
                    if (x + i >= 0 && y + j >= 0 && x + i < rows && y + j < cols){
                    outputPixelRed += int (red[x +i][y+j] )* convFilter[i+1][j+1];   
                     outputPixelGreen += int(green[x +i][y+j]) * convFilter[i+1][j+1];    
                     outputPixelBlue +=int( blue[x +i][y+j] )* convFilter[i+1][j+1];               
                }}
            }        

            outputPixelRed = outputPixelRed/16;
            outputPixelGreen=outputPixelGreen/ 16;
            outputPixelBlue =outputPixelBlue/ 16;

            if (outputPixelRed > 255 ) 
                outputPixelRed = 255;
            if (outputPixelRed < 0 ) 
                outputPixelRed = 0;

            if (outputPixelGreen > 255) 
                 outputPixelGreen= 255;
            if (outputPixelGreen < 0) 
                 outputPixelGreen= 0;
            

            if (outputPixelBlue > 255) 
                outputPixelBlue= 255;
            if (outputPixelBlue < 0) 
                outputPixelBlue= 0;   

            red[x][y] = outputPixelRed ;
            green[x][y] = outputPixelGreen;
            blue[x][y] =outputPixelBlue ;
        }
    }
     auto endTime= chrono::high_resolution_clock::now();
    cout << "Blure: " << chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count() << " milliseconds" << endl;

}

void purpleHaze(){
    auto startTime= chrono::high_resolution_clock::now();
    int t_red;
    int t_green;
    int t_blue;
    
     for (int i = 0; i < rows; i++){
        for (int j = 0; j < cols ; j++){

            t_red= 0.5* red[i][j] + 0.3 *green[i][j] + 0.5 *blue[i][j];
            t_green= 0.16* red[i][j]+ 0.5 *green[i][j]+ 0.16 *blue[i][j];
            t_blue= 0.6 *red[i][j]+ 0.2 *green[i][j] + 0.8 *blue[i][j];

            if (t_red > 255 ) 
                t_red = 255;
            if (t_red < 0 ) 
                t_red = 0;

            if (t_green > 255) 
                 t_green= 255;
            if (t_green < 0) 
                 t_green= 0;
            

            if (t_blue > 255) 
                t_blue= 255;
            if (t_blue < 0) 
                t_blue= 0;
            
            red[i][j] = (uint)t_red;
            green[i][j] = (uint)t_green;
            blue[i][j]= (uint)t_blue;
            
        }
        
    }
     auto endTime= chrono::high_resolution_clock::now();
    cout << "Purple: " << chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count() << " milliseconds" << endl;

    
}



void diagonalHatch (){
    auto startTime= chrono::high_resolution_clock::now();
    float gradient = (float)rows/(float)cols;
    for ( int i = 0 ; i< rows  ; i++ ){
        for( int j= 0 ; j< cols ; j++){
            if (i==(cols-gradient* j)){
                red[i][j] = 255;
                green[i][j] = 255;
                blue[i][j] = 255;
            }
            if (i==(0.5*cols-gradient*j)){
                red[i][j] = 255;
                green[i][j] = 255;
                blue[i][j] = 255;
            }
            if (i==(1.5*cols-gradient*j)){
                red[i][j] = 255;
                green[i][j] = 255;
                blue[i][j] = 255;
            }
                     
        }
    }
     auto endTime= chrono::high_resolution_clock::now();
    cout << "Lines: " << chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count() << " milliseconds" << endl;
}




int main(int argc, char* argv[]) {
    char* fileBuffer;
    int bufferSize;
    if (!fillAndAllocate(fileBuffer, argv[1], rows,  cols, bufferSize)) {
        cout << "File read error" << std::endl;
        return 1;
    }


    RGB_Allocate(red);
    RGB_Allocate(green);
    RGB_Allocate(blue);

    auto startTime = chrono::high_resolution_clock::now();

    getPixelsFromBMP24(bufferSize, rows, cols, fileBuffer);  
    flipVertical();
   filterBlured();
    purpleHaze();
   diagonalHatch();
    writeOutBmp24(fileBuffer, "out.bmp", bufferSize) ;


    auto endExecTime = chrono::high_resolution_clock::now();
    cout << "Execution: " << chrono::duration_cast<std::chrono::milliseconds>(endExecTime - startTime).count() << " milliseconds" << endl;
    return 0;
}
