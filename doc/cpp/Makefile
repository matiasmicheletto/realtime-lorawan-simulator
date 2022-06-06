CPP = g++
CC = gcc
OPT  = -c -fmessage-length=0 -m64 -fPIC -fexceptions -DNDEBUG -DIL_STD -I/usr/include/c++/5/backward/ -I/usr/local/include -I/opt/ibm/ILOG/CPLEX_Studio201/concert/include/ -I/opt/ibm/ILOG/CPLEX_Studio201/cplex/include/
#OPT  = -c -fmessage-length=0 -m64 -fPIC -fexceptions -DNDEBUG -DIL_STD -I/usr/include/c++/5/backward/ -I/usr/local/include
RES  =

OBJ  = main.o instancia.o ed.o ilp.o heu.o util.o
#OBJ  = main.o instancia.o ed.o heu.o util.o

BIN  = rod
RM = rm -f

.PHONY: all all-before all-after clean clean-custom

all: all-before $(BIN) all-after


clean: clean-custom
	${RM} $(OBJ) $(BIN)

$(BIN): $(OBJ)
#	$(CPP)  -DNDEBUG -DIL_STD $(OBJ) -lm -lpthread -ldl -L /usr/lib/happycoders/ -lnsl -xO4 -xarch=v8plus -o $(BIN)
	$(CPP)  -DNDEBUG -DIL_STD $(OBJ) -L /opt/ibm/ILOG/CPLEX_Studio201/cplex/lib/x86-64_linux/static_pic -lilocplex -lcplex -L /opt/ibm/ILOG/CPLEX_Studio201/concert/lib/x86-64_linux/static_pic -lconcert -lm -lpthread -ldl -L /usr/lib/happycoders/ -lnsl -xO4 -xarch=v8plus -o $(BIN)

main.o: main.cpp
	$(CPP) $(OPT) -c main.cpp -o main.o $(CXXFLAGS)

instancia.o: instancia.cpp
	$(CPP) $(OPT) -c instancia.cpp -o instancia.o $(CXXFLAGS)

ilp.o: ilp.cpp
	$(CPP) $(OPT) -c ilp.cpp -o ilp.o $(CXXFLAGS)

heu.o: heu.cpp
	$(CPP) $(OPT) -c heu.cpp -o heu.o $(CXXFLAGS)

util.o: util.cpp
	$(CPP) $(OPT) -c util.cpp -o util.o $(CXXFLAGS)

