#include "CLR/AppDomain.hxx"
#include "CLR/InstructionTree.hxx"

using namespace std;

int main(int argc, const char *argv[]) {

    const AssemblyData *assembly;

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

/*
#ifdef WIN32
    AppDomain domain(R"(appcode\)");
#else
    AppDomain domain("./PicoVM/appcode/");
#endif

    const auto& id = domain.loadAssembly(assembly); // loading
    const auto thread = domain.createThread();
    thread->setup(id);
    thread->run();
*/

    // Few simple tests for our AppDomain stub
#ifdef WIN32
    AppDomain domain(R"(appcode\)");
#else
    AppDomain domain("./PicoVM/appcode/");
#endif
    const auto& id = domain.loadAssembly(assembly); // loading
    domain.loadAssembly(assembly); // double-loading attempt
    delete assembly;

    const auto* clrData1 = domain.getAssembly(id); // getting AssemblyData reference from our AppDomain, using GUID as a key
    const auto* clrData = domain.getAssembly(clrData1->getName(), clrData1->getVersion()); // getting AssemblyData reference from our AppDomain, using name and value pair as a key 

    // Print some module and entrypoint data.
    cout << clrData->cliMetaDataTables._Module[0].str() << endl;
    auto version = clrData->getVersion();
    auto name = clrData->getName();
    cout << "Name: " << string(name.begin(), name.end()) << endl; 
    cout << "Version: " << dec << version[0] << " " << version[1] << " " << version[2] << " " << version[3] << endl; 

    uint32_t entryPoint = clrData->cliHeader.entryPointToken;
    if (entryPoint != 0) {
        cout << "entryPoint: " << hex << entryPoint << endl;

        auto type = static_cast<CLIMetadataTableItem>(entryPoint >> 24);
        cout << "Table name: " << getTableName(type) << endl;

        const MethodDefRow& methodDef = clrData->getMethodDef(entryPoint);

        cout << "methodName=" << string(methodDef.name.begin(), methodDef.name.end()) << endl;
        cout << methodDef.methodBody.str(true) << endl;

        auto tree = InstructionTree::MakeTree(methodDef.methodBody.data);

        cout << tree->str() << endl;

    } else {
        cout << "No entrypoint, this must be a library assembly" << endl;
        cout << "List of methods defined by this assembly:" << endl << endl;

        for (uint32_t n = 1; n < clrData->getMethodCount(); ++n) {
            const MethodDefRow& methodDef = clrData->getMethodDef(n);

            cout << "MethodName=" << string(methodDef.name.begin(), methodDef.name.end()) << endl;
            cout << methodDef.methodBody.str(true) << endl;
        }
    }

    return 0;
}
