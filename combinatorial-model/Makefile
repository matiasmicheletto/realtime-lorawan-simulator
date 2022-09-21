CPP 	= g++
CC 		= gcc
OPT 	= -Wall
TARGET 	= runnable
OBJSDIR = ./obj
OBJ 	= main.o
RM 		= rm -f

.PHONY: all all-before all-after clean clean-custom

all: all-before $(TARGET) all-after

clean: clean-custom
	${RM} $(OBJ) $(TARGET)

$(TARGET): $(OBJ)
	$(CPP) $(patsubst %.o, $(OBJSDIR)/%.o, $(OBJ)) -o $(TARGET)

main.o: src/main.cpp
	$(CPP) $(OPT) -c ./src/main.cpp -o $(OBJSDIR)/main.o 
