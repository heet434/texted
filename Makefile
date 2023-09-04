TARGET=texted
CXX=g++ 
DEBUG=-g
OPT=-O0
WARN=-w -Wno-unknown-pragmas
NCURSES=-lncurses
CXXFLAGS=$(DEBUG) $(OPT) $(WARN) $(NCURSES) -pipe -std=c++17
LD=g++
LDFLAGS=$(NCURSES)
OBJS= main.o texted.o
all: $(OBJS)
	$(LD) -o $(TARGET) $(OBJS) $(LDFLAGS)
	@rm -rf *.o
 
main.o: main.cpp
	$(CXX) -c $(CXXFLAGS) main.cpp -o main.o
 
texted.o: texted.cpp
	$(CXX) -c $(CXXFLAGS) texted.cpp  -o texted.o
