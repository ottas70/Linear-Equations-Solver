#include "cmdline.hpp"
#include "matrix_loader_from_file.hpp"
#include "matrix.hpp"
#include <iostream>
#include <chrono>

template <typename TimePoint>
std::chrono::milliseconds to_ms(TimePoint tp) {
    return std::chrono::duration_cast<std::chrono::milliseconds>(tp);
}

void cmdline::help() const {
    std::cout << "Program can have only 2 arguments at the same time\n";
    std::cout << "--help -> this menu\n"
                 "--sequence -> sequentional algorithm\n"
                 "--parallel -> parallel algorithm\n";
    std::cout << "Necessary argument is filename/path to text file with matrix.\n"
                 "It can be the only argument or third argument after selection of algorithm type.\n"
                 "Default setup uses sequentinal algorithm";
}

void cmdline::sequentional(std::string filename) const {
    matrix_loader_from_file loader;
    auto tuple = loader.load(filename);
    matrix m(std::get<0>(tuple),std::get<1>(tuple),std::move(std::get<2>(tuple)));

    auto start = std::chrono::high_resolution_clock::now();
    m.solveMatrix();
    auto end = std::chrono::high_resolution_clock::now();
    std::cout << "Time: " << to_ms(end - start).count() << "ms";
    std::cout << "\n";
}

void cmdline::parallel(std::string filename) const {
    matrix_loader_from_file loader;
    auto tuple = loader.load(filename);
    matrix m(std::get<0>(tuple),std::get<1>(tuple),std::move(std::get<2>(tuple)));

    auto start2 = std::chrono::high_resolution_clock::now();
    m.solveMatrixConcurrent();
    auto end2 = std::chrono::high_resolution_clock::now();
    std::cout << "Time: " << to_ms(end2 - start2).count() << "ms";
    std::cout << "\n";
}

void cmdline::invalid() const{
    std::cout << "Invalid input. For help type --help";
}
