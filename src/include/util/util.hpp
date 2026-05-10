#ifndef UTIL_UTIL_HPP
#define UTIL_UTIL_HPP

// Local includes
#include <util/types.h>

#define UTIL_PI 3.141592653589793238462643383279502884197169399375105820974944592307816406286

namespace util {

float64 degToRad(float64 deg) {
    return deg * UTIL_PI / 180.0;
}
float64 radToDeg(float64 rad) {
    return rad * 180.0 / UTIL_PI;
}

} // namespace util

#endif  // UTIL_UTIL_HPP