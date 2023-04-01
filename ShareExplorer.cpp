#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <thread>
#include <mutex>
#include <cstdio>

std::mutex mtx;

void list_files(const std::string& path, const std::string& output_file) {
    std::string command = "dir /b /a /s \"" + path + "\" >> \"" + output_file + "\"";
    std::unique_lock<std::mutex> lock(mtx);
    std::system(command.c_str());
    lock.unlock();
}

void display_help(const char* program_name) {
    std::cout << "Usage: " << program_name << " -i INPUT_FILE -o OUTPUT_FILE [-h|--help]" << std::endl;
    std::cout << std::endl;
    std::cout << "Options:" << std::endl;
    std::cout << "  -i INPUT_FILE   Path to the input file containing paths to list files" << std::endl;
    std::cout << "  -o OUTPUT_FILE  Path to the output file to store the results" << std::endl;
    std::cout << "  -h, --help      Show this help message and exit" << std::endl;
}

int main(int argc, char* argv[]) {
    std::string input_file;
    std::string output_file;

    for (int i = 1; i < argc; i++) {
        std::string arg = argv[i];
        if (arg == "-i" && i + 1 < argc) {
            input_file = argv[++i];
        }
        else if (arg == "-o" && i + 1 < argc) {
            output_file = argv[++i];
        }
        else if (arg == "-h" || arg == "--help") {
            display_help(argv[0]);
            return 0;
        }
        else {
            std::cerr << "Unknown argument: " << arg << std::endl;
            return 1;
        }
    }

    if (input_file.empty() || output_file.empty()) {
        display_help(argv[0]);
        return 1;
    }

    std::vector<std::string> paths;
    std::string line;
    std::ifstream input(input_file);
    while (std::getline(input, line)) {
        paths.push_back(line);
    }
    input.close();

    std::vector<std::thread> threads;

    for (const auto& path : paths) {
        threads.emplace_back(std::thread(list_files, path, output_file));
    }

    for (auto& thread : threads) {
        if (thread.joinable()) {
            thread.join();
        }
    }

    return 0;
}
