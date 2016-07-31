#include "FrameworkCore/CGenericType.h"
#include "FrameworkEnvironment/CEnvironment.h"
#include "FrameworkCore/CTime.h"
#include <iostream>
#include <map>
#include "math.h"

using namespace std;
using namespace FrameworkCore;

//#define NUMERIC_TO_STRING_TEST
//#define STRUCT_TEST
//#define GENERIC_MAP_TEST
//#define ASSERTION_TEST
//#define NUMERIC_SPEED_TEST
//#define STRUCT_COMPARISON
//#define ARRAY_TEST
#define OBJECT_DUMP_TEST

 int __cdecl main(int argc, char argv[]) {
    cout << "TEST" << endl;

#ifdef NUMERIC_TO_STRING_TEST
    CGenericType var;
    CGenericType intval;
    ////////////////////////////////////////////////////////////////////////////////////////

    var = 1;
    cout << "var type: " << var.get_type_description() << endl;
    cout << "var value: " << var << endl;
    cout << endl;

    intval = 10;
    cout << "intval type: " << intval.get_type_description() << endl;
    cout << "intval value: " << intval << endl;
    cout << endl;

    var = intval;
    cout << "var type: " << var.get_type_description() << endl;
    cout << "var value: " << var << endl;
    cout << endl;

    var = var + intval;
    cout << "var type: " << var.get_type_description() << endl;
    cout << "var value: " << var << endl;
    cout << endl;

    
    var = "cambio di tipo";
    cout << "var type: " << var.get_type_description() << endl;
    cout << "var value: " << var << endl;
    cout << endl;

#endif

    ////////////////////////////////////////////////////////////////////////////////////////
#ifdef STRUCT_TEST    
    CGenericType st;
    st.SetStruct();

    st % "first_field" = 10;
    st % "second field" = "example";
    st % "first_field" % "subfield" = 100;
    st % "first_field" % "subfield2" = "OK";
    st % "first_field" % "subfield2" % "sub-subfield" = 3;

    cout << "1: " << st % "first_field" << endl;
    cout << "2: " << st % "second field" << endl;
    cout << "3: " << st % "first_field" % "subfield" << endl;
    cout << "4: " << st % "first_field" % "subfield2" << endl;
    cout << "5: " << st % "first_field" % "subfield2" % "sub-subfield" << endl;
   
    st = 10;
    cout << "st: " << st << endl;

    cout << "input a char to continue" << endl;
#endif

    ////////////////////////////////////////////////////////////////////////////////////////
#ifdef GENERIC_MAP_TEST
    map<string, CGenericType> mymap;

    CGenericType cc;
    cc= 10;
    mymap["first"] = cc;

    cc= "hello";

    mymap["second"] = cc;

    cout << "first: " << mymap["first"] << endl;
    cout << "second: " << mymap["second"] << endl;

    mymap.clear();
#endif

    ////////////////////////////////////////////////////////////////////////////////////////
#ifdef ASSERTION_TEST
    CGenericType field;
    field = "field1";
    CGenericType data;
    data % "field1" = 1;
    cout << "data.field: " << data % field << endl;
    field = 0;
    data % field;   // <- ASSERTION FAILED
#endif

    ////////////////////////////////////////////////////////////////////////////////////////
#ifdef NUMERIC_SPEED_TEST
    CTime ct;
    long double a = 0;
    long double b = 0;
    long double c = 1;
    long double d = 0;
    long double x;
    
#define N 1000000
    ct.Tic();
    for (unsigned int i=0; i<N; i++) {
        x = a*cos(b)/c + d;
        a = long double(i/0.1);
        b = long double(i/0.1);
        c = long double(1.0 + i/0.1);
        d = long double(i);
        x = d;
        }
    double T1 = ct.Toc();
    
    CGenericType aa;
    CGenericType bb;
    CGenericType cc;
    CGenericType dd;
    aa = 0;
    bb = 0;
    cc = 1;
    dd = 0;
    CGenericType xx;

    xx=0;
    ct.Tic();
    for (int i=0; i<N; i++) {
        xx = aa.toLongDouble()*cos(bb.toLongDouble())/cc.toLongDouble() + dd.toLongDouble();
        aa = long double(i/0.1);
        bb = long double(i/0.1);
        cc = long double(1.0 + i/0.1);
        dd = long double(i);
        xx = dd.toLongDouble();
        }
    double T2 = ct.Toc();
    cout << fixed;
    long double RES = a+b+c+d+x;
    cout << RES << endl << endl;
    cout << "native type: " << fixed << T1 << " sec" << endl;
    RES = (aa+bb+cc+dd+xx).toNumber();
    cout << "CGenericType type: " << fixed << T2 << " sec " << endl;
    cout << endl << endl << RES << endl << endl;
    cout << endl << "SPEED RATIO: " << T2/T1 << endl;
#endif

#ifdef STRUCT_COMPARISON
    CGenericType a,b;
    a % "one" = 1;
    a % "one" % "one" = 4;
    a % "two" % "one" = 6;

    ///////////////////////////////////
    b = "one";
    if (a==b)
        cout << "a is EQUAL of b" << endl;
    else
        cout << "a is different from b" << endl;

    ///////////////////////////////////
    b % "one" = 4;
    if (a==b)
        cout << "a is EQUAL of b" << endl;
    else
        cout << "a is different from b" << endl;

    ///////////////////////////////////
    b % "one" = 1;
    b % "one" % "one" = 4;
    b % "two" % "one" = 6;
   if (a==b)
        cout << "a is EQUAL of b" << endl;
    else
        cout << "a is different from b" << endl;

    ///////////////////////////////////
    b % "ciao" = "becomes diffferent";
    if (a==b)
        cout << "a is EQUAL of b" << endl;
    else
        cout << "a is different from b" << endl;

    ///////////////////////////////////
    b=a;
    if (a==b)
        cout << "a is EQUAL of b" << endl;
    else
        cout << "a is different from b" << endl;
#endif

#ifdef ARRAY_TEST
    CGenericType a1, a2;
    a1[0] = 1;
    a1[1] = "ciao";
    a2[0] = 0;
    if (a1==a2)
        cout << "a1 is equal of a2" << endl;
    else
        cout << "a1 is not equal of a2" << endl;
        
    a1[10] = 1;

    cout << "a1 size: " << a1.size() << endl;
    cout << "a1[10] : " << a1[10] << endl;
    cout << "addressing erroneous obj: " << a1[9] << endl;;

    a1=a2;
    if (a1==a2)
        cout << "a1 is equal of a2" << endl;
    else
        cout << "a1 is not equal of a2" << endl;
    
    a1 = 0;
    cout << "a1 size: " << a1.size() << endl;
    a1[10] = 1;
    cout << "a1 size: " << a1.size() << endl;

#endif

#ifdef OBJECT_DUMP_TEST
    char c;
    CGenericType a;
    a = 1;
    cout << a.Dump() << endl << endl;
    
    a = "hello world";
    cout << a << endl << endl;

    a % "first_field" = 1.0;
    a % "second field" = "hello";
    a % "third field" % "apple" = 10;
    a % "third field" % "orange" = 20;
    a % "third field" % "beaf" = "nice job";
    cout << a << endl << endl;

    (a % "forth field") [0] = 1;
    (a % "forth field") [2] = 3;
    cout << a << endl << endl;

#endif


    char endchar;
    cin >> endchar;
    }