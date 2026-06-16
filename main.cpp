#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdint>
#include <string>
#include <vector>
#include <random>

#include "math_utils.h"
#include "input_output.h"
#include "elgamal.h"

using namespace std;


random_device rd;
mt19937 gen(rd());

enum class MenuOption {
    EXIT = 0,
    POW_EILER_BINAR,
    EVKLID_NOD,
    EVKLID_INV,
    ELGAMAL,
    MITM,
    FRACTION
};
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


int main() {
    setlocale(LC_ALL, "ru");
    
    bool mainRunning = 1;
    int choice;
    while (mainRunning) {
        printMainMenu();
        if (!(cin >> choice)) {
            cerr << "Ошибка: ведите число.\n";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            continue;
        }
        
        MenuOption option = static_cast<MenuOption>(choice);

        switch (option) {
            case MenuOption::EXIT: {
                // cout << "\nЗавершение работы программы.\n";
                mainRunning = false;
                break;
            }

            case MenuOption::POW_EILER_BINAR: {
                cout << "\n--------Возведение в степень--------\n";
                int64_t a, x, p;
                cout << "\nВведите основание a, степень x, модуль p: ";
                cin >> a >> x >> p;

                bool prime = isPrime(p);
                bool coprime = (a % p != 0);

                if (!prime) {
                    cout << "\np=" << p << " - сложое число, условия теоремы Ферма не выполнены.\n";
                } else if (!coprime) {
                    cout << "\nа=" << a << " делится на  сложое число, условия теоремы Ферма не выполнены.\n";
                }

                if (prime && coprime) {
                    cout << "\nТеорема Ферма:\n";
                    int64_t resFerma = powModFerma(a, x, p);
                    cout << "Итог: " << resFerma << "\n";
                }

                cout << "\nБинарное возведение:\n";
                int64_t resBinary = powModBinary(a, x, p);
                cout << "Итог: " << resBinary << "\n";

                break;
            }

            case MenuOption::EVKLID_NOD: {
                cout << "\n--------Вычисление НОД и коэффициентов u, v--------\n";
                int64_t c, d;
                cout << "Введите c и d: ";
                cin >> c >> d;
                
                vector<int64_t> vecQ, vecR, vecX, vecY;
                evklid(c, d, vecQ, vecR, vecX, vecY);

                printTableEvklid(vecQ, vecR, vecX, vecY);

                cout << "\n" << 
                "c * u + d * v = НОД:\n" <<
                c << " * " << vecX[vecX.size()-2] << " + " << 
                d << " * " << vecY[vecY.size()-2] << " = " << 
                vecR[vecR.size()-2] << "\n";

                break;
            }

            case MenuOption::EVKLID_INV: {
                cout << "\n--------Вычисление обратного элемента--------\n";
                int64_t c, m;
                cout << "Введите c и m: ";
                cin >> c >> m;
    
                vector<int64_t> vecQ, vecR, vecX, vecY;
                evklid(c, m, vecQ, vecR, vecX, vecY);

                printTableEvklid(vecQ, vecR, vecX, vecY);

                int64_t gcd = vecR[vecR.size()-2];
                cout << "\n" <<
                "НОД = " << gcd << (gcd == 1 ? " == " : " != ") << "1\n";

                if (gcd == 1) {
                    cout << "Обратное число d = " << (vecX[vecX.size()-2] % m + m) % m << "\n";
                } else {
                    cout << "Обратного числа не существует.\n";
                }

                break;
            }

            case MenuOption::ELGAMAL: {
                

                vector<uint8_t> inputData;
                vector<int64_t> keys;
                string outputPath;

                bool elgamalRunning = 1;
                bool paramsEditRunning;

                while (elgamalRunning) {

                    inputData.clear();

                    printElgamalMenu();
                    
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
                                printElgamalSettings("Шифрование", inputData, keys, outputPath);

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
                                            keys = generateElgamalKeys(minP, maxP, gen);

                                        } else {
                                            try {
                                                keys = enterElgamalKeys(line);
                                            } catch (const exception& e) {
                                                cerr << "Ошибка: " << e.what() << "\n";
                                            }
                                        }
                                        break;
                                    }
                                    
                                    case ParamsElgamalOption::ADD_OUTPUT: {
                                        cout << "\n--Протокол Эль-Гамаля -> Шифрование -> Добавление вывода--\n" <<
                                        "Введите путь к файлу, либо нажмите ENTER, чтобы оставить вывод из консоли\n";

                                        string path;
                                        cin.ignore();
                                        getline(cin, path);

                                        if (path.empty()) {
                                            outputPath = "";
                                        } else {
                                            if (solveOutputFile(path))
                                                outputPath = path;
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
                                printElgamalSettings("Расфрование", inputData, keys, outputPath);
                                
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

                                        try {
                                            keys = enterElgamalKeys(line);
                                        } catch (const exception& e) {
                                            cerr << "Ошибка: " << e.what() << "\n";
                                        }

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
                                            if (solveOutputFile(path))
                                                outputPath = path;
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
                

                break;
            }

            case MenuOption::MITM: {
                cout << "\n-------- Эмуляция атаки посередине (MitM) --------\n";

                int64_t p = 23, g = 5, xB = 6, yB = genY(p, g, xB);
                cout << "\n1) Боб: p=" << p << " g=" << g << " xB=" << xB << " yB=" << yB << "\n";

                int64_t xM = uniform_int_distribution<int64_t>(2, p-2)(gen);
                int64_t yM = powModPrime(g, xM, p);
                cout << "2) Мэллори перехватывает yB и создаёт подменный yM=" << yM << " (xM=" << xM << ")\n";

                int64_t message = 7;
                cout << "\n3) Алиса шифрует m=" << message << " на yM\n";
                auto kA = genK(p, gen);
                auto cipherA = elgamalEncryptByte(message, p, g, yM, kA);
                cout << "   Алиса оправляет шифр a=" << cipherA.a << " b=" << cipherA.b << "\n";

                cout << "4) Мэллори перехватывает данные и расшифровывает своим закрытым ключом\n";
                auto plainM = elgamalDecryptByte(cipherA, p, xM);
                cout << "   Мэллори расшифровала: " << plainM << "\n";

                auto cipherB = elgamalEncryptByte(plainM, p, g, yB, genK(p, gen));
                cout << "\n5) Мэллори шифрует сообщение по yB и отправляет Бобу\n";

                auto plainB = elgamalDecryptByte(cipherB, p, xB);
                cout << "\n6) Боб расшифровал: " << plainB << "\n";

                cout << "\nAтака прошла успешна. Мэллори незаметно прочитала сообщение.\n";

                break;
            }

            case MenuOption::FRACTION: {
                cout << "\n-------- Эмуляция атаки посередине (MitM) --------\n";
                int64_t a = 1256, b = 847, d = 119;

                cout << "Уравнение: " << a << "x + " << b << "y = " << d << "\n\n";

                vector<int64_t> vecQ, vecR, vecX, vecY;
                evklid(a, b, vecQ, vecR, vecX, vecY);

                cout << "Цепная дробь:\n";

                vector<int64_t> continuedFraction = vecQ;
                cout << a << "/" << b << " = " << continuedFraction[0];
                size_t i = 1;
                for (; i < continuedFraction.size(); ++i) {
                    cout << " + 1/(" << continuedFraction[i];
                } 
                for (size_t j = 1; j < i; ++j) {
                    cout << ")";
                }
                cout << "\n[" << continuedFraction[0];
                for (size_t i = 1; i < continuedFraction.size(); ++i) {
                    cout << ", " << continuedFraction[i];
                }
                cout << "]\n\n";
                

                int64_t nod = vecR[vecR.size() - 2];
                if (d % nod != 0) {
                    cout << "Нет решения, D не делится на НОД=" << nod << "\n";
                    return 0;
                }

                int64_t u = vecX[vecX.size() - 2], v = vecY[vecY.size() - 2];
                int64_t x0 = u * d / nod, y0 = v * d / nod;
                int64_t x = x0 % b, y = y0 % a;

                cout << 
                "Представление НОД:\n" << a << " * " << u << " + " << b << " * " << v << " = " << nod << "\n" <<
                "Частное решение:\n" << a << " * " << x0 << " + " << b << " * " << y0 << " = " << d << "\n" <<
                "Уменьшенное решение:\n" << a << " * " << x << " + " << b << " * " << y << " = " << d << "\n" <<
                "\nРешение в общем виде:\n" << a << " * (" << x << " + " << b << "*t)" << " + " 
                    << b << " * (" << y << " - " << a << "*t)" << " = " << d << "\n";

                break;
            }

            default: {
                cerr << "Ошибка: такого выбора нет.";
                break;
            }
        }
    }
}
