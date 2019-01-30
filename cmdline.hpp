#include <string>

class cmdline {

public:

    void help() const;
    void sequentional(std::string filename) const;
    void parallel(std::string filename) const;
    void invalid() const;

};
