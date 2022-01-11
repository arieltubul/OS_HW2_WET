#ifndef Log_H
#define Log_H

#include <pthread.h>
#include <iostream>
#include <fstream>


using namespace std;
/*
 * this class would describe the log file where we document the operations that were done in bank
 */
class Log
{
private:
    pthread_mutex_t logMutex;
public:
    ofstream logFile;

    //C'tor
    Log()
    {
        logFile.open("log.txt");
        if (pthread_mutex_init(&logMutex, NULL) != 0)
        {
			perror("log mutex initialization");
			exit(1);
        }
    }

    //D'tor
    ~Log()
    {
        logFile.close();
        if (pthread_mutex_destroy(&logMutex) != 0)
        {
			perror("log mutex destroy");
			exit(1);
        }
    }

    void lockLog()
    {
        if (pthread_mutex_lock(&logMutex) != 0)
        {
			perror("log mutex lock");
			exit(1);
        }
    }

    void unlockLogFile()
    {
        if (pthread_mutex_unlock(&logMutex) != 0)
        {
			perror("log mutex unlock");
			exit(1);
        }
    }
};
#endif
