OBJ:= ssfs.o superblock.o diskop.o
SRC:= ssfs.cpp superblock.cpp diskop.cpp superblock.hpp
SRC2:= ssfs_mkdsk.cpp superblock.cpp superblock.hpp
OBJ2:= ssfs_mkdsk.o superblock.o
CXX:= g++
CXXFLAGS:= -Wall -g -std=c++14

ssfs_mkdsk: $(OBJ2)
	$(CXX) $(OBJ) -o ssfs_mkdsk
ssfs_mkdsk.o: $(SRC2)
	$(CXX) -c $(SRC)

ssfs: $(OBJ)
	$(CXX) $(OBJ) -o ssfs
ssfs.o: $(SRC)
	$(CXX) -c $(SRC)
superblock.o: superblock.cpp
	$(CXX) -c superblock.cpp
diskop.o: diskop.cpp
	$(CXX) -c diskop.cpp
clean: ssfs
	rm $(OBJ) $(OBJ2) ssfs ssfs_mkdsk
