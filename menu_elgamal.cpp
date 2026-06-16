#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdint>
#include <vector>
#include <string>
#include <random>
#include <filesystem>

#include "math_utils.h"
#include "input_output.h"
#include "elgamal.h"

using namespace std;


extern mt19937 gen;


enum class ElgamalOption {
    EXIT = 0,
    ENCRYPT,
    DECRYPT
};
enum class ParamsElgamalOption {
    EXIT = 0,
    RUN,
    ADD_INPUT,
    ADD_KEY,
    ADD_OUTPUT
};

vector<uint8_t> inputData;
vector<int64_t> keys;
string outputPath;

void menuElgamal() {
    bool elgamalRunning = 1;
    bool paramsEditRunning;

    while (elgamalRunning) {

        inputData.clear();

        cout << "\n--------Протокол Эль-Гамаля--------\n" <<
        "1. Шифрование\n" <<
        "2. Дешифрование\n" <<
        "0. Выход\n";
        cout << "Выберите опцию: ";
        
        int modeChoice;
        if (!(cin >> modeChoice)) {
            cerr << "Ошибка: ведите число.\n";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            continue;
        }
        
        ElgamalOption modeOption = static_cast<ElgamalOption>(modeChoice);

        switch(modeOption) {

            case ElgamalOption::EXIT: {
                elgamalRunning = 0;
                break;
            }

            case ElgamalOption::ENCRYPT: {
                
                paramsEditRunning = 1;
                while (paramsEditRunning) {
                    cout << "\n----Протокол Эль-Гамаля -> Шифрование----\n" <<
                    "1. Начать шифрование\n" <<

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
                    
                    "4. Добавить вывод шифртекста (" << 
                        (outputPath.empty() ? "консоль" : outputPath) << ")\n" <<
                    
                    "0. Выход\n";
                    
                    cout << "Выберите опцию: ";
                    int paramsChoice;
                    if (!(cin >> paramsChoice)) {
                        cerr << "Ошибка: Введите число.\n";
                        cin.clear();
                        cin.ignore(numeric_limits<streamsize>::max(), '\n');
                        continue;
                    }
                    
                    ParamsElgamalOption paramsOption = static_cast<ParamsElgamalOption>(paramsChoice);
                    
                    switch(paramsOption) {
                        case ParamsElgamalOption::EXIT: {
                            paramsEditRunning = 0;
                            break;
                        }
                        case ParamsElgamalOption::RUN: {
                            if (inputData.empty() || keys.empty()) {
                                cerr << "Ошибка: Необходимо добавить параметры\n";
                                break;
                            }

                            cout << "\n--Протокол Эль-Гамаля -> Шифрование -> Старт--\n";
                            auto cipherPairs = elgamalEncrypt(inputData, keys[0], keys[1], keys[3], gen);
                            auto ciphertext = pairsToBytes(cipherPairs);
                            if (outputPath.empty()) {
                                cout << "Шифротекст:\n" << dataToHex(ciphertext) << "\n";
                            } else {
                                if (dataToBinaryFile(ciphertext, outputPath)) {
                                    cout << "Шифротекст успешно записан.\n";
                                } else {
                                    cerr << "Ошибка записи.\n";
                                }
                            }
                            break;
                        }
                        case ParamsElgamalOption::ADD_INPUT: {
                            cout << "\n--Протокол Эль-Гамаля -> Шифрование -> Ввод исходных данных--\n" <<
                            "Введите путь к файлу, либо нажмите ENTER, чтобы начать ввод из консоли\n";

                            string path;
                            cin.ignore();
                            getline(cin, path);

                            if (path.empty()) {
                                cout << "Введите строку, для завершения введите 'exit' с новой строки:\n";
                                inputData = readConsoleToBytes();
                                if (!inputData.empty() && inputData.back() == '\n')
                                    inputData.pop_back();

                            } else {
                                ifstream file(path, ios::binary);
                                if (!file) {
                                    cerr << "Ошибка: Не удалось открыть файл.\n";
                                    break;
                                }
                                inputData = fromStreamToData(file);
                                if (inputData.empty())
                                    cerr << "Ошибка: Файл пуст.\n";
                                file.close();
                            }

                            break;
                        }
                        case ParamsElgamalOption::ADD_KEY: {
                            cout << "\n--Протокол Эль-Гамаля -> Шифрование -> Ввод ключа--\n" <<
                            "Введите по порядку p g x, либо нажмите ENTER, чтобы сгенерировать ключ\n";

                            keys.clear();

                            string line;
                            cin.ignore();
                            getline(cin, line);
                            
                            if (line.empty()) {
                                cout << "Введите минимальное и максимальное значение p: ";
                                int64_t minP, maxP;
                                cin >> minP >> maxP;

                                if (minP < 256) {
                                    cerr << "Ошибка: минимальное p должно быть больше 255\n";
                                    break;
                                }

                                keys.push_back(genP(minP, maxP, gen)); // p
                                keys.push_back(genG(keys[0], gen)); // g
                                keys.push_back(genX(keys[0], gen)); // x
                                keys.push_back(genY(keys[0], keys[1], keys[2])); // y
                            } else {
                                istringstream iss(line);
                                int64_t p, g, x;
                                if (!(iss >> p >> g >> x)) {
                                    cerr << "Ошибка: Ожидалось 4 числа.\n";
                                    break;
                                } if (!isPrime(p)) {
                                    cerr << "Ошибка: p должно быть простым числом.\n";
                                    break;
                                } if (p <= 255) {
                                    cerr << "Ошибка: p должно быть > 255.\n";
                                    break;
                                } if (g < 2 || g > p - 2) {
                                    cerr << "Ошибка: g должно быть в диапазоне 2..p-2.\n";
                                    break;
                                } if (x < 2 || x > p - 2) {
                                    cerr << "Ошибка: x должно быть в диапазоне 2..p-2.\n";
                                }
                                int64_t y = genY(p, g, x);

                                keys.push_back(p);
                                keys.push_back(g);
                                keys.push_back(x);
                                keys.push_back(y);
                            }

                            break;
                        }
                        case ParamsElgamalOption::ADD_OUTPUT: {
                            cout << "\n--Протокол Эль-Гамаля -> Шифрование -> Добавление вывода--\n" <<
                            "Введите путь к файлу, либо нажмите ENTER, чтобы оставить вывод из консоли\n";

                            string path;
                            cin.ignore();
                            getline(std::cin, path);

                            if (path.empty()) {
                                outputPath = "";
                                break;
                            } else {
                                namespace fs = filesystem;
                                fs::path filePath(path);
                                fs::path parentDir = filePath.parent_path();
                                
                                if (!parentDir.empty() && !fs::exists(parentDir)) {
                                    cerr << "Ошибка: Такого пути не существует.\n";
                                    break;
                                }

                                if (!fs::exists(filePath)) {
                                    std::cout << "Создать файл '" << path << "' ? (Y/n): ";
                                    char answer;
                                    cin >> answer;
                                    if (answer != 'n' && answer != 'N') {
                                        outputPath = path;
                                    }
                                } else {
                                    outputPath = path;
                                }
                            }

                            break;
                        }
                        default: {
                            cerr << "Ошибка: такого выбора нет.";
                            break;
                        }
                    }
                }

                break;
            }                    

            case ElgamalOption::DECRYPT: {
                paramsEditRunning = 1;
                while (paramsEditRunning) {
                    cout << "\n----Протокол Эль-Гамаля -> Расшифрование----\n" <<
                        "1. Начать расшифрование\n" <<

                        "2. Добавить шифротекст (";
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

                        "4. Добавить вывод расшифрованных данных (" << 
                            (outputPath.empty() ? "консоль" : outputPath) << ")\n" <<

                        "0. Выход\n";

                    cout << "Выберите опцию: ";
                    int paramsChoice;
                    if (!(cin >> paramsChoice)) {
                        cerr << "Ошибка: Введите число.\n";
                        cin.clear();
                        cin.ignore(numeric_limits<streamsize>::max(), '\n');
                        continue;
                    }

                    ParamsElgamalOption paramsOption = static_cast<ParamsElgamalOption>(paramsChoice);

                    switch(paramsOption) {
                        case ParamsElgamalOption::EXIT: {
                            paramsEditRunning = 0;
                            break;
                        }
                        case ParamsElgamalOption::RUN: {
                            if (inputData.empty() || keys.empty()) {
                                cerr << "Ошибка: Необходимо добавить параметры\n";
                                break;
                            }

                            cout << "\n--Протокол Эль-Гамаля -> Расшифрование -> Старт--\n";
                            auto cipherPairs = bytesToPairs(inputData);
                            if (cipherPairs.empty()) {
                                cerr << "Ошибка: Некорректный шифротекст.\n";
                                break;
                            }
                            auto plaintext = elgamalDecrypt(cipherPairs, keys[0], keys[2]);
                            if (outputPath.empty()) {
                                cout << "Расшифрованный текст:\n";
                                for (uint8_t c : plaintext) {
                                        cout << static_cast<char>(c);
                                }
                                cout << "\n";
                            } else {
                                if (dataToBinaryFile(plaintext, outputPath)) {
                                    cout << "Расшифрованные данные успешно записаны.\n";
                                } else {
                                    cerr << "Ошибка записи.\n";
                                }
                            }
                            break;
                        }
                        case ParamsElgamalOption::ADD_INPUT: {
                            cout << "\n--Протокол Эль-Гамаля -> Расшифрование -> Ввод шифротекста--\n" <<
                            "Введите путь к файлу, либо нажмите ENTER, чтобы ввести HEX-строку\n";

                            string path;
                            cin.ignore();
                            getline(cin, path);

                            if (path.empty()) {
                                if (path.empty()) {
                                cout << "Введите HEX-строку (байты через пробел):\n";
                                string input;
                                getline(cin, input);
                                inputData = hexToData(input);
                                }
                            } else {
                                ifstream file(path, ios::binary);
                                if (!file) {
                                    cerr << "Ошибка: Не удалось открыть файл.\n";
                                    break;
                                }
                                inputData = fromStreamToData(file);
                                if (inputData.empty())
                                    cerr << "Ошибка: Файл пуст.\n";
                                file.close();
                            }

                            break;
                        }
                        case ParamsElgamalOption::ADD_KEY: {
                            cout << "\n--Протокол Эль-Гамаля -> Расшифрование -> Ввод ключа--\n" <<
                            "Введите по порядку p g x\n";

                            keys.clear();

                            string line;
                            cin.ignore();
                            getline(cin, line);

                            istringstream iss(line);
                            int64_t p, g, x;
                            if (!(iss >> p >> g >> x)) {
                                cerr << "Ошибка: Ожидалось 3 числа.\n";
                                break;
                            } if (!isPrime(p)) {
                                cerr << "Ошибка: p должно быть простым числом.\n";
                                break;
                            } if (p <= 255) {
                                cerr << "Ошибка: p должно быть > 255.\n";
                                break;
                            } if (g < 2 || g > p - 2) {
                                cerr << "Ошибка: g должно быть в диапазоне 2..p-2.\n";
                                break;
                            } if (x < 2 || x > p - 2) {
                                cerr << "Ошибка: x должно быть в диапазоне 2..p-2.\n";
                            }
                            int64_t y = genY(p, g, x);

                            keys.push_back(p);
                            keys.push_back(g);
                            keys.push_back(x);
                            keys.push_back(y);

                            break;
                        }
                        case ParamsElgamalOption::ADD_OUTPUT: {
                            cout << "\n--Протокол Эль-Гамаля -> Расшифрование -> Добавление вывода--\n" <<
                            "Введите путь к файлу, либо нажмите ENTER, чтобы оставить вывод в консоли\n";

                            string path;
                            cin.ignore();
                            getline(cin, path);

                            if (path.empty()) {
                                outputPath = "";
                            } else {
                                namespace fs = filesystem;
                                fs::path filePath(path);
                                fs::path parentDir = filePath.parent_path();

                                if (!parentDir.empty() && !fs::exists(parentDir)) {
                                    cerr << "Ошибка: Такого пути не существует.\n";
                                    break;
                                }

                                if (!fs::exists(filePath)) {
                                    cout << "Создать файл '" << path << "' ? (Y/n): ";
                                    char answer;
                                    cin >> answer;
                                    if (answer != 'n' && answer != 'N') {
                                        outputPath = path;
                                    }
                                } else {
                                    outputPath = path;
                                }
                            }

                            break;
                        }
                        default: {
                            cerr << "Ошибка: такого выбора нет.";
                            break;
                        }
                    } 
                }
                break;
            }
        
            default: {
                cerr << "Ошибка: такого выбора нет.";
                break;
            }
        }
    }
}
