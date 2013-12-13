
INSTALL_PREFIX = /usr
INSTALL_HEADERS = $(INSTALL_PREFIX)/include/hiberlite
INSTALL_LIB = $(INSTALL_PREFIX)/lib

all : libhiberlite.a sqlite3.o tests sample

OBJS=BeanLoader.o BeanUpdater.o ChildKiller.o CppModel.o Database.o ModelExtractor.o Registry.o SQLiteStmt.o Visitor.o shared_res.o sqlite3.o

CXXFLAGS=-std=c++0x -Iinclude/ -Wall -Isqlite-amalgamation
LDFLAGS=-lpthread -ldl

libhiberlite.a : $(OBJS)
	ar -r -s libhiberlite.a $(OBJS)

tests : libhiberlite.a

install :
	mkdir -p $(INSTALL_HEADERS)
	cp include/* $(INSTALL_HEADERS)/
	mkdir -p $(INSTALL_LIB)
	cp libhiberlite.a $(INSTALL_LIB)/

sqlite3.o :
	gcc -c sqlite-amalgamation/sqlite3.c -o sqlite3.o

%.o : src/%.cpp include/*
	g++ -c $(CXXFLAGS) $< -o $@

tests : tests.cpp libhiberlite.a
	g++ $(CXXFLAGS) -L./ tests.cpp -o tests -lhiberlite $(LDFLAGS)

sample : sample.cpp libhiberlite.a
	g++ $(CXXFLAGS) -L./ sample.cpp -o sample -lhiberlite $(LDFLAGS)

clean:
	rm -rf *.o tests sample
