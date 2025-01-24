#ifndef MPG2KM_H
#define MPG2KM_H

/**
 * Converts miles per gallon (MPG) to kilometers per liter (KML).
 * @param mpg The mileage in miles per gallon.
 * @return The equivalent mileage in kilometers per liter.
 */
double mpg2kml(double mpg);

/**
 * Converts miles per gallon (MPG) to liters per 100 kilometers (L/100km).
 * This is useful for converting to a measurement commonly used outside the U.S.
 * @param mpg The mileage in miles per gallon.
 * @return The number of liters used per 100 kilometers. Returns 0 to handle division by zero.
 */
double mpg2lphm(double mpg);

/**
 * Converts liters per 100 kilometers (L/100km) to miles per gallon (MPG).
 * @param lphm The fuel consumption in liters per 100 kilometers.
 * @return The equivalent mileage in miles per gallon. Returns 0 to handle division by zero.
 */
double lph2mpg(double lphm);

#endif // MPG2KM_H
