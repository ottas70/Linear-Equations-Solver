#include "matrix.hpp"
#include <iostream>
#include <cmath>
#include <algorithm>
#include <thread>

matrix::matrix(size_t row, size_t column, std::unique_ptr<double[]> matrix) {
    rows = row;
    cols = column;
    data = std::move(matrix);
}

double &matrix::operator()(size_t i, size_t j) {
    return data[i * cols + j];
}

double matrix::operator()(size_t i, size_t j) const {
    return data[i * cols + j];
}

double matrix::getRhs(int row) const {
    return (*this)(row, cols - 1);
}

void matrix::printMatrix(std::ostream &stream) const {
    for (int i = 0; i < rows; ++i) {
        stream << "[";
        for (int j = 0; j < cols; ++j) {
            stream << " " << (*this)(i, j) << " ";
        }
        stream << "]\n";
    }
}

void matrix::solveMatrix() {
    gaussElimination();

    int rankOfMatrix = rank(cols - 1);
    int rankOfExtendedMatrix = rank(cols);

    if (rankOfMatrix != rankOfExtendedMatrix) {
        std::cout << "Matrix does not have a solution\n";
        return;
    }

    if (cols - 1 - rankOfMatrix != 0) {
        std::cout << "Infinite number of solutions" << "\n";
        std::cout << "With particular solution: ";
        countOneSolution();
        countKernel(cols - 1 - rankOfMatrix);
        return;
    }

    calculateValues();
}

void matrix::solveMatrixConcurrent() {
    gaussEliminationConcurrent();

    int rankOfMatrix = rank(cols - 1);
    int rankOfExtendedMatrix = rank(cols);

    if (rankOfMatrix != rankOfExtendedMatrix) {
        std::cout << "Matrix does not have a solution\n";
        return;
    }

    if (cols - 1 - rankOfMatrix != 0) {
        std::cout << "Infinite number of solutions" << "\n";
        std::cout << "With particular solution: ";
        countOneSolution();
        countKernel(cols - 1 - rankOfMatrix);
        return;
    }

    calculateValues();
}


void matrix::gaussElimination() {
    //projdu všechny sloupce kromě pravé strany
    for (int currentCol = 0; currentCol < cols - 1; ++currentCol) {

        partialPivoting(currentCol);

        //pro každý řádek zařídím nuly pod pivotem
        for (int rowIndex = currentCol + 1; rowIndex < rows; ++rowIndex) {

            double koeficient = countKoeficient(currentCol, rowIndex);

            if (koeficient == 0) {
                continue;
            }

            //projdu celý řádek a odečtu od něho ten horní
            multiplyAndSubstractRows(currentCol, rowIndex, koeficient);

        }

    }

    //std::cout << "\nAfter Gauss:\n";
    //printMatrix(std::cout);
    //std::cout << "\n";

}

void matrix::threadFunc(int col, int rowFrom, int rowTo) {
    for (int i = rowFrom; i < rowTo; ++i) {
        double koeficient = countKoeficient(col, i);

        if (koeficient == 0) {
            continue;
        }

        multiplyAndSubstractRows(col, i, koeficient);
    }
}

void matrix::gaussEliminationConcurrent() {
    //projdu všechny sloupce kromě pravé strany
    for (int currentCol = 0; currentCol < cols - 1; ++currentCol) {

        partialPivoting(currentCol);

        std::vector<std::thread> threads;

        int num_threads = std::thread::hardware_concurrency();
        int part_size = (rows - 1 - currentCol) / num_threads;
        int mod = (rows - 1 - currentCol) % num_threads;

        for (int i = 0; i < num_threads; ++i) {
            int from = i * part_size + std::min(i, mod) + currentCol + 1;
            int to = (i + 1) * part_size + std::min(i + 1, mod) + currentCol + 1;
            threads.emplace_back(&matrix::threadFunc, this, currentCol, from, to);
        }

        for (auto &thread : threads) {
            thread.join();
        }
        threads.clear();

    }

    //std::cout << "\nAfter Concurrent Gauss:\n";
    //printMatrix(std::cout);
    //std::cout << "\n";
}

void matrix::partialPivoting(int col) {
    int max_row_index = col;
    int max_value = std::abs((*this)(col, col));
    for (int row = col + 1; row < rows; ++row) {
        if (std::abs((*this)(row, col)) > max_value) {
            max_value = (*this)(row, col);
            max_row_index = row;
        }
    }

    if (max_row_index != col) {
        swapRows(col, max_row_index);
    }
}

