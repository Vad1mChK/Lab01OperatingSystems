// DirectIoExternalMemorySorter.cpp

#include "DirectIoExternalMemorySorter.hpp"

#include <filesystem>
#include <algorithm>
#include <chrono>
#include <cstdint>
#include <iostream>
#include <queue>
#include <vector>
#include <cstring>
#include <memory>    // For smart pointers
#include <cstdlib>   // For posix_memalign and free
#include "../util/sorter_utils.hpp" // Ensure this path is correct

// Helper Struct for Merge Heap Nodes
struct HeapNode {
    uint32_t value;
    size_t chunk_index;

    HeapNode(uint32_t val, size_t idx) : value(val), chunk_index(idx) {}

    bool operator>(const HeapNode& other) const {
        return value > other.value;
    }
};

// Generate a random binary file of uint32_t values
void DirectIoExternalMemorySorter::generateRandomFile(const std::string& filename, size_t size_mb) {
    // Calculate total number of elements
    size_t num_elements = (size_mb * BytesInMb) / sizeof(uint32_t);

    // Define buffer size (e.g., 4MB)
    size_t buffer_size = 4 * BytesInMb;
    size_t elements_per_buffer = buffer_size / sizeof(uint32_t);

    // Allocate aligned buffer
    uint32_t* buffer;
    if (posix_memalign(reinterpret_cast<void**>(&buffer), LAB2_BLOCK_SIZE, buffer_size) != 0) {
        std::cerr << "Failed to allocate aligned memory.\n";
        return;
    }

    // Open file using Lab2 with write flags (assuming Lab2 handles flags internally)
    fd_t fd = lab2_.open(filename);
    if (fd < 0) {
        std::cerr << "Failed to open file for writing: " << filename << '\n';
        free(buffer);
        return;
    }

    auto t_start = std::chrono::steady_clock::now();

    size_t elements_written = 0;
    while (elements_written < num_elements) {
        size_t current_chunk = std::min(elements_per_buffer, num_elements - elements_written);

        // Fill buffer with random data
        for (size_t i = 0; i < current_chunk; ++i) {
            buffer[i] = RandomUint32();
        }

        // Write buffer to file
        size_t bytes_to_write = (elements_per_buffer) * sizeof(uint32_t); // Correct size to write
        ssize_t bytes_written = lab2_.write(fd, buffer, bytes_to_write);
        if (bytes_written != bytes_to_write) {
          std::cerr << "Failed to write correct number of bytes. Expected " << bytes_to_write << ", got " << bytes_written << "\n";
        }

        if (bytes_written != static_cast<ssize_t>(current_chunk * sizeof(uint32_t))) {
            std::cerr << "Failed to write to file: " << filename << '\n';
            lab2_.close(fd);
            free(buffer);
            return;
        }

        elements_written += current_chunk;
        echo("Written " +
          std::to_string(elements_written) +
          " elements (" +
          std::to_string(100 * elements_written / num_elements) +
          "% completion)"
          );
    }

    // Close file
    lab2_.close(fd);

    // Free buffer
    free(buffer);

    auto t_end = std::chrono::steady_clock::now();
    auto time_elapsed = std::chrono::duration_cast<std::chrono::nanoseconds>(t_end - t_start).count();

    std::cout << "ema-sort-int: Time taken to generate random file of size " << size_mb << " MB is "
              << time_elapsed << " ns\n";
    std::cout << "Random file generated: " << filename << " (" << size_mb << " MB)\n";
}

