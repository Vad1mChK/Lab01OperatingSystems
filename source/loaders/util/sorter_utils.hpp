//
// Created by vad1mchk on 2024/11/10.
//

#ifndef MONOLITH_SORTER_UTILS_HPP
#define MONOLITH_SORTER_UTILS_HPP
#include <cstddef>
#include <cstdint>
#include <string>

static const size_t BytesInMb = static_cast<size_t>(1024 * 1024);

std::string SanitizeInputFilename(const std::string& input_filename);

uint32_t RandomUint32();

#endif  // MONOLITH_SORTER_UTILS_HPP
