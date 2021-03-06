CC = g++
CXXFLAGS = -Wall -g #-O3
LDLIBS = -Wall -g
SRCDIR = src
BUILDDIR = build
TARGET = bin/crispy
TARGETDIR = bin
SRCEXT = cc
LIBS = -lGL -lglut -lX11 -lXext

SOURCES = $(shell find $(SRCDIR) -type f -name *.$(SRCEXT))
OBJECTS = $(patsubst $(SRCDIR)/%,$(BUILDDIR)/%,$(SOURCES:.$(SRCEXT)=.o))

INC = -I include

all: $(TARGET)

$(TARGET): $(OBJECTS)
		$(CC) $(LDFLAGS) $^ $(INC) $(LDLIBS) $(LIBS) -o $@

$(BUILDDIR)/%.o: $(SRCDIR)/%.$(SRCEXT) 
		$(CC) -c $(CXXFLAGS) $(INC) $< -o $@

clean :
	rm -f $(BUILDDIR)/*
	rm -f $(TARGETDIR)/*
