#include <stdio.h>
#include <iostream>
#include <vector>
#include <string>
#include <unordered_map>
#include <chrono>

#include "planner.hpp"
#include "dotwriter.hpp"
#include "input_reader.hpp"
#include "argparse.hpp"

int main(int argc, char *argv[]){
    auto t1 = std::chrono::high_resolution_clock::now();

    argparse::ArgumentParser program("MSRM Assembly Planner");
    program.add_argument("Filename")
           .help("Path to the XML assembly description.");

    try {
        program.parse_args(argc, argv);
    }
    catch (const std::runtime_error& err) {
        std::cout << err.what() << std::endl;
        std::cout << program;
        exit(0);
    }
  
    auto input = program.get<std::string>("Filename");

    try{
        InputReader rdr(input);
        Graph<> * assembly;
        Config * config;
        bool result;

        std::tie(assembly, config, result) = rdr.read("assembly");

        // std::cout << "Reach Map: " << std::endl;
        // std::cout << "- - - - - - - - -" << std::endl;
        // for(auto & outer_map_pair : config->reach_->map_) {
        //     for(auto & inner_map_pair : outer_map_pair.second) {
        //         std::cout << inner_map_pair.first << " : " << inner_map_pair.second << std::endl;
        //     }
        //     std::cout << "- - - - - - - - -" << std::endl;
        // }

        if(!result){
            std::cout << "Error in Input Reader." << std::endl;
            std::cout << "/ Could not read Input File /." << std::endl;
            return false;
        }

        Planner planner;
        planner(assembly, assembly->root_, config);
    }
    catch (const std::runtime_error& err) {
        std::cout << "Could not open XML. File not found." << std::endl;
        return 0;
    }

    // auto t2 = std::chrono::high_resolution_clock::now();
    // auto duration = std::chrono::duration_cast<std::chrono::milliseconds>( t2 - t1 ).count();
    // std::cout << "Duration: "<< duration << "ms.";

    return 1;
}