CXX=g++
CXXFLAGS=-I../../include/ -Wall -fPIC -fopenmp -O3

APP_NAME=benchmark
OBJDIR=objs
default: $(APP_NAME)

.PHONY: dirs clean

dirs:
		/bin/mkdir -p $(OBJDIR)/

clean:
		/bin/rm $(OBJDIR)/* *~ $(APP_NAME)

OBJS=$(OBJDIR)/main.o 

$(APP_NAME): $(OBJS)
		$(CXX) $(CXXFLAGS) -o $@ $(OBJS)

$(OBJDIR)/%.o: %.cpp
		$(CXX) $< $(CXXFLAGS) -c -o $@