// Sort chunks of the input file and save them as temporary files
void DirectIoExternalMemorySorter::sortByChunksAndSave(
    const std::string& input_filename,
    const std::string& temp_directory,
    size_t chunk_size_mb
) {
    // Calculate chunk size in bytes and elements
    size_t chunk_size_bytes = chunk_size_mb * BytesInMb;
    size_t chunk_size_in_elements = chunk_size_bytes / sizeof(uint32_t);

    // Allocate aligned buffer
    uint32_t* buffer;
    if (posix_memalign(reinterpret_cast<void**>(&buffer), 4096, chunk_size_bytes)) {
        std::cerr << "Failed to allocate aligned memory for chunk sorting.\n";
        return;
    }

    // Open input file using Lab2 with read flags
    fd_t input_fd = lab2_.open(input_filename);
    if (input_fd < 0) {
        std::cerr << "Failed to open input file: " << input_filename << '\n';
        free(buffer);
        return;
    }

    // Get input file size
    off_t file_size = lab2_.lseek(input_fd, 0, SEEK_END);
    if (file_size < 0) {
        std::cerr << "Failed to determine size of input file: " << input_filename << '\n';
        lab2_.close(input_fd);
        free(buffer);
        return;
    }
    lab2_.lseek(input_fd, 0, SEEK_SET); // Reset to beginning

    size_t num_elements = file_size / sizeof(uint32_t);
    size_t num_chunks = (num_elements + chunk_size_in_elements - 1) / chunk_size_in_elements;

    std::cout << "Sorting " << num_chunks << " chunks...\n";

    auto t_start = std::chrono::steady_clock::now();

    for (size_t i = 0; i < num_chunks; ++i) {
        size_t elements_to_read = std::min(chunk_size_in_elements, num_elements - i * chunk_size_in_elements);
        size_t bytes_to_read = elements_to_read * sizeof(uint32_t);

        // Read chunk into buffer
        ssize_t bytes_read = lab2_.read(input_fd, buffer, bytes_to_read);

        if (bytes_read != static_cast<ssize_t>(bytes_to_read)) {
            std::cerr << "Failed to read chunk " << i << " from input file.\n";
            lab2_.close(input_fd);
            free(buffer);
            return;
        }
      std::cout << "Read " << bytes_read << "B, as expected\n";

        // Sort the chunk
        std::sort(buffer, buffer + elements_to_read);

        // Define temporary chunk file name
        std::ostringstream temp_filename_stream;
        temp_filename_stream << temp_directory << "/"
                             << SanitizeInputFilename(input_filename) << "_chunk_" << i << ".dat";
        std::string temp_filename = temp_filename_stream.str();

        // Open temporary chunk file with write flags
        fd_t chunk_fd = lab2_.open(temp_filename);
        if (chunk_fd < 0) {
            std::cerr << "Failed to open temp file for writing: " << temp_filename << '\n';
            lab2_.close(input_fd);
            free(buffer);
            return;
        }

        // Write sorted chunk to temporary file
        ssize_t bytes_written = lab2_.write(chunk_fd, buffer, bytes_to_read);
        if (bytes_written != static_cast<ssize_t>(bytes_to_read)) {
            std::cerr << "Failed to write to temp file: " << temp_filename << '\n';
            lab2_.close(chunk_fd);
            lab2_.close(input_fd);
            free(buffer);
            return;
        }

        // Close temporary chunk file
        lab2_.close(chunk_fd);

        std::cout << "Chunk " << i << " sorted and saved to " << temp_filename << '\n';
    }

    // Close input file
    lab2_.close(input_fd);

    // Free buffer
    free(buffer);

    auto t_end = std::chrono::steady_clock::now();
    auto time_elapsed = std::chrono::duration_cast<std::chrono::nanoseconds>(t_end - t_start).count();

    std::cout << "ema-sort-int: Time to sort chunks from " << input_filename << " is "
              << time_elapsed << " ns\n";
}

#include <iomanip> // For std::hex and std::dec

