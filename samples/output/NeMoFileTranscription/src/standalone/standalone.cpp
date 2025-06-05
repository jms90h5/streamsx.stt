// e1S2aDbB_090T0kQdnhQ03GLKZ3_02j1_1makZLqt7NvQBXb41rWsO8yAhEt9ULHMZgNES758_1Pj8CqHuPGNtnjA5

#include <SPL/Runtime/ProcessingElement/StandaloneApplication.h>

#include "standalone.h"

namespace Distillery 
{
    class DistilleryApplication;
}

Distillery::DistilleryApplication * instantiate_new_app() 
{ 
    SPL::StandaloneApplication app;
    return app.releaseImpl(); 
}

int main(int argc, char** argv, char** envp) 
{ 
    return SPL::StandaloneApplication::main(argc, argv, envp); 
}
