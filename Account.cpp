#include "Account.h"

//TODO is C'tor with no paramteres is necessary?
//Account::Account():accNum(0),password(0),balance(0),readCounter(0)
//{
//    //write mutex
//    if (pthread_mutex_init(&accWriteLock, NULL) != 0)
//    {
//        perror("mutex init fails in account");
//        exit(FAILURE);
//    }
//    //read mutex
//    if (pthread_mutex_init(&accReadLock, NULL) != 0)
//    {
//        perror("mutex init fails in account");
//        exit(FAILURE);
//    }
//}

Account::Account(int account, int password, int initial_amount):accNum(account),password(password),balance(initial_amount),readCounter(0)
{
    //write mutex
    if (pthread_mutex_init(&accWriteLock, NULL) != 0)
    {
        perror("mutex init fails in account");
        exit(FAILURE);
    }
    //read mutex
    if (pthread_mutex_init(&accReadLock, NULL) != 0)
    {
        perror("mutex init fails in account");
        exit(FAILURE);
    }
}

Account::~Account()
{
    //write mutex
    if (pthread_mutex_destroy(&accWriteLock) != 0)
    {
        perror("mutex destroy fails in account");
        exit(FAILURE);
    }
    //read mutex
    if (pthread_mutex_destroy(&accReadLock) != 0)
    {
        perror("mutex destroy fails in account");
        exit(FAILURE);
    }
}

//getters
int Account::getPassword() const
{
    return password;
}

int Account::getAccNum() const
{
    return accNum;
}

int Account::getBalance() const
{
    return balance;
}

int Account::getReadCounter() const
{
    return readCounter;
}



//setters
void Account::setAccBalance(int amount)
{
    balance = balance + amount;
}



//lockers
void Account::accLockWriters() 
{
    lockWrapper(&accWriteLock);
}
void Account:: accUnlockWriters()
{
    unlockWrapper(&accWriteLock);
}


void Account::accLockReaders() 
{
    lockWrapper(&accReadLock);
    readCounter++;
    if (readCounter == ONE_READER) 
         lockWrapper(&accWriteLock);
     unlockWrapper(&accReadLock);
}
void Account::accUnlockReaders()
{
    lockWrapper(&accReadLock);
    readCounter--;
    if (readCounter == ONE_READER-1)
         unlockWrapper(&accWriteLock);
     unlockWrapper(&accReadLock);
}

bool Account::operator<(const Account& rhs)
{
	if (accNum < rhs.accNum) return true;
	return false;
}


void lockWrapper(pthread_mutex_t* l)
{
    if (pthread_mutex_lock(l) != 0) 
    {
		perror("mutex lock fails in account");
		exit(FAILURE);
	}
}

void unlockWrapper(pthread_mutex_t* l)
{
    if (pthread_mutex_unlock(l) != 0) 
    {
		perror("mutex unlock fails in account");
		exit(FAILURE);
	}
}
