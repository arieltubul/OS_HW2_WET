#include "ATM.h"
//C'tor
ATM::ATM()
    :atmNum(UNINIT) {}

//D'tor
ATM::~ATM() {}

//getters:
string ATM::getAtmInput() const
{
    return atmInput;
}
//setters:
void ATM::setAtmNum(int ATM_num)
{
    atmNum = ATM_num;
}
void ATM::setAtmInput(const char* input)
{
   atmInput = string(input);
}

//general function for operations
void* atmFunc (void* atmInputFile)
{
    ATM* curr_ATM = (ATM*)atmInputFile;
    ifstream input_file(curr_ATM->getAtmInput().c_str());
    // check if the file is already open
	if (input_file.is_open() == false)
	{
		cerr << "input file cannot be opened" << endl;
		exit(ERROR);
	}
    
    char op;
	int args[MAX_ARG];
	string line ;

    while (getline(input_file, line)) {
        if (!line.length())
            continue;

        parseInput(&op, args, line);
        usleep(ATM_SLEEP);
        //determining what operation we have to do
        switch (op)
        {
        case 'O':
            curr_ATM->openNewAcc(args[0], args[1], args[2]);
            break;
        case 'D':
            curr_ATM->deposit(args[0], args[1], args[2]);
            break;
        case 'W':
            curr_ATM->withdraw(args[0], args[1], args[2]);
            break;
        case 'B':
            curr_ATM->getAccBalance(args[0], args[1]);
            break;
        case 'Q':
            curr_ATM->closeAcc(args[0], args[1]);
            break;
        case 'T':
            curr_ATM->transaction(args[0], args[1], args[2], args[3]);
            break;
        default:
            cerr << "illegal operation" << endl;
            exit(ERROR);
        }
    }
    pthread_exit(NULL); //terminates the thread after finishing its file lines
}

void ATM::openNewAcc(int account, int password, int initial_amount)
{
    bank->bankLockWriter();
    sleep(ACTION_SLEEP);
    if(accExist(account))
    {
        bankLog->lockLog();
        bankLog->logFile << "Error " << atmNum << ": Your transaction failed - account with the same id exists " << endl;
        bankLog->unlockLogFile();
    }

    // account does not exist so we open a new one
    else
    {
        Account *new_acc = new Account(account, password, initial_amount);
        bank->addNewAcc(account, new_acc);
        bankLog->lockLog();
        bankLog->logFile << atmNum << ": New account id is " << account << "  with password  "<< password
        << " and initial balance " << initial_amount << endl;
        bankLog->unlockLogFile();
    }
    bank->bankUnlockWriter();
}

void ATM::closeAcc(int account, int password)
{
    bank->bankLockWriter();
    sleep(ACTION_SLEEP);
    // check if the account does not exist
    if(!accExist(account))
    {
        bankLog->lockLog();
        bankLog->logFile << "Error " << atmNum << ": Your transaction failed - account id "<< account
        <<" does not exists" << endl;
        bankLog->unlockLogFile();
    }
        //account exist
    else {
        if (checkPassword(account, password))
        {
            Account &curr = bank->getAcc(account);
            //FIXME what happens when i delete this account semaphore ?
            // maybe hold the write semaphore ?
            // the problem is that accLockWriters is locked when deleting the account and destroying its
            // semaphore so behavior is unexpected
            // SOLUTION:
            // because we locked the bank writer so nobody can operate any operation and we can unlock the acccount mutex
            //curr.accLockReaders();
            bankLog->lockLog();
            bankLog->logFile << atmNum << ": Account " << account << " is now closed. Balance was "
            << curr.getBalance() << endl;
            bankLog->unlockLogFile();
            bank->deleteAcc(account);
        }

        else//wrong password
        {
            bankLog->lockLog();
            bankLog->logFile << "Error " << atmNum << ": Your transaction failed - password for account id "
            << account <<" is incorrect" << endl;
            bankLog->unlockLogFile();
        }
    }
    bank->bankUnlockWriter();
}

void ATM::deposit(int account, int password, int amount)
{
    bank->bankLockReader();
    sleep(ACTION_SLEEP);
    // check if the account does not exist
    if(!accExist(account))
    {
        bankLog->lockLog();
		bankLog->logFile << "Error " << atmNum << ": Your transaction failed - account id "<< account
        <<" does not exists" << endl;
		bankLog->unlockLogFile();
    }

    else
    {
        if (checkPassword(account, password))
        {
            Account &curr = bank->getAcc(account);
            curr.accLockWriters();
            curr.setAccBalance(amount);
            bankLog->lockLog();
		    bankLog->logFile << atmNum << ": Account " << account << " new balance is "
            << curr.getBalance() << " after " << amount <<" $ was deposited " << endl;
		    bankLog->unlockLogFile();
            curr.accUnlockWriters();
        }

        else //wrong password
        {
            bankLog->lockLog();
		    bankLog->logFile << "Error " << atmNum << ": Your transaction failed - password for account id "
            << account <<" is incorrect" << endl;
		    bankLog->unlockLogFile();
        }
    }
    bank->bankUnlockReader();
}

