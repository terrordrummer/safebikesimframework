#include "FrameworkCore/CTrace.h"
#include "FrameworkCore/CSynch.h"
#include <sstream>

using namespace FrameworkCore;
using namespace std;

static map<unsigned int , CTrace*> Class_Counter;
static bool Atexit_registered = false;

void ClearTraceData() {
	map<unsigned int , CTrace*>::iterator it = Class_Counter.begin();
	while (it != Class_Counter.end()) {
	    it->second->SetLogStream("cout");;
		Class_Counter.erase(it);
		it = Class_Counter.begin();
	}
}


/*
  ____ _____
 / ___|_   _| __ __ _  ___ ___
| |     | || '__/ _` |/ __/ _ \
| |___  | || | | (_| | (_|  __/
 \____| |_||_|  \__,_|\___\___|

*/
CTrace::CTrace() :    _streamType(CTrace::STREAM_cout)
                    , _fileStream (NULL)
                    , _streamName("")
                    , _instCounter(0)
                    , _streamStaticIndex(0)
                    //, _bufferize(false)
                    {

    //_synch.InstanceLock("CTrace");
	//_streamStaticIndex = (unsigned int)Class_Counter.size();
	// if some classes are already instanced then get the last class index plus one
	//if (_streamStaticIndex > 0)
	//	_streamStaticIndex = Class_Counter.rbegin()->first + 1;
	//Class_Counter[_streamStaticIndex] = this;
	//if (!Atexit_registered) {
	//	atexit(	ClearTraceData );
	//	Atexit_registered = true;
	//}
    //_synch.InstanceUnlock("CTrace");
}

/*
  ____ _____
 / ___|_   _| __ __ _  ___ ___
| |     | || '__/ _` |/ __/ _ \
| |___  | || | | (_| | (_|  __/
 \____| |_||_|  \__,_|\___\___|

*/
CTrace::CTrace(std::string str_param)  {
    CTrace();
    SetLogStream(str_param);
    };

/*
 /\/|___ _____
|/\/ ___|_   _| __ __ _  ___ ___
  | |     | || '__/ _` |/ __/ _ \
  | |___  | || | | (_| | (_|  __/
   \____| |_||_|  \__,_|\___\___|

*/
CTrace::~CTrace() {
    _synch.InstanceLock("CTrace");
    //if (_fileStream!=NULL && _bufferize)
    //        _fileStream->close();
	// remove the class from the static array
	//Class_Counter.erase(_streamStaticIndex);
    _synch.InstanceUnlock("CTrace");
    };

/*
 ____       _   _                ____  _
/ ___|  ___| |_| |    ___   __ _/ ___|| |_ _ __ ___  __ _ _ __ ___
\___ \ / _ \ __| |   / _ \ / _` \___ \| __| '__/ _ \/ _` | '_ ` _ \
 ___) |  __/ |_| |__| (_) | (_| |___) | |_| | |  __/ (_| | | | | | |
|____/ \___|\__|_____\___/ \__, |____/ \__|_|  \___|\__,_|_| |_| |_|
                           |___/
*/
/**
 *  \param str_param stream name
 *
 * Valid stream names are:
 * \li "cout"
 * \li "cerr"
 * \li "clog"
 * \li "none"
 * \li any other valid file name
 */
void CTrace::SetLogStream(std::string str_param) {
    _synch.InstanceLock("CTrace");
    /* destroy previous stream if it was a file output */
    if (_streamType == CTrace::STREAM_tofile) {
        delete _fileStream;
        _fileStream = NULL;
        }
    /* switch to the stream selection   */
    if (!str_param.compare(std::string("cout")) || str_param.length()==0) {
        _streamType = CTrace::STREAM_cout;
        } 
    else if (!str_param.compare(std::string("cerr"))) {
        _streamType = CTrace::STREAM_cerr;
        } 
    else if (!str_param.compare(std::string("clog"))) {
        _streamType = CTrace::STREAM_clog;
        } 
    else if (!str_param.compare(std::string("none"))) {
        _streamType = CTrace::STREAM_none;
        } 
    else {
        _fileStream = new ofstream(str_param.c_str(), ios::out | ios::app);
        _streamType = CTrace::STREAM_tofile;
        _streamName = str_param;
        }
     _synch.InstanceUnlock("CTrace");
    };

