#include "q_m.h"
#include "../q_m.cpp"

int main(int argc, char* argv[]) {
    std::string filename;
    if (argc > 1) {
        filename = argv[1];
    } else {
        filename = "Test7.txt";
        std::cout << "No input file specified. Using default: " << filename << "\n";
    }

    return qm_minimize(filename);
}
