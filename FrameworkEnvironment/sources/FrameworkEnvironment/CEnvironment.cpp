#include "FrameworkEnvironment/CEnvironment.h"
#include <direct.h>
#include <iostream>
#include <fstream>
#include <stdlib.h>
#include "windows.h"

//{{NO_DEPENDENCIES}}
// Microsoft Visual C++ generated include file.
// Used by FrameworkEnvironment.rc
//
#define IDD_TELEMETRY                   101
#define IDC_EDIT1                       1001
#define IDC_EDIT                        1001
#define IDC_PROGRESS1                   1002

// Next default values for new objects
// 
#ifdef APSTUDIO_INVOKED
#ifndef APSTUDIO_READONLY_SYMBOLS
#define _APS_NEXT_RESOURCE_VALUE        102
#define _APS_NEXT_COMMAND_VALUE         40001
#define _APS_NEXT_CONTROL_VALUE         1003
#define _APS_NEXT_SYMED_VALUE           101
#endif
#endif



using namespace FrameworkEnvironment;
using namespace FrameworkCore;
using namespace std;

// FILE IO
#define BUFFER_SIZE 8192
#define TAB "\t"
char buffer[BUFFER_SIZE];
char name[BUFFER_SIZE];

// ZERO TIMESTAMP
bool firstTimestamp = true;
double zeroTimeStamp;

//---------------------------------------------------------------------------
INT_PTR CALLBACK DlgProc(HWND hWndDlg, UINT Msg,
		       WPARAM wParam, LPARAM lParam)
{
	switch(Msg)
	{
	case WM_INITDIALOG:
		return TRUE;

	case WM_COMMAND:
		switch(wParam)
		{
		case IDOK:
			return TRUE;
		case IDCANCEL:
			return TRUE;
		}
		break;
	}

	return FALSE;
}
//---------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////////
// AUX CLASS USED TO ACCESS TO THE INTERNAL ENVIRONMENT OF A CEnvironment CLASS
// Used by Merge method in order to avoid the used to access to the _environment
// variable directly.
///////////////////////////////////////////////////////////////////////////////
class CAuxEnvironment : public CEnvironment {
public:
    _environmentType *local_environment;

    _environmentType *GetEnvironment() {
        local_environment = &_environment;
      return local_environment;
        };
    };

///////////////////////////////////////////////////////////////////////////////
#define LOGELEMENT(stream, p_x) { stream << "[" << p_x._logTime << "]" << p_x._name << ":" << p_x._element.Dump() << endl; }

#define LOG(p_name, p_value)  {         \
        CLogElement x;                  \
        x._logTime = _time.Toc();        \
        x._name = p_name;                \
        x._element = p_value;            \
        if (_logDataFlag)              \
            _logData.push_back(x);      \
        if (_logDataToTextFileFlag) \
            LOGELEMENT(_logFout, x);    \
        }


/*
  ____ _____            _                                      _
 / ___| ____|_ ____   _(_)_ __ ___  _ __  _ __ ___   ___ _ __ | |_
| |   |  _| | '_ \ \ / / | '__/ _ \| '_ \| '_ ` _ \ / _ \ '_ \| __|
| |___| |___| | | \ V /| | | | (_) | | | | | | | | |  __/ | | | |_
 \____|_____|_| |_|\_/ |_|_|  \___/|_| |_|_| |_| |_|\___|_| |_|\__|
*/
CEnvironment::CEnvironment() : _logDataFlag(false),
                               _logDataToTextFileFlag(false) {
    _environment.clear();
    }

/*
 /\/|___ _____            _                                      _
|/\/ ___| ____|_ ____   _(_)_ __ ___  _ __  _ __ ___   ___ _ __ | |_
  | |   |  _| | '_ \ \ / / | '__/ _ \| '_ \| '_ ` _ \ / _ \ '_ \| __|
  | |___| |___| | | \ V /| | | | (_) | | | | | | | | |  __/ | | | |_
   \____|_____|_| |_|\_/ |_|_|  \___/|_| |_|_| |_| |_|\___|_| |_|\__|
*/
CEnvironment::~CEnvironment() {
    // Stop logging
    StopLogging();
    if (_logDataToTextFileFlag)
        StopLoggingToTextFile();
    // clear map and destroy all the elements
    _environment.clear();
    }


