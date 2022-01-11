//includes
#include <ostream>
#include <unistd.h>
#include <pthread.h>
#include <cstdlib>
#include <cstdio>
#include <iostream>
#include "Bank.h"
#include "Log.h"
#include "ATM.h"

//defines


using namespace std;

Log* bankLog;
Bank* bank;
bool isWorking;

//managing the bank operations through activating threads for all accounts and bank operations
int main(int argc, char const *argv[])
{
    int atmNum = atoi(argv[1]);
    if((argc <= 1) || (argc - 2 != atmNum) || atmNum <= 0)
    {
        cout << "illegal arguments" << endl;
        exit(FAILURE);
    }

    bankLog = new Log();
    bank = new Bank();
    isWorking = true; //telling us if ATMS still work


	// allocate array for ID of threads and array for ATMs
    pthread_t* idArr = new pthread_t[atmNum + 2];//+2 for threads of commission and printing
    ATM* atmArr = new ATM[atmNum];

    for (int i=0; i < atmNum; i++)
    {
        atmArr[i].setAtmInput(argv[i + 2]);
        atmArr[i].setAtmNum(i + 1);//atms id must begin with positive value
        if(pthread_create(&idArr[i], NULL, &atmFunc, (void*)&atmArr[i]) != 0)
        {
            perror("pthread create fails in ATMs");
            if(idArr)
                delete [] idArr; //if we managed to create any other thread, so we need to delete the whole array
			if(atmArr)
                delete [] atmArr;
			exit(ERROR);
        }
    }

    //commission thread of bank
	if(pthread_create(&idArr[atmNum], NULL, &chargeCommission, (void*)(&isWorking)) != 0)
	{
        perror("pthread create fails in commission");
        if(idArr)
            delete [] idArr;
		if(atmArr)
            delete [] atmArr;
		exit(ERROR);
	}

	//printing thread of bank
	if(pthread_create(&idArr[atmNum + 1], NULL, &BankStatePrinting, (void*)(&isWorking)) != 0)
	{
        perror("pthread create fails in printing for Bank");
        if(idArr)
            delete [] idArr;
		if(atmArr)
            delete [] atmArr;
		exit(ERROR);
	}

    // wait for all the ATM threads to call pthread_exit(NULL);
	for (int j = 0; j < atmNum; j++)
	{
		if(pthread_join(idArr[j], NULL) != 0)
        {
            perror("pthread join fails in ATMs");
            if(idArr)
                delete [] idArr;
		    if(atmArr)
                delete [] atmArr;
		    exit(ERROR);
        }
	}

    // close Bank
	// we change isWorking to false to finish the bank operations
	isWorking = false;
    //printing
    if(pthread_join(idArr[atmNum + 1], NULL) != 0)
    {
        perror("pthread join fails for printing");
        if(idArr)
            delete [] idArr;
        if(atmArr)
            delete [] atmArr;
        exit(ERROR);
    }
    //commission
	if(pthread_join(idArr[atmNum], NULL) != 0)
    {
        perror("pthread join fails for commission");
        if(idArr)
            delete [] idArr;
        if(atmArr)
            delete [] atmArr;
        exit(ERROR);
    }


//free allocations
    delete [] atmArr;
    delete [] idArr;
    delete bankLog;
    delete bank;
    return SUCCESS;
}
