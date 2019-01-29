#include <iostream>
#include <vector>
#include "matrix.hpp"

int main() {
    //infinite solutions 1
    /*std::unique_ptr<double[]> data(
            new double[20]{1,3,2,0,3,
                           1,1,1,-1,5,
                           2,8,5,3,7,
                           3,9,6,2,12});

    matrix m(4,5, std::move(data));*/

    //infinite solutions 2
    std::unique_ptr<double[]> data(
            new double[20]{1,-2,3,-4,4,
                           0,1,-1,1,-3,
                           1,3,0,-3,1,
                           0,-7,3,1,-3});

    matrix m(4,5, std::move(data));

    //infinite solutions 3
    /*std::unique_ptr<double[]> data(
            new double[20]{1,2,4,-3,0,
                           3,5,6,-4,0,
                           4,5,-2,3,0,
                           3,8,24,-19,0});

    matrix m(4,5, std::move(data));*/

    //no solution
    /*std::unique_ptr<double[]> data(
            new double[12]{1,2,3,1,
                          2,1,2,1,
                          4,5,8,2});

    matrix m(3,4, std::move(data));*/

    //one solution
    /*std::unique_ptr<double[]> data(
            new double[12]{2,3,7,47,
                          3,8,1,50,
                          0,3,3,27});

    matrix m(3,4, std::move(data));*/



    m.printMatrix(std::cout);
    m.solveMatrix();
}