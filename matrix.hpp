#include <memory>
#include <vector>

class matrix {

public:

    matrix(size_t row, size_t column, std::unique_ptr<double[]> matrix);

    double& operator()(size_t i, size_t j);
    double operator()(size_t i, size_t j) const;

    void printMatrix(std::ostream& stream) const;

    void solveMatrix();

    size_t col() const;

    size_t row() const;

private:

    int findPivotPosition(int rowIndex) const;

    void countOneSolution();

    void countKernel();

    int rank() const;

    int rankOfExtendedMatrix() const;

    void gaussElimination();

    void swapRows(int firstRow, int secondRow);

    void calculateValues();

    std::vector<int> nonPositionOfPivots();

private:

    std::unique_ptr<double[]> data;
    size_t rows;
    size_t cols;

};

