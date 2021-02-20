#pragma once

// Converts a given hsv value to rgb
// 0 <= H <= 360
// 0 <= S <= 100
// 0 <= V <= 100
// RGB is an array with the size of 3 where the function inserts
// the rgb values like 0=r, 1=g, 2=b
// Original: https://www.codespeedy.com/hsv-to-rgb-in-cpp/ | Accessed 08.01.2021
void convertHSVtoRGB(float H, float S,float V,int rgb[]);