void DirectIoExternalMemorySorter::mergeChunksAndSave(
    const std::string& temp_directory,
    const std::string& input_filename,
    const std::string& output_filename,
    size_t num_chunks
) {
  // TODO prevent the thing from writing zeroes beyond the expected file size
    using std::chrono::steady_clock;
    auto t_start = steady_clock::now();

    std::vector<fd_t> chunk_fds(num_chunks, -1);
    std::priority_queue<HeapNode, std::vector<HeapNode>, std::greater<>> min_heap;

    // Open temporary files and populate initial heap
    for (size_t i = 0; i < num_chunks; ++i) {
        std::string temp_filename = temp_directory + "/" + SanitizeInputFilename(input_filename) + "_chunk_" + std::to_string(i) + ".dat";
        chunk_fds[i] = lab2_.open(temp_filename);
        if (chunk_fds[i] < 0) {
            std::cerr << "Failed to open chunk file: " << temp_filename << '\n';
            continue;
        }

        uint32_t value;
        ssize_t read_bytes = lab2_.read(chunk_fds[i], &value, sizeof(uint32_t));
        if (read_bytes == sizeof(uint32_t)) {
            min_heap.emplace(value, i);
            std::cout << "Chunk " << i << " initial value: " << value << " (0x"
                      << std::hex << value << std::dec << ")\n";
        } else if (read_bytes == 0) {
            std::cerr << "Chunk " << i << " is empty.\n";
            lab2_.close(chunk_fds[i]);
            chunk_fds[i] = -1; // Mark as closed
        } else {
            std::cerr << "Error reading from chunk " << i << ". Bytes read: " << read_bytes << '\n';
            lab2_.close(chunk_fds[i]);
            chunk_fds[i] = -1; // Mark as closed
        }
    }

    fd_t output_fd = lab2_.open(output_filename);
    if (output_fd < 0) {
        std::cerr << "Failed to open output file: " << output_filename << '\n';
        // Close all opened chunk files before returning
        for (size_t i = 0; i < num_chunks; ++i) {
            if (chunk_fds[i] >= 0) {
                lab2_.close(chunk_fds[i]);
            }
        }
        return;
    }

    size_t buffer_size = 4 * BytesInMb; // 4 MB
    uint32_t* write_buffer = static_cast<uint32_t*>(aligned_alloc(4096, buffer_size));
    if (!write_buffer) {
        std::cerr << "Failed to allocate write buffer.\n";
        lab2_.close(output_fd);
        return;
    }
    size_t buffer_count = 0;

    size_t total_written = 0;
    size_t iteration = 0;

    while (!min_heap.empty()) {
        iteration++;
        HeapNode node = min_heap.top();
        min_heap.pop();

        write_buffer[buffer_count++] = node.value;
        total_written++;

        // Debug: Current value and buffer status
        std::cout << "Iteration " << iteration << ": Writing value " << node.value
                  << " (0x" << std::hex << node.value << std::dec << "), Buffer count: "
                  << buffer_count << '\n';

        if (buffer_count * sizeof(uint32_t) == buffer_size) {
            ssize_t bytes_written = lab2_.write(output_fd, write_buffer, buffer_count * sizeof(uint32_t));
            if (bytes_written != static_cast<ssize_t>(buffer_count * sizeof(uint32_t))) {
                std::cerr << "Failed to write to output file. Bytes written: " << bytes_written << '\n';
                free(write_buffer);
                lab2_.close(output_fd);
                return;
            }
            std::cout << "Buffer flushed to output. Total written: " << total_written << " elements.\n";
            buffer_count = 0;
        }

        uint32_t next_value;
        ssize_t read_bytes = lab2_.read(chunk_fds[node.chunk_index], &next_value, sizeof(uint32_t));
        if (read_bytes == sizeof(uint32_t)) {
            min_heap.emplace(next_value, node.chunk_index);
            std::cout << "Pushed next value " << next_value << " (0x" << std::hex << next_value << std::dec
                      << ") from chunk " << node.chunk_index << " to heap.\n";
        } else if (read_bytes == 0) {
            std::cout << "Chunk " << node.chunk_index << " exhausted.\n";
            lab2_.close(chunk_fds[node.chunk_index]);
            chunk_fds[node.chunk_index] = -1;
            std::string temp_file = temp_directory + "/chunk_" + std::to_string(node.chunk_index) + ".dat";
            if (std::remove(temp_file.c_str()) != 0) {
                std::cerr << "Failed to delete temporary file: " << temp_file << '\n';
            } else {
                std::cout << "Deleted temporary file: " << temp_file << '\n';
            }
        } else {
            std::cerr << "Error reading from chunk " << node.chunk_index << ". Bytes read: " << read_bytes << '\n';
            lab2_.close(chunk_fds[node.chunk_index]);
            chunk_fds[node.chunk_index] = -1;
        }
    }

    // Flush any remaining data in the write buffer
    if (buffer_count > 0) {
        ssize_t bytes_written = lab2_.write(output_fd, write_buffer, buffer_count * sizeof(uint32_t));
        if (bytes_written != static_cast<ssize_t>(buffer_count * sizeof(uint32_t))) {
            std::cerr << "Failed to write remaining data to output file. Bytes written: "
                      << bytes_written << '\n';
            free(write_buffer);
            lab2_.close(output_fd);
            return;
        }
        std::cout << "Final buffer flushed to output. Total written: " << total_written << " elements.\n";
    }

    free(write_buffer);
    lab2_.close(output_fd);

    auto t_end = steady_clock::now();
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(t_end - t_start).count();
    std::cout << "Merge completed in " << ms << " ms. Total elements written: "
              << total_written << '\n';
}