/*
 ____       _   ____        _
/ ___|  ___| |_|  _ \  __ _| |_ __ _
\___ \ / _ \ __| | | |/ _` | __/ _` |
 ___) |  __/ |_| |_| | (_| | || (_| |
|____/ \___|\__|____/ \__,_|\__\__,_|
*/
void CEnvironment::SetData(std::string _name, CGenericType &var) {
    // variable are overwritten
    envLock();
    _environment[_name] = var;
    LOG(_name, var);
    envUnlock();
    }

void CEnvironment::SetData(std::string _name, unsigned char var) {
    // variable are overwritten
    envLock();
    _environment[_name] = var;
    LOG(_name, _environment[_name]);
    envUnlock();
    }

void CEnvironment::SetData(std::string _name, char var) {
    // variable are overwritten
    envLock();
    _environment[_name] = var;
    LOG(_name, _environment[_name]);
    envUnlock();
    }

void CEnvironment::SetData(std::string _name, unsigned short var) {
    // variable are overwritten
    envLock();
    _environment[_name] = var;
    LOG(_name, _environment[_name]);
    envUnlock();
    }

void CEnvironment::SetData(std::string _name, short var) {
    // variable are overwritten
    envLock();
    _environment[_name] = var;
    LOG(_name, _environment[_name]);
    envUnlock();
    }

void CEnvironment::SetData(std::string _name, unsigned int var) {
    // variable are overwritten
    envLock();
    _environment[_name] = var;
    LOG(_name, _environment[_name]);
    envUnlock();
    }

void CEnvironment::SetData(std::string _name, int var) {
    // variable are overwritten
    envLock();
    _environment[_name] = var;
    LOG(_name, _environment[_name]);
    envUnlock();
    }

void CEnvironment::SetData(std::string _name, unsigned long var) {
    // variable are overwritten
    envLock();
    _environment[_name] = var;
    LOG(_name, _environment[_name]);
    envUnlock();
    }

void CEnvironment::SetData(std::string _name, long var) {
    // variable are overwritten
    envLock();
    _environment[_name] = var;
    LOG(_name, _environment[_name]);
    envUnlock();
    }

void CEnvironment::SetData(std::string _name, unsigned long long var) {
    // variable are overwritten
    envLock();
    _environment[_name] = var;
    LOG(_name, _environment[_name]);
    envUnlock();
    }

void CEnvironment::SetData(std::string _name, long long var) {
    // variable are overwritten
    envLock();
    _environment[_name] = var;
    LOG(_name, _environment[_name]);
    envUnlock();
    }

void CEnvironment::SetData(std::string _name, float var) {
    // variable are overwritten
    envLock();
    _environment[_name] = var;
    LOG(_name, _environment[_name]);
    envUnlock();
    }

void CEnvironment::SetData(std::string _name, double var) {
    // variable are overwritten
    envLock();
    _environment[_name] = var;
    LOG(_name, _environment[_name]);
    envUnlock();
    }

void CEnvironment::SetData(std::string _name, long double var) {
    // variable are overwritten
    envLock();
    _environment[_name] = var;
    LOG(_name, _environment[_name]);
    envUnlock();
    }

void CEnvironment::SetData(std::string _name, const char *var) {
    // variable are overwritten
    envLock();
    _environment[string(_name)] = var;
    LOG(_name, _environment[_name]);
    envUnlock();
    }

void CEnvironment::SetData(std::string _name, std::string &var) {
    // variable are overwritten
    envLock();
    _environment[_name] = var;
    LOG(_name, _environment[_name]);
    envUnlock();
    }

/*
 _____      _     _
| ____|_  _(_)___| |_
|  _| \ \/ / / __| __|
| |___ >  <| \__ \ |_
|_____/_/\_\_|___/\__|
*/
bool CEnvironment::Exist(std::string _name) {
    bool retval;
    envLock();
    if (_environment.find(_name) == _environment.end())
        retval = false;
    else
        retval = true;
    envUnlock();
    return retval;
    }

/*
__        ___
\ \      / / |__   ___
 \ \ /\ / /| '_ \ / _ \
  \ V  V / | | | | (_) |
   \_/\_/  |_| |_|\___/
*/
std::set<std::string> CEnvironment::Who() {
    std::set<std::string> vars;
    envLock();
    // cycle through the vars and insert the _name into the final set
    _environmentType::iterator it = _environment.begin();
    while (it != _environment.end())
        vars.insert(it->first);
    // returns the vars _name set
    envUnlock();
    return vars;
    }

