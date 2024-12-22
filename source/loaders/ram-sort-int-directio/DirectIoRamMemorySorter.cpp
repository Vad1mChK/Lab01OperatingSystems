#include "DirectIoRamMemorySorter.hpp"

#include <algorithm>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <limits>
#include <vector>
#include <chrono>

#include "../util/sorter_utils.hpp"

// Generate a random binary file of uint32_t values
void DirectIoRamMemorySorter::generateRandomFile(const std::string& filename, size_t size_mb) {
    // Calculate total number of elements
    size_t num_elements = (size_mb * BytesInMb) / sizeof(uint32_t);

    // Define buffer size (e.g., 4MB)
    size_t buffer_size = getBufferSizeBytes();
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
        size_t bytes_to_write = current_chunk * sizeof(uint32_t); // Use current_chunk instead of elements_per_buffer
        ssize_t bytes_written = lab2_.write(fd, buffer, bytes_to_write);
        if (bytes_written != static_cast<ssize_t>(bytes_to_write)) {
            std::cerr << "Failed to write to file: " << filename << " (Expected "
                      << bytes_to_write << " bytes, wrote " << bytes_written << " bytes)\n";
            lab2_.close(fd);
            free(buffer);
            return;
        }

        elements_written += current_chunk;
    }

    // Close file
    lab2_.close(fd);

    // Free buffer
    free(buffer);

    auto t_end = std::chrono::steady_clock::now();
    auto time_elapsed = std::chrono::duration_cast<std::chrono::nanoseconds>(t_end - t_start).count();

    std::cout << "ram-sort-int: Time taken to generate random file of size " << size_mb << " MB is "
              << time_elapsed << " ns\n";
    std::cout << "Random file generated: " << filename << " (" << size_mb << " MB)\n";
}


// Sort the entire file in memory and write the sorted data to the output file
void DirectIoRamMemorySorter::sortInMemory(
    const std::string& input_filename, const std::string& output_filename
) {
  // Read the entire file into memory

  // std::ifstream input(input_filename, std::ios::binary | std::ios::ate);
  // if (!input) {
  //   std::cout << "Failed to open input file: " << input_filename << '\n';
  //   return;
  // }

  auto t_start = std::chrono::steady_clock::now();

  fd_t input_fd = lab2_.open(input_filename);
  if (input_fd < 0) {
    std::cout << "Failed to open input file: " << input_filename << '\n';
    return;
  }

  // Determine file size

  // std::streamsize file_size = input.tellg();
  // input.seekg(0, std::ios::beg);

  off_t file_size = lab2_.lseek(input_fd, 0, SEEK_END);
  lab2_.lseek(input_fd, 0, SEEK_SET);

  size_t num_elements = file_size / sizeof(uint32_t);
  std::vector<uint32_t> data(num_elements);

  // Read data

  // if (!input.read(reinterpret_cast<char*>(data.data()), file_size)) {
  //   std::cout << "Failed to read input file: " << input_filename << '\n';
  //   return;
  // }
  // input.close();

  ssize_t bytes_read = lab2_.read(input_fd, data.data(), file_size);
  if (bytes_read != file_size) {
    std::cout << "Failed to read input file: " << input_filename << '\n';
    lab2_.close(input_fd);
    return;
  }
  lab2_.close(input_fd);

  auto t_end = std::chrono::steady_clock::now();
  std::chrono::duration<size_t, std::nano> time_elapsed = t_end - t_start;
  std::cout << "ram-sort-int: Time taken to read input file" << input_filename << " is "
      << time_elapsed.count() << " ns" << '\n';
  t_start = std::chrono::steady_clock::now();

  // Sort the data in memory
  std::cout << "Sorting " << num_elements << " elements in memory..." << '\n';
  std::sort(data.begin(), data.end());

  t_end = std::chrono::steady_clock::now();
  time_elapsed = t_end - t_start;
  std::cout << "ram-sort-int: Time taken to sort data of size " << file_size / BytesInMb << "MB is "
      << time_elapsed.count() << " ns" << '\n';
  t_start = std::chrono::steady_clock::now();

  // Write the sorted data to the output file

  // std::ofstream output(output_filename, std::ios::binary);
  // if (!output) {
  //   std::cout << "Failed to open output file: " << output_filename << '\n';
  //   return;
  // }
  // output.write(reinterpret_cast<const char*>(data.data()), file_size);
  // output.close();

  fd_t output_fd = lab2_.open(output_filename);
  if (output_fd < 0) {
    std::cout << "Failed to open output file: " << output_filename << '\n';
    return;
  }
  ssize_t bytes_written = lab2_.write(output_fd, data.data(), file_size);
  if (bytes_written != file_size) {
    std::cout << "Failed to write to output file: " << output_filename << " (Expected "
              << file_size << " bytes, wrote " << bytes_written << " bytes)\n";
    lab2_.close(output_fd);
    return;
  }
  lab2_.close(output_fd);

  t_end = std::chrono::steady_clock::now();
  time_elapsed = t_end - t_start;
  std::cout << "ram-sort-int: Time taken to write data to file " << output_filename << " is "
      << time_elapsed.count() << " ns" << '\n';
  std::cout << "In-memory sort completed. Output file: " << output_filename << '\n';
}

