
OBJECT_FILES = bin\\CachedFile.o bin\\HeapFile.o bin\\DataBase.o
INCLUDES = -IC:\Programs\mingw-w64\include -IC:\mingw-w64\include -Isrc
LIBS = -LC:\Programs\mingw-w64\lib\boost\1_75 -LC:\mingw-w64\lib -lboost_iostreams
CXXFLAGS = -m64 -std=c++17 -Ofast -s -static

allocator: TestBlockAllocator.exe
	TestBlockAllocator

heap: TestHeapFile.exe
	TestHeapFile

cached: TestCachedFile.exe
	TestCachedFile

files_securere: $(OBJECT_FILES) bin\\TestCachedFile.o

%.exe: bin\\%.o $(OBJECT_FILES)
	g++ $^ -o $@ $(CXXFLAGS) $(LIBS) tests\Debug.cpp

bin\\%.o: src\\%.cpp
	g++ -c $< -o $@ $(CXXFLAGS) $(INCLUDES)
	
bin\\%.o: tests\\%.cpp
	g++ -c $< -o $@ $(CXXFLAGS) $(INCLUDES)