/*
__        ___         _____    ____  _        _
\ \      / / |__   __|_   _|__/ ___|| |_ _ __(_)_ __   __ _
 \ \ /\ / /| '_ \ / _ \| |/ _ \___ \| __| '__| | '_ \ / _` |
  \ V  V / | | | | (_) | | (_) |__) | |_| |  | | | | | (_| |
   \_/\_/  |_| |_|\___/|_|\___/____/ \__|_|  |_|_| |_|\__, |
                                                      |___/
*/
std::string CEnvironment::WhoToString() {
    stringstream vars;
    vars.str("");
    envLock();
    // cycle through the vars and insert the _name into the final set
    _environmentType::iterator it = _environment.begin();
    while (it != _environment.end()) {
        vars << it->first << endl;
        it++;
        }
    envUnlock();
    // returns the vars _name set
    return vars.str();
    }

/*
  ____      _   ____        _
 / ___| ___| |_|  _ \  __ _| |_ __ _
| |  _ / _ \ __| | | |/ _` | __/ _` |
| |_| |  __/ |_| |_| | (_| | || (_| |
 \____|\___|\__|____/ \__,_|\__\__,_|
*/
bool CEnvironment::GetData(std::string _name, CGenericType &var) {
    bool retval;
    envLock();
    // check for the existence of the variable
    _environmentType::iterator it = _environment.find(_name);
    if (it != _environment.end()) {
        // variable founded
        var = it->second;
        retval = true;
        }
    else
        // variable not found
        retval = false;
    envUnlock();
    return retval;
    }

/*
  ____      _   _   _                 _
 / ___| ___| |_| \ | |_   _ _ __ ___ | |__   ___ _ __
| |  _ / _ \ __|  \| | | | | '_ ` _ \| '_ \ / _ \ '__|
| |_| |  __/ |_| |\  | |_| | | | | | | |_) |  __/ |
 \____|\___|\__|_| \_|\__,_|_| |_| |_|_.__/ \___|_|
*/
CGenericType::numericType CEnvironment::GetNumber(std::string _name) {
    CGenericType::numericType retval;
    envLock();
    _environmentType::iterator it = _environment.find(_name);
    if (it != _environment.end()) {
        // returns the variable numeric value
        retval = it->second.toNumber();
        }
    else
        // returns ZERO if variable is not found
        retval = 0;
    // unlock and return value
    envUnlock();
    return retval;
    }

/*
 ____
|  _ \ ___ _ __ ___   _____   _____
| |_) / _ \ '_ ` _ \ / _ \ \ / / _ \
|  _ <  __/ | | | | | (_) \ V /  __/
|_| \_\___|_| |_| |_|\___/ \_/ \___|
*/
void CEnvironment::Remove(std::string _name) {
    envLock();
    _environment.erase(_name);
    envUnlock();
    }

/*
 ____                      _____     _____ _ _
|  _ \ _   _ _ __ ___  _ _|_   _|__ |  ___(_) | ___
| | | | | | | '_ ` _ \| '_ \| |/ _ \| |_  | | |/ _ \
| |_| | |_| | | | | | | |_) | | (_) |  _| | | |  __/
|____/ \__,_|_| |_| |_| .__/|_|\___/|_|   |_|_|\___|
                      |_|
*/
void CEnvironment::DumpToFile(std::string file_name) {
    // cycle through _environment elements and dump each to file
    envLock();
    // prepare file to be written
    fstream f = fstream(file_name.c_str(), ios_base::out);
    if (f.is_open()) {
        _environmentType::iterator it = _environment.begin();
        while (it != _environment.end()) {
            f << it->first << ":" << it->second.Dump() << endl;
            it++;
            }
        f.close();
        }
    envUnlock();
    }
