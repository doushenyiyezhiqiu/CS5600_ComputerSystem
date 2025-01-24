#include "mpg2km.h"

/**
 * Converts miles per gallon (MPG) to kilometers per liter (KML).
 * @param mpg The mileage in miles per gallon.
 * @return The equivalent mileage in kilometers per liter.
 */
double mpg2kml(double mpg) {
    return mpg * 0.425144;
}

/**
 * Converts miles per gallon (MPG) to liters per 100 kilometers (L/100km).
 * @param mpg The mileage in miles per gallon.
 * @return The number of liters used per 100 kilometers.
 *         Returns 0 if mpg is 0 to prevent division by zero.
 */
double mpg2lphm(double mpg) {
    if (mpg == 0) return 0;  // Prevent division by zero
    return 235.214583 / mpg;
}

/**
 * Converts liters per 100 kilometers (L/100km) to miles per gallon (MPG).
 * @param lphm The fuel consumption in liters per 100 kilometers.
 * @return The equivalent mileage in miles per gallon.
 *         Returns 0 if lphm is 0 to prevent division by zero.
 */
double lph2mpg(double lphm) {
    if (lphm == 0) return 0;  // Prevent division by zero
    return 235.214583 / lphm;
}
