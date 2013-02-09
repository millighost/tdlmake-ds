# set path=%path%;v:/mingw/bin
# 
SRCS = tdlmake-ds.cc pattern.cc
OBJS = $(SRCS:.cc=.o)
LDFLAGS = -static-libstdc++ -static-libgcc
CXXFLAGS = -O -Wall

all: tdlmake-ds.exe

tdlmake-ds.exe: $(OBJS)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o $@ $(OBJS)
%.o: %.cc
	$(CXX) -c $(CXXFLAGS) $<
