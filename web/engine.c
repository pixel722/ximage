#define XIMAGE_IMPLEMENTATION
#include "../ximage.h"

#include "engine.h"


color_t canvas[WIDTH*HEIGHT];
ximage_t img = { .width=WIDTH, .height=HEIGHT, .data=canvas };


float d2r(float deg)
{
    return deg*(float)PI/180.0f;
}

float sinf(float x)
{
    while (x >  PI) x -= 2.0f*PI;
    while (x < -PI) x += 2.0f*PI;

    if (x >  PI/2.0f) x =  PI - x;
    if (x < -PI/2.0f) x = -PI - x;

    float y = x - x*x*x/6.0f + x*x*x*x*x/120.0f - x*x*x*x*x*x*x/5040.0f + x*x*x*x*x*x*x*x*x/362880.0f;

    return y;
}

float cosf(float x)
{
    while (x >  PI) x -= 2.0f*PI;
    while (x < -PI) x += 2.0f*PI;

    if (x < 0.0f) x = -x;

    float y = 1 - x*x/2.0f + x*x*x*x/24.0f - x*x*x*x*x*x/720.f + x*x*x*x*x*x*x*x/40320.0f;

    return y;
}
