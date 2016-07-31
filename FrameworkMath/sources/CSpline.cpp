#include "FrameworkMath/CSpline.h"

using namespace FrameworkMath;
using namespace FrameworkCore;


/*
  ____ ____        _ _           _____
 / ___/ ___| _ __ | (_)_ __   __|_   _|
| |   \___ \| '_ \| | | '_ \ / _ \| |
| |___ ___) | |_) | | | | | |  __/| |
 \____|____/| .__/|_|_|_| |_|\___||_|
            |_|
*/
template <typename T>
CSplineT<T>::CSplineT() : _computed(false) {
    Clear();
    }

/*
 ____            _
|  _ \ _   _ ___| |__
| |_) | | | / __| '_ \
|  __/| |_| \__ \ | | |
|_|    \__,_|___/_| |_|
*/
template <typename T>
void CSplineT<T>::Push(T X, T P, T P_dot = (T)NOTASSIGNED, T P_dot_dot = (T)NOTASSIGNED) {
    // push elements into the spline data containers
    _p[X] = P;
    _p_dot[X] = P_dot;
    if (P_dot_dot == (T)NOTASSIGNED)
        _p_dot_dot[X] = 0.0;
    else
        _p_dot_dot[X] = P_dot_dot;
    }

/*
  ____                            _       ____        _ _
 / ___|___  _ __ ___  _ __  _   _| |_ ___/ ___| _ __ | (_)_ __   ___
| |   / _ \| '_ ` _ \| '_ \| | | | __/ _ \___ \| '_ \| | | '_ \ / _ \
| |__| (_) | | | | | | |_) | |_| | ||  __/___) | |_) | | | | | |  __/
 \____\___/|_| |_| |_| .__/ \__,_|\__\___|____/| .__/|_|_|_| |_|\___|
                     |_|                       |_|
*/
template <typename T>
bool CSplineT<T>::ComputeSpline() {
    // at least one point is needed, if not present insert origin
    if (_p.size() == 0)
        Push(0,0,0,0);
    // add two virtual points at the beginning and at the end
    Push( (T) INF, (T)0, (T)0, (T)0);
    // clear previously created spline coefficients
    _coefficients.clear();
    // autimatic computation of unassigned data values
    // dot vectors
    std::map<T,T>::iterator it = _p_dot.begin();
    while (it != _p_dot.end()) {
        if (it->second == NOTASSIGNED) {
            std::map<T,T>::iterator it_prev = it;
            std::map<T,T>::iterator it_next = it;
            it_prev--;
            it_next++;
            // first derivative has to be computed
            if (it == _p_dot.begin())
                // element is at the beginning, derivative is null
                it->second = (T)0.0;
            else if (it_next == _p_dot.end())
                // element is the last, assign zero
                it->second = (T)0.0;
            else
                // element is in the middle, take the tangent of the line connecting the previous and the next
                it->second = (_p[it_next->first] - _p[it_prev->first]) / (it_next->first - it_prev->first);
            }
        // next element
        it++;
        }
    it = _p_dot_dot.begin();
    while (it != _p_dot_dot.end()) {
        if (it->second == NOTASSIGNED) {
            std::map<T,T>::iterator it_prev = it;
            std::map<T,T>::iterator it_next = it;
            it_prev--;
            it_next++;
            // second derivative has to be computed
            if (it == _p_dot_dot.begin())
                // element is at the beginning, derivative is null
                it->second = (T)0.0;
            else if (it_next == _p_dot_dot.end())
                // element is the last, assign zero
                it->second = (T)0.0;
            else
                // element is in the middle, take the tangent of the line connecting the previous and the next
                it->second = (_p_dot[it_next->first] - _p_dot[it_prev->first]) / (it_next->first - it_prev->first);
            }
        // next element
        it++;
        }
    // Data is ready, compute the spline sequence
    it = ++_p.begin();
    while (it != _p.end()) {
        std::map<T,T>::iterator it_prev = it;
        it_prev--;
        // solve spline coefficients for this segment
        _splineCoeffType spline_coeff;
        T iP0 = it_prev->first;
        T iP1 = it->first;
        SolveSpline(iP0, _p[iP0], _p_dot[iP0], _p_dot_dot[iP0],
                    iP1, _p[iP1], _p_dot[iP1], _p_dot_dot[iP1],
                    spline_coeff);
        // store spline coefficients
        _coefficients[iP0] = spline_coeff;
        // move to next spline segment
        it++;
        }
    // Spline DONE!
    _computed = true;
    return true;
    }

/*
    _   _
   / \ | |_
  / _ \| __|
 / ___ \ |_
/_/   \_\__|
*/
template <typename T>
T CSplineT<T>::At(T x) {
    // if not computed then returns zero value
    if (!_computed)
        return (T)0.0;
    // look for the segment interval
    std::map<T, _splineCoeffType>::iterator it = _coefficients.upper_bound(x);
    // compute spline value
    if (it == _coefficients.begin())
        // return initial spline value if out of range
        return it->second.a0;
    else if (it == _coefficients.end())
        // the last virtual point obtained, it means that X is out of range, return last spline value
        return (--it)->second.a0;
    else {
        it--;
        return GetSplineValue(it->second, x - it->first);
        }
    }

/*
  ____ _
 / ___| | ___  __ _ _ __
| |   | |/ _ \/ _` | '__|
| |___| |  __/ (_| | |
 \____|_|\___|\__,_|_|
*/
template <typename T>
void CSplineT<T>::Clear() {
    _coefficients.clear();
    _p.clear();
    _p_dot.clear();
    _p_dot_dot.clear();
    _computed = false;
    }

