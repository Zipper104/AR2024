#include <stdio.h>
#include <math.h>

typedef long long int Money;

#define APARTMENT_PRICE 10000000 // Цена квартиры
#define INITIAL_PAYMENT 1000000 // Первоначальный взнос
#define BOB_INTEREST_RATE 0.227  // Годовая процентная ставка для Боба
#define ALICE_INTEREST_RATE 0.20  // Годовая процентная ставка для Элисы
#define LOAN_TERM_YEARS 30         // Срок ипотеки Боба
#define ANNUAL_SALARY_INCREASE 0.09 // Ежегодное повышение зарплаты
#define INFLATION_RATE 0.05   // Ежегодная инфляция

struct Mortgage {
    double annual_rate;
    Money principal;
    Money monthly_payment;
    Money remaining_loan;
};

struct Bank {
    double interest_rate;
    Money savings;
};

struct Individual {
    Money salary;
    Money expenses;
    Money capital;
    struct Mortgage mortgage;
    struct Bank bank;
    Money apartment_price;
};

struct Individual Alice;
struct Individual Bob;

// Функция для расчета ежемесячного платежа по ипотеке (формула аннуитета)
double calculate_monthly_payment(double principal, double annual_rate, int years) {
    double monthly_rate = annual_rate / 12.0;
    int total_payments = years * 12;
    return (principal * monthly_rate) / (1 - pow(1 + monthly_rate, -total_payments));
}
void initialize_individuals() {
    // Инициализация Алисы
    Alice.salary = 200000;
    Alice.expenses = 50000;
    Alice.capital = 0;
    Alice.apartment_price = APARTMENT_PRICE;
    Alice.mortgage.principal = 0;
    Alice.bank.interest_rate = ALICE_INTEREST_RATE;
    Alice.bank.savings = 0;


    // Инициализация Боба
    Bob.salary = 200000;
    Bob.expenses = 50000;
    Bob.capital = 0;
    Bob.apartment_price = APARTMENT_PRICE;
     Bob.mortgage.annual_rate = BOB_INTEREST_RATE;
    Bob.mortgage.principal = APARTMENT_PRICE - INITIAL_PAYMENT;
    Bob.mortgage.monthly_payment = calculate_monthly_payment(Bob.mortgage.principal, Bob.mortgage.annual_rate, LOAN_TERM_YEARS);
    Bob.mortgage.remaining_loan = Bob.mortgage.principal;
     Bob.bank.interest_rate = 0;
     Bob.bank.savings = 0;

}

void simulate_month(int month, int year) {
    // --- Алиса ---
    // Увеличиваем капитал на зарплату за месяц и вычитаем расходы
    Alice.capital += Alice.salary - Alice.expenses;

    // Рассчитываем процент по сбережениям и добавляем его
    Alice.bank.savings += (Alice.salary / 2.0);
    Alice.bank.savings *= (1 + Alice.bank.interest_rate / 12.0);



    // --- Боб ---
    // Увеличиваем капитал на зарплату за месяц и вычитаем расходы
    Bob.capital += Bob.salary - Bob.expenses;


    // Выплата ипотеки
    double monthly_interest = (Bob.mortgage.remaining_loan * Bob.mortgage.annual_rate / 12.0);
    double monthly_principal_payment = Bob.mortgage.monthly_payment - monthly_interest;

    if (Bob.mortgage.remaining_loan > 0) {
        Bob.mortgage.remaining_loan -= monthly_principal_payment;
        if (Bob.mortgage.remaining_loan < 0) {
             Bob.mortgage.remaining_loan = 0; // Защита от ухода в минус
        }
         Bob.capital -= Bob.mortgage.monthly_payment; // Добавили списание платежа
    }

    // Ежегодное повышение зарплаты
    if (month == 12) {
        Alice.salary *= (1 + ANNUAL_SALARY_INCREASE);
        Bob.salary *= (1 + ANNUAL_SALARY_INCREASE);
    }

   // Ежегодная инфляция
    if (month == 12) {
        Alice.apartment_price *= (1 + INFLATION_RATE);
        Bob.apartment_price *= (1+ INFLATION_RATE);

    }
}

void print_status(int month, int year) {
    printf("Месяц %d, Год %d\n", month, year);
    printf("  Алиса: Капитал = %lld, Сбережения = %lld,  Стоимость квартиры= %lld\n", Alice.capital, Alice.bank.savings, Alice.apartment_price);
    printf("  Боб: Капитал = %lld, Остаток по ипотеке = %lld, Стоимость квартиры = %lld\n", Bob.capital, Bob.mortgage.remaining_loan, Bob.apartment_price);
     printf("--------------------------\n");
}
int main() {
    initialize_individuals();

    int month = 1;
    int year = 2024;

    while (year < 2054 ||  Bob.mortgage.remaining_loan > 0 || Alice.bank.savings < Alice.apartment_price)
     {
        simulate_month(month, year);
        print_status(month, year);

        month++;
        if (month > 12) {
            month = 1;
            year++;
        }
    }

     if (Bob.mortgage.remaining_loan == 0)
         printf("\n Боб выплатил ипотеку!\n");
    if (Alice.bank.savings >= Alice.apartment_price )
         printf("\n Алиса накопила на квартиру!\n");

    return 0;
}