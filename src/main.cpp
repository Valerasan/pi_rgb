#include <chrono>
#include <csignal>
#include <fstream>
#include <iostream>
#include <iterator>
#include <thread>
#include <vector>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_RESIZE2_IMPLEMENTATION
#include "stb_image_resize2.h"

#ifdef ON_RPI
#include "led-matrix.h"
#include <unistd.h>
#endif

static volatile std::sig_atomic_t g_stop = 0;
static void on_signal(int) { g_stop = 1; }

int main(int argc, char** argv) {


    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " image.png\n";
        return 1;
    }

    // int w, h, ch;
    // unsigned char* img = stbi_load(argv[1], &w, &h, &ch, 3);
    // if (!img) {
    //     std::cerr << "Failed to open " << argv[1] << ": "
    //               << stbi_failure_reason() << "\n";
    //     return 1;
    // }
    // std::cout << "Loaded " << w << "x" << h << "\n";

    // const int N = 64;
    // std::vector<unsigned char> small(N * N * 3);
    // stbir_resize_uint8_linear(img, w, h, 0, small.data(), N, N, 0, STBIR_RGB);
    // stbi_image_free(img);


    std::ifstream file(argv[1], std::ios::binary);
    if (!file) {
        std::cerr << "Failed to open " << argv[1] << "\n";
        return 1;
    }
    std::vector<unsigned char> bytes((std::istreambuf_iterator<char>(file)),
                                     std::istreambuf_iterator<char>());


    int w, h, frame_count, ch;
    int* delays = nullptr;
    unsigned char* data = stbi_load_gif_from_memory(
    bytes.data(), (int)bytes.size(), &delays, &w, &h, &frame_count, &ch, 3);
    if (!data) {
        std::cerr << "Failed to decode GIF: " << stbi_failure_reason() << "\n";
        return 1;
    }


    const int N = 64;
    std::vector<std::vector<unsigned char>> frames (frame_count, std::vector<unsigned char>(N * N * 3));
    std::vector<int> delay_ms(frame_count);

    for(int f = 0; f < frame_count; ++f) {
        const unsigned char* src = data + (size_t)f * w * h * 3;
        stbir_resize_uint8_linear(src, w, h, 0, frames[f].data(), N, N, 0, STBIR_RGB);

        delay_ms[f] = delays[f] <= 10 ? 100 : delays[f];
    }

    stbi_image_free(data);
    stbi_image_free(delays);


#ifdef ON_RPI
rgb_matrix::RGBMatrix::Options options;
    options.hardware_mapping = "adafruit-hat";  
    options.rows = 64;
    options.cols = 64;
    options.chain_length = 1;
    options.brightness = 50;
    options.pwm_bits = 7;                        
    options.pwm_lsb_nanoseconds = 200;
    options.limit_refresh_rate_hz = 100;

    rgb_matrix::RuntimeOptions runtime;
    runtime.gpio_slowdown = 0; 

    rgb_matrix::RGBMatrix* matrix = rgb_matrix::RGBMatrix::CreateFromOptions(options, runtime);
    if (!matrix) {
        std::cerr << "Failed to initialize the matrix\n";
        return 1;
    }

    std::signal(SIGINT, on_signal);
    std::signal(SIGTERM, on_signal);

    rgb_matrix::FrameCanvas* canvas = matrix->CreateFrameCanvas();

    auto next = std::chrono::steady_clock::now();
    while (!g_stop) {
        for (int f = 0; f < frame_count && !g_stop; ++f) {
            const std::vector<unsigned char>& px = frames[f];
            for (int y = 0; y < N; ++y)
                for (int x = 0; x < N; ++x) {
                    const unsigned char* p = &px[(y * N + x) * 3];
                    canvas->SetPixel(x, y, p[0], p[1], p[2]);
                }


            std::this_thread::sleep_until(next);
            canvas = matrix->SwapOnVSync(canvas);
            next += std::chrono::milliseconds(delay_ms[f]);
        }
    }

    matrix->Clear();
    delete matrix;

#else
    std::cout << "WSL: stub, no hardware available\n";
#endif
    (void)argc; (void)argv;
}