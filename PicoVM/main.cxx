#include <iostream>

#include "CLR/AssemblyData.hxx"
#include "CLR/EnumCasting.hxx"

#include "CLR/AppDomain.hxx"

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

    // Few simple tests for our AppDomain stub
    AppDomain domain;
    const AppDomain::AssemblyID& id = domain.loadAssembly(*assembly); // loading
    domain.loadAssembly(*assembly); // double-loading attempt
    delete assembly; // deleting original object

    AssemblyData& clrData = domain.getAssembly(id); // getting AssemblyData reference from the AppDomain

    // Print some module and entrypoint data.
    cout << clrData.cliMetaDataTables.module.str() << endl;

    uint32_t entryPoint = clrData.cliHeader.entryPointToken;
    cout << "entryPoint: " << hex << entryPoint << endl;

    auto type = static_cast<CLIMetadataTableItem>(entryPoint >> 24);
    cout << "Table name: " << getTableName(type) << endl;

    MethodBody body;
    clrData.getMethodBody(entryPoint & 0xFFFFFF, body);

    cout << body.str(true) << endl;

    return 0;
}
