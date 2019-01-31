#include <iostream>
#include <string>
#include "cmdline.hpp"

int main(int argc, char *argv[]) {

    cmdline cmd;

    std::string help = "--help";
    std::string sequence = "--sequence";
    std::string parallel = "--parallel";

    if(argc == 2){
        if(argv[1] == help){
            cmd.help();
            return 0;
        }
        cmd.sequentional(argv[1]);
        return 0;
    }
    if(argc == 3){
        if(argv[1] == sequence){
            cmd.sequentional(argv[2]);
            return 0;
        }
        if(argv[1] == parallel){
            cmd.parallel(argv[2]);
            return 0;
        }
        cmd.invalid();
        return 0;
    }

    cmd.invalid();
    return  0;

}