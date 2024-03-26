#include <iostream>
#include <sstream>

#include <generator/KaratsubaGen.h>


u_int32_t get_int() {
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


int main (int argc, char *args[]) {
    int bit_depth = 0;

    // used for geting n from terminal
    if (argc > 1) {
        std::stringstream ss(args[1]);

        if (!((ss >> bit_depth) && ss.eof()) || bit_depth <= 0) {
            bit_depth = get_int();
        }
    }
    else {

        bit_depth = get_int();
    }
    


    return 0;
}