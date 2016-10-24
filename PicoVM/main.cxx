#include <iostream>

#include "CLR/AssemblyData.hxx"
#include "CLR/EnumCasting.hxx"

using namespace std;

int main(int argc, const char *argv[]) {

    AssemblyData *assembly;

    if (argc > 1) {
        assembly = new AssemblyData(argv[1]);
    }
    else {
#ifdef WIN32
        assembly = new AssemblyData(R"(appcode\FibLoop.exe)");
#else
        assembly = new AssemblyData("./PicoVM/appcode/FibLoop.exe");
#endif
    }

    cout << assembly->cliMetaDataTables.module.str() << endl;

    uint32_t entryPoint = assembly->cliHeader.entryPointToken;
    cout << "entryPoint: " << hex << entryPoint << endl;

    auto type = static_cast<CLIMetadataTableItem>(entryPoint >> 24);
    cout << "Table name: " << getTableName(type) << endl;

    MethodBody body;
    assembly->getMethodBody(entryPoint & 0xFFFFFF, body);

    cout << body.str(true) << endl;

    return 0;
}
