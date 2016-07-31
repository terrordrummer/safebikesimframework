#pragma once 

#include <string>
#include <iostream>
#include "CMyPlugin2.h"

using namespace FrameworkPlugin;
using namespace std;

bool CMyPlugin2::Initialize(string config_file) {
    cout << "MyPlugin2 Initialize() with file " << config_file << endl;
    return true;
    }
    
void CMyPlugin2::Shutdown() {
    cout << "MyPlugin2 Shutdown()" << endl;
    }
    
std::string CMyPlugin2::GetName() {
    return string("MyPlugin2");
    }
    
std::string CMyPlugin2::GetVersion() {
    return string("1.1");
    }
