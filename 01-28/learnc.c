/*
* learnc.c / Practice Assignment
*
* Liangshe Li / CS5600 / Northeastern University
* Spring 2025 / Jan 21, 2025
*
*/

#include <stdio.h>
#include "mpg2km.h"  // Include the header for the conversion functions

/**
 * Main function to test the fuel efficiency conversion functions.
 * This function demonstrates usage of mpg2kml, mpg2lphm, and lph2mpg, and it tests
 * them with standard and edge case inputs.
 */
int main() {
    // Display conversion results for typical inputs
    printf("30 MPG to KML: %f\n", mpg2kml(30));
    printf("30 MPG to LPHM: %f\n", mpg2lphm(30));
    printf("8 LPHM to MPG: %f\n", lph2mpg(8));

    // Testing with edge cases to handle zero inputs
    printf("0 MPG to LPHM (edge case): %f\n", mpg2lphm(0));
    printf("0 LPHM to MPG (edge case): %f\n", lph2mpg(0));

    return 0; // Return successful execution status
}
