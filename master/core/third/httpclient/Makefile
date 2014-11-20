# File Makefile

files := $(shell ls)
srcs  := $(filter %.cc, $(files))
objs  := $(patsubst %.cc, %.o, $(srcs))

CXX := g++
CXXFLAGS := -Wall -O
LDFLAGS := -lz

all : main
.PHONY : all

main : $(objs)
	$(CXX)  $^ -o $@ $(LDFLAGS)

%.o : %.cc %.h

.PHONY : clean 
clean:
	-rm main *.o *~
	  