double matrix::countKoeficient(int column, int row) const {
    for (int colIndex = column; colIndex < cols - 1; ++colIndex) {
        if ((*this)(column, colIndex) == 0) {
            continue;
        } else {
            return -((*this)(row, colIndex) / (*this)(column, colIndex));
        }
    }

    return 0;

}

void matrix::multiplyAndSubstractRows(int startColumn, int row, double koeficient) {
    for (int colIndex = startColumn; colIndex < cols; ++colIndex) {
        double &firstNum = (*this)(row, colIndex);
        double secondNum = (*this)(startColumn, colIndex);
        double result = (firstNum + secondNum * koeficient);
        if(std::abs(result) < 1.0e-10){
            firstNum = 0;
        }else{
            firstNum = result;
        }
    }
}

void matrix::calculateValues() {
    double results[cols - 1];

    //počítám od posledního řádku(rovnice)
    for (int rowIndex = rows - 1; rowIndex >= 0; --rowIndex) {

        double rhs = getRhs(rowIndex);

        solveLinearEquation(rowIndex, rowIndex, rhs, results);

    }

    std::cout << "Result is vector (";
    for (double d : results) {
        std::cout << " " << d << " ";
    }
    std::cout << ")\n";

}

void matrix::solveLinearEquation(int row, int firstElementPosition, double rhs, double* results) {
    double leftSideSum = 0;
    for (int i = cols - 2; i > firstElementPosition; --i) {
        leftSideSum += (*this)(row, i) * results[i];
    }

    results[firstElementPosition] = ((rhs - leftSideSum) / (*this)(row, firstElementPosition));
}

void matrix::countOneSolution() {
    double results[cols-1];
    std::vector<int> pivots = positionsOfPivots();

    fillNonPivotPositions(results, pivots, -1);

    for (int rowIndex = rows - 1; rowIndex >= 0; --rowIndex) {

        int pivotPosition = findPivotPosition(rowIndex);

        if (pivotPosition == -1) {
            continue;
        }

        double rhs = getRhs(rowIndex);

        solveLinearEquation(rowIndex, pivotPosition, rhs, results);

    }

    std::cout << "(";
    for (double d : results) {
        std::cout << " " << d << " ";
    }
    std::cout << ")";

}


void matrix::countKernel(int numOfVectors) {
    std::cout << " + span [";
    std::vector<int> pivots = positionsOfPivots();

    for (int kernelVectorCount = 0; kernelVectorCount < numOfVectors; ++kernelVectorCount) {

        double results[cols - 1];

        fillNonPivotPositions(results, pivots, kernelVectorCount);

        for (int rowIndex = rows - 1; rowIndex >= 0; --rowIndex) {

            int pivotPosition = findPivotPosition(rowIndex);
            if (pivotPosition == -1) {
                continue;
            }

            double rhs = 0;

            solveLinearEquation(rowIndex, pivotPosition, rhs, results);

        }

        std::cout << "(";
        for (double d : results) {
            std::cout << " " << d << " ";
        }
        std::cout << ")";
    }
    std::cout << "]\n";

}

void matrix::fillNonPivotPositions(double* results, std::vector<int> pivotsColumns, int kernelCount) {
    int kernelCounter = 0;
    for (int i = 0; i < cols - 1; ++i) {
        if(std::find(pivotsColumns.begin(), pivotsColumns.end(), i) == pivotsColumns.end()){
            if(kernelCount != -1){
                if(kernelCount == kernelCounter){
                    results[i] = 1;
                }else{
                    results[i] = 0;
                }
                kernelCounter++;
                continue;
            }
            results[i] = 0;
        }
    }
}

std::vector<int> matrix::positionsOfPivots() {
    std::vector<int> pivotColumns;
    for (int i = 0; i < rows; ++i) {
        int column = findPivotPosition(i);
        if(column != -1){
            pivotColumns.push_back(column);
        }

    }
    return pivotColumns;

}

int matrix::findPivotPosition(int rowIndex) const {
    for (int i = 0; i < cols - 1; ++i) {
        if ((*this)(rowIndex, i) != 0) {
            return i;
        }
    }
    return -1;
}

int matrix::rank(int numOfCols) const {
    int count = 0;
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < numOfCols; ++j) {
            if ((*this)(i, j) != 0) {
                count++;
                break;
            }
        }
    }
    return count;
}

size_t matrix::row() const {
    return rows;
}

size_t matrix::col() const {
    return cols;
}

void matrix::swapRows(int row1, int row2) {
    for (int i = 0; i < cols; ++i) {
        double temp = (*this)(row1, i);
        (*this)(row1, i) = (*this)(row2, i);
        (*this)(row2, i) = temp;
    }
}
