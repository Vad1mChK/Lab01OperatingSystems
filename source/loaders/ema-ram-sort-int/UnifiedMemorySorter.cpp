//
// Created by vad1mchk on 18/11/24.
//
#include "UnifiedMemorySorter.hpp"

#include "../ram-sort-int/RamMemorySorter.hpp"

void UnifiedMemorySorter::printHelp() {
  std::cout << "UnifiedMemorySorter: Combine RAM and External Memory Sorters\n"
            << "Available commands:\n"
            << "\tgenerate <output_file> <size_mb>\n\t\tGenerate a random binary file\n"
            << "\tfull-benchmark <input_file> <output_file_prefix> <file-size-mb> <chunk_size_mb> <ram_sorters> <ema_sorters>\n"
            << "\t\tSort files using RAM and External Memory sorters in parallel\n"
            << "\tcheck <input_file>\n\t\tCheck if a file is sorted\n"
            << "\thelp\n\t\tPrint this help message\n";
}
