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
        assembly = new AssemblyData("./appcode/FibLoop.exe");
#endif
    }

    cout << assembly->cliMetaDataTables.module.str() << endl;

    uint32_t entryPoint = assembly->cliHeader.entryPointToken;
    cout << "entryPoint: " << hex << entryPoint << endl;

    auto type = static_cast<CLIMetadataTableIndex>(entryPoint >> 24);
    cout << "Table name: " << getTableName(type) << endl;

    MethodBody body;
    assembly->getMethodBody(entryPoint & 0xFFFFFF, body);

    /*
    cout << "Method name: " << string(body.methodDef.name.begin(), body.methodDef.name.end()) << endl;
    cout << "Method body size: " << dec << body.data.size() << endl;
    
    for (uint32_t n = 0; n < body.exceptions.size(); ++n) {
        cout << body.exceptions[n].toString() << endl;
    }
    */

    cout << body.str(true) << endl;

    return 0;
}
