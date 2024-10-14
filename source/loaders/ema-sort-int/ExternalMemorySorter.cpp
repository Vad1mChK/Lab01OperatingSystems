#include "ExternalMemorySorter.hpp"

#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <random>
#include <cstdint>
#include <limits>
#include <sstream>
#include <queue>
#include <cstdio>  // For remove()

uint32_t ExternalMemorySorter::randomUint32() {
    static std::random_device rd;
    static std::mt19937 engine(rd());
    static std::uniform_int_distribution<uint32_t> dist(0, std::numeric_limits<uint32_t>::max());
    return dist(engine);
}

// Generate a random binary file of uint32_t values
void ExternalMemorySorter::generateRandomFile(const std::string& filename, size_t size_in_mb) {
    std::ofstream file(filename, std::ios::binary);
    if (!file) {
        std::cerr << "Failed to open file for writing: " << filename << std::endl;
        return;
    }

    size_t num_elements = size_in_mb * 1024 * 1024 / sizeof(uint32_t);

    for (size_t i = 0; i < num_elements; ++i) {
        uint32_t number = randomUint32();
        file.write(reinterpret_cast<const char*>(&number), sizeof(number));
    }

    std::cout << "Random file generated: " << filename << " (" << size_in_mb << " MB)" << std::endl;
}

// Sort chunks of the input file and save them as temporary files
void ExternalMemorySorter::sortByChunksAndSave(
    const std::string& input_filename,
    const std::string& temp_directory,
    size_t chunk_size_mb
) {
    std::ifstream input(input_filename, std::ios::binary);
    if (!input) {
        std::cerr << "Failed to open input file: " << input_filename << std::endl;
        return;
    }

    size_t chunk_size_in_elements = chunk_size_mb * 1024 * 1024 / sizeof(uint32_t);
    std::vector<uint32_t> buffer(chunk_size_in_elements);

    input.seekg(0, std::ios::end);
    size_t file_size_in_bytes = input.tellg();
    input.seekg(0, std::ios::beg);

    size_t num_elements = file_size_in_bytes / sizeof(uint32_t);
    size_t num_chunks = (num_elements + chunk_size_in_elements - 1) / chunk_size_in_elements;

    std::cout << "Sorting " << num_chunks << " chunks..." << std::endl;

    for (size_t i = 0; i < num_chunks; ++i) {
        size_t elements_to_read = std::min(chunk_size_in_elements, num_elements - i * chunk_size_in_elements);
        input.read(reinterpret_cast<char*>(buffer.data()), elements_to_read * sizeof(uint32_t));
        size_t elements_read = input.gcount() / sizeof(uint32_t);

        buffer.resize(elements_read);  // Resize buffer to actual number of elements read
        std::sort(buffer.begin(), buffer.end());

        std::ostringstream temp_filename_stream;
        temp_filename_stream << temp_directory << "/chunk_" << i << ".dat";
        std::string temp_filename = temp_filename_stream.str();

        std::ofstream temp_file(temp_filename, std::ios::binary);
        if (!temp_file) {
            std::cerr << "Failed to open temp file: " << temp_filename << std::endl;
            return;
        }

        temp_file.write(reinterpret_cast<const char*>(buffer.data()), elements_read * sizeof(uint32_t));
        temp_file.close();

        std::cout << "Chunk " << i + 1 << " sorted and saved to " << temp_filename << std::endl;
    }

    input.close();
}

