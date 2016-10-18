all: main


CLR/AssemblyData.o:
	clang++ -std=c++11 -g -c CLR/AssemblyData.cxx -o CLR/AssemblyData.o


main.o:
	clang++ -std=c++11 -g -c main.cxx -o main.o


main: CLR/AssemblyData.o main.o
	clang++ -std=c++11 main.o CLR/AssemblyData.o -o main


clean:
	rm -f main.o CLR/AssemblyData.o
