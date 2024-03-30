#include <chrono>
#include <fstream>
#include <iostream>
#include <sstream>
#include <filesystem>

#include <generator/KaratsubaGen.h>

using namespace std::chrono;

int get_int() {
    int x = 0;
    std::string line;
    bool done = false;

    std::cout << "Please, input an integer (more than 0)" << std::endl;

    while (!done && std::getline(std::cin, line)) {
        std::stringstream ss(line);

        done = (ss >> x) && ss.eof();

        if (!done) {
            std::cout << "You need to input an integer" << std::endl;
        }

        if (x <= 0) {
            std::cout << "Input a number more than 0" << std::endl;
        }

        done &= x > 0;
    }

    return x;
}

int main(int argc, char *args[]) {
    int bitDepth = 0;

    // used for geting n from terminal
    if (argc > 1) {
        std::stringstream ss(args[1]);

        if (!((ss >> bitDepth) && ss.eof()) || bitDepth <= 0) {
            bitDepth = get_int();
        }
    } else {

        bitDepth = get_int();
    }

    KaratsubaGen gen(bitDepth);

    auto start = high_resolution_clock::now();

    GraphPtr graph = gen.startGeneration();
    
    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(stop - start);
    std::clog << "Time Gen taken: " << duration.count() << " microseconds"
              << std::endl;

    std::string path = "dataset/";
    std::filesystem::remove_all(path);

    graph->setWritePath(path);

    start = high_resolution_clock::now();

    graph->toVerilog();

    stop = high_resolution_clock::now();
    duration = duration_cast<microseconds>(stop - start);
    std::clog << "Time write taken: " << duration.count() << " microseconds"
              << std::endl;

    return 0;
}