// Merge sorted chunks from temporary files into the output file
void ExternalMemorySorter::mergeChunksAndSave(
    const std::string& temp_directory,
    const std::string& output_filename,
    size_t num_chunks
) {
    struct HeapNode {
        uint32_t value;
        size_t chunk_index;

        bool operator>(const HeapNode& other) const {
            return value > other.value;
        }
    };

    std::vector<std::ifstream> temp_files(num_chunks);
    std::vector<uint32_t> current_values(num_chunks);
    std::vector<bool> has_more(num_chunks, true);

    for (size_t i = 0; i < num_chunks; ++i) {
        std::ostringstream temp_filename_stream;
        temp_filename_stream << temp_directory << "/chunk_" << i << ".dat";
        std::string temp_filename = temp_filename_stream.str();

        temp_files[i].open(temp_filename, std::ios::binary);
        if (!temp_files[i]) {
            std::cerr << "Failed to open temp file for merging: " << temp_filename << std::endl;
            return;
        }

        if (!temp_files[i].read(reinterpret_cast<char*>(&current_values[i]), sizeof(uint32_t))) {
            has_more[i] = false;
        }
    }

    std::ofstream output(output_filename, std::ios::binary);
    if (!output) {
        std::cerr << "Failed to open output file for writing: " << output_filename << std::endl;
        return;
    }

    auto cmp = [](const HeapNode& left, const HeapNode& right) {
        return left.value > right.value;
    };

    std::priority_queue<HeapNode, std::vector<HeapNode>, decltype(cmp)> min_heap(cmp);

    for (size_t i = 0; i < num_chunks; ++i) {
        if (has_more[i]) {
            min_heap.push({current_values[i], i});
        }
    }

    while (!min_heap.empty()) {
        HeapNode node = min_heap.top();
        min_heap.pop();

        output.write(reinterpret_cast<const char*>(&node.value), sizeof(uint32_t));

        size_t idx = node.chunk_index;
        if (temp_files[idx].read(reinterpret_cast<char*>(&current_values[idx]), sizeof(uint32_t))) {
            min_heap.push({current_values[idx], idx});
        } else {
            has_more[idx] = false;
            temp_files[idx].close();
            // Delete temp file
            std::ostringstream temp_filename_stream;
            temp_filename_stream << temp_directory << "/chunk_" << idx << ".dat";
            std::string temp_filename = temp_filename_stream.str();
            std::remove(temp_filename.c_str());
        }
    }

    output.close();
}

// External memory sort implementation
void ExternalMemorySorter::externalMemorySort(
    const std::string& input_filename,
    const std::string& output_filename,
    size_t chunk_size_mb
) {
    std::string temp_directory = ".";  // Use current directory for temp files

    // Step 1: Sort chunks and save them to temporary files
    sortByChunksAndSave(input_filename, temp_directory, chunk_size_mb);

    // Step 2: Calculate the number of chunks
    std::ifstream input(input_filename, std::ios::binary | std::ios::ate);
    if (!input) {
        std::cerr << "Failed to open input file for size calculation: " << input_filename << std::endl;
        return;
    }

    size_t file_size_in_bytes = input.tellg();
    input.close();

    size_t chunk_size_in_bytes = chunk_size_mb * 1024 * 1024;
    size_t num_chunks = (file_size_in_bytes + chunk_size_in_bytes - 1) / chunk_size_in_bytes;

    // Step 3: Merge the sorted chunks into the final output file
    mergeChunksAndSave(temp_directory, output_filename, num_chunks);

    std::cout << "External memory sort completed. Output file: " << output_filename << std::endl;
}

// Check if the file is sorted
void ExternalMemorySorter::checkFileSorted(const std::string& input_filename) {
    std::ifstream input(input_filename, std::ios::binary);
    if (!input) {
        std::cerr << "Failed to open file for checking: " << input_filename << std::endl;
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
                std::cout << "File is not sorted. Error at value "
                  << std::hex << current_value << " after " << prev_value << std::endl;
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
void ExternalMemorySorter::printHelp() {
    std::cout << "Available subcommands:\n"
              << "\tgenerate <output_file> <size_mb>\n\t\tGenerate a random binary file of uint32_t values\n"
              << "\tsort <input_file> <output_file> <chunk_size_mb>\n\t\tSort the file in chunks and save sorted result\n"
              << "\tcheck <input_file>\n\t\tCheck if the file is sorted\n"
              << "\thelp\n\t\tPrint this help message (no args).\n"
              << "\tfull-benchmark <input_file> <output_file> <repeat-count>\n\t\t"
                 "Generate a 256MB file, sort it with 32MB chunk size, check the results, repeat everything several times.\n";
}
