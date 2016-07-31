#pragma once 

#include <string>
#include <iostream>
#include "CMyPlugin1.h"

using namespace FrameworkPlugin;
using namespace std;

bool CMyPlugin1::Initialize(string config_file) {
    cout << "MyPlugin1 Initialize() with file " << config_file << endl;
    return true;
    }
    
void CMyPlugin1::Shutdown() {
    cout << "MyPlugin1 Shutdown()" << endl;
    }
    
std::string CMyPlugin1::GetName() {
    return string("MyPlugin1");
    }
    
std::string CMyPlugin1::GetVersion() {
    return string("1.0");
    }
