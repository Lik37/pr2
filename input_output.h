#ifndef INPUT_OUTPUT_H
#define INPUT_OUTPUT_H

#include <cstdint>
#include <vector>

std::vector<uint8_t> readConsoleToBytes();
std::vector<uint8_t> fromStreamToData(std::istream& source);

bool dataToBinaryFile(const std::vector<uint8_t>& data, const std::string& filename);

std::string dataToHex(const std::vector<uint8_t>& data);
std::vector<uint8_t>hexToData(std::string input);


void printMainMenu();
void printElgamalMenu();
void printElgamalSettings(std::string mode, std::vector<uint8_t> inputData, std::vector<int64_t> keys, std::string outputPath);

void printTableEvklid(const std::vector<int64_t>& vecQ, const std::vector<int64_t>& vecR, 
                      const std::vector<int64_t>& vecX, const std::vector<int64_t>& vecY);

std::vector<int64_t> enterElgamalKeys(std::string line);
bool solveOutputFile(std::string path);

#endif