/*
 ____       _   _                ____  _
/ ___|  ___| |_| |    ___   __ _/ ___|| |_ _ __ ___  __ _ _ __ ___
\___ \ / _ \ __| |   / _ \ / _` \___ \| __| '__/ _ \/ _` | '_ ` _ \
 ___) |  __/ |_| |__| (_) | (_| |___) | |_| | |  __/ (_| | | | | | |
|____/ \___|\__|_____\___/ \__, |____/ \__|_|  \___|\__,_|_| |_| |_|
                           |___/
*/
void CTrace::SetLogStream(const char * str_param) {
    std::string s(str_param);
    SetLogStream(s);
    };

/* write data to actual out stream */
CTrace& CTrace::operator << (std::string& str) {
    _synch.InstanceLock("CTrace");
    /* switch to the actual stream  */
    switch(_streamType) {
        case CTrace::STREAM_cout:
            cout << str;
            break;
        case CTrace::STREAM_cerr:
            cerr << str;
            break;
        case CTrace::STREAM_clog:
            clog << str;
            break;
        case CTrace::STREAM_tofile:
            //if (_bufferize) {
            //    (*_fileStream) << str;
            //    }
            //else {
                // write immediatly into the stream and close it
                ofstream f(_streamName.c_str(), ios::out | ios::app);
                f << str;
                f.close();
            //}
            
        }
    _synch.InstanceUnlock("CTrace");
    return *this;
    };

/* write data to actual out stream */
CTrace& CTrace::operator << (const char *sentence) {
    std::string s(sentence);
    *this << s;
    return *this;
    };

/* convert data into std string and write it to the actual out stream */
CTrace& CTrace::operator << (bool val) {
    std::stringstream s;
    s << boolalpha << val;
    *this << s.str();
    return *this;
    };

/* convert data into std string and write it to the actual out stream */
CTrace& CTrace::operator << (short val) {
    std::stringstream s;
    s << val;
    *this << s.str();
    return *this;
    };

/* convert data into std string and write it to the actual out stream */
CTrace& CTrace::operator << (unsigned short val) {
    std::stringstream s;
    s << val;
    *this << s.str();
    return *this;
    };

/* convert data into std string and write it to the actual out stream */
CTrace& CTrace::operator << (int val) {
    std::stringstream s;
    s << val;
    *this << s.str();
    return *this;
    };

/* convert data into std string and write it to the actual out stream */
CTrace& CTrace::operator << (unsigned int val) {
    std::stringstream s;
    s << val;
    *this << s.str();
    return *this;
    };

/* convert data into std string and write it to the actual out stream */
CTrace& CTrace::operator << (long val) {
    std::stringstream s;
    s << val;
    *this << s.str();
    return *this;
    };

/* convert data into std string and write it to the actual out stream */
CTrace& CTrace::operator << (unsigned long val) {
    std::stringstream s;
    s << val;
    *this << s.str();
    return *this;
    };

/* convert data into std string and write it to the actual out stream */
CTrace& CTrace::operator << (double val) {
    std::stringstream s;
    s << val;
    *this << s.str();
    return *this;
    };

/* convert data into std string and write it to the actual out stream */
CTrace& CTrace::operator << (long double val) {
    std::stringstream s;
    s << val;
    *this << s.str();
    return *this;
    };

/* convert data into std string and write it to the actual out stream */
CTrace& CTrace::operator << (streambuf* sb) {
    std::stringstream s;
    s << sb;
    *this << s.str();
    return *this;
    };

/* convert data into std string and write it to the actual out stream */
CTrace& CTrace::operator << (ostream& ( *pf )(ostream&)) {
    std::stringstream s;
    s << pf;
    *this << s.str();
    return *this;
    };

/* convert data into std string and write it to the actual out stream */
CTrace& CTrace::operator << (ios& ( *pf )(ios&)) {
    std::stringstream s;
    s << pf;
    *this << s.str();
    return *this;
    };

/* convert data into std string and write it to the actual out stream */
CTrace& CTrace::operator << (ios_base& ( *pf )(ios_base&)) {
    std::stringstream s;
    s << pf;
    *this << s.str();
    return *this;
    };

//
//void CTrace::Bufferize(bool flag) {
//	_bufferize = flag;
//}