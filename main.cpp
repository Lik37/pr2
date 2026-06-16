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
#include "menu_elgamal.h"

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

void printMenu() {
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

int main() {
    setlocale(LC_ALL, "ru");
    
    bool mainRunning = 1;
    int choice;
    while (mainRunning) {
        printMenu();
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
                menuElgamal();
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
                int64_t a = 1256, b = 847, d = 119;

                cout << "Уравнение: " << a << "x + " << b << "y = " << d << endl << endl;

                vector<int64_t> vecQ, vecR, vecX, vecY;
                evklid(a, b, vecQ, vecR, vecX, vecY);

                cout << "Цепная дробь:" << endl;

                vector<int64_t> continuedFraction = vecQ;
                cout << a << "/" << b << " = " << continuedFraction[0];
                size_t i = 1;
                for (; i < continuedFraction.size(); ++i) {
                    cout << " + 1/(" << continuedFraction[i];
                } 
                for (size_t j = 1; j < i; ++j) {
                    cout << ")";
                }
                cout << endl;
                cout << "[" << continuedFraction[0];
                for (size_t i = 1; i < continuedFraction.size(); ++i) {
                    cout << ", " << continuedFraction[i];
                }
                cout << "]" << endl << endl;

                int64_t nod = vecR[vecR.size() - 2];
                if (d % nod != 0) {
                    cout << "Нет решения, D не делится на НОД=" << nod << endl;
                    return 0;
                }

                int64_t u = vecX[vecX.size() - 2], v = vecY[vecY.size() - 2];
                int64_t x0 = u * d / nod, y0 = v * d / nod;
                int64_t x = x0 % b, y = y0 % a;

                cout << "Представление НОД:\n" << a << " * " << u << " + " << b << " * " << v << " = " << nod << endl;
                cout << "Частное решение:\n" << a << " * " << x0 << " + " << b << " * " << y0 << " = " << d << endl;
                cout << "Уменьшенное решение:\n" << a << " * " << x << " + " << b << " * " << y << " = " << d << endl;
                cout << endl;
                cout << "Решение в общем виде:\n" << a << " * (" << x << " + " << b << "*t)" << " + " 
                    << b << " * (" << y << " - " << a << "*t)" << " = " << d << endl;

                break;
            }

            default: {
                cerr << "Ошибка: такого выбора нет.";
                break;
            }
        }
    }
}