// External memory sort implementation
void DirectIoExternalMemorySorter::externalMemorySort(
    const std::string& input_filename,
    const std::string& output_filename,
    size_t chunk_size_mb
) {
    // Define a unique temporary directory for this sort operation
    std::string temp_directory = "temp_chunks_" + SanitizeInputFilename(input_filename);

    // Create temporary directory if it doesn't exist
    if (!std::filesystem::exists(temp_directory)) {
        if (!std::filesystem::create_directory(temp_directory)) {
            std::cerr << "Failed to create temporary directory: " << temp_directory << '\n';
            return;
        }
    }

    // Step 1: Sort chunks and save them to temporary files
    sortByChunksAndSave(input_filename, temp_directory, chunk_size_mb);

    // Step 2: Calculate the number of chunks by counting files in temp_directory
    size_t num_chunks = 0;
    for (const auto& entry : std::filesystem::directory_iterator(temp_directory)) {
        if (entry.is_regular_file()) {
            ++num_chunks;
        }
    }

    if (num_chunks == 0) {
        std::cerr << "No chunks were created in temporary directory: " << temp_directory << '\n';
        return;
    }

    std::cout << "Merging " << num_chunks << " sorted chunks...\n";

    // Step 3: Merge the sorted chunks into the final output file
    mergeChunksAndSave(temp_directory, input_filename, output_filename, num_chunks);

    // Step 4: Cleanup temporary directory
    try {
        std::filesystem::remove_all(temp_directory);
    } catch (const std::filesystem::filesystem_error& e) {
        std::cerr << "Warning: Failed to delete temporary directory: " << temp_directory
                  << ". Error: " << e.what() << '\n';
    }

    std::cout << "External memory sort completed. Output file: " << output_filename << '\n';
}

// Check if the file is sorted
void DirectIoExternalMemorySorter::checkFileSorted(const std::string& input_filename) {
    // Open file using Lab2 with read flags
    fd_t fd = lab2_.open(input_filename);
    if (fd < 0) {
        std::cerr << "Failed to open file for checking: " << input_filename << '\n';
        return;
    }

    auto t_start = std::chrono::steady_clock::now();

    // Define buffer size (e.g., 4MB)
    size_t buffer_size = 4 * BytesInMb;
    size_t elements_per_buffer = buffer_size / sizeof(uint32_t);

    // Allocate aligned buffer
    uint32_t* buffer;
    if (posix_memalign(reinterpret_cast<void**>(&buffer), 4096, buffer_size)) {
        std::cerr << "Failed to allocate aligned memory for sortedness check.\n";
        lab2_.close(fd);
        return;
    }

    bool is_sorted = true;
    uint32_t prev_value = 0;
    bool first_element = true;

    while (true) {
        ssize_t bytes_read = lab2_.read(fd, buffer, buffer_size);
        if (bytes_read < 0) {
            std::cerr << "Error reading from file: " << input_filename << '\n';
            is_sorted = false;
            break;
        }

        if (bytes_read == 0) {
            break; // EOF
        }

        size_t elements_read = bytes_read / sizeof(uint32_t);
        for (size_t i = 0; i < elements_read; ++i) {
            uint32_t current_value = buffer[i];
            if (!first_element && current_value < prev_value) {
                std::cout << "File is not sorted. Error at value " << current_value
                          << " after " << prev_value << '\n';
                is_sorted = false;
                break;
            }
            prev_value = current_value;
            first_element = false;
        }

        if (!is_sorted) {
            break;
        }
    }

    // Free buffer and close file
    free(buffer);
    lab2_.close(fd);

    auto t_end = std::chrono::steady_clock::now();
    auto time_elapsed = std::chrono::duration_cast<std::chrono::nanoseconds>(t_end - t_start).count();

    if (is_sorted) {
        std::cout << "File '" << input_filename << "' is sorted.\n";
    } else {
        std::cout << "File '" << input_filename << "' is NOT sorted.\n";
    }

    std::cout << "ema-sort-int: Time to check if " << input_filename << " is sorted is "
              << time_elapsed << " ns\n";
}

// Print help message
void DirectIoExternalMemorySorter::printHelp() {
    std::cout << "Available subcommands:\n"
              << "\tgenerate <output_file> <size_mb>\n\t\tGenerate a random binary file of uint32_t values\n"
              << "\tsort <input_file> <output_file> <chunk_size_mb>\n\t\tSort the file in chunks and save sorted result\n"
              << "\tcheck <input_file>\n\t\tCheck if the file is sorted\n"
              << "\thelp\n\t\tPrint this help message (no args).\n"
              << "\tfull-benchmark <input_file> <output_file> <repeat-count>\n\t\t"
                 "Generate a 256MB file, sort it with 32MB chunk size, check the results, repeat "
                 "everything several times.\n";
}

void DirectIoExternalMemorySorter::echo(std::string message) {
  std::cout << "[e.s.i.d] " << message << '\n';
}