// Check if the file is sorted
void DirectIoRamMemorySorter::checkFileSorted(const std::string& filename) {
    // Define buffer size (e.g., 4MB)
    constexpr size_t buffer_size = 4 * BytesInMb; // Adjust as needed
    size_t elements_per_buffer = buffer_size / sizeof(uint32_t);

    // Allocate aligned buffer
    uint32_t* buffer;
    if (posix_memalign(reinterpret_cast<void**>(&buffer), LAB2_BLOCK_SIZE, buffer_size) != 0) {
        std::cerr << "Failed to allocate aligned memory.\n";
        return;
    }

    // Open the file using lab2_
    fd_t input_fd = lab2_.open(filename);
    if (input_fd < 0) {
        std::cerr << "Failed to open file for checking: " << filename << '\n';
        free(buffer);
        return;
    }

    // Determine the file size
    off_t file_size = lab2_.lseek(input_fd, 0, SEEK_END);
    if (file_size < 0) {
        std::cerr << "Failed to determine file size: " << filename << '\n';
        lab2_.close(input_fd);
        free(buffer);
        return;
    }

    // Ensure the file size is a multiple of uint32_t
    if (file_size % sizeof(uint32_t) != 0) {
        std::cerr << "File size is not a multiple of uint32_t: " << filename << '\n';
        lab2_.close(input_fd);
        free(buffer);
        return;
    }

    size_t total_elements = file_size / sizeof(uint32_t);
    lab2_.lseek(input_fd, 0, SEEK_SET); // Reset file pointer to beginning

    auto t_start = std::chrono::steady_clock::now();

    size_t elements_processed = 0;
    uint32_t prev_value = 0;
    bool first_element = true;
    bool is_sorted = true;

    while (elements_processed < total_elements) {
        // Calculate how many elements to read in this iteration
        size_t elements_to_read = std::min(elements_per_buffer, total_elements - elements_processed);
        size_t bytes_to_read = elements_to_read * sizeof(uint32_t);

        // Read data into buffer
        ssize_t bytes_read = lab2_.read(input_fd, buffer, bytes_to_read);
        if (bytes_read != static_cast<ssize_t>(bytes_to_read)) {
            std::cerr << "Failed to read from file: " << filename << '\n';
            is_sorted = false;
            break;
        }

        // Iterate through the buffer to check sortedness
        for (size_t i = 0; i < elements_to_read; ++i) {
            uint32_t current_value = buffer[i];
            if (!first_element) {
                if (current_value < prev_value) {
                    std::cout << "File is not sorted. Error at element " << (elements_processed + i)
                              << ": " << current_value << " after " << prev_value << '\n';
                    is_sorted = false;
                    break;
                }
            } else {
                first_element = false;
            }
            prev_value = current_value;
        }

        if (!is_sorted) {
            break;
        }

        elements_processed += elements_to_read;
    }

    // Close the file and free the buffer
    lab2_.close(input_fd);
    free(buffer);

    // Report the result
    if (is_sorted) {
        std::cout << "File is sorted.\n";
    } else {
        std::cout << "File is not sorted.\n";
    }

    auto t_end = std::chrono::steady_clock::now();
    auto time_elapsed = std::chrono::duration_cast<std::chrono::nanoseconds>(t_end - t_start).count();

    std::cout << "ram-sort-int: Time taken to check if file " << filename << " is sorted is "
              << time_elapsed << " ns\n";
}


// Print help message
void DirectIoRamMemorySorter::printHelp() {
  std::cout << "Available commands:\n"
            << "\tgenerate <output_file> <size_mb>\n\t\tGenerate a random binary file of uint32_t "
               "values\n"
            << "\tsort <input_file> <output_file>\n\t\tSort the file entirely in memory\n"
            << "\tcheck <input_file>\n\t\tCheck if the file is sorted\n"
            << "\thelp\n\t\tPrint this help message\n"
            << "\tfull-benchmark <input_file> <output_file> <repeat-count>\n\t\t"
            << "Generate a file of size 256MB, sort it in memory, save the result, check it, and "
               "repeat several times.\n";
}
