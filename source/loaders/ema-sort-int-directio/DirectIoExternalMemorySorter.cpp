#include "DirectIoExternalMemorySorter.hpp"

#include <filesystem>

#include <algorithm>
#include <chrono>
#include <cstdint>
#include <cstdio>  // For remove()
#include <fstream>
#include <iostream>
#include <limits>
#include <queue>
#include <sstream>
#include <vector>

#include "../util/sorter_utils.hpp"
#include "lab2_library.hpp"

void DirectIoExternalMemorySorter::generateRandomFile(const std::string& filename, size_t size_mb) {
  fd_t const fd = lab2_open(filename);
  if (fd < 0) {
    std::cerr << "Failed to open file for writing: " << filename << '\n';
    return;
  }

  auto t_start = std::chrono::steady_clock::now();

  size_t const num_elements = size_mb * BytesInMb / sizeof(uint32_t);
  size_t const buffer_size = LAB2_BLOCK_SIZE; // Aligned to the block size for O_DIRECT
  size_t const elements_per_buffer = buffer_size / sizeof(uint32_t);

  // Allocate aligned memory for writing
  uint32_t* buffer;
  if (posix_memalign(reinterpret_cast<void**>(&buffer), LAB2_BLOCK_SIZE, buffer_size) != 0) {
    std::cerr << "Failed to allocate aligned memory for writing.\n";
    lab2_close(fd);
    return;
  }

  size_t elements_written = 0;

  while (elements_written < num_elements) {
    size_t const elements_to_write = std::min(elements_per_buffer, num_elements - elements_written);

    // Fill the buffer with random data
    for (size_t i = 0; i < elements_to_write; ++i) {
      buffer[i] = RandomUint32();
    }

    // Write the buffer to the file
    ssize_t const bytes_written = lab2_write(fd, buffer, elements_to_write * sizeof(uint32_t));
    if (bytes_written < 0) {
      std::cerr << "Failed to write to file: " << filename << '\n';
      free(buffer);
      lab2_close(fd);
      return;
    }

    elements_written += elements_to_write;
  }

  // Free allocated buffer
  free(buffer);

  // Sync and close the file
  lab2_fsync(fd);
  lab2_close(fd);

  auto t_end = std::chrono::steady_clock::now();
  std::chrono::duration<size_t, std::nano> const time_elapsed = t_end - t_start;
  std::cout << "ema-sort-int: Time taken to generate random file of size " << size_mb << " MB is "
            << time_elapsed.count() << " ns" << '\n';
  std::cout << "Random file generated: " << filename << " (" << size_mb << " MB)" << '\n';
}


void DirectIoExternalMemorySorter::sortByChunksAndSave(
    const std::string& input_filename, const std::string& temp_directory, size_t chunk_size_mb
) {
    fd_t const input_fd = lab2_open(input_filename);
    if (input_fd < 0) {
        std::cerr << "Failed to open input file: " << input_filename << '\n';
        return;
    }

    auto t_start = std::chrono::steady_clock::now();

    size_t const chunk_size_in_elements = chunk_size_mb * BytesInMb / sizeof(uint32_t);
    size_t const buffer_size = chunk_size_in_elements * sizeof(uint32_t);
    uint32_t* buffer;

    // Allocate aligned memory for the buffer
    if (posix_memalign(reinterpret_cast<void**>(&buffer), 4096, buffer_size) != 0) {
        std::cerr << "Failed to allocate aligned memory for buffer.\n";
        lab2_close(input_fd);
        return;
    }

    // Determine the file size
    off_t const file_size_in_bytes = lab2_lseek(input_fd, 0, SEEK_END);
    if (file_size_in_bytes < 0) {
        std::cerr << "Failed to determine input file size.\n";
        free(buffer);
        lab2_close(input_fd);
        return;
    }
    lab2_lseek(input_fd, 0, SEEK_SET);

    size_t num_elements = file_size_in_bytes / sizeof(uint32_t);
    size_t num_chunks = (num_elements + chunk_size_in_elements - 1) / chunk_size_in_elements;

    std::cout << "Sorting " << num_chunks << " chunks..." << '\n';

    for (size_t i = 0; i < num_chunks; ++i) {
        size_t const elements_to_read =
            std::min(chunk_size_in_elements, num_elements - i * chunk_size_in_elements);

        // Read from the input file
        ssize_t const bytes_read = lab2_read(input_fd, buffer, elements_to_read * sizeof(uint32_t));
        if (bytes_read < 0) {
            std::cerr << "Failed to read chunk " << i << " from input file.\n";
            free(buffer);
            lab2_close(input_fd);
            return;
        }

        size_t const elements_read = bytes_read / sizeof(uint32_t);

        // Sort the buffer
        std::sort(buffer, buffer + elements_read);

        // Create the temp file name
        std::ostringstream temp_filename_stream;
        temp_filename_stream << temp_directory << "/"
                             << SanitizeInputFilename(input_filename) << "_chunk_" << i << ".dat";
        std::string const temp_filename = temp_filename_stream.str();

        fd_t const temp_fd = lab2_open(temp_filename);
        if (temp_fd < 0) {
            std::cerr << "Failed to open temp file: " << temp_filename << '\n';
            free(buffer);
            lab2_close(input_fd);
            return;
        }

        // Write the sorted chunk to the temp file
        ssize_t const bytes_written = lab2_write(temp_fd, buffer, elements_read * sizeof(uint32_t));
        if (bytes_written < 0) {
            std::cerr << "Failed to write sorted chunk " << i << " to temp file.\n";
            free(buffer);
            lab2_close(temp_fd);
            lab2_close(input_fd);
            return;
        }

        lab2_fsync(temp_fd);
        lab2_close(temp_fd);

        std::cout << "Chunk " << i + 1 << " sorted and saved to " << temp_filename << '\n';
    }

    free(buffer);
    lab2_close(input_fd);

    auto t_end = std::chrono::steady_clock::now();
    std::chrono::duration<size_t, std::nano> const time_elapsed = t_end - t_start;
    std::cout << "ema-sort-int: Time to sort chunks from " << input_filename << " is "
              << time_elapsed.count() << " ns" << '\n';
}

