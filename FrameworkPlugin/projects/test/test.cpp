#include <vector>
#include <string>
#include <iostream>
#include "FrameworkPlugin/CPluginManager.h"

using namespace FrameworkPlugin;
using namespace std;

 int __cdecl main(int argc, char argv[]) {
    
    char c;
    CPluginManager pm;
    vector<string> list;
    vector<string>::iterator it;
    
    cout << endl << "TEST" << endl;
    
    // get the search path
    cout << endl << "search path order:" << endl;
    list = pm.GetSearchPathList();
    it = list.begin();
    while (it != list.end()) {
        cout << *it << endl;
        it++;
        }

    cout << endl << "inserting some test path" << endl;
    pm.InsertSearchPath("prova");
    pm.AppendSearchPath("append prova");
    
    cout << endl << "new search path order:" << endl;
    list = pm.GetSearchPathList();
    it = list.begin();
    while (it != list.end()) {
        cout << *it << endl;
        it++;
        }

    cout << endl << "deleting some search path" << endl;
    pm.RemoveSearchPath("prova");
    pm.RemoveSearchPath("./");
    pm.RemoveSearchPath("append prova");
    cout << "new search path order:" << endl;
    list = pm.GetSearchPathList();
    it = list.begin();
    while (it != list.end()) {
        cout << *it << endl;
        it++;
        }
    // GET THE AVAILABLE PLUGINS
    cout << endl << "Available plugin paths:" << endl;
    list = pm.ScanForPluginPaths();
    it = list.begin();
    while (it != list.end()) {
        cout << *it << endl;
        it++;
        }
    cout << endl << "Available plugin names:" << endl;
    list = pm.ScanForPluginNames();
    it = list.begin();
    while (it != list.end()) {
        cout << *it << endl;
        it++;
        }
    
    cin >> c;

    //// LOAD TWO EXAMPLE PLUGINS BY CODE
    pm.LoadPlugin("CMyPlugin1","cfg1");
    pm.LoadPlugin("CMyPlugin2","cfg2");

    // LOAD PLUGINS FROM FILE
    //pm.LoadPluginsFromFile("pluginlist.cfg");

    CPlugin *p1 = pm.GetPlugin("CMyPlugin1");
    CPlugin *p2 = pm.GetPlugin("CMyPlugin2");

    if (p1 != NULL)
        cout << "plugin: " << p1->GetName() << ", version " << p1->GetVersion() << endl;
    else
        cout << "failed to load CMyPlugin1 plugin" << endl;

    if (p2 != NULL)
        cout << "plugin: " << p2->GetName() << ", version " << p2->GetVersion() << endl;
    else
        cout << "failed to load CMyPlugin2 plugin" << endl;

    cin >> c;

    pm.UnloadAll();

    }