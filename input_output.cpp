#include <iostream>
#include <sstream>
#include <fstream>
#include <cstdint>
#include <vector>
#include <iomanip>

#include "input_output.h"

using namespace std;


vector<uint8_t> readConsoleToBytes() {
    vector<uint8_t> resultBuffer;
    string line;

    while (true) {
        if (!getline(cin, line)) {
            break; 
        }

        if (line == "exit") {
            break;
        }

        for (char c : line) {
            resultBuffer.push_back(static_cast<uint8_t>(c));
        }

        resultBuffer.push_back(static_cast<uint8_t>('\n'));
    }
    
    return resultBuffer;
}

vector<uint8_t> fromStreamToData(istream& source) {
    vector<uint8_t> resultBuffer;
    const size_t chunkSize = 4096;
    while (source) {
        size_t oldSize = resultBuffer.size();
        resultBuffer.resize(oldSize + chunkSize);
        
        source.read(reinterpret_cast<char*>(&resultBuffer[oldSize]), chunkSize);;
        
        streamsize bytesRead = source.gcount();
        
        resultBuffer.resize(oldSize + bytesRead);
    }
    
    return resultBuffer;
}


bool dataToBinaryFile(const vector<uint8_t>& data, const string& filename) {
    ofstream outFile(filename, ios::binary);
    if (!outFile) 
        return false;
    
    outFile.write(reinterpret_cast<const char*>(data.data()), data.size());
    return outFile.good(); 
}

string dataToHex(const vector<uint8_t>& data){
    stringstream ss;
    for (uint8_t byte : data) {
        ss << hex << setw(2) << setfill('0') << (int)byte << " ";
    }
    return ss.str();
}
vector<uint8_t> hexToData(string input) {
    vector<uint8_t> data;
    istringstream iss(input);
    uint16_t byteVal;
    while (iss >> hex >> byteVal)
        data.push_back(static_cast<uint8_t>(byteVal));
    return data;
}

