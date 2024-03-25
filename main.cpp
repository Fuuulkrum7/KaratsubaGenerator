#include <iostream>
#include <sstream>

#include <generator/KaratsubaGen.h>


int get_int() {
    int x = 0;
    std::string line;
    bool done = false;
    
    while (!done && std::getline(std::cin, line)) {
        std::stringstream ss(line);

        done = (ss >> x) && ss.eof();

        if (!done) {
            std::cout << "You need to input an integer" << std::endl;
        }
    }

    return x;
}


int main (int argc, char *args[]) {
    int bit_depth = 0;

    // used for geting n from terminal
    if (argc > 1) {
        std::stringstream ss(args[1]);

        if (!((ss >> bit_depth) && ss.eof())) {
            std::cout << "Please, input an integer" << std::endl;
            bit_depth = get_int();
        }
    }
    else {
        bit_depth = get_int();
    }
    


    return 0;
}