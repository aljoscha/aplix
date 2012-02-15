PROGRAM = aplix

INCLUDEDIRS =
LIBDIRS =
LIBS = -lboost_program_options

CXXSOURCES = main.cpp network.cpp parser.cpp simplex.cpp tree.cpp
CXXOBJECTS = $(CXXSOURCES:.cpp=.o)
CXXFLAGS = -ftemplate-depth-128 -O3 -finline-functions -Wno-inline -Wall -fPIC  -DNDEBUG
CXX = g++

LDFLAGS = $(LIBDIRS) -Wl,-Bstatic $(LIBS) -Wl,-Bdynamic 

all: $(PROGRAM)

$(PROGRAM): $(CXXOBJECTS)
	$(CXX) -o $@ -Wl,--start-group $(CXXOBJECTS) $(LDFLAGS) -Wl,--end-group


basic_sample.o: basic_sample.cpp basic_sample.h
	$(CXX) $(CXXFLAGS) -c -o basic_sample.o basic_sample.cpp

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

clean:
	$(RM) -f $(CXXOBJECTS) $(PROGRAM)


run:
	./$(PROGRAM)
