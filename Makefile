# compile with:
# set path=%path%;v:/mingw/bin
# mingw32-make

SRCS = tdlmake-ds.cc pattern.cc
OBJS = $(SRCS:.cc=.o)
LDFLAGS = -static-libstdc++ -static-libgcc
CXXFLAGS = -O -Wall
CPPFLAGS = -DTEST

all: tdlmake-ds.exe

tdlmake-ds.exe: $(OBJS)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o $@ $(OBJS)
%.o: %.cc
	$(CXX) $(CPPFLAGS) -c $(CXXFLAGS) $<
