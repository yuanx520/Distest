CXX=g++
CXXFLAGS+=$(shell pkg-config --cflags opencv) -std=c++0x
CXXFLAGS+=-I.
LIBS+=$(shell pkg-config --libs opencv) 


SRCS:=distest.cpp 
TARGETS:=distest
all: $(TARGETS)


distest: distest.o
	$(CXX) $(LDFLAGS) -o $@ $^ $(LIBS)

.PHONY: clean
clean:
	rm -f *.o 
