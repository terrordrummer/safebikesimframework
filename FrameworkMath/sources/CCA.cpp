#include "FrameworkMath/CCA.h"
#include "math.h"

using namespace FrameworkMath;
using namespace std;

//////////////////////////////////////////////////
//         FORTRAN FUNCTION DEFINITIONS
//////////////////////////////////////////////////

extern "C" {
    void LINK_KNOTS(double *, double *, double *, double *, double *, double *, double *, double *, double *, double *, double *, double *);
}

//////////////////////////////////////////////////

/*
 _ _       _        _                _
| (_)_ __ | | __   | | ___ __   ___ | |_
| | | '_ \| |/ /   | |/ / '_ \ / _ \| __|
| | | | | |   <    |   <| | | | (_) | |_
|_|_|_| |_|_|\_\___|_|\_\_| |_|\___/ \__|
              |_____|
*/
void CCA::LinkKnot( tKnot           &P0,
                    tKnot           &P1, 
                    tArcProperties  &arc1,
                    tArcProperties  &arc2,
                    double          &knot1yaw, 
                    double          &knot2yaw) {
    // create local var copy
    double x10          = P0.x;
    double y10          = P0.y;
    double alpha10      = P0.Yaw;
    double x20          = P1.x;
    double y20          = P1.y;
    double alpha20      = P1.Yaw;
    double alpha1add    = knot1yaw;
    double alpha2add    = knot2yaw;
    double length1, curv1;
    double length2, curv2;

    // call fortran function
    LINK_KNOTS(&x10, &y10, &alpha10, &x20, &y20, &alpha20, &length1, &curv1, &length2, &curv2, &alpha1add, &alpha2add);

    // store results
    arc1.length     = length1;
    arc1.curvature  = curv1;
    arc2.length     = length2;
    arc2.curvature  = curv2;
    knot1yaw        = alpha1add;
    knot2yaw        = alpha2add;
}

/*
 _       _                        _       _           _                _
(_)_ __ | |_ ___ _ __ _ __   ___ | | __ _| |_ ___    | | ___ __   ___ | |_ ___
| | '_ \| __/ _ \ '__| '_ \ / _ \| |/ _` | __/ _ \   | |/ / '_ \ / _ \| __/ __|
| | | | | ||  __/ |  | |_) | (_) | | (_| | ||  __/   |   <| | | | (_) | |_\__ \
|_|_| |_|\__\___|_|  | .__/ \___/|_|\__,_|\__\___|___|_|\_\_| |_|\___/ \__|___/
                     |_|                        |_____|
*/
bool CCA::Interpolate2DPoints(vector<tPoint2D>  &points2D, vector<tOrientedArc> &arcs) {
    // construct the vector of Oriented points2D
    vector<tKnot> V;
    unsigned int size;
    size = (unsigned int)points2D.size();
    arcs.clear();
    // if dimension is less than two then return true but no arcs exist
    if (size < 2)
        return true;
    // initialize return value
    V.clear();
    V.resize(size);
    // cycle through the vectors and generate the yaw
    for (unsigned int i=1; i<size-1; i++) {
        V[i].x = points2D[i].x;
        V[i].y = points2D[i].y;
        V[i].Yaw = atan2(points2D[i+1].y - points2D[i-1].y, points2D[i+1].x - points2D[i-1].x);
        }
    // first point
    V[0].x = points2D[0].x;
    V[0].y = points2D[0].y;
    if (size == 2) {
        // only two points2D, get the connecting line tangent
        V[0].Yaw = atan2(points2D[1].y - points2D[0].y, points2D[1].x - points2D[0].x);
    } else {
        // initial yaw
        double yaw1 = atan2(points2D[1].y - points2D[0].y, points2D[1].x - points2D[0].x);
        double yaw2 = atan2(points2D[2].y - points2D[0].y, points2D[2].x - points2D[0].x);
        V[0].Yaw = yaw1 + (yaw1-yaw2);
        }
    // last point
    V[size-1].x = points2D[size-1].x;
    V[size-1].y = points2D[size-1].y;
    if (size == 2) {
        // only two points2D, get the connecting line tangent
        V[size-1].Yaw = atan2(points2D[1].y - points2D[0].y, points2D[1].x - points2D[0].x);
    } else {
        // final yaw
        double yaw1 = atan2(points2D[size-1].y - points2D[size-2].y, points2D[size-1].x - points2D[size-2].y);
        double yaw2 = atan2(points2D[size-2].y - points2D[size-3].y, points2D[size-2].x - points2D[size-3].x);
        V[size-1].Yaw = yaw1 + (yaw1-yaw2);
        }
    // interpolate the oriented points2D
    return InterpolateKnots(V, arcs);
    }

