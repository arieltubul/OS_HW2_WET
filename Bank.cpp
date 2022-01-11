#include "Bank.h"


//C'tor
Bank::Bank():bankBalance(0),readCount(0)
{
    //write mutex
    if (pthread_mutex_init(&bankWriteLock, NULL) != 0)
    {
        perror("mutex init fails in account");
        exit(FAILURE);
    }
    //read mutex
    if (pthread_mutex_init(&bankReadLock, NULL) != 0)
    {
        perror("mutex init fails in account");
        exit(FAILURE);
    }
}

//D'tor
Bank::~Bank()
{
    //write mutex
    if (pthread_mutex_destroy(&bankWriteLock) != 0) 
    {
        perror("mutex destroy fails in account");
        exit(FAILURE);
    }
    //read mutex
    if (pthread_mutex_destroy(&bankReadLock) != 0)
    {
        perror("mutex destroy fails in account");
        exit(FAILURE);
    }

    map<int, Account *>::iterator it = accList.begin();
    for (; it != accList.end(); it++)
    {
        delete it->second;
        //TODO : should we add here erase line in deleteAcc? 
    }
}

int Bank::getBankAmount() const
{
    return bankBalance;
}

Account& Bank::getAcc(int accNumber)
{
    return *accList[accNumber];
}

void Bank::setBankBalance(int amount)
{
    bankBalance += amount;
}

void Bank::addNewAcc(int accNumber, Account *new_acc)
{
    accList[accNumber] = new_acc;
}

void Bank::deleteAcc(int accNumber)
{
    map<int, Account*>::iterator it = accList.find(accNumber);
    delete it->second;//operating the account D'tor
    accList.erase(it);
}

void Bank::bankLockWriter() 
{
    lockWrapper(&bankWriteLock);
}

void Bank::bankUnlockWriter()
{
    unlockWrapper(&bankWriteLock);
}

void Bank::bankLockReader()
{
    lockWrapper(&bankReadLock);
    readCount++;
    if (readCount == ONE_READER) 
        lockWrapper(&bankWriteLock);
    unlockWrapper(&bankReadLock);
}

void Bank::bankUnlockReader() 
{
    lockWrapper(&bankReadLock);
    readCount--;
    if (readCount == ONE_READER-1) 
        unlockWrapper(&bankWriteLock);
    unlockWrapper(&bankReadLock);   
}

map<int, Account*>::iterator Bank::getBegin()
{
    return accList.begin();
}

map<int, Account*>::iterator Bank::getEnd()
{
    return accList.end();
}

void* BankStatePrinting(void* Working)
{
    //as long as bank is working
	while (*((bool*)Working))
    {
        usleep(PRINT_STATE_SLEEP); // half a second
        printLeftTopCorner();
        cout << "Current Bank Status" << endl;
        
        bank->bankLockReader();
        map<int, Account*>::iterator it = bank->getBegin();
        for (; it != bank->getEnd(); it++)
        {
            it->second->accLockReaders();
            cout << "Account " << it->second->getAccNum() << ": Balance - ";
	    	cout << it->second->getBalance() << " $ , Account Password - " << it->second->getPassword() << endl;
            it->second->accUnlockReaders();
        }
		cout << "The Bank has " << bank->getBankAmount() << " $" << endl;
        bank->bankUnlockReader();
    }
    pthread_exit(NULL);
}

void* chargeCommission(void* Working)
{
    double commissionPercentage = 0.0;
    double chargeAmount = 0.0;
	while (*((bool*)Working))
    {
        sleep(BANK_COMMISION_SLEEP);
		commissionPercentage = (rand() % (MIN_COMMISSION + 1)) + (double)(MAX_COMMISSION - MIN_COMMISSION);

        bank->bankLockReader();
        map<int, Account*>::iterator it = bank->getBegin();
        for (; it != bank->getEnd(); it++)
        {
            it->second->accLockWriters();
            chargeAmount = round(commissionPercentage * 0.01 * it->second->getBalance());
            it->second->setAccBalance(-chargeAmount); //add negative amount to balance
            // FIXME maybe write unlock and read lock?
            bankLog->lockLog();
            bankLog->logFile << "Bank: commissions of " << commissionPercentage
			<< " % were charged, the bank gained " << chargeAmount << " $ from account " << it->second->getAccNum() << endl;
            bankLog->unlockLogFile();
            it->second->accUnlockWriters();
            bank->setBankBalance(chargeAmount);
        }
        bank->bankUnlockReader();
    }
    pthread_exit(NULL);
}

//clearing screen and starts to print from top left corner
static void printLeftTopCorner()
{
    printf("\033[2J");
    printf("\033[1;1H");
}
