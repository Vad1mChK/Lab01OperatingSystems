#include "RamMemorySorter.hpp"

#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <random>
#include <cstdint>
#include <limits>

uint32_t randomUint32() {
    static std::random_device rd;
    static std::mt19937 engine(rd());
    static std::uniform_int_distribution<uint32_t> dist(0, std::numeric_limits<uint32_t>::max());
    return dist(engine);
}

// Generate a random binary file of uint32_t values
void RamMemorySorter::generateRandomFile(const std::string& filename, size_t size_mb) {
    std::ofstream file(filename, std::ios::binary);
    if (!file) {
        std::cout << "Failed to open file for writing: " << filename << std::endl;
        return;
    }

    size_t num_elements = size_mb * 1024 * 1024 / sizeof(uint32_t);

    for (size_t i = 0; i < num_elements; ++i) {
        uint32_t number = randomUint32();
        file.write(reinterpret_cast<const char*>(&number), sizeof(number));
    }

    std::cout << "Random file generated: " << filename << " (" << size_mb << " MB)" << std::endl;
}

// Sort the entire file in memory and write the sorted data to the output file
void RamMemorySorter::sortInMemory(const std::string& input_filename, const std::string& output_filename) {
    // Read the entire file into memory
    std::ifstream input(input_filename, std::ios::binary | std::ios::ate);
    if (!input) {
        std::cout << "Failed to open input file: " << input_filename << std::endl;
        return;
    }

    std::streamsize file_size = input.tellg();
    input.seekg(0, std::ios::beg);

    size_t num_elements = file_size / sizeof(uint32_t);
    std::vector<uint32_t> data(num_elements);

    if (!input.read(reinterpret_cast<char*>(data.data()), file_size)) {
        std::cout << "Failed to read input file: " << input_filename << std::endl;
        return;
    }
    input.close();

    // Sort the data in memory
    std::cout << "Sorting " << num_elements << " elements in memory..." << std::endl;
    std::sort(data.begin(), data.end());

    // Write the sorted data to the output file
    std::ofstream output(output_filename, std::ios::binary);
    if (!output) {
        std::cout << "Failed to open output file: " << output_filename << std::endl;
        return;
    }

    output.write(reinterpret_cast<const char*>(data.data()), file_size);
    output.close();

    std::cout << "In-memory sort completed. Output file: " << output_filename << std::endl;
}

// Check if the file is sorted
void RamMemorySorter::checkFileSorted(const std::string& filename) {
    std::ifstream input(filename, std::ios::binary);
    if (!input) {
        std::cout << "Failed to open file for checking: " << filename << std::endl;
        return;
    }

    uint32_t prev_value;
    bool first = true;
    bool is_sorted = true;

    while (true) {
        uint32_t current_value;
        if (!input.read(reinterpret_cast<char*>(&current_value), sizeof(uint32_t))) {
            break;  // EOF
        }
        if (!first) {
            if (current_value < prev_value) {
                std::cout << "File is not sorted. Error at value " << current_value << " after " << prev_value << std::endl;
                is_sorted = false;
                break;
            }
        } else {
            first = false;
        }
        prev_value = current_value;
    }

    if (is_sorted) {
        std::cout << "File is sorted." << std::endl;
    } else {
        std::cout << "File is not sorted." << std::endl;
    }

    input.close();
}

// Print help message
void RamMemorySorter::printHelp() {
    std::cout << "Available commands:\n"
              << "\tgenerate <output_file> <size_mb>\n\t\tGenerate a random binary file of uint32_t values\n"
              << "\tsort <input_file> <output_file>\n\t\tSort the file entirely in memory\n"
              << "\tcheck <input_file>\n\t\tCheck if the file is sorted\n"
              << "\thelp\n\t\tPrint this help message\n"
              << "\tfull-benchmark <input_file> <output_file> <repeat-count>\n\t\t"
              <<"Generate a file of size 256MB, sort it in memory, save the result, check it, and repeat several times.\n";
}
