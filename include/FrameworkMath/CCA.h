#pragma once


#include "FrameworkMath/common_defines.h"
#include "FrameworkMath/MathDefines.h"

//STL includes
#include <vector>

namespace FrameworkMath {

    /** \brief Constant Curvature Interpolation class.
     *
     * This class implements the methods needed to interpolate a sequence of points in the plane using a sequence of
     * arcs.
     */
    class framework_math_class CCA {
    public:

        /** \brief Connects two oriented points with two arcs.
         *
         * This method takes two oriented points as input and finds two tangent arcs that connect them. If necessary,
         * the yaw angle of the initial or final (or both) points are rotated of ±pi as needed to find the shortest
         * solution in term of length.
         */
        static void LinkKnot(   tKnot           &P0,
                                tKnot           &P1, 
                                tArcProperties  &arc1,
                                tArcProperties  &arc2,
                                double          &knot1yaw, 
                                double          &knot2yaw);

        /** \brief Interpolates a sequente of 2D points with a sequence of arcs.                        */
        static bool Interpolate2DPoints(std::vector<tPoint2D>  &points2D, std::vector<tOrientedArc> &arcs);

        /** \brief Interpolates a sequence of oriented points with a sequence of arcs.                  */
        static bool InterpolateKnots(std::vector<tKnot>  &knots, std::vector<tOrientedArc> &arcs);

        /** \brief Move along a constant curvilinear arc.                                               */
        static tKnot MoveAlongArc(tOrientedArc &arc, double length);

        /** \brief Returns a fitted set of points along the arc sequence.                               */
        static void FitCurve(std::vector<tOrientedArc> &arcs, std::vector<tKnot> &knots);
    };

}