void ATM::withdraw(int account, int password, int amount)
{
    bank->bankLockReader();
    sleep(ACTION_SLEEP);
    if(!accExist(account))
    {
        bankLog->lockLog();
		bankLog->logFile << "Error " << atmNum << ": Your transaction failed - account id "<< account <<" does not exists" << endl;
		bankLog->unlockLogFile();
    }

    else
    {
        if (checkPassword(account, password))
        {
            Account &curr = bank->getAcc(account);
            curr.accLockWriters();
            //check if the account balance > amount to withdraw
            if (curr.getBalance() >= amount)
            {
                curr.setAccBalance(-amount);
                bankLog->lockLog();
		        bankLog->logFile << atmNum << ": Account " << account << " new balance is "
                << curr.getBalance() << " after " << amount <<" $ was withdrew " << endl;
		        bankLog->unlockLogFile();
            }
            else
            {
                bankLog->lockLog();
		        bankLog->logFile << "Error " << atmNum << ": Your transaction failed - account id "
                << account <<" balance is lower than " << amount << endl;
		        bankLog->unlockLogFile();
            }
            curr.accUnlockWriters();
        }
        else
        {
            //wrong password
            bankLog->lockLog();
		    bankLog->logFile << "Error " << atmNum << ": Your transaction failed - password for account id "
            << account <<" is incorrect" << endl;
		    bankLog->unlockLogFile();
        }
    }
    bank->bankUnlockReader();
}

void ATM::getAccBalance(int acc, int password)
{
    bank->bankLockReader();
    sleep(ACTION_SLEEP);
    if(!accExist(acc))
    {
        bankLog->lockLog();
		bankLog->logFile << "Error " << atmNum << ": Your transaction failed - account id "<< acc <<" does not exists" << endl;
		bankLog->unlockLogFile();
    }
    else
    {
        if(checkPassword(acc, password))
        {
            Account &curr = bank->getAcc(acc);
            curr.accLockWriters();
            bankLog->lockLog();
		    bankLog->logFile << atmNum << ": Account " << acc << " balance is " << curr.getBalance() << endl;
		    bankLog->unlockLogFile();
            curr.accUnlockWriters();
        }
        else
        {
            bankLog->lockLog();
		    bankLog->logFile << "Error " << atmNum << ": Your transaction failed - password for account id "
            << acc <<" is incorrect" << endl;
		    bankLog->unlockLogFile();
        }
    }
    bank->bankUnlockReader();
}

void ATM::transaction(int source_acc, int password, int dest_acc, int amount)
{
    bank->bankLockReader();
    sleep(ACTION_SLEEP);
    // FIXME according to notes in tablet
    if(!accExist(source_acc) || !accExist(dest_acc))
    {
        bankLog->lockLog();
		bankLog->logFile << "error " << atmNum << ": your transaction failed - account id "<< source_acc <<" does not exists" << endl;
		bankLog->unlockLogFile();
    }

    else
    {
        if(checkPassword(source_acc, password))
        {
            Account &from = bank->getAcc(source_acc);
            Account &to = bank->getAcc(dest_acc);

            //we will lock by order to prevent deadlock, meaning that if 2 accounts transfer simultaneously to each other
            //so we will lock their write_locks by order so that we won't get deadlock
            if(source_acc < dest_acc)
            {
                from.accLockWriters();
                to.accLockWriters();
            }

            else
            {
                to.accLockWriters();
                from.accLockWriters();
            }

            if (from.getBalance() >= amount)
            {
                from.setAccBalance(-amount);
                to.setAccBalance(amount);
                bankLog->lockLog();
		        bankLog->logFile << atmNum << ": transfer " << amount << " from account "<< source_acc << " to account "
                << dest_acc << " new account balance is "<< from.getBalance() << " new target account balance is "
                << to.getBalance()<< endl;
		        bankLog->unlockLogFile();
            }
            else
            {
                bankLog->lockLog();
		        bankLog->logFile << "error " << atmNum << ": your transaction failed - account id "
                << source_acc <<" balance is lower than " << amount << endl;
		        bankLog->unlockLogFile();
            }

            // for unlock the order doesn't matter
            from.accUnlockWriters();
            to.accUnlockWriters();
        }

        else //wrong password
        {
            bankLog->lockLog();
		    bankLog->logFile << "error " << atmNum << ": your transaction failed - password for account id "
            << source_acc <<" is incorrect" << endl;
		    bankLog->unlockLogFile();
        }
    }
    bank->bankUnlockReader();
}
//TODO : LOOK IN WAHTSAPP
void parseInput(char* Operation, int args[MAX_ARG], string line)
{
    char* cmd = NULL, *token = NULL;
//	const char* delimiters = (char*)" ";
	int i = 0;

    cmd = new char[strlen(line.c_str()) + 1];
    strcpy(cmd,line.c_str());
    
    *Operation = *strtok(cmd, " ");

	token = strtok(NULL, " ");
    while (token) {
        args[i] = atoi(token);
        token = strtok(NULL, " ");
        i++;
    }
    delete[] cmd;
}

bool accExist(int account)
{
    map<int, Account*>::iterator it = bank->getBegin();
    for(; it != bank->getEnd(); it++)
        if (it->second->getAccNum() == account)
            return true;
    return false;
}

bool checkPassword(int account, int password)
{
    Account &acc = bank->getAcc(account);
    if (acc.getPassword() == password)
        return true;
    return false;
}