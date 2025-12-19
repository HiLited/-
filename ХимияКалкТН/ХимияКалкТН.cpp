#include <iostream>
#include <limits>
#include <cmath>
using namespace std;

// === Химические константы ===
const double R = 8.314;       // Дж/(моль·К)
const double Kw = 1e-14;       // Только при 25°C!
const double NA = 6.022e23;    // 1/моль

// --- Пределы ---
const double CONC_MIN = 1e-12;     // минимальная реалистичная концентрация
const double CONC_MAX = 1e2;       // максимум 100 М — больше физически не бывает

// === Универсальная строгая функция чтения числа ===
double read_double_strict(const string& prompt, double minVal, double maxVal) {
    double value;
    while (true) {
        cout << prompt;
        cin >> value;

        if (!cin.good()) {
            cout << "Ошибка: нужно ввести ЧИСЛО. Повторите.\n";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            continue;
        }

        if (value < minVal || value > maxVal) {
            cout << "Ошибка: значение должно быть в диапазоне ["
                << minVal << " ; " << maxVal << "]\n";
            continue;
        }

        return value;
    }
}

// === Расчётные функции ===
double calc_pH(double H) {
    return -log10(H);
}
double calc_pOH(double OH) {
    return -log10(OH);
}
double calc_H_from_pH(double pH) {
    return pow(10, -pH);
}

// K = products / reactants
double calc_equilibrium(double products, double reactants) {
    if (reactants <= 0) return INFINITY;
    return products / reactants;
}

// ΔG° = −RT ln(K)
double calc_Gibbs(double K, double T) {
    return -R * T * log(K);
}

double calc_Ka(double H, double A, double HA) {
    if (HA <= 0) return INFINITY;
    return (H * A) / HA;
}

double calc_Kb(double BH, double OH, double B) {
    if (B <= 0) return INFINITY;
    return (BH * OH) / B;
}

// Константы гидролиза
double calc_Kh_from_Ka(double Ka) { return Kw / Ka; }
double calc_Kh_from_Kb(double Kb) { return Kw / Kb; }

int main() {
    setlocale(LC_ALL, "Russian");

    cout << "ВНИМАНИЕ: Kw = 1e-14 справедливо только при 25°C.\n";
    cout << "При другой температуре результаты гидролиза и pH могут быть неточны.\n";

    int choice;
    while (true) {
        cout << "\n===== Химический калькулятор =====\n";
        cout << "1. Константы\n";
        cout << "2. pH по [H+]\n";
        cout << "3. pOH по [OH-]\n";
        cout << "4. [H+] по pH\n";
        cout << "5. Константа равновесия K\n";
        cout << "6. ΔG° = -RT ln(K)\n";
        cout << "7. Ka из равновесных концентраций\n";
        cout << "8. Kb из равновесных концентраций\n";
        cout << "9. Kh через Ka\n";
        cout << "10. Kh через Kb\n";
        cout << "0. Выход\n";
        cout << "Выберите пункт: ";
        cin >> choice;

        if (!cin.good()) {
            cout << "Ошибка: вводите только числа.\n";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            continue;
        }

        if (choice == 0) break;

        if (choice == 1) {
            cout << "\n--- Константы ---\n";
            cout << "R  = " << R << " Дж/(моль·К)\n";
            cout << "Na = " << NA << " 1/моль\n";
            cout << "Kw = " << Kw << " при 25°C\n";
        }

        // pH
        else if (choice == 2) {
            double H = read_double_strict("[H+] (М): ", CONC_MIN, CONC_MAX);
            cout << "pH = " << calc_pH(H) << "\n";
        }

        else if (choice == 3) {
            double OH = read_double_strict("[OH-] (М): ", CONC_MIN, CONC_MAX);
            cout << "pOH = " << calc_pOH(OH) << "\n";
        }

        // [H+] из pH
        else if (choice == 4) {
            double pH = read_double_strict("pH: ", -5.0, 20.0);
            cout << "[H+] = " << calc_H_from_pH(pH) << " М\n";
        }

        // K
        else if (choice == 5) {
            double P = read_double_strict("Произведение концентраций продуктов (>=0): ", 0.0, 1e308);
            double Rct = read_double_strict("Произведение концентраций реагентов (>0): ", 1e-308, 1e308);
            cout << "K = " << calc_equilibrium(P, Rct) << "\n";
        }

        // ΔG°
        else if (choice == 6) {
            double K = read_double_strict("Введите K (>0): ", numeric_limits<double>::min(), 1e308);
            double T = read_double_strict("Температура (K): ", 1.0, 5000.0);
            cout << "ΔG° = " << calc_Gibbs(K, T) << " Дж/моль\n";
            cout << "(Примечание: K должен быть безразмерным; использовано приближение.)\n";
        }

        // Ka
        else if (choice == 7) {
            cout << "Введите РАВНОВЕСНЫЕ концентрации!\n";
            double H = read_double_strict("[H+]: ", CONC_MIN, CONC_MAX);
            double A = read_double_strict("[A-]: ", CONC_MIN, CONC_MAX);
            double HA = read_double_strict("[HA]: ", CONC_MIN, CONC_MAX);
            cout << "Ka = " << calc_Ka(H, A, HA) << "\n";
        }

        // Kb
        else if (choice == 8) {
            cout << "Введите РАВНОВЕСНЫЕ концентрации!\n";
            double BH = read_double_strict("[BH+]: ", CONC_MIN, CONC_MAX);
            double OH = read_double_strict("[OH-]: ", CONC_MIN, CONC_MAX);
            double B = read_double_strict("[B]: ", CONC_MIN, CONC_MAX);

            // проверка согласованности OH с Kw
            double possible_H = Kw / OH;
            double rel_err = fabs(possible_H * OH - Kw) / Kw;
            if (rel_err > 1e-3) {
                cout << "Предупреждение: [H+]*[OH-] не равно Kw — данные не соответствуют 25°C.\n";
            }

            cout << "Kb = " << calc_Kb(BH, OH, B) << "\n";
        }

        else if (choice == 9) {
            double Ka = read_double_strict("Ka (>0): ", numeric_limits<double>::min(), 1e308);
            cout << "Kh = Kw / Ka = " << calc_Kh_from_Ka(Ka) << "\n";
        }

        else if (choice == 10) {
            double Kb = read_double_strict("Kb (>0): ", numeric_limits<double>::min(), 1e308);
            cout << "Kh = Kw / Kb = " << calc_Kh_from_Kb(Kb) << "\n";
        }

        else {
            cout << "Нет такого пункта меню.\n";
        }
    }

    return 0;
}
