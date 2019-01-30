#include <memory>
#include <vector>

class matrix {

public:

    matrix(size_t row, size_t column, std::unique_ptr<double[]> matrix);

    double& operator()(size_t i, size_t j);
    double operator()(size_t i, size_t j) const;

    void printMatrix(std::ostream& stream) const;

    void solveMatrix();

    void solveMatrixConcurrent();

    size_t col() const;

    size_t row() const;

    double getRhs(int row) const;

private:

    double countKoeficient(int column, int row) const;

    void multiplyAndSubstractRows(int startColumn, int row, double koeficient);

    int findPivotPosition(int rowIndex) const;

    void countOneSolution();

    void countKernel(int numOfVectors);

    int rank(int numOfCols) const;

    void gaussElimination();

    void gaussEliminationConcurrent();

    void calculateValues();

    std::vector<int> nonPositionOfPivots();

    void solveLinearEquation(int row, int firstElementPosition, double rhs, std::vector<double> &results);

    void threadFunc(int col, int row);

private:

    std::unique_ptr<double[]> data;
    size_t rows;
    size_t cols;

};

