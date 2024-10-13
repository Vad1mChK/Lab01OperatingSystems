#include "ExternalMemorySorter.hpp"

#include <algorithm>
#include <ctime>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <limits>
#include <random>
#include <string>
#include <vector>

uint32_t ExternalMemorySorter::randomUint32() {
  // Create a random device (this seeds the random number generator)
  static std::random_device rd;

  // Use the Mersenne Twister engine for generating random numbers
  static std::mt19937 engine(rd());

  // Create a uniform distribution for uint32_t values
  static std::uniform_int_distribution<uint32_t> dist(0, std::numeric_limits<uint32_t>::max());

  // Return a random uint32_t value
  return dist(engine);
}

// Helper function: Generate a large random binary file
void ExternalMemorySorter::generateRandomFile(const std::string& filename, size_t size_in_mb) {
    std::ofstream file(filename, std::ios::binary);
    if (!file) {
        std::cerr << "Failed to open file for writing" << std::endl;
        return;
    }

    srand(static_cast<unsigned>(time(0)));
    size_t num_elements = size_in_mb * 1024 * 1024 / sizeof(uint32_t);

    for (size_t i = 0; i < num_elements; ++i) {
        uint32_t number = randomUint32();
        file.write(reinterpret_cast<const char*>(&number), sizeof(number));
    }

    std::cout << "Random file generated: " << filename << " (" << size_in_mb << " MB)" << std::endl;
}

// Helper function: Load, sort, and save a chunk of data
void sortChunk(std::ifstream& input, std::ofstream& output, size_t chunk_size_in_elements, size_t offset) {
    input.seekg(offset * chunk_size_in_elements * sizeof(uint32_t));

    std::vector<uint32_t> buffer(chunk_size_in_elements);
    input.read(reinterpret_cast<char*>(buffer.data()), chunk_size_in_elements * sizeof(uint32_t));

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

    size_t chunk_size_in_elements = chunk_size_in_mb * 1024 * 1024 / sizeof(uint32_t);

    // Get the total file size
    input.seekg(0, std::ios::end);
    size_t file_size_in_bytes = input.tellg();
    input.seekg(0, std::ios::beg);

    size_t num_elements = file_size_in_bytes / sizeof(uint32_t);
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

    size_t chunk_size_in_elements = chunk_size_in_mb * 1024 * 1024 / sizeof(uint32_t);

    // Get total file size
    input.seekg(0, std::ios::end);
    size_t file_size_in_bytes = input.tellg();
    input.seekg(0, std::ios::beg);

    size_t num_elements = file_size_in_bytes / sizeof(uint32_t);
    size_t num_chunks = (num_elements + chunk_size_in_elements - 1) / chunk_size_in_elements;

    uint32_t prev_max = 0;
    bool is_sorted = true;
    bool first_chunk = true;

    for (size_t i = 0; i < num_chunks; ++i) {
        std::vector<uint32_t> buffer(chunk_size_in_elements);
        input.read(reinterpret_cast<char*>(buffer.data()), chunk_size_in_elements * sizeof(uint32_t));

        if (i == 0) {
          std::cout << "Top 5 elements in the first chunk: ";
          for (size_t j = 0; j < std::min<size_t>(5, buffer.size()); ++j) {
            std::cout << buffer[j] << " ";
          }
          std::cout << std::endl;
        }

        // Ensure the chunk itself is sorted
        for (size_t j = 1; j < buffer.size(); ++j) {
            if (buffer[j] < buffer[j - 1]) {
                std::cout << "Error: Chunk " << i << " is not internally sorted at index " << j << std::endl;
                is_sorted = false;
                break;
            }
        }

        if (!is_sorted) {
            break;
        }

        auto min_val = *std::min_element(buffer.begin(), buffer.end());
        auto max_val = *std::max_element(buffer.begin(), buffer.end());

        std::cout << "Chunk " << i << ": min " << min_val << ", max " << max_val << std::endl;

        // Ensure sorting between chunks
        if (!first_chunk && min_val < prev_max) {
            std::cout << "Error: Chunk " << i << " min value (" << min_val << ") is less than previous chunk max value (" << prev_max << ")" << std::endl;
            is_sorted = false;
            break;
        }
        prev_max = max_val;
        first_chunk = false;
    }

    if (is_sorted) {
        std::cout << "File is fully sorted." << std::endl;
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