void DirectIoExternalMemorySorter::mergeChunksAndSave(
    const std::string& temp_directory,
    const std::string& input_filename,
    const std::string& output_filename,
    size_t num_chunks
) {
    auto t_start = std::chrono::steady_clock::now();

    struct HeapNode final {
        uint32_t value;
        size_t chunk_index;

        HeapNode(uint32_t val, size_t ch_idx) : value(val), chunk_index(ch_idx) {}

        bool operator>(const HeapNode& other) const {
            return value > other.value;
        }
    };

    std::vector<fd_t> temp_fds(num_chunks);
    std::vector<uint32_t> current_values(num_chunks);
    std::vector<bool> has_more(num_chunks, true);

    for (size_t i = 0; i < num_chunks; ++i) {
        std::ostringstream temp_filename_stream;
        temp_filename_stream << temp_directory << "/"
                             << SanitizeInputFilename(input_filename) << "_chunk_" << i << ".dat";
        std::string const temp_filename = temp_filename_stream.str();

        temp_fds[i] = lab2_open(temp_filename);
        if (temp_fds[i] < 0) {
            std::cerr << "Failed to open temp file for merging: " << temp_filename << '\n';
            return;
        }

        if (lab2_read(temp_fds[i], &current_values[i], sizeof(uint32_t)) <= 0) {
            has_more[i] = false;
        }
    }

    fd_t const output_fd = lab2_open(output_filename);
    if (output_fd < 0) {
        std::cerr << "Failed to open output file for writing: " << output_filename << '\n';
        for (fd_t const temp_fd : temp_fds) {
            if (temp_fd >= 0) {
                lab2_close(temp_fd);
            }
        }
        return;
    }

    auto cmp = [](const HeapNode& left, const HeapNode& right) { return left.value > right.value; };
    std::priority_queue<HeapNode, std::vector<HeapNode>, decltype(cmp)> min_heap(cmp);

    for (size_t i = 0; i < num_chunks; ++i) {
        if (has_more[i]) {
            min_heap.emplace(current_values[i], i);
        }
    }

    while (!min_heap.empty()) {
        HeapNode node = min_heap.top();
        min_heap.pop();

        ssize_t const bytes_written = lab2_write(output_fd, &node.value, sizeof(uint32_t));
        if (bytes_written <= 0) {
            std::cerr << "Failed to write to output file: " << output_filename << '\n';
            break;
        }

        size_t const idx = node.chunk_index;
        if (lab2_read(temp_fds[idx], &current_values[idx], sizeof(uint32_t)) > 0) {
            min_heap.emplace(current_values[idx], idx);
        } else {
            has_more[idx] = false;
            lab2_close(temp_fds[idx]);
            temp_fds[idx] = -1;

            // Delete temp file
            std::ostringstream temp_filename_stream;
            temp_filename_stream << temp_directory << "/chunk_" << idx << ".dat";
            std::string const temp_filename = temp_filename_stream.str();
            (void)std::remove(temp_filename.c_str());
        }
    }

    lab2_fsync(output_fd);
    lab2_close(output_fd);

    auto t_end = std::chrono::steady_clock::now();
    std::chrono::duration<size_t, std::nano> const time_elapsed = t_end - t_start;
    std::cout << "ema-sort-int: Time to merge chunks into " << output_filename << " is "
              << time_elapsed.count() << " ns" << '\n';
}

