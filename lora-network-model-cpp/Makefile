CPP 	= g++
CC 		= gcc
OPT 	= -Wall
TARGET 	= runnable
LIBSDIR = ./src/libs
OBJSDIR = ./obj
OBJ 	= main.o random.o uniform.o normal.o custom.o clouds.o node.o enddevice.o gateway.o networkoptimizer.o
RM 		= rm -f

.PHONY: all all-before all-after clean clean-custom

all: all-before $(TARGET) all-after

clean: clean-custom
	${RM} $(OBJ) $(TARGET)

$(TARGET): $(OBJ)
	$(CPP) $(patsubst %.o, $(OBJSDIR)/%.o, $(OBJ)) -o $(TARGET)

main.o: src/main.cpp
	$(CPP) $(OPT) -c ./src/main.cpp -o $(OBJSDIR)/main.o 
random.o: src/libs/random/random.cpp	
	$(CPP) $(OPT) -c $(LIBSDIR)/random/random.cpp -o $(OBJSDIR)/random.o
uniform.o: src/libs/random/uniform.cpp	
	$(CPP) $(OPT) -c $(LIBSDIR)/random/uniform.cpp -o $(OBJSDIR)/uniform.o
normal.o: src/libs/random/normal.cpp	
	$(CPP) $(OPT) -c $(LIBSDIR)/random/normal.cpp -o $(OBJSDIR)/normal.o
custom.o: src/libs/random/custom.cpp	
	$(CPP) $(OPT) -c $(LIBSDIR)/random/custom.cpp -o $(OBJSDIR)/custom.o
clouds.o: src/libs/random/clouds.cpp	
	$(CPP) $(OPT) -c $(LIBSDIR)/random/clouds.cpp -o $(OBJSDIR)/clouds.o
node.o: src/libs/network/node.cpp	
	$(CPP) $(OPT) -c $(LIBSDIR)/network/node.cpp -o $(OBJSDIR)/node.o
enddevice.o: src/libs/network/enddevice.cpp	
	$(CPP) $(OPT) -c $(LIBSDIR)/network/enddevice.cpp -o $(OBJSDIR)/enddevice.o
gateway.o: src/libs/network/gateway.cpp	
	$(CPP) $(OPT) -c $(LIBSDIR)/network/gateway.cpp -o $(OBJSDIR)/gateway.o
networkoptimizer.o: src/libs/optimizer/networkoptimizer.cpp	
	$(CPP) $(OPT) -c $(LIBSDIR)/optimizer/networkoptimizer.cpp -o $(OBJSDIR)/networkoptimizer.o
