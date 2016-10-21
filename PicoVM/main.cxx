#include "CLR/AssemblyData.hxx"

int main(int argc, const char *argv[]) {

  if (argc > 1) {
    AssemblyData *assembly = new AssemblyData(argv[1]);
  } else {
#ifdef WIN32
    AssemblyData *assembly = new AssemblyData(R"(appcode\FibLoop.exe)");
#else
    AssemblyData *assembly = new AssemblyData("./appcode/FibLoop.exe");
#endif
  }

  return 0;
}