/*
 _   _           _                       _____                    _____ _ _
| | | |_ __   __| |_   _ _ __ ___  _ __ |  ___| __ ___  _ __ ___ |  ___(_) | ___
| | | | '_ \ / _` | | | | '_ ` _ \| '_ \| |_ | '__/ _ \| '_ ` _ \| |_  | | |/ _ \
| |_| | | | | (_| | |_| | | | | | | |_) |  _|| | | (_) | | | | | |  _| | | |  __/
 \___/|_| |_|\__,_|\__,_|_| |_| |_| .__/|_|  |_|  \___/|_| |_| |_|_|   |_|_|\___|
                                  |_|
*/
void CEnvironment::UndumpFromFile(std::string file_name) {
    // lock _environment
    envLock();
    // data file is merged
    string          var_name;
    string          dump_data;
    CGenericType    aux;
    fstream f = fstream(file_name.c_str(), ios_base::in);
    if (f.is_open()) {
        while (!f.eof()) {
            // for each line read var _name and dump data
            getline(f, var_name,':');
            getline(f, dump_data);
            // undump object
            aux.Undump(dump_data);
            // set object data
            SetData(var_name,aux);
            // reset aux
            aux = 0;
            }
        f.close();
        }
    // unlock _environment
    envUnlock();
    }

/*
 __  __
|  \/  | ___ _ __ __ _  ___
| |\/| |/ _ \ '__/ _` |/ _ \
| |  | |  __/ | | (_| |  __/
|_|  |_|\___|_|  \__, |\___|
                 |___/
*/
void CEnvironment::Merge(CEnvironment &env_to_merge) {
    envLock();
    CAuxEnvironment *aux = (CAuxEnvironment *)&env_to_merge;
    _environmentType::iterator it = aux->GetEnvironment()->begin();
    while (it != aux->GetEnvironment()->end()) {
        // every variable will be overwritten
        _environment[it->first] = it->second;
        if (_logDataFlag)
            LOG(it->first, it->second);
        }
    envUnlock();
    }

/*
  ____ _
 / ___| | ___  __ _ _ __
| |   | |/ _ \/ _` | '__|
| |___| |  __/ (_| | |
 \____|_|\___|\__,_|_|
*/
void CEnvironment::Clear() {
    envLock();
    _environment.clear();
    _logData.clear();
    envUnlock();
    }


/*
 ____  _             _   _                      _
/ ___|| |_ __ _ _ __| |_| |    ___   __ _  __ _(_)_ __   __ _
\___ \| __/ _` | '__| __| |   / _ \ / _` |/ _` | | '_ \ / _` |
 ___) | || (_| | |  | |_| |__| (_) | (_| | (_| | | | | | (_| |
|____/ \__\__,_|_|   \__|_____\___/ \__, |\__, |_|_| |_|\__, |
                                    |___/ |___/         |___/
*/
void CEnvironment::StartLogging(LOGGINGTYPE type) {
    _logDataFlag = true;
    if (type == RESET_LOGGING_DATA) {
        _logData.clear();
        _time.Tic();}
    }

/*
 ____  _              _                      _
/ ___|| |_ ___  _ __ | |    ___   __ _  __ _(_)_ __   __ _
\___ \| __/ _ \| '_ \| |   / _ \ / _` |/ _` | | '_ \ / _` |
 ___) | || (_) | |_) | |__| (_) | (_| | (_| | | | | | (_| |
|____/ \__\___/| .__/|_____\___/ \__, |\__, |_|_| |_|\__, |
               |_|               |___/ |___/         |___/
*/
void CEnvironment::StopLogging() {
    _logDataFlag = false;
    }


