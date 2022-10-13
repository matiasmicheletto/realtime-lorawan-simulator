CPP 	= g++
CC 		= gcc
OPT 	= -Wall
TARGET 	= new_runnable
OBJSDIR = ./obj
OBJ 	= main.o random.o uniform.o normal.o custom.o clouds.o 
RM 		= rm -f

.PHONY: all all-before all-after clean clean-custom

all: all-before $(TARGET) all-after

clean: clean-custom
	${RM} $(OBJ) $(TARGET)

$(TARGET): $(OBJ)
	$(CPP) $(patsubst %.o, $(OBJSDIR)/%.o, $(OBJ)) -o $(TARGET)

main.o: src/main.cpp
	$(CPP) $(OPT) -c ./src/main.cpp -o $(OBJSDIR)/main.o 
random.o: src/random/random.cpp	
	$(CPP) $(OPT) -c ./src/random/random.cpp -o $(OBJSDIR)/random.o
uniform.o: src/random/uniform.cpp	
	$(CPP) $(OPT) -c ./src/random/uniform.cpp -o $(OBJSDIR)/uniform.o
normal.o: src/random/normal.cpp	
	$(CPP) $(OPT) -c ./src/random/normal.cpp -o $(OBJSDIR)/normal.o
custom.o: src/random/custom.cpp	
	$(CPP) $(OPT) -c ./src/random/custom.cpp -o $(OBJSDIR)/custom.o
clouds.o: src/random/clouds.cpp	
	$(CPP) $(OPT) -c ./src/random/clouds.cpp -o $(OBJSDIR)/clouds.o