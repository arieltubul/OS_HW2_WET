#ifndef ACCOUNT_H_
#define ACCOUNT_H_

#include <pthread.h>
#include <cstdio>
#include <cstdlib>

//defines
#define ONE_READER 1
#define FAILURE 1
#define SUCCESS 0

class Account
{
private:
    int accNum;
    int password;
    int balance;

    int readCounter;
    pthread_mutex_t accReadLock;
    pthread_mutex_t accWriteLock;

public:

	//default C'tor
    Account();

	//parametrize C'tor
    Account(int account, int password, int initial_amount);

    //D'tor
    ~Account();

    //getters
    int getAccNum() const;
    int getBalance() const;
    int getPassword() const;
    int getReadCounter() const;

    //setters
    void setAccBalance(int amount);


    //we overload this operator in order to solve the case that 2 accounts transact to each other
    //so we need to lock the locks in the same order
    bool operator<(const Account& rhs);


    //shell functions for read and write locks
    void accLockReaders();
    void accUnlockReaders();

    void accLockWriters();
    void accUnlockWriters();
};


//functions for read and write locks
void lockWrapper(pthread_mutex_t* l);
void unlockWrapper(pthread_mutex_t* l);

#endif