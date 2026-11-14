#include <iostream>
#include <vector>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_RESIZE2_IMPLEMENTATION
#include "stb_image_resize2.h"

#ifdef ON_RPI
#include "led-matrix.h"
#include <unistd.h>
#endif

int main(int argc, char** argv) {


    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " image.png\n";
        return 1;
    }

    int w, h, ch;
    unsigned char* img = stbi_load(argv[1], &w, &h, &ch, 3);
    if (!img) {
        std::cerr << "Failed to open " << argv[1] << ": "
                  << stbi_failure_reason() << "\n";
        return 1;
    }
    std::cout << "Loaded " << w << "x" << h << "\n";

    const int N = 64;
    std::vector<unsigned char> small(N * N * 3);
    stbir_resize_uint8_linear(img, w, h, 0, small.data(), N, N, 0, STBIR_RGB);
    stbi_image_free(img);


#ifdef ON_RPI
rgb_matrix::RGBMatrix::Options options;
    options.hardware_mapping = "adafruit-hat";  
    options.rows = 64;
    options.cols = 64;
    options.chain_length = 1;
    options.brightness = 50;

    rgb_matrix::RuntimeOptions runtime;
    runtime.gpio_slowdown = 0; 

    rgb_matrix::RGBMatrix* matrix = rgb_matrix::RGBMatrix::CreateFromOptions(options, runtime);
    if (!matrix) {
        std::cerr << "Failed to initialize the matrix\n";
        return 1;
    }

    for (int y = 0; y < N; ++y)
        for (int x = 0; x < N; ++x) {
            const unsigned char* p = &small[(y * N + x) * 3];
            matrix->SetPixel(x, y, p[0], p[1], p[2]);
        }

    std::cout << "Done. Press Enter to exit.\n";
    std::cin.get();  
    delete matrix;
#else
    std::cout << "WSL: stub, no hardware available\n";
#endif
    (void)argc; (void)argv;
}