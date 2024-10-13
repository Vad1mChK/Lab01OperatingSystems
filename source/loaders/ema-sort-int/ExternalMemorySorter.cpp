#include "ExternalMemorySorter.hpp"

#include <algorithm>
#include <cstdint>
#include <ctime>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <limits>
#include <sstream>
#include <string>
#include <vector>

#include "ExternalMemorySorter.hpp"

using uint32 = uint32_t;

// Helper function: Generate a large random binary file
void ExternalMemorySorter::generateRandomFile(const std::string& filename, size_t size_in_mb) {
    std::ofstream file(filename, std::ios::binary);
    if (!file) {
        std::cerr << "Failed to open file for writing" << std::endl;
        return;
    }

    srand(static_cast<unsigned>(time(0)));
    size_t num_elements = size_in_mb * 1024 * 1024 / sizeof(uint32);

    for (size_t i = 0; i < num_elements; ++i) {
        uint32 number = rand() % std::numeric_limits<uint32>::max();
        file.write(reinterpret_cast<const char*>(&number), sizeof(number));
    }

    std::cout << "Random file generated: " << filename << " (" << size_in_mb << " MB)" << std::endl;
}

// Helper function: Load, sort, and save a chunk of data
void sortChunk(std::ifstream& input, std::ofstream& output, size_t chunk_size_in_elements, size_t offset) {
    input.seekg(offset * chunk_size_in_elements * sizeof(uint32));

    std::vector<uint32> buffer(chunk_size_in_elements);
    input.read(reinterpret_cast<char*>(buffer.data()), chunk_size_in_elements * sizeof(uint32));

    std::sort(buffer.begin(), buffer.end());

    output.write(reinterpret_cast<const char*>(buffer.data()), input.gcount());
}

// Perform external memory sort (disk-based chunk sorting)
void ExternalMemorySorter::externalMemorySort(const std::string& input_filename, const std::string& output_filename, size_t chunk_size_in_mb) {
    std::ifstream input(input_filename, std::ios::binary);
    if (!input) {
        std::cerr << "Failed to open input file: " << input_filename << std::endl;
        return;
    }

    std::ofstream output(output_filename, std::ios::binary);
    if (!output) {
        std::cerr << "Failed to open output file: " << output_filename << std::endl;
        return;
    }

    size_t chunk_size_in_elements = chunk_size_in_mb * 1024 * 1024 / sizeof(uint32);

    // Get the total file size
    input.seekg(0, std::ios::end);
    size_t file_size_in_bytes = input.tellg();
    input.seekg(0, std::ios::beg);

    size_t num_elements = file_size_in_bytes / sizeof(uint32);
    size_t num_chunks = (num_elements + chunk_size_in_elements - 1) / chunk_size_in_elements;  // Round up to cover remainder

    std::cout << "Sorting " << num_chunks << " chunks..." << std::endl;

    for (size_t i = 0; i < num_chunks; ++i) {
        std::cout << "Sorting chunk " << i + 1 << " of " << num_chunks << std::endl;
        sortChunk(input, output, chunk_size_in_elements, i);
    }

    std::cout << "File sorted and written to " << output_filename << std::endl;
}

// Check if the file is sorted and output chunk min/max information
void ExternalMemorySorter::checkFileSorted(const std::string& input_filename, size_t chunk_size_in_mb) {
    std::ifstream input(input_filename, std::ios::binary);
    if (!input) {
        std::cerr << "Failed to open input file: " << input_filename << std::endl;
        return;
    }

    size_t chunk_size_in_elements = chunk_size_in_mb * 1024 * 1024 / sizeof(uint32);

    // Get total file size
    input.seekg(0, std::ios::end);
    size_t file_size_in_bytes = input.tellg();
    input.seekg(0, std::ios::beg);

    size_t num_elements = file_size_in_bytes / sizeof(uint32);
    size_t num_chunks = (num_elements + chunk_size_in_elements - 1) / chunk_size_in_elements;

    uint32 prev_max = 0;
    bool is_sorted = true;

    for (size_t i = 0; i < num_chunks; ++i) {
        std::vector<uint32> buffer(chunk_size_in_elements);
        input.read(reinterpret_cast<char*>(buffer.data()), chunk_size_in_elements * sizeof(uint32));

        auto min_val = *std::min_element(buffer.begin(), buffer.end());
        auto max_val = *std::max_element(buffer.begin(), buffer.end());

        std::cout << "Chunk " << i << ": min " << min_val << ", max " << max_val << std::endl;

        if (min_val < prev_max) {
            is_sorted = false;
        }
        prev_max = max_val;
    }

    if (is_sorted) {
        std::cout << "File is sorted." << std::endl;
    } else {
        std::cout << "File is not sorted." << std::endl;
    }
}

// Print help message
void ExternalMemorySorter::printHelp() {
    std::cout << "Available subcommands:\n"
      << "\tgenerate <output_file> <size_mb>\n\t\tGenerate a random binary file of uint32_t values\n"
      << "\tsort <input_file> <output_file> <chunk_size_mb>\n\t\tSort the file in chunks and save sorted result\n"
      << "\tcheck <input_file> <chunk_size_mb>\n\t\tCheck if the file is sorted and output chunk min/max\n"
      << "\thelp\n\t\tPrint this help message (no args).\n";
}
