CPP 	= g++
CC 		= gcc
OPT 	= -Wall
TARGET 	= binary
LIBSDIR = ./src/lib
OBJSDIR = ./obj
OBJ 	= main.o instance.o objective.o
RM 		= rm -f

.PHONY: all all-before all-after clean clean-custom

all: all-before $(TARGET) all-after

clean: clean-custom
	${RM} $(OBJ) $(TARGET)

$(TARGET): $(OBJ)
	$(CPP) $(patsubst %.o, $(OBJSDIR)/%.o, $(OBJ)) -o $(TARGET) -lpthread

main.o: src/main.cpp
	$(CPP) $(OPT) -c ./src/main.cpp -o $(OBJSDIR)/main.o 
instance.o: src/lib/instance.cpp
	$(CPP) $(OPT) -c $(LIBSDIR)/instance.cpp -o $(OBJSDIR)/instance.o
objective.o: src/lib/objective.cpp
	$(CPP) $(OPT) -c $(LIBSDIR)/objective.cpp -o $(OBJSDIR)/objective.o