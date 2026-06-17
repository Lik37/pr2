#include <iostream>
#include <sstream>
#include <fstream>
#include <filesystem>
#include <cstdint>
#include <vector>
#include <iomanip>

#include "input_output.h"
#include "elgamal.h"
#include "math_utils.h"

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



void printMainMenu() {
    cout << "\n========Меню========\n" <<
    "1. Возведение в степень через Ферма и бинаронее возведение\n" <<
    "2. Расчёт c*d mod m = 1\n" <<
    "3. Расчёт d = c^-1 mod m\n" << 
    "4. Эль-Гамаль, шифрование, дешифрование\n" << 
    "5. MitM, эмуляция атаки посередине на протокол Эль-Гамаля\n" << 
    "6. Решение уравнения 1256а + 847b = 119\n" << 
    "0. Выход\n";
    cout << "Выберите задание: ";
}

void printElgamalMenu() {
    cout << "\n--------Протокол Эль-Гамаля--------\n" <<
    "1. Шифрование\n" <<
    "2. Дешифрование\n" <<
    "0. Выход\n";
    cout << "Выберите опцию: ";
}

void printElgamalSettings(string mode, const vector<uint8_t>& inputData, 
                          const vector<int64_t>& keys, string outputPath) {
    cout << "\n----Протокол Эль-Гамаля -> " << mode << "----\n" <<
    "1. Начать\n" <<

    "2. Добавить исходные данные (";
        if (inputData.empty()) {
            cout << "нет";
        } else {
            cout << inputData.size() << " bytes";
        }
        cout << ")\n" <<
    
    "3. Добавить ключ (";
        if (keys.empty()) {
            cout << "нет";
        } else {
            cout << 
            "Открытый[p g y]=[" << 
                keys[0] << " " << 
                keys[1] << " " << 
                keys[3] << "], " << 
            "Закрытый[x]=[" << 
                keys[2] << "]";
        }
        cout << ")\n" <<
    
    "4. Добавить вывод алгоритма (" << 
        (outputPath.empty() ? "консоль" : outputPath) << ")\n" <<
    
    "0. Выход\n";
    
    cout << "Выберите опцию: ";
}


void printTableEvklid(const vector<int64_t>& vecQ, const vector<int64_t>& vecR, 
                      const vector<int64_t>& vecX, const vector<int64_t>& vecY) {
        
    cout << "\n" <<
    "r\tu\tv\tq" << "\n" <<
    vecR[0] << "\t" << vecX[0] << "\t" << vecY[0] << "\t" << "-\n" << 
    vecR[1] << "\t" << vecX[1] << "\t" << vecY[1] << "\t" << "-\n";  
    
    for (size_t i = 2; i < vecR.size(); ++i) {
        cout << 
        vecR[i] << "\t" << 
        vecX[i] << "\t" << 
        vecY[i] << "\t" << 
        vecQ[i] << "\n";
    }
}


vector<int64_t> enterElgamalKeys(string line) {
    vector<int64_t> keys;

    istringstream iss(line);
    int64_t p, g, x;
    if (!(iss >> p >> g >> x)) {
        throw invalid_argument("Ожидалось 4 числа.");
    } if (!isPrime(p)) {
        throw invalid_argument("p должно быть простым числом.");
    } if (p <= 255) {
        throw invalid_argument("p должно быть > 255.");
    } if (g < 2 || g > p - 2) {
        throw invalid_argument("g должно быть в диапазоне 2..p-2.");
    } if (x < 2 || x > p - 2) {
        throw invalid_argument("x должно быть в диапазоне 2..p-2.");
    }
    int64_t y = genY(p, g, x);

    keys.push_back(p);
    keys.push_back(g);
    keys.push_back(x);
    keys.push_back(y);

    return keys;
}

bool solveOutputFile(string path) {
    namespace fs = filesystem;
    fs::path filePath(path);
    fs::path parentDir = filePath.parent_path();

    if (!parentDir.empty() && !fs::exists(parentDir)) {
        cerr << "Ошибка: Такого пути не существует.\n";
        return 0;
    }

    if (!fs::exists(filePath)) {
        cout << "Создать файл '" << path << "' ? (Y/n): ";
        char answer;
        cin >> answer;
        if (answer != 'n' && answer != 'N') {
            return 1;
        }
    } else {
        return 1;
    }

    return 0;
}

