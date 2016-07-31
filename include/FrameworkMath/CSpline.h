#pragma once

#include "FrameworkMath/common_defines.h"
#include "FrameworkMath/MathDefines.h"
#include "FrameworkCore/CGenericType.h"

//STL includes
#include <map>

namespace FrameworkMath {

#define NOTASSIGNED NAN

    /** \brief Spline template class.
     *
     * A spline is a segmented function, each segment is continous with the next and the previous up to 
     * its n-th derivative. Spline segments are implemented as polinomial of 6° degree, connection continuity 
     * is mantained up to the 2° derivative (curvature). To define the spline just push (X,Y) points and 
     * optionally their derivatives. The derivatives that are not pushed are assigned automatically.
     * When all the data has been pushed the spline can be computed. At the end the spline value can 
     * be retrived at any X value between the minimum and maximum X value pushed. External X values for the 
     * spline will return the spline value at the closest X range.
     */
    template <typename T>
    class  CSplineT {
    protected:
        /** \brief Spline polinomial segment coefficients.                                          */
        typedef struct {
            T a5,a4,a3,a2,a1,a0;
            } _splineCoeffType;

        /** \brief Function (X,Y) coordinate.                                                       */
        std::map<T,T> _p;
        /** \brief Function first derivative.                                                       */
        std::map<T,T> _p_dot;
        /** \brief Function second derivative.                                                      */
        std::map<T,T> _p_dot_dot;
        
        /** \brief Spline coefficients.                                                             */
        std::map<T, _splineCoeffType>   _coefficients;

        /** \brief Spline computation flag.                                                         */
        bool _computed;

    public:
        /** \brief Constructor.                                                                     */
        CSplineT();

        /** \brief push new points to the spline.
         *
         * Unassigned parameters are NAN and will be computed automatically when the spline will be calculated.
         */
        void Push(T X, T F, T F_dot = (T)NOTASSIGNED, T F_dot_dot = (T)NOTASSIGNED);
        
        /** \brief Compute the spline.                                                              */
        bool ComputeSpline();

        /** \brief Returns the spline value.                                                        */
        T At(T x);

        /** \brief Clear spline data.                                                               */
        void Clear();

        /** \brief Store spline data into a generic type variable.                                  */
        FrameworkCore::CGenericType ToGenericType();

        /** \brief Restore spline data from a generic type variable.                                */
        bool FromGenericType(FrameworkCore::CGenericType &spline);

    protected:
        /** \brief Compute the spline coefficients.                                                 */
        void SolveSpline(T X0, T P0, T P0_dot, T P0_dotdot, 
                         T X1, T P1, T P1_dot, T P1_dotdot, 
                         _splineCoeffType &spline_coeff);

        /** \brief Get the spline value inside a segment.                                           */
        T GetSplineValue(_splineCoeffType &coeffs, T X);

        };

    /** \brief Spline default implementation with double as numeric.                                */
    class framework_math_class CSpline : public CSplineT<double> {};

    /** \brief Spline default implementation with float as numeric.                                 */
    class framework_math_class CSplineFloat : public CSplineT<float> {};

        /** \brief Spline default implementation with long double as numeric.                       */
    class framework_math_class CSplineLongDouble : public CSplineT<long double> {};
    }