void DirectIoExternalMemorySorter::externalMemorySort(
    const std::string& input_filename, const std::string& output_filename, size_t chunk_size_mb
) {
  // Determine the temporary directory path
  std::string temp_directory = std::filesystem::temp_directory_path().string();
  if (temp_directory.empty()) {
    temp_directory = ".";
  }

  // Step 1: Sort chunks and save them to temporary files
  sortByChunksAndSave(input_filename, temp_directory, chunk_size_mb);

  // Step 2: Calculate the number of chunks using lab2_* functions
  fd_t const input_fd = lab2_open(input_filename);
  if (input_fd < 0) {
    std::cerr << "Failed to open input file for size calculation: " << input_filename << '\n';
    return;
  }

  // Seek to the end to determine file size
  off_t const file_size_in_bytes = lab2_lseek(input_fd, 0, SEEK_END);
  if (file_size_in_bytes < 0) {
    std::cerr << "Failed to seek to end of input file: " << input_filename << '\n';
    lab2_close(input_fd);
    return;
  }

  // Calculate number of chunks based on file size and chunk size
  size_t const chunk_size_in_bytes = chunk_size_mb * BytesInMb;
  size_t const num_chunks = (file_size_in_bytes + chunk_size_in_bytes - 1) / chunk_size_in_bytes;

  // Optional: Print file size and number of chunks for debugging
  std::cout << "Input file size: " << file_size_in_bytes << " bytes\n";
  std::cout << "Chunk size: " << chunk_size_in_bytes << " bytes\n";
  std::cout << "Number of chunks: " << num_chunks << '\n';

  // Close the input file as it's no longer needed
  lab2_close(input_fd);

  // Step 3: Merge the sorted chunks into the final output file
  mergeChunksAndSave(temp_directory, input_filename, output_filename, num_chunks);

  std::cout << "External memory sort completed. Output file: " << output_filename << '\n';
}

void DirectIoExternalMemorySorter::checkFileSorted(const std::string& input_filename) {
    fd_t const input_fd = lab2_open(input_filename);
    if (input_fd < 0) {
        std::cerr << "Failed to open file for checking: " << input_filename << '\n';
        return;
    }

    auto t_start = std::chrono::steady_clock::now();

    uint32_t prev_value = 0;
    bool first = true;
    bool is_sorted = true;

    constexpr size_t buffer_size = LAB2_BLOCK_SIZE; // Aligned buffer size for O_DIRECT
    uint32_t* buffer;
    if (posix_memalign(reinterpret_cast<void**>(&buffer), LAB2_BLOCK_SIZE, buffer_size) != 0) {
        std::cerr << "Failed to allocate aligned memory.\n";
        lab2_close(input_fd);
        return;
    }

    while (true) {
        ssize_t const bytes_read = lab2_read(input_fd, buffer, buffer_size);
        if (bytes_read < 0) {
            std::cerr << "Failed to read file: " << input_filename << '\n';
            is_sorted = false;
            break;
        }
        if (bytes_read == 0) {
            break; // EOF
        }

        size_t const elements_read = bytes_read / sizeof(uint32_t);

        for (size_t i = 0; i < elements_read; ++i) {
            uint32_t const current_value = buffer[i];
            if (!first) {
                if (current_value < prev_value) {
                    std::cout << "File is not sorted. Error at value " << std::hex << current_value
                              << " after " << prev_value << '\n';
                    is_sorted = false;
                    break;
                }
            } else {
                first = false;
            }
            prev_value = current_value;
        }

        if (!is_sorted) {
            break;
        }
    }

    if (is_sorted) {
        std::cout << "File is sorted." << '\n';
    } else {
        std::cout << "File is not sorted." << '\n';
    }

    free(buffer);
    lab2_close(input_fd);

    auto t_end = std::chrono::steady_clock::now();
    std::chrono::duration<size_t, std::nano> time_elapsed = t_end - t_start;
    std::cout << "ema-sort-int: Time to check if " << input_filename << " is sorted is "
              << time_elapsed.count() << " ns" << '\n';
}

// Print help message
void DirectIoExternalMemorySorter::printHelp() {
  std::cout << "Available subcommands:\n"
            << "\tgenerate <output_file> <size_mb>\n\t\tGenerate a random binary file of uint32_t "
               "values\n"
            << "\tsort <input_file> <output_file> <chunk_size_mb>\n\t\tSort the file in chunks and "
               "save sorted result\n"
            << "\tcheck <input_file>\n\t\tCheck if the file is sorted\n"
            << "\thelp\n\t\tPrint this help message (no args).\n"
            << "\tfull-benchmark <input_file> <output_file> <repeat-count>\n\t\t"
               "Generate a 256MB file, sort it with 32MB chunk size, check the results, repeat "
               "everything several times.\n";
}
