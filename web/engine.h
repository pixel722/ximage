#ifndef DEMO_H
#define DEMO_H


#define WIDTH 800
#define HEIGHT 600

#define PI  3.141592653589793f


typedef struct
{
    float x;
    float y;
    float z;
} v3f;

typedef struct
{
    float x;
    float y;
} v2f;

typedef struct
{
    int x;
    int y;
} v2i;


float d2r(float deg);
float sinf(float x);
float cosf(float x);

#endif
