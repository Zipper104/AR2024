#include <stdio.h>


typedef long long int Money;


struct Ipoteka
{
    double stavka;
    Money kredit;
    Money mesyac_platezh_alice;
};


struct Vklad
{
    Money vkladik;    
    Money mesyac_platezh_bob;
};


struct Chelovek
{
    Money zarplata;
    Money account;
    Money rashodi;
    Money mashina;
    Money dengi;
    Money kvartira;
    struct Ipoteka Ipoteka;
    struct Vklad Vklad;
};



struct Chelovek ALICE;


struct Chelovek BOB;


void alice_ipoteka()
{
    ALICE.account -= ALICE.Ipoteka.mesyac_platezh_alice;
}


void alice_init()
{
    ALICE.account = 0;
    ALICE.zarplata = 200*1000;
    ALICE.rashodi = 50*1000;

    ALICE.Ipoteka.stavka = 0.17;
    ALICE.Ipoteka.kredit = 13*1000*1000;
    ALICE.Ipoteka.mesyac_platezh_alice = 150*1000;
}


void alice_zarplata(const int month, const int year)
{
    if(month == 1) {
        ALICE.account += ALICE.zarplata;
    }
    if(month==12){
        ALICE.zarplata *= 1.09;
    }
}

void alice_rashodi(int month)
{
if (month == 1){
        ALICE.rashodi *= 1.1;
    }
    ALICE.account -= ALICE.rashodi;
}

void bob_init(){
    BOB.account = 0;
    BOB.zarplata = 200*1000;
    BOB.rashodi = 50*1000;
    BOB.mashina = 10*1000;
    BOB.Vklad.mesyac_platezh_bob = 120*10000;
}


void bob_zarplata(const int month, const int year)
{
    if(month == 12) {
        BOB.account += BOB.zarplata;
    }
    if(month == 1) {
        BOB.zarplata *= 1.09;
    }
    BOB.account += BOB.zarplata;
}


void Bob_vklad(int deposit)
{
    int month = 9;
    int year = 2024;

    if ( !((month == 9) && (year == 2054)) ) {
        deposit = 20;
        BOB.Vklad.vkladik = ((deposit/12)*0.01+1);
        BOB.account -= BOB.Vklad.mesyac_platezh_bob;
        BOB.dengi += (BOB.zarplata -BOB.Vklad.mesyac_platezh_bob - BOB.rashodi - BOB.mashina);
        BOB.account += (BOB.Vklad.vkladik + BOB.dengi);
    }
}


void bob_mashina(int year, int month)
{   
    if (year == 2031 && month == 3 && BOB.account == 15 * 1000 * 1000){
        BOB.account -= 15 * 1000 * 1000;
    }
    else if(year >= 2031 && month > 3){
        BOB.account -= 15 * 1000;
    }
    else if(month == 9){
        BOB.account -= 50 * 1000;
    }
}


void bob_rashodi(int month)
{
    if (month == 1){
        BOB.rashodi *= 1.1;
    }
    BOB.account -= BOB.rashodi;
}

void bob_kvartira(int year)
{
    year = 2024;
    if (year== 2034 || year == 2044 || year == 2054) {
        BOB.kvartira += 1000;
    }
} 


void simulation()
{
    int month = 9;
    int year = 2024;

    while( !((month == 9) && (year == 2054)) ) {
        
        alice_zarplata(month, year);
        alice_ipoteka();
        alice_rashodi(month);
        
        bob_zarplata(month, year);
        bob_kvartira(year);
        bob_mashina(year, month);
        bob_rashodi(month);
        

        month++;
        if(month == 13) {
            month = 1;
            year++;
        }
    }
}


void alice_print()
{
    printf ("ALICE = %lld \n", ALICE.account);
}


void bob_print()
{
    printf ("BOB = %lld \n", BOB.account);
}


int main()
{
    alice_init();

    bob_init();

    simulation();

    alice_print();

    bob_print();
    return 1;
}