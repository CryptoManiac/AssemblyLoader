#include "CLR/AssemblyData.hxx"

int main(int argc, const char* argv[]) {
	if (argc > 1) {
		AssemblyData *assembly = new AssemblyData(argv[1]);
	}
	else {
		AssemblyData *assembly = new AssemblyData("appcode\\FibLoop.exe");
	}
}