/*
 _                ____ _____    _                     _
| |    ___   __ _|___ \_   _|__| | ___ _ __ ___   ___| |_ _ __ _   _
| |   / _ \ / _` | __) || |/ _ \ |/ _ \ '_ ` _ \ / _ \ __| '__| | | |
| |__| (_) | (_| |/ __/ | |  __/ |  __/ | | | | |  __/ |_| |  | |_| |
|_____\___/ \__, |_____||_|\___|_|\___|_| |_| |_|\___|\__|_|   \__, |
            |___/                                              |___/

*/
void CEnvironment::Log2Telemetry(string filename, bool showProgress) {
    HWND progressWnd;
    
    if (showProgress) {
        // show the progress bar
        progressWnd = CreateDialog(GetModuleHandle(0), MAKEINTRESOURCE(IDD_TELEMETRY), NULL, DlgProc);
        ShowWindow(progressWnd, SW_SHOW);
        SetWindowPos(progressWnd, HWND_TOPMOST, 0, 0, 50, 50, SWP_NOMOVE|SWP_NOSIZE);
    }
    // scan the file to collect the list of objects that could be added to telemetry.
    // candidate objects must have the field "timestamp"
    ifstream f;
    long int fsize;
    f.open(filename.c_str());
    f.seekg(0, ios_base::end);
    fsize = f.tellg();
    f.seekg(0, ios_base::beg);
    int i=0;
    char c;
    _candidateObjects.clear();
    while (!f.eof()) {
        buffer[0] = 0;
        f.get(buffer, BUFFER_SIZE, ']');
        f.get(c);
        f.get(name, BUFFER_SIZE, ':');    // get the object name
        f.get(c);
        f.getline(buffer, BUFFER_SIZE);
        if (_candidateObjects.find(name) == _candidateObjects.end()) {
            // object is not yet a candidate, parse its data and find the "timestamp" field
            CGenericType t;
            t.Undump(string(buffer));
            // look for timestamp field
            if (t.hasField("timestamp")) {
                _candidateObjects[string(name)] = t;
                if (firstTimestamp) {
                    firstTimestamp = false;
                    zeroTimeStamp  = (t % "timestamp").toNumber()/1000.0;
                }
            }
        }
        i++;
        //if (i % 1000 == 0) {
        //    double pos = f.tellg();
        //    double perc = pos / double(fsize);
        //    perc = floor(perc*10000.0) / 100.0;
        //    cout << "pre-scan : " << perc << "%" << endl;
        //}
    }
    f.close();

    char actualpath[512];
    char fullpath[512];
    GetFullPathName(filename.c_str(), 512, fullpath, NULL);
    GetFullPathName(".\\", 512, actualpath, NULL);
    //// create the folder with the same file name
    //string path = string(fullpath.c_str());
    //string fileName = string(filename.c_str());
    //string actDir = path.substr(0,path.find_last_of('\\'));
    //_chdir(actDir.c_str());
    string newDir = string(fullpath).substr(0, string(fullpath).find_last_of('.'));
    system( (string("mkdir ") + newDir.c_str()).c_str() );
    //_chdir( newDir.c_str() );

    // now for each object create its telemetry file and fill the header
    for (t_candidateMap::iterator it = _candidateObjects.begin(); it != _candidateObjects.end(); it++) {
        // create the file
        _telemetry[it->first] = new fstream();
        _telemetry[it->first]->open(((newDir + string("\\") + it->first + string(".tel")).c_str()), ios_base::out);
        _telemetry[it->first]->precision(12);
        // write header
        *(_telemetry[it->first]) << "timestamp" << TAB;
        CGenericType t = it->second;
        vector<string> fields = t.GetFields();
        for (unsigned int i=0; i<fields.size(); i++)
            if (fields[i] != "timestamp")
                *(_telemetry[it->first]) << fields[i] << TAB;
        *(_telemetry[it->first]) << endl;
    }
    
    // re-read the log file and store telemetry data
    f.clear();
    string Fname = string(filename.c_str());
    f.open(Fname.c_str());
    i=0;
    while (!f.eof()) {
        buffer[0] = 0;
        f.get(buffer, BUFFER_SIZE, ']');
        f.get(c);
        f.get(name, BUFFER_SIZE, ':');    // get the object name
        f.get(c);
        f.getline(buffer, BUFFER_SIZE);
        if (_candidateObjects.find(name) != _candidateObjects.end()) {
            // log data, get the list of fields
            CGenericType t;
            t.Undump(buffer);
            vector<string> fields = t.GetFields();
            // store timestamp
            *(_telemetry[string(name)]) << (t % "timestamp").toNumber()/1000.0 - zeroTimeStamp << TAB;
            for (unsigned int i=0; i<fields.size(); i++)
                if (fields[i] != "timestamp")
                    *(_telemetry[string(name)]) << (t % (fields[i])).toNumber() << TAB;
            *(_telemetry[string(name)]) << endl;
        }
        i++;
        //if (i % 1000 == 0) {
        //    double pos = f.tellg();
        //    double perc = pos / double(fsize);
        //    perc = floor(perc*10000.0) / 100.0;
        //    cout << "writing telemetry: " << perc << "%" << endl;
        //}
    }

    // cycle through the telemetry streams and close
    for (t_candidateMap::iterator it = _candidateObjects.begin(); it != _candidateObjects.end(); it++) {
        _telemetry[it->first]->close();
    }
    _telemetry.clear();

   
    if (showProgress) {
        Sleep(5000);
        // show the progress bar
        DestroyWindow(progressWnd);
    }

}

