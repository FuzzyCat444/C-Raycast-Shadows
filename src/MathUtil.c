#include "MathUtil.h"

// Reference for "arcAngle" approximation: https://www-labs.iro.umontreal.ca/~mignotte/IFT2425/Documents/EfficientApproximationArctgFunction.pdf
// Using quadratic approximation: x * (pi/4 + 0.273 * (1 - x))
// Maximum error is 0.22 degrees.
double fastAtan2(double y, double x) {
    double ax =  x < 0.0 ? -x : x;
    double ay = y < 0.0 ? -y : y;
    
    if (ay < ax) {
        double tangent = ay / ax;
        double arcAngle = tangent * 
            (MU_ATANCONSTANT2 - MU_ATANCONSTANT1 * tangent);
        if (y < 0.0) {
            if (x < 0.0) {
                return MU_PI + arcAngle;
            } else {
                return MU_2PI - arcAngle;
            }
        } else {
            if (x < 0.0) {
                return MU_PI - arcAngle;
            } else {
                return arcAngle;
            }
        }
    } else {
        double cotangent = ax / ay;
        double arcAngle = cotangent * 
            (MU_ATANCONSTANT2 - MU_ATANCONSTANT1 * cotangent);
        if (y < 0.0) {
            if (x < 0.0) {
                return MU_3PI2 - arcAngle;
            } else {
                return MU_3PI2 + arcAngle;
            }
        } else {
            if (x < 0.0) {
                return MU_PI2 + arcAngle;
            } else {
                return MU_PI2 - arcAngle;
            }
        }
    }
}