/*
 _____      ____                      _     _____
|_   _|__  / ___| ___ _ __   ___ _ __(_) __|_   _|   _ _ __   ___
  | |/ _ \| |  _ / _ \ '_ \ / _ \ '__| |/ __|| || | | | '_ \ / _ \
  | | (_) | |_| |  __/ | | |  __/ |  | | (__ | || |_| | |_) |  __/
  |_|\___/ \____|\___|_| |_|\___|_|  |_|\___||_| \__, | .__/ \___|
                                                 |___/|_|
*/
template <typename T>
CGenericType CSplineT<T>::ToGenericType() {
    CGenericType spline;
    // cycle through map and store the data
    std::map<T, _splineCoeffType>::iterator it = _coefficients.begin();
    for (unsigned int i=0; i<_coefficients.size(); i++) {
        spline[i] % "x"  = it->first;
        spline[i] % "a5" = it->second.a5;
        spline[i] % "a4" = it->second.a4;
        spline[i] % "a3" = it->second.a3;
        spline[i] % "a2" = it->second.a2;
        spline[i] % "a1" = it->second.a1;
        spline[i] % "a0" = it->second.a0;
        }
    return spline;
    }

/*
 _____                     ____                      _     _____
|  ___| __ ___  _ __ ___  / ___| ___ _ __   ___ _ __(_) __|_   _|   _ _ __   ___
| |_ | '__/ _ \| '_ ` _ \| |  _ / _ \ '_ \ / _ \ '__| |/ __|| || | | | '_ \ / _ \
|  _|| | | (_) | | | | | | |_| |  __/ | | |  __/ |  | | (__ | || |_| | |_) |  __/
|_|  |_|  \___/|_| |_| |_|\____|\___|_| |_|\___|_|  |_|\___||_| \__, | .__/ \___|
                                                                |___/|_|
*/
template <typename T>
bool CSplineT<T>::FromGenericType(CGenericType &spline) {
    // clear the spline
    Clear();
    // load spline data
    for (unsigned int i=0; i<spline.size(); i++) {
        if (!spline[i].hasField("x")) {
            // this is an error condition
            Clear();
            return false;
            }
        // ok X data exist, store the spline segment data
        T x = (T)(spline[i] % "x").toNumber();
        _splineCoeffType splineCoeff;
        splineCoeff.a5 = (T)(spline[i] % "a5").toNumber();
        splineCoeff.a4 = (T)(spline[i] % "a4").toNumber();
        splineCoeff.a3 = (T)(spline[i] % "a3").toNumber();
        splineCoeff.a2 = (T)(spline[i] % "a2").toNumber();
        splineCoeff.a1 = (T)(spline[i] % "a1").toNumber();
        splineCoeff.a0 = (T)(spline[i] % "a0").toNumber();
        _coefficients[x] = splineCoeff;
        }
    // ok
    return true;
    }

/*
 ____        _           ____        _ _
/ ___|  ___ | |_   _____/ ___| _ __ | (_)_ __   ___
\___ \ / _ \| \ \ / / _ \___ \| '_ \| | | '_ \ / _ \
 ___) | (_) | |\ V /  __/___) | |_) | | | | | |  __/
|____/ \___/|_| \_/ \___|____/| .__/|_|_|_| |_|\___|
                              |_|
*/
template <typename T>
void CSplineT<T>::SolveSpline(T X0, T P0, T P0_dot, T P0_dotdot, 
                             T X1, T P1, T P1_dot, T P1_dotdot, 
                             _splineCoeffType &spline_coeff) {
    // compute spline solution
    T L = X1-X0;

    spline_coeff.a0 =  P0;
    spline_coeff.a1 =  P0_dot;
    spline_coeff.a2 =  T(0.5)*P0_dotdot;
    spline_coeff.a3 = -T(0.5)*(-T(20.0)*P1 + T(3.0)*P1_dotdot*L*L + T(12.0)*P0_dot*L + T(20.0)*P0 -        L*L*P1_dotdot + T( 8.0)*L*P1_dot ) / L / L / L;
    spline_coeff.a4 =  T(0.5)*(-T(30.0)*P1 + T(3.0)*P0_dotdot*L*L + T(16.0)*P0_dot*L + T(30.0)*P0 - T(2.0)*L*L*P1_dotdot + T(14.0)*L*P1_dot ) / L / L / L / L;
    spline_coeff.a5 = -T(0.5)*(-T(12.0)*P1 +        P0_dotdot*L*L + T( 6.0)*P0_dot*L + T(12.0)*P0 -        L*L*P1_dotdot + T( 6.0)*L*P1_dot ) / L / L / L / L / L;
    }

/*
  ____      _   ____        _ _          __     __    _
 / ___| ___| |_/ ___| _ __ | (_)_ __   __\ \   / /_ _| |_   _  ___
| |  _ / _ \ __\___ \| '_ \| | | '_ \ / _ \ \ / / _` | | | | |/ _ \
| |_| |  __/ |_ ___) | |_) | | | | | |  __/\ V / (_| | | |_| |  __/
 \____|\___|\__|____/| .__/|_|_|_| |_|\___| \_/ \__,_|_|\__,_|\___|
                     |_|
*/
template <typename T>
T CSplineT<T>::GetSplineValue(_splineCoeffType &coeffs, T L) {
    T res = (T)coeffs.a0;
    T t2,t3,t4,t5;

    res += coeffs.a1 * L;

    t2 = L*L;
    res += coeffs.a2 * t2;

    t3=t2*L;
    res += coeffs.a3 * t3;

    t4=t3*L;
    res += coeffs.a4 * t4;

    t5=t4*L;
    res += coeffs.a5 * t5;

    return res;
    }