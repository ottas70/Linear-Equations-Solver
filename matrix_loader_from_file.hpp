#include <tuple>
#include <memory>
#include <string>

class matrix_loader_from_file {

public:

    std::tuple<int, int, std::unique_ptr<double[]>> load(std::string filename);

};


