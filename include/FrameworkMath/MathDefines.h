#pragma once

/** \brief Math framework.                      */
namespace FrameworkMath {

#define NAN ((long double)0x7fffffffffffffff)   /**< \brief Not a number value (IEEE.754)       */
#define ISZERO(x) (fabs(x) < 1e-12)             /**< \brief Checks if a number is zero.         */
#define INF 1e12                                /**< \brief Conventional infinite value.        */

    /** \brief 2D Point.                                                                        */
    typedef struct {
        double x;   /**< \brief x coordinate.                                                   */
        double y;   /**< \brief y coordinate.                                                   */
        } tPoint2D;

    /** \brief Knot is a 2D Point with orientation.                                             */
    typedef struct {
        double x;   /**< \brief x coordinate.                                                   */
        double y;   /**< \brief y coordinate.                                                   */
        double Yaw; /**< \brief Yaw coordinate in radiants.                                     */
        } tKnot;

    /** \brief Arc invariant properties: length and curvature.                                  */
    typedef struct {
        double length;      /**< \brief Arc's length.                                           */
        double curvature;   /**< \brief Arc's curvaure.                                         */
        } tArcProperties;

    /** \brief Oriended arc description in the 2D plane.                                        */       
    typedef struct {
        tKnot           P0;     /**< \brief Starting point with given orientation.              */
        tArcProperties  prop;   /**< \brief Arc properties.                                     */
    } tOrientedArc;

    }