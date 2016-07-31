#include "FrameworkCore/CGenericType.h"
#include "FrameworkEnvironment/CEnvironment.h"
#include <iostream>

using namespace FrameworkCore;
using namespace FrameworkEnvironment;
using namespace std;


// #define ENVIRONMENT_LOGGING_SPEED_TEST  // Testing the speed of logging environment data
#define ENVIRONMENT_DUMP_UNDUMP_TEST    // Testing the dump/undump functions of environment

#ifdef ENVIRONMENT_LOGGING_SPEED_TEST
 int __cdecl main(int argc, char argv[]) {
    CEnvironment env;
    CGenericType var;

    //// SIMPLE LOGGING
    //env.StartLogging();
    //var=1.0;
    //env.Set("alpha",var);
    //var = "prova";
    //env.Set("beta",var);
    //
    //CGenericType a;
    //a % "first_field" = 1.0;
    //a % "second field" = "hello";
    //a % "third field" % "apple" = 10;
    //a % "third field" % "orange" = 20;
    //a % "third field" % "beaf" = "nice job";
    //env.Set("a",a);

    //(a % "forth field") [0] = 1;
    //(a % "forth field") [2] = 3;
    //env.Set("b",a);
    //env.StopLogging();
    //env.DumpLogDataToTextFile("environment_first_test.log");
    //env.ClearLogData();
    
    
    // ADVANCED LOGGING TEST WITH NUMERICAL TABLE EXPORT
    CTime time, delta_time;
    env.Clear();
    time.Tic();
    env.Set("time",time.Toc());
    env.StartLoggingToTextFile("logout.txt");

    CGenericType x,y,z;

#define N 100000

    for (int i=0; i<N; i++) {
        delta_time.Tic();
        var % "me" = rand();
        env.Set("alpha",var);
        
        x % "front_frame" % "x" = (rand()%100)/100.0;
        x % "front_frame" % "y" = 10.0 + (rand()%100)/100.0;
        x % "front_frame" % "z" = 15.0 + (rand()%100)/100.0;
        x % "front_frame" % "R" = (rand()%100)/100.0;
        x % "front_frame" % "P" = 10.0 + (rand()%100)/100.0;
        x % "front_frame" % "Y" = 15.0 + (rand()%100)/100.0;

        x % "rear_frame" % "x" = (rand()%100)/100.0;
        x % "rear_frame" % "y" = 10.0 + (rand()%100)/100.0;
        x % "rear_frame" % "z" = 15.0 + (rand()%100)/100.0;
        x % "rear_frame" % "R" = (rand()%100)/100.0;
        x % "rear_frame" % "P" = 10.0 + (rand()%100)/100.0;
        x % "rear_frame" % "Y" = 15.0 + (rand()%100)/100.0;
        env.Set("wheels",x);

        y[0] = 0.0  + (rand()%100)/100.0;
        y[1] = 2.0  + (rand()%100)/100.0;
        y[2] = 4.0  + (rand()%100)/100.0;
        y[3] = 6.0  + (rand()%100)/100.0;
        y[4] = 8.0  + (rand()%100)/100.0;
        y[5] = 10.0 + (rand()%100)/100.0;
        env.Set("curve",y);

        z % "engine_parameters" % "RPM"     = rand()%10;
        z % "engine_parameters" % "Torque"  = rand()%10;
        z % "vehicle_parameters" % "speed"  = rand()%10;
        (z % "vehicle_parameters" % "accelerations") [0] = rand()%10;
        (z % "vehicle_parameters" % "accelerations") [1] = rand()%10;
        (z % "vehicle_parameters" % "accelerations") [2] = rand()%10;
        env.Set("data",z);

        env.Set("delta_time", delta_time.Toc());
        env.Set("time",time.Toc());
        //Sleep(1);
        if (i % 200 == 0) {
            cout << double(i)/double(N)*100.0 << endl;
            cout << delta_time.Toc() << endl;
            }
        }
    env.StopLoggingToTextFile();
    env.Clear();

    cout << "finished." << endl;
    char c;
    cin >> c;
    }
#endif
 
#ifdef ENVIRONMENT_DUMP_UNDUMP_TEST
 int __cdecl main(int argc, char argv[]) {
     char c;
    CGenericType a;
    CGenericType b;
    CGenericType d;

    a % "first_field" = 1.0;
    a % "second field" = "hello";
    a % "third field" % "apple" = 10;
    a % "third field" % "orange" = 20;
    a % "third field" % "beaf" = "nice job";

    (a % "forth field") [0] = 1;
    (a % "forth field") [2] = 3;
    
    b = a % "third field";
    
    cout << a.Dump() << endl << endl;
    cout << b.Dump() << endl << endl;

    CEnvironment *env = new CEnvironment();
    env->Set("a",a);
    env->Set("b",a % "third field");
    cout << "ENVIRONMENT: " << endl << env->WhoToString();
    env->DumpToFile("environmentDump.txt");
    env->Clear();
    cout << endl << endl << "AFTER DUMP: " << env->WhoToString();
    env->UndumpFromFile("environmentDump.txt");
    cout << endl << endl << "AFTER UNDUMP: " << env->WhoToString();

    a % "5" = 12;

    env->Get("a", a);
    env->Get("b", b);
    env->Get("d", d);

    cout << a.Dump() << endl << endl;
    cout << b.Dump() << endl << endl;
    cout << d.Dump() << endl << endl;

    char endchar;
    cin >> endchar; 
    
    }

#endif