/*
 _       _                        _       _           _                _
(_)_ __ | |_ ___ _ __ _ __   ___ | | __ _| |_ ___    | | ___ __   ___ | |_ ___
| | '_ \| __/ _ \ '__| '_ \ / _ \| |/ _` | __/ _ \   | |/ / '_ \ / _ \| __/ __|
| | | | | ||  __/ |  | |_) | (_) | | (_| | ||  __/   |   <| | | | (_) | |_\__ \
|_|_| |_|\__\___|_|  | .__/ \___/|_|\__,_|\__\___|___|_|\_\_| |_|\___/ \__|___/
                     |_|                        |_____|
*/
bool CCA::InterpolateKnots(vector<tKnot>  &knots, vector<tOrientedArc> &arcs) {
    // Initialize the arc data container
    arcs.clear();
    // check the size
    if (knots.size() < 2)
        return true;
    // cycle through the points and connect them
    for (unsigned int i=0; i<knots.size()-1; i++) {
        // link the knots
        tArcProperties arc_prop1, arc_prop2;
        double knot1yaw, knot2yaw;
        LinkKnot(   knots[i], knots[i+1], arc_prop1, arc_prop2, knot1yaw, knot2yaw);
        tOrientedArc A;
        // push the first arc
        A.P0 = knots[i];
        A.P0.Yaw += knot1yaw;
        A.prop = arc_prop1;
        arcs.push_back(A);
        // push the second arc
        A.P0 = MoveAlongArc(A,arc_prop1.length);
        A.prop = arc_prop2;
        arcs.push_back(A);
    }
    return true;
    }

/*
 __  __                   _    _                      _
|  \/  | _____   _____   / \  | | ___  _ __   __ _   / \   _ __ ___
| |\/| |/ _ \ \ / / _ \ / _ \ | |/ _ \| '_ \ / _` | / _ \ | '__/ __|
| |  | | (_) \ V /  __// ___ \| | (_) | | | | (_| |/ ___ \| | | (__
|_|  |_|\___/ \_/ \___/_/   \_\_|\___/|_| |_|\__, /_/   \_\_|  \___|
                                             |___/
*/
tKnot CCA::MoveAlongArc(tOrientedArc &arc, double length) {
    // return value
    tKnot P = arc.P0;
    if (fabs(arc.prop.curvature) > 1e-8) {
        // arc is curved, move along the circle arc
        double sc = length*arc.prop.curvature;
        P.x      += ( sin(sc)*cos(arc.P0.Yaw) - (1.0-cos(sc))*sin(arc.P0.Yaw) )/arc.prop.curvature;
        P.y      += ( sin(sc)*sin(arc.P0.Yaw) + (1.0-cos(sc))*cos(arc.P0.Yaw) )/arc.prop.curvature;
        P.Yaw    += sc;
    } else {
        // segment is straight
        P.x   += length*cos(arc.P0.Yaw);
        P.y   += length*sin(arc.P0.Yaw);
        }
    // return the final point
    return P;
    }

/*
 _____ _ _    ____
|  ___(_) |_ / ___|   _ _ ____   _____
| |_  | | __| |  | | | | '__\ \ / / _ \
|  _| | | |_| |__| |_| | |   \ V /  __/
|_|   |_|\__|\____\__,_|_|    \_/ \___|
*/
void CCA::FitCurve(std::vector<tOrientedArc> &arcs, vector<tKnot> &knots) {
    // initialization
    knots.clear();
    if (arcs.size() == 0)
        return;

#define NSTEPS 10

    for (unsigned int iarc=0; iarc<arcs.size(); iarc++) {
        double arc_length = arcs[iarc].prop.length;
        // set a fixed number of steps for each arc
        for (unsigned int j=0; j<NSTEPS; j++) {
            tKnot P = MoveAlongArc(arcs[iarc], arc_length * (double(j) / double(NSTEPS)) );
            knots.push_back(P);
            }
        }
    // add the last point
    tKnot last = MoveAlongArc(arcs[arcs.size()-1], arcs[arcs.size()-1].prop.length);
    knots.push_back(last);
    }