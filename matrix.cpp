#include "matrix.hpp"
#include <iostream>
#include <cmath>
#include <algorithm>
#include <thread>

matrix::matrix(size_t row, size_t column, std::unique_ptr<double[]> matrix){
    rows = row;
    cols = column;
    data = std::move(matrix);
}

double& matrix::operator()(size_t i, size_t j) {
    return data[i * cols + j];
}

double matrix::operator()(size_t i, size_t j) const{
    return data[i * cols + j];
}

double matrix::getRhs(int row) const {
    return (*this)(row,cols-1);
}

void matrix::printMatrix(std::ostream& stream) const {
    for (int i = 0; i < rows; ++i) {
        stream << "[";
        for (int j = 0; j < cols; ++j) {
            stream << " " << (*this)(i,j) << " ";
        }
        stream << "]\n";
    }
}

void matrix::solveMatrix() {
    gaussElimination();

    int rankOfMatrix = rank(cols-1);
    int rankOfExtendedMatrix = rank(cols);

    if(rankOfMatrix != rankOfExtendedMatrix){
        std::cout << "Matrix does not have a solution\n";
        return;
    }

    if(cols-1 - rankOfMatrix != 0){
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

    int rankOfMatrix = rank(cols-1);
    int rankOfExtendedMatrix = rank(cols);

    if(rankOfMatrix != rankOfExtendedMatrix){
        std::cout << "Matrix does not have a solution\n";
        return;
    }

    if(cols-1 - rankOfMatrix != 0){
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

        //pro každý řádek zařídím nuly pod pivotem
        for (int rowIndex = currentCol + 1; rowIndex <  rows; ++rowIndex) {

            double koeficient = countKoeficient(currentCol,rowIndex);

            if(koeficient == 0){
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

void matrix::threadFunc(int col, int row){
    double koeficient = countKoeficient(col,row);

    if(koeficient == 0){
        return;
    }

    multiplyAndSubstractRows(col, row, koeficient);
}

void matrix::gaussEliminationConcurrent() {
    //projdu všechny sloupce kromě pravé strany
    for (int currentCol = 0; currentCol < cols - 1; ++currentCol) {

        std::vector<std::thread> threads;

        //pro každý řádek zařídím nuly pod pivotem
        for (int rowIndex = currentCol + 1; rowIndex <  rows; ++rowIndex) {

            threads.emplace_back(&matrix::threadFunc, this, currentCol,rowIndex);

        }

        for (auto& thread : threads) {
            thread.join();
        }
        threads.clear();

    }

    //std::cout << "\nAfter Concurrent Gauss:\n";
    //printMatrix(std::cout);
    //std::cout << "\n";
}

double matrix::countKoeficient(int column, int row) const {
    for (int colIndex = column; colIndex < cols - 1; ++colIndex) {
        if((*this)(column,colIndex) == 0){
            continue;
        }else{
            return -((*this)(row,colIndex)/(*this)(column,colIndex));
        }
    }

    return 0;

}

void matrix::multiplyAndSubstractRows(int startColumn, int row, double koeficient){
    for (int colIndex = startColumn; colIndex < cols ; ++colIndex) {
        double& firstNum = (*this)(row,colIndex);
        double secondNum = (*this)(startColumn, colIndex);
        firstNum += secondNum*koeficient;
    }
}

void matrix::calculateValues() {
    std::vector<double> results;

    //počítám od posledního řádku(rovnice)
    for (int rowIndex = rows - 1; rowIndex >= 0; --rowIndex) {

        double rhs = getRhs(rowIndex);

        solveLinearEquation(rowIndex,rowIndex,rhs,results);

    }

    std::reverse(results.begin(),results.end());

    std::cout << "Result is vector (";
    for(double d : results){
        std::cout << " " << d << " ";
    }
    std::cout << ")\n";

}

void matrix::solveLinearEquation(int row,int firstElementPosition, double rhs, std::vector<double> &results) {
    double leftSideSum = 0;
    for (int i = cols - 2; i > firstElementPosition; --i) {
        leftSideSum += (*this)(row, i)*results.at(cols-2-i);
    }

    results.push_back((rhs-leftSideSum)/(*this)(row,firstElementPosition));
}

void matrix::countOneSolution() {
    std::vector<int> nonPivotPosition = nonPositionOfPivots();
    std::vector<double> results;

    for (int rowIndex = rows - 1; rowIndex >= 0; --rowIndex) {

        int pivotPosition = findPivotPosition(rowIndex);

        if(pivotPosition == -1){
            if(std::find(nonPivotPosition.begin(), nonPivotPosition.end(), rowIndex) != nonPivotPosition.end()) {
                results.push_back(0);
            }
            continue;
        }

        double rhs = getRhs(rowIndex);

        solveLinearEquation(rowIndex,pivotPosition, rhs,results);

        if(std::find(nonPivotPosition.begin(), nonPivotPosition.end(), rowIndex) != nonPivotPosition.end()) {
            results.push_back(0);
        }

    }

    std::reverse(results.begin(),results.end());

    std::cout << "(";
    for(double d : results){
        std::cout << " " << d << " ";
    }
    std::cout << ")";

}


void matrix::countKernel(int numOfVectors) {
    std::cout << " + span [";

    std::vector<int> nonPivotPosition = nonPositionOfPivots();

    for (int kernelVectorCount = 0; kernelVectorCount < numOfVectors; ++kernelVectorCount) {

        std::vector<double> results;
        int emptySpacesCounter = 0;

        for (int rowIndex = rows - 1; rowIndex >= 0; --rowIndex) {

            int pivotPosition = findPivotPosition(rowIndex);
            if (pivotPosition == -1) {
                if (std::find(nonPivotPosition.begin(), nonPivotPosition.end(), rowIndex) != nonPivotPosition.end()) {
                    if(kernelVectorCount == emptySpacesCounter){
                        results.push_back(1);
                    }else{
                        results.push_back(0);
                    }
                    emptySpacesCounter++;
                }
                continue;
            }

            double rhs = 0;

            solveLinearEquation(rowIndex,pivotPosition, rhs,results);

            if (std::find(nonPivotPosition.begin(), nonPivotPosition.end(), rowIndex) != nonPivotPosition.end()) {
                if(kernelVectorCount == emptySpacesCounter){
                    results.push_back(1);
                }else{
                    results.push_back(0);
                }
                emptySpacesCounter++;
            }

        }

        std::reverse(results.begin(), results.end());

        std::cout << "(";
        for (double d : results) {
            std::cout << " " << d << " ";
        }
        std::cout << ")";
    }
    std::cout << "]\n";

}

std::vector<int> matrix::nonPositionOfPivots() {
    std::vector<int> pivotColumns;
    std::vector<int> nonPositions;
    for (int i = 0; i < rows; ++i) {
        int column = findPivotPosition(i);
        pivotColumns.push_back(column);
        if(column != -1 && i!=column){
            nonPositions.push_back(i);
        }
        if(column == -1 && std::find(pivotColumns.begin(), pivotColumns.end(), i) == pivotColumns.end()){
            nonPositions.push_back(i);
        }
    }
    return nonPositions;
}

int matrix::findPivotPosition(int rowIndex) const {
    for (int i = 0; i < cols - 1; ++i) {
        if((*this)(rowIndex,i) != 0){
            return i;
        }
    }
    return -1;
}

int matrix::rank(int numOfCols) const{
    int count = 0;
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < numOfCols; ++j) {
            if((*this)(i,j) != 0){
                count++;
                break;
            }
        }
    }
    return count;
}

size_t matrix::row() const{
    return rows;
}

size_t matrix::col() const{
    return cols;
}









