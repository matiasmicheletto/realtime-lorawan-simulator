CPP      = g++
CFLAGS   = -std=c++11 -Wall
SRCDIR   = src
OBJDIR   = obj
LIBDIR   = $(SRCDIR)/lib

SOURCES  = $(wildcard $(SRCDIR)/*.cpp) $(wildcard $(LIBDIR)/**/*.cpp)
HEADERS  = $(wildcard $(LIBDIR)/**/*.h)
OBJECTS  = $(patsubst $(SRCDIR)/%.cpp,$(OBJDIR)/%.o,$(SOURCES))
TARGET   = datgen

INCLUDES = -I$(LIBDIR) -I$(LIBDIR)/util -I$(LIBDIR)/instance

$(TARGET): $(OBJECTS)
	$(CPP) $(CFLAGS) $(INCLUDES) $^ -o $@

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp $(HEADERS)
	@mkdir -p $(@D)
	$(CPP) $(CFLAGS) $(INCLUDES) -c $< -o $@

clean:
	rm -rf $(OBJDIR)

.PHONY: clean