/*
 ____                        _                ____        _       _____   _____         _   _____ _ _
|  _ \ _   _ _ __ ___  _ __ | |    ___   __ _|  _ \  __ _| |_ __ |_   _|_|_   _|____  _| |_|  ___(_) | ___
| | | | | | | '_ ` _ \| '_ \| |   / _ \ / _` | | | |/ _` | __/ _` || |/ _ \| |/ _ \ \/ / __| |_  | | |/ _ \
| |_| | |_| | | | | | | |_) | |__| (_) | (_| | |_| | (_| | || (_| || | (_) | |  __/>  <| |_|  _| | | |  __/
|____/ \__,_|_| |_| |_| .__/|_____\___/ \__, |____/ \__,_|\__\__,_||_|\___/|_|\___/_/\_\\__|_|   |_|_|\___|
                      |_|               |___/
*/
void CEnvironment::DumpLogDataToTextFile(string filename) {
    ofstream fout;
    envLock();
    fout.open(filename.c_str());
    list<CLogElement>::iterator it = _logData.begin();
    while(it != _logData.end()) {
        LOGELEMENT(fout, (*it));
        it++;
        }
    fout.close();
    envUnlock();
    _logData.clear();
    }

/*
  ____ _                 _                ____        _
 / ___| | ___  __ _ _ __| |    ___   __ _|  _ \  __ _| |_ __ _
| |   | |/ _ \/ _` | '__| |   / _ \ / _` | | | |/ _` | __/ _` |
| |___| |  __/ (_| | |  | |__| (_) | (_| | |_| | (_| | || (_| |
 \____|_|\___|\__,_|_|  |_____\___/ \__, |____/ \__,_|\__\__,_|
                                    |___/
*/
void CEnvironment::ClearLogData() {
    _logData.clear();
    }

/*
 ____  _             _   _                      _            _____   _____         _   _____ _ _
/ ___|| |_ __ _ _ __| |_| |    ___   __ _  __ _(_)_ __   __ |_   _|_|_   _|____  _| |_|  ___(_) | ___
\___ \| __/ _` | '__| __| |   / _ \ / _` |/ _` | | '_ \ / _` || |/ _ \| |/ _ \ \/ / __| |_  | | |/ _ \
 ___) | || (_| | |  | |_| |__| (_) | (_| | (_| | | | | | (_| || | (_) | |  __/>  <| |_|  _| | | |  __/
|____/ \__\__,_|_|   \__|_____\___/ \__, |\__, |_|_| |_|\__, ||_|\___/|_|\___/_/\_\\__|_|   |_|_|\___|
                                    |___/ |___/         |___/
*/
void CEnvironment::StartLoggingToTextFile(string filename, LOGGINGTYPE type) {
    if (type == RESET_LOGGING_DATA)
        _logFout.open(filename.c_str());    // reset logging data
    else if (type == APPEND_LOGGING_DATA)
        _logFout.open(filename.c_str());    // append logging data
    _logDataToTextFileFlag = true;
    }

/*
 ____  _              _                      _            _____   _____         _   _____ _ _
/ ___|| |_ ___  _ __ | |    ___   __ _  __ _(_)_ __   __ |_   _|_|_   _|____  _| |_|  ___(_) | ___
\___ \| __/ _ \| '_ \| |   / _ \ / _` |/ _` | | '_ \ / _` || |/ _ \| |/ _ \ \/ / __| |_  | | |/ _ \
 ___) | || (_) | |_) | |__| (_) | (_| | (_| | | | | | (_| || | (_) | |  __/>  <| |_|  _| | | |  __/
|____/ \__\___/| .__/|_____\___/ \__, |\__, |_|_| |_|\__, ||_|\___/|_|\___/_/\_\\__|_|   |_|_|\___|
               |_|               |___/ |___/         |___/
*/
void CEnvironment::StopLoggingToTextFile() {
    _logDataToTextFileFlag = false;
    _logFout.close();
    }
