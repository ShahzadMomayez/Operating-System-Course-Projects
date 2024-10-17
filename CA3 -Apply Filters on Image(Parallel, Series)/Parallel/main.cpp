#include <fstream>
#include <iostream>
#include <algorithm>
#include "main.hpp"
using namespace std;

void RGB_Allocate(unsigned char **&dude)
{  
    
    dude = new unsigned char *[rows];
    for (int i = 0; i <= rows; i++)
        dude[i] = new unsigned char[cols];
}


bool fillAndAllocate(char*& buffer, const char* fileName, int& rows, int& cols, int& bufferSize) {
    std::ifstream file(fileName);
    if (!file) {
        std::cout << "File" << fileName << " doesn't exist!" << std::endl;
        return false;
    }

    file.seekg(0, std::ios::end);
    std::streampos length = file.tellg();
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

void* getPixelsFromBMP24(void* tread_id) {
    
    long tid = (long)tread_id;
    int extra = cols % 4;
    int temp = tid * (rows / NUMBER_OF_THREADS);
    int count = temp* (cols * 3 + extra) + 1;
    for (int i = temp ; i < (tid + 1) * ((rows) / NUMBER_OF_THREADS); i++)
    {
        count += extra;

        for (int j = cols - 1; j >= 0; j--) {
            for (int k = 0; k < 3; k++) {
   
                switch (k) {               
                case 0:
                    red[i][j] = fileBuffer[bufferSize - count]; 
      
                    break;
                case 1:
                    green[i][j]= fileBuffer[bufferSize - count];

                    break;
                case 2:
                    blue[i][j] = fileBuffer[bufferSize - count];
                    
                    break;
                }
                count ++;
            }
        }
    }
    pthread_exit(NULL);
}

void* writeOutBmp24(void* tread_id) {
    std::ofstream write(OUTPUTFILE);
    if (!write) {
        std::cout << "Failed to write " << OUTPUTFILE << std::endl;
        exit(-1);
    }

    long tid = (long)tread_id;
    int extra = cols % 4;
    int temp = tid * (rows / NUMBER_OF_THREADS);
    int count = temp* (cols * 3 + extra) + 1;
    for (int i = temp ; i < (tid + 1) * ((rows) / NUMBER_OF_THREADS); i++)
    {
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
    pthread_exit(NULL);
}

void* flipVertical(void* thread_id) {   
    long tid = (long)thread_id;
    int init_val =tid * ((rows) / NUMBER_OF_THREADS);
    int end_val =(tid + 1) * ((rows) / NUMBER_OF_THREADS) ;
    unsigned char temp;  

    for (int i = 0; i < rows/2; ++i)
        for (int j = init_val; j < end_val ; ++j){
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
    pthread_exit(NULL);
}

void* filterBlured(void* thread_id){
    long tid = (long)thread_id;
    unsigned int convFilter[3][3] = {
        {1,2,1},
        {2,4,2},
        {1,2,1}
    };   
    int init_val =tid * ((rows) / NUMBER_OF_THREADS);
    int end_val =(tid + 1) * ((rows) / NUMBER_OF_THREADS);
    
    for (int x = init_val; x < end_val ; x++) {
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
    pthread_exit(NULL);
}

void* purpleHaze(void* thread_id){
    
    long tid = (long)thread_id;
    int t_red;
    int t_green;
    int t_blue;
    int init_val =tid * ((rows) / NUM_OF_PURPLE_THREADS);
    int end_val =(tid + 1) * ((rows) / NUM_OF_PURPLE_THREADS);

     for (int i = init_val; i < end_val; ++i){
        for (int j = 0; j < cols ; ++j){

            t_red= 0.5* red[i][j] + 0.3 *green[i][j] + 0.5 *blue[i][j];
            t_green= 0.16* red[i][j]+ 0.5 *green[i][j]+ 0.16 *blue[i][j];
            t_blue= 0.6 *red[i][j]+ 0.2 *green[i][j] + 0.8 *blue[i][j];

            if (t_red > 255) {
                red[i][j] = 255;
            } else {
                red[i][j] = (uint)t_red;
            }

            if (t_green > 255) {
             green[i][j] = 255;
            } else {
                green[i][j] = (uint)t_green;
            }

            if (t_blue > 255) {
                blue[i][j] = 255;
            } else {
                   blue[i][j] = (uint)t_blue;
            }
        }       
    }   
}



void* diagonalHatch (void* thread_id){
    
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
     
}

void getInput(){
    auto startTime = std::chrono::high_resolution_clock::now();
    pthread_t threads[NUMBER_OF_THREADS];
    int return_code;
    for (long tid = 0; tid < NUMBER_OF_THREADS; tid++)
    {
        return_code = pthread_create(&threads[tid], NULL, getPixelsFromBMP24, (void *)tid);

        if (return_code)
        {
            cout << "Error! pthread_create() Failed!" << return_code << endl;
            exit(-1);
        }
    }

    for (long tid = 0; tid < NUMBER_OF_THREADS; tid++)
    {
        return_code = pthread_join(threads[tid], NULL);
        if (return_code)
        {
            cout << "Error! pthread_join() Failed!" << return_code << endl;
            exit(-1);
        }
    }
    auto endTime = std::chrono::high_resolution_clock::now();
    cout << "Read: " << std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count() << " milliseconds" << endl;

   
}

void getOutput(){
     auto startTime = std::chrono::high_resolution_clock::now();
    pthread_t threads[NUMBER_OF_THREADS];
    int return_code;
    for (long tid = 0; tid < NUMBER_OF_THREADS; tid++)
    {
        return_code = pthread_create(&threads[tid], NULL, writeOutBmp24, (void *)tid);

        if (return_code)
        {
            cout << "Error! pthread_create() Failed!" << return_code << endl;
            exit(-1);
        }
    }

    for (long tid = 0; tid < NUMBER_OF_THREADS; tid++)
    {
        return_code = pthread_join(threads[tid], NULL);
        if (return_code)
        {
            cout << "Error! pthread_join() Failed!" << return_code << endl;
            exit(-1);
        }
    }
     auto endTime = std::chrono::high_resolution_clock::now();
    cout << "Write:" << std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count() << " milliseconds" << endl;

}
void geFlipVertical(){
    auto startTime = std::chrono::high_resolution_clock::now();
    pthread_t threads[NUMBER_OF_THREADS];
    int return_code;
    for (long tid = 0; tid < NUMBER_OF_THREADS; tid++)
    {
        return_code = pthread_create(&threads[tid], NULL, flipVertical, (void *)tid);

        if (return_code)
        {
            cout << "Error! pthread_create() Failed!" << return_code << endl;
            exit(-1);
        }
    }

    for (long tid = 0; tid < NUMBER_OF_THREADS; tid++)
    {
        return_code = pthread_join(threads[tid], NULL);
        if (return_code)
        {
            cout << "Error! pthread_join() Failed!" << return_code << endl;
            exit(-1);
        }
    }
    auto endTime = std::chrono::high_resolution_clock::now();
    cout << "Flip:" << std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count() << " milliseconds" << endl;
}

void getFilterBlured(){
     auto startTime = std::chrono::high_resolution_clock::now();
     pthread_t threads[NUMBER_OF_THREADS];
    int return_code;
    for (long tid = 0; tid < NUMBER_OF_THREADS; tid++)
    {
        return_code = pthread_create(&threads[tid], NULL, filterBlured, (void *)tid);

        if (return_code)
        {
            cout << "Error! pthread_create() Failed!" << return_code << endl;
            exit(-1);
        }
    }

    for (long tid = 0; tid < NUMBER_OF_THREADS; tid++)
    {
        return_code = pthread_join(threads[tid], NULL);
        if (return_code)
        {
            cout << "Error! pthread_join() Failed!" << return_code << endl;
            exit(-1);
        }
    }
    auto endTime = std::chrono::high_resolution_clock::now();
    cout << "Blure: " << std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count() << " milliseconds" << endl;
}

void getDiagonalHatch(){
    auto startTime = std::chrono::high_resolution_clock::now();
    int NumOfHatchThreads =2 ;
    pthread_t threads[2];
    int return_code;
    for (long tid = 0; tid < 2; tid++)
    {
        return_code = pthread_create(&threads[tid], NULL, diagonalHatch, (void *)tid);

        if (return_code)
        {
            cout << "Error! pthread_create() Failed!" << return_code << endl;
            exit(-1);
        }
    }

    for (long tid = 0; tid < 2; tid++)
    {
        return_code = pthread_join(threads[tid], NULL);
        if (return_code)
        {
            cout << "Error! pthread_join() Failed!" << return_code << endl;
            exit(-1);
        }
    }
     auto endTime = std::chrono::high_resolution_clock::now();
    cout << "Lines: " << std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count() << " milliseconds" << endl;
 
}

void getPurpleHaze(){
    auto startTime = std::chrono::high_resolution_clock::now();
    pthread_t threads[NUM_OF_PURPLE_THREADS];
    int return_code;
    for (long tid = 0; tid < NUM_OF_PURPLE_THREADS; tid++)
    {
        return_code = pthread_create(&threads[tid], NULL, purpleHaze, (void *)tid);

        if (return_code)
        {
            cout << "Error! pthread_create() Failed!" << return_code << endl;
            exit(-1);
        }
    }

    for (long tid = 0; tid < NUM_OF_PURPLE_THREADS; tid++)
    {
        return_code = pthread_join(threads[tid], NULL);
        if (return_code)
        {
            cout << "Error! pthread_join() Failed!" << return_code << endl;
            exit(-1);
        }
    }
     auto endTime = std::chrono::high_resolution_clock::now();
    cout << "Purple: " << std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count() << " milliseconds" << endl;

}

int main(int argc, char* argv[]) {
    
    
    if (!fillAndAllocate(fileBuffer, argv[1], rows,  cols, bufferSize)) {
        std::cout << "File read error" << std::endl;
        return 1;
        std :: cout <<rows << cols ;
    }

    
    RGB_Allocate(red);
    RGB_Allocate(green);
    RGB_Allocate(blue);

    auto startTime = chrono::high_resolution_clock::now();
   
    getInput();
    geFlipVertical();
    getFilterBlured();
    getPurpleHaze();
    getDiagonalHatch();
    getOutput() ;

    auto endTime = chrono::high_resolution_clock::now();
    cout << "Execution: " << chrono::duration_cast<chrono::milliseconds>(endTime - startTime).count() << " milliseconds" << endl;
    return 0;
}
