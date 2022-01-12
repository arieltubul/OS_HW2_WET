CC=g++
CFLAGS=-std=c++11 -Wall -Werror -pedantic-errors -DNDEBUG -pthread
CCLINK=$(CC)
OBJS=main.o Account.o Bank.o ATM.o
RM=rm -rf
TARGET=Bank

$(TARGET): $(OBJS)
	$(CCLINK) $(CFLAGS) -o $(TARGET) $(OBJS)

Account.o: Account.cpp Account.h
Bank.o: Bank.cpp Bank.h Account.h Log.h
ATM.o: ATM.cpp ATM.h Bank.h
main.o: main.cpp Account.h Bank.h Log.h

clean:
	$(RM) $(OBJS) $(TARGET)