#ifndef ATM_H
#define ATM_H

#include <fstream>
#include <iostream>
#include <string>
#include <cstring>
#include <unistd.h>
#include <pthread.h>
#include "Bank.h"

#define ERROR 1
#define MAX_ARG 4
#define ATM_SLEEP 100000
#define ACTION_SLEEP 1
#define UNINIT -1

using namespace std;

class ATM
{
private:
    int atmNum;
    string atmInput;

public:
    ATM();
    ~ATM();

    string getAtmInput() const;

    void setAtmNum(int ATM_num);
    void setAtmInput(const char* input);

    //operations
    void openNewAcc(int account, int password, int initialAmount);
    void deposit(int account, int password, int amount);
    void withdraw(int account, int password, int amount);
    void getAccBalance(int acc, int password);
    void closeAcc(int account, int password);
    void transaction(int sourceAcc, int password, int destAcc, int amount);

    //general function for operations
    friend void* atmFunc (void* atmInputFile);
};

void* atmFunc (void* atmInputFile);
void parseInput(char* Operation, int args[MAX_ARG], string line);

bool accExist(int acc);
bool checkPassword(int account, int password);

#endif
