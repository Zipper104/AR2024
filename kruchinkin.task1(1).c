#include <stdio.h>
#include <inttypes.h>

// Определяем тип данных для хранения денежных сумм (64-битное целое число)
typedef int64_t Money;

// Структура для хранения информации об ипотеке
struct Mortgage {
    double rate;           // Процентная ставка по ипотеке (годовая)
    Money credit;          // Сумма кредита
    Money monthly_payment; // Ежемесячный платеж по ипотеке
};

// Структура для хранения информации о банковском вкладе
struct Bank {
    Money deposit;         // Сумма вклада
    double interest_rate;   // Годовая процентная ставка по вкладу
    Money monthly_payment; // Ежемесячный платеж в банк
};

// Структура для хранения информации о финансовом состоянии человека
struct Individual {
    Money salary;         // Ежемесячная зарплата
    Money expenses;       // Ежемесячные расходы
    Money net_worth;       // Общая стоимость имущества (капитал)
    Money apartment;      // Стоимость квартиры
    Money savings;        // Накопления
    struct Mortgage mortgage; // Информация об ипотеке
    struct Bank bank;     // Информация о банковском вкладе
};

// Константы, задающие начальные значения для моделирования
const int SALARY_BASE = 200000;
const int EXPENSES_BASE = 50000;
const int APARTMENT_BASE = 30000;
const int MORTGAGE_CREDIT = 13000000;
const int MORTGAGE_MONTHLY_PAYMENT = 150000;
const int BANK_MONTHLY_PAYMENT = 120000;
const double MORTGAGE_RATE = 0.17;
const double SALARY_RAISE = 0.09;
const double BANK_INTEREST = 0.01;

// Глобальные переменные: экземпляры структуры Individual для Alice и Bob
struct Individual Alice;
struct Individual Bob;

// Инициализация данных Alice
void init_alice(struct Individual* alice) {
    alice->net_worth = 0;
    alice->salary = SALARY_BASE;
    alice->expenses = EXPENSES_BASE;
    alice->savings = 0;
    alice->mortgage.rate = MORTGAGE_RATE;
    alice->mortgage.credit = MORTGAGE_CREDIT;
    alice->mortgage.monthly_payment = MORTGAGE_MONTHLY_PAYMENT;
}

// Инициализация данных Bob
void init_bob(struct Individual* bob) {
    bob->net_worth = 0;
    bob->salary = SALARY_BASE;
    bob->expenses = EXPENSES_BASE;
    bob->apartment = APARTMENT_BASE;
    bob->savings = 0;
    bob->bank.monthly_payment = BANK_MONTHLY_PAYMENT;
    bob->bank.interest_rate = BANK_INTEREST;
    bob->bank.deposit = 0;
}

// Функция для начисления зарплаты
void process_salary(struct Individual* individual, int month) {
    if (month == 1) {
      individual->salary *= (1 + SALARY_RAISE); // Прибавка к зарплате в январе
    }
    individual->net_worth += individual->salary;
}

// Функция для выплат по ипотеке
void process_mortgage(struct Individual* alice) {
    alice->net_worth -= alice->mortgage.monthly_payment;
}

// Функция для банковского вклада Боба
void process_bank_deposit(struct Individual* bob, int month) {
    bob->savings += (bob->salary - bob->bank.monthly_payment - bob->expenses - bob->apartment);
    if (month == 12) {
      bob->bank.deposit = bob->savings * bob->bank.interest_rate; //начисление процентов в конце года
      bob->net_worth += bob->bank.deposit;
    }
     bob->net_worth -= bob->bank.monthly_payment; //ежемесячные выплаты в банк
}

// Функция для изменения стоимости квартиры Боба
void process_apartment(struct Individual* bob, int year) {
    if (year % 10 == 4) {
        bob->apartment += 5000;
    }
}

// Функция для вывода результатов сравнения
void print_results(const struct Individual* alice, const struct Individual* bob) {
    if (alice->net_worth > bob->net_worth) {
        printf("Alice has more money than Bob on %" PRId64 "\n", alice->net_worth - bob->net_worth);
    } else if (bob->net_worth > alice->net_worth) {
        printf("Bob has more money than Alice on %" PRId64 "\n", bob->net_worth - alice->net_worth);
    } else {
      printf("Alice and Bob have the same amount of money\n");
    }

    printf("Alice net worth = %" PRId64 "\n", alice->net_worth);
    printf("Bob net worth = %" PRId64 "\n", bob->net_worth);
}

// Моделирование
void simulation() {
    init_alice(&Alice);
    init_bob(&Bob);

    for (int year = 2024; year <= 2054; ++year) {
        for (int month = 1; month <= 12; ++month) {
            process_salary(&Alice, month);
            process_mortgage(&Alice);
            process_salary(&Bob, month);
            process_bank_deposit(&Bob, month);
            process_apartment(&Bob, year);
        }
    }
}

int main() {
    simulation();
    print_results(&Alice, &Bob);
    return 0;
}