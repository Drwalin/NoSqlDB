
OBJECT_FILES = bin\\CachedFile.o bin\\HeapFile.o bin\\BlockAllocator.o bin\\DataBase.o
INCLUDES = -IC:\Programs\mingw-w64\include -Isrc
LIBS = -LC:\Programs\mingw-w64\lib\boost\1_75 -lboost_iostreams
CXXFLAGS = -m64 -std=c++17 -Ofast -s -static

TestCachedFile: TestCachedFile.exe
	TestCachedFile

files_securere: $(OBJECT_FILES) bin\\TestCachedFile.o

%.exe: bin\\%.o $(OBJECT_FILES)
	g++ $^ -o $@ $(CXXFLAGS) $(LIBS)

bin\\%.o: src\\%.cpp
	g++ -c $< -o $@ $(CXXFLAGS) $(INCLUDES)
	
bin\\%.o: tests\\%.cpp
	g++ -c $< -o $@ $(CXXFLAGS) $(INCLUDES)



