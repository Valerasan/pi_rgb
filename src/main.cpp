#include <iostream>

#ifdef ON_RPI
#include "led-matrix.h"
using rgb_matrix::RGBMatrix;
using rgb_matrix::Canvas;
#endif

int main(int argc, char** argv) {
#ifdef ON_RPI
    RGBMatrix::Options options;
    options.hardware_mapping = "adafruit-hat";  
    options.rows = 32;
    options.cols = 32;
    options.chain_length = 1;

    rgb_matrix::RuntimeOptions runtime;
    runtime.gpio_slowdown = 2;  

    RGBMatrix* matrix = RGBMatrix::CreateFromOptions(options, runtime);
    if (!matrix) {
        std::cerr << "Failed to initialize the matrix\n";
        return 1;
    }

    Canvas* canvas = matrix;
    canvas->Fill(0, 0, 255);          
    std::cin.get();                    

    delete matrix;
#else
    std::cout << "WSL: stub, no hardware available\n";
#endif
    (void)argc; (void)argv;
}