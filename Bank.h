#ifndef BANK_H
#define BANK_H

#include <map>
#include <iostream>
#include <cmath>
#include <unistd.h>
#include "Account.h"
#include "Log.h"

#define PRINT_STATE_SLEEP 500000
#define BANK_COMMISION_SLEEP 3
#define MIN_COMMISSION 2
#define MAX_COMMISSION 4

using namespace std;

class Bank
{
private:
    int bankBalance;
    int readCount;
    pthread_mutex_t bankReadLock;
    pthread_mutex_t bankWriteLock;
    map<int, Account*> accList; //structure for all accounts

public:
    Bank();
    ~Bank();
    //getters + setters
    int getBankAmount() const;
    Account& getAcc(int accNumber);
    void setBankBalance(int amount);

    void addNewAcc(int accNumber, Account* newAcc);
    void deleteAcc(int accNumber);

    //lockers
    void bankLockWriter();
    void bankUnlockWriter();
    void bankLockReader();
    void bankUnlockReader();

    //iterators
    map<int, Account*>::iterator getBegin();
    map<int, Account*>::iterator getEnd();
};

//2 bank's operations
void* BankStatePrinting(void* isWorking);
void* chargeCommission(void* isWorking);

static void printLeftTopCorner();

//already declared in main
extern Bank* bank;
extern Log* bankLog;
extern bool isWorking;

#endif