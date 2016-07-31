#include "FrameworkMath/CPolyInterpolation.h"

using namespace std;
using namespace FrameworkMath;

template <typename T>
CPolyInterpolationT<T>::CPolyInterpolationT(unsigned int order) {
    _order = order+1;
    _init = false;
}

template <typename T>
void CPolyInterpolationT<T>::SetOrder(unsigned int order) {
    _order = order+1;
    Clear();
}

template <typename T>
void CPolyInterpolationT<T>::PushData(T x, T y) {
    if (!_init) {
        for (int i=_order-1; i>0; i--) {
            // initialize the queue with duplicate elements
            _Xdata.push_front(x-T(i));
            _Ydata.push_front(y);
        } 
    }
    else {
        // remove the last element
        _Xdata.pop_back();
        _Ydata.pop_back();
    }

    // add the new element
    _Xdata.push_front(x);
    _Ydata.push_front(y);

    // mark queue as initialized
    _init = true;
}

template <typename T>
T CPolyInterpolationT<T>::At(T x) {
    T res = 0;
    for (unsigned int i=0; i<_order; i++) {
        T Term_i = _Ydata.at(i);
        T x_i    = _Xdata.at(i);
        for (unsigned int j=0; j<_order; j++) {
            if (i != j) {
                T x_j = _Xdata.at(j);
                Term_i = Term_i*(x - x_j) / (x_i - x_j);
            }
        }
        res += Term_i;
    }
    return res;
}

template <typename T>
void CPolyInterpolationT<T>::Clear() {
    _Xdata.clear();
    _Ydata.clear();
    _init = false;
}

