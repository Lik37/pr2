#ifndef INPUT_OUTPUT_H
#define INPUT_OUTPUT_H

#include <cstdint>
#include <vector>

std::vector<uint8_t> readConsoleToBytes();
std::vector<uint8_t> fromStreamToData(std::istream& source);

bool dataToBinaryFile(const std::vector<uint8_t>& data, const std::string& filename);

std::string dataToHex(const std::vector<uint8_t>& data);
std::vector<uint8_t>hexToData(std::string input);

#endif
