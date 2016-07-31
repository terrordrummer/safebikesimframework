// FrameworkMathTest.cpp : definisce il punto di ingresso dell'applicazione console.
//

// standard includes
#include <iostream>
#include <fstream>

// framework includes
#include "FrameworkMath/CSpline.h"
#include "FrameworkMath/CCA.h"

// namespaces
using namespace FrameworkMath;
using namespace std;

//////////////////////////////////////////////////
//      TEST SELECTION

//#define SPLINE_TEST
#define CCA_TEST

//////////////////////////////////////////////////

/*
                 _
 _ __ ___   __ _(_)_ __
| '_ ` _ \ / _` | | '_ \
| | | | | | (_| | | | | |
|_| |_| |_|\__,_|_|_| |_|
*/
int main(int argc, char* argv[])
{
#ifdef SPLINE_TEST
    CSpline S;
    S.Push(0,2,0,0);
    S.Push(1,1,0,0);
    S.Push(0.3,-1.0,1,0);
    S.Push(2,0.01,0,1);
    S.Push(1.6,-1,-1,-1);
    S.ComputeSpline();

    for (float x=-1.01; x<=3.01; x+=0.1)
        cout << x << " = " << S.At(x) << endl;

    char c;
    cin >> c;

    // try automatic computation
    S.Clear();

    S.Push(0,0);
    S.Push(0,2);
    S.Push(1,1);
    S.Push(0.3,-1.0);
    S.Push(2,0.01);
    S.Push(1.6,-1);

    S.ComputeSpline();

    for (float x=-1.01; x<=3.01; x+=0.1)
        cout << x << " = " << S.At(x) << endl;

    cin >> c;
#endif

#ifdef CCA_TEST
    vector<tKnot> V;
    vector<tOrientedArc> A;

    V.resize(4);

    V[0].x = 0;
    V[0].y = 0;
    V[0].Yaw = 0;

    V[1].x = -10;
    V[1].y = -10;
    V[1].Yaw = 0;

    V[2].x = 30;
    V[2].y = 00;
    V[2].Yaw = 3.1416/2.0;

    V[3].x = 20;
    V[3].y = -10;
    V[3].Yaw = 3.1416*1.5;

    CCA::InterpolateKnots(V,A);

    for (unsigned int i=0; i<A.size(); i++) {
        cout << "arc[" << i << "]: L = " << A[i].prop.length << "; c = " << A[i].prop.curvature << endl;
    }

    // put results to file
    vector<tKnot> K;

    CCA::FitCurve(A, K);

    ofstream Fout("fit.txt", ios::out);

    Fout << "X\tY\tYaw\n";
    for (unsigned int i=0; i<K.size(); i++)
        Fout << K[i].x << "\t" << K[i].y << "\t" << K[i].Yaw << endl;

    Fout.close();

    char c;
    cin >> c;
#endif
	return 0;
}