#include "matrix.hpp"
#include <iostream>
#include <cmath>
#include <algorithm>

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
    if(rank() != rankOfExtendedMatrix()){
        std::cout << "Matrix does not have a solution";
        return;
    }

    if(cols-1 - rank() != 0){
        std::cout << "Infinite number of solutions" << "\n";
        std::cout << "With particular solution: ";
        countOneSolution();
        countKernel();
        return;
    }

    calculateValues();
}

void matrix::gaussElimination() {
    //projdu všechny sloupce kromě pravé strany
    for (int currentCol = 0; currentCol < cols - 1; ++currentCol) {

        //pro každý řádek zařídím nuly pod pivotem
        for (int rowIndex = currentCol + 1; rowIndex <  rows; ++rowIndex) {

            double koeficient = 0;
            for (int colIndex = currentCol; colIndex < cols - 1; ++colIndex) {
                if((*this)(currentCol,colIndex) == 0){
                    continue;
                }else{
                    koeficient = -((*this)(rowIndex,colIndex)/(*this)(currentCol,colIndex));
                    break;
                }
            }

            if(koeficient == 0){
                continue;
            }

            //projdu celý řádek a odeču od něho ten horní
            for (int colIndex = currentCol; colIndex < cols ; ++colIndex) {
                double& firstNum = (*this)(rowIndex,colIndex);
                double secondNum = (*this)(currentCol, colIndex);
                firstNum += secondNum*koeficient;
            }

        }

    }

    std::cout << "\nAfter Gauss:\n";
    printMatrix(std::cout);
    std::cout << "\n";

}

void matrix::calculateValues() {
    std::vector<double> results;

    //počítám od posledního řádku(rovnice)
    for (int rowIndex = rows - 1; rowIndex >= 0; --rowIndex) {

        double rhs = (*this)(rowIndex,cols-1);

        double leftSideSum = 0;
        for (int i = cols - 2; i > rowIndex; --i) {
            leftSideSum += (*this)(rowIndex, i)*results.at(cols-2-i);
        }

        results.push_back((rhs-leftSideSum)/(*this)(rowIndex,rowIndex));

    }

    std::reverse(results.begin(),results.end());

    std::cout << "Result is vector (";
    for(double d : results){
        std::cout << " " << d << " ";
    }
    std::cout << ")";

}

void matrix::swapRows(int firstRow, int secondRow) {

    for (int i = 0; i <= cols-1 ; i++)
    {
        double temp = (*this)(firstRow,i);
        (*this)(firstRow,i) = (*this)(secondRow,i);
        (*this)(secondRow,i) = temp;
    }
}

size_t matrix::row() const{
    return rows;
}

size_t matrix::col() const{
    return cols;
}

int matrix::rank() const{
    int count = 0;
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols - 1; ++j) {
            if((*this)(i,j) != 0){
                count++;
                break;
            }
        }
    }
    return count;
}

int matrix::rankOfExtendedMatrix() const{
    int count = 0;
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            if((*this)(i,j) != 0){
                count++;
                break;
            }
        }
    }
    return count;
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


        double rhs = (*this)(rowIndex,cols-1);

        double leftSideSum = 0;
        for (int i = cols - 2; i > pivotPosition; --i) {
            leftSideSum += (*this)(rowIndex, i)*results.at(cols-2-i);
        }

        results.push_back((rhs-leftSideSum)/(*this)(rowIndex,pivotPosition));

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


void matrix::countKernel() {
    std::cout << "+ span [";

    std::vector<int> nonPivotPosition = nonPositionOfPivots();

    for (int kernelVectorCount = 0; kernelVectorCount < rows - rank(); ++kernelVectorCount) {

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

            double leftSideSum = 0;
            for (int i = cols - 2; i > pivotPosition; --i) {
                leftSideSum += (*this)(rowIndex, i) * results.at(cols - 2 - i);
            }

            results.push_back((rhs - leftSideSum) / (*this)(rowIndex, pivotPosition));

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
    std::cout << "]";

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




