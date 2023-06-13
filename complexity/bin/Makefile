CPP 	= g++
CC 		= gcc
OPT 	= -Wall
TARGET 	= gpprs
LIBSDIR = ./src/lib
OBJSDIR = ./obj
OBJ 	= main.o util.o random.o uniform.o normal.o custom.o clouds.o instance.o objective.o
RM 		= rm -f

.PHONY: all all-before all-after clean clean-custom

all: all-before $(TARGET) all-after

clean: clean-custom
	${RM} $(OBJ) $(TARGET)

$(TARGET): $(OBJ)
	$(CPP) $(patsubst %.o, $(OBJSDIR)/%.o, $(OBJ)) -o $(TARGET) -lpthread

main.o: src/main.cpp
	$(CPP) $(OPT) -c ./src/main.cpp -o $(OBJSDIR)/main.o 
util.o: src/lib/util.cpp
	$(CPP) $(OPT) -c $(LIBSDIR)/util.cpp -o $(OBJSDIR)/util.o
instance.o: src/lib/instance.cpp
	$(CPP) $(OPT) -c $(LIBSDIR)/instance.cpp -o $(OBJSDIR)/instance.o
objective.o: src/lib/objective.cpp
	$(CPP) $(OPT) -c $(LIBSDIR)/objective.cpp -o $(OBJSDIR)/objective.o
random.o: src/lib/random/random.cpp	
	$(CPP) $(OPT) -c $(LIBSDIR)/random/random.cpp -o $(OBJSDIR)/random.o
uniform.o: src/lib/random/uniform.cpp	
	$(CPP) $(OPT) -c $(LIBSDIR)/random/uniform.cpp -o $(OBJSDIR)/uniform.o
normal.o: src/lib/random/normal.cpp	
	$(CPP) $(OPT) -c $(LIBSDIR)/random/normal.cpp -o $(OBJSDIR)/normal.o
custom.o: src/lib/random/custom.cpp	
	$(CPP) $(OPT) -c $(LIBSDIR)/random/custom.cpp -o $(OBJSDIR)/custom.o
clouds.o: src/lib/random/clouds.cpp	
	$(CPP) $(OPT) -c $(LIBSDIR)/random/clouds.cpp -o $(OBJSDIR)/clouds.o