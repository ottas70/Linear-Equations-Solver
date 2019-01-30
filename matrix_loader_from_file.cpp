#include "matrix_loader_from_file.hpp"
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <tuple>

std::tuple<int, int, std::unique_ptr<double[]>> matrix_loader_from_file::load(std::string filename) {

    int rows = 0;
    int columns = 0;

    std::vector<std::string> numbers;

    std::string line;
    std::ifstream myfile (filename);
    if (myfile.is_open()) {
        while (getline(myfile,line)) {
            std::stringstream ss(line);
            std::istream_iterator<std::string> begin(ss);
            std::istream_iterator<std::string> end;
            std::vector<std::string> line(begin, end);
            numbers.insert(numbers.end(), line.begin(), line.end());
            if(rows == 0){
                columns = line.size();
            }else{
                if(columns != line.size()){
                    std::cout << "Matrix is not correct";
                    exit(-1);
                }
            }
            rows++;
        }
        myfile.close();
    } else {
        std::cout << "Unable to open file";
        exit(-1);
    }

    columns = numbers.size()/rows;

    std::unique_ptr<double[]> data(new double[numbers.size()]);
    for (int i = 0; i < numbers.size(); ++i) {
        try{
            data[i] = stod(numbers.at(i));
        }catch (...){
            std::cout << "Matrix is not correct";
            exit(-1);
        }

    }

    return std::make_tuple(rows, columns, std::move(data));

}
