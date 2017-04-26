OBJ:= ssfs.o superblock.o diskop.o
SRC:= ssfs.cpp superblock.cpp diskop.cpp
CXX:= g++
CXXFLAGS:= -Wall

ssfs: $(OBJ)
	$(CXX) $(OBJ) -o ssfs
ssfs.o: $(SRC)
	$(CXX) -c $(SRC)
superblock.o: superblock.cpp
	$(CXX) -c superblock.cpp
diskop.o: diskop.cpp
	$(CXX) -c diskop.cpp
clean: ssfs
	rm $(OBJ) ssfs
