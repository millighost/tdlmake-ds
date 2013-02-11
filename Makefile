# compile with:
# set path=%path%;v:/mingw/bin
# mingw32-make

ifdef windir
  RM = del
else
  RM = rm
endif

CPPFLAGS = -DTEST
CXXFLAGS = -O -Wall
LDFLAGS = -static-libstdc++ -static-libgcc

SRCS = tdlmake-ds.cc pattern.cc sysdep.cc
OBJS = $(SRCS:.cc=.o)

all: tdlmake-ds.exe

tdlmake-ds.exe: $(OBJS)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o $@ $(OBJS)
%.o: %.cc
	$(CXX) $(CPPFLAGS) -c $(CXXFLAGS) $<
clean:
	$(RM) $(OBJS)
