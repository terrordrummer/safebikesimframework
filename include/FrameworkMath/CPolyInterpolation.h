#pragma once

#include "FrameworkMath/common_defines.h"
#include "FrameworkMath/MathDefines.h"
#include <deque>

namespace FrameworkMath {

    template <typename T>
    class CPolyInterpolationT {
    private:
        std::deque<T> _Xdata;
        std::deque<T> _Ydata;
        unsigned int  _order;
        bool          _init;
    public:
        CPolyInterpolationT(unsigned int order = 2);

        void SetOrder(unsigned int order);
        void PushData(T time, T y);
        T At(T time);
        void Clear();
    };

    /** \brief Time interpolation default implementation with double as numeric.                                */
    class framework_math_class CPolyInterpolation : public CPolyInterpolationT<double> {};

    /** \brief Time interpolation default implementation with float as numeric.                                 */
    class framework_math_class CPolyInterpolationFloat : public CPolyInterpolationT<float> {};

        /** \brief Time interpolation default implementation with long double as numeric.                       */
    class framework_math_class CPolyInterpolationLongDouble : public CPolyInterpolationT<long double> {};
}