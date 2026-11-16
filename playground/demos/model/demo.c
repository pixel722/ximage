#include <math.h>
#include <float.h>
#include <stdio.h>
#include <stdlib.h>

#define XI_DEBUG
#define XI_ASSERT(cond, msg)\
    if (!(cond)) { fprintf(stderr, "%s:%d: " msg "\n", __FILE__, __LINE__); exit(-1); }
#define XI_ASSERT_VA(cond, msg, ...)\
    if (!(cond)) { fprintf(stderr, "%s:%d: " msg "\n", __FILE__, __LINE__, __VA_ARGS__); exit(-1); }
#define XIMAGE_IMPLEMENTATION
#include "../../../ximage.h"


#define WIDTH  800
#define HEIGHT 800
color_t canvas[WIDTH*HEIGHT];
float buffer[WIDTH*HEIGHT];


#define PI  3.141592653589793f

float d2r(float deg)
{
    return deg*(float)PI/180.0f;
}

float xi_sinf(float x)
{
    while (x >  PI) x -= 2.0f*PI;
    while (x < -PI) x += 2.0f*PI;

    if (x >  PI/2.0f) x =  PI - x;
    if (x < -PI/2.0f) x = -PI - x;

    float y = x - x*x*x/6.0f + x*x*x*x*x/120.0f - x*x*x*x*x*x*x/5040.0f + x*x*x*x*x*x*x*x*x/362880.0f;

    return y;
}

float xi_cosf(float x)
{
    while (x >  PI) x -= 2.0f*PI;
    while (x < -PI) x += 2.0f*PI;

    if (x < 0.0f) x = -x;

    float y = 1 - x*x/2.0f + x*x*x*x/24.0f - x*x*x*x*x*x/720.f + x*x*x*x*x*x*x*x/40320.0f;

    return y;
}


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


float xscale;
float yscale;
float xoffset;
float yoffset;


v2f to_ndc(v3f world)
{
    return (v2f){ world.x / world.z, world.y / world.z };
}

v2i to_screen(v2f ndc)
{
    return (v2i){ (int)(xscale*ndc.x + xoffset), (int)(yscale*ndc.y + yoffset) };
}

v3f rotate(v3f v, float c, float s)
{
    return (v3f){
        v.x*c - v.z*s,
        v.y,
        v.x*s + v.z*c
    };
}

v3f v3f_add(v3f a, v3f b)
{
    return (v3f){ a.x+b.x, a.y+b.y, a.z+b.z };
}


void xi_draw_triangle_3z3c(
    ximage_t *xi, float *zbuffer,
    int x1, int y1,
    int x2, int y2,
    int x3, int y3,
    float z1, float z2, float z3,
    color_t c1, color_t c2, color_t c3)
{
    int xstart = xi_maxi(         0,  xi_mini(x1, xi_mini(x2, x3)));
    int xend   = xi_mini( xi->width,  xi_maxi(x1, xi_maxi(x2, x3)));
    int ystart = xi_maxi(         0,  xi_mini(y1, xi_mini(y2, y3)));
    int yend   = xi_mini(xi->height,  xi_maxi(y1, xi_maxi(y2, y3)));

    float X1 = (float)x1;
    float Y1 = (float)y1;
    float X2 = (float)x2;
    float Y2 = (float)y2;
    float X3 = (float)x3;
    float Y3 = (float)y3;

    float D = (X3-X1)*(Y2-Y1) - (X2-X1)*(Y3-Y1);
    if (D > 0.0f)
        return;

    for (int y = ystart; y < yend; ++y)
    {
        for (int x = xstart; x < xend; ++x)
        {
            float X = (float)x + 0.5f;
            float Y = (float)y + 0.5f;

            float l1 = ((X3-X)*(Y2-Y) - (X2-X)*(Y3-Y)) / D;
            float l2 = ((X1-X)*(Y3-Y) - (X3-X)*(Y1-Y)) / D;
            float l3 = 1.0f - l1 - l2;

            float z = l1*z1 + l2*z2 + l3*z3;
            float *zb = zbuffer + y*xi->width + x;

            if (l1 >= 0.0f && l2 >= 0.0f && l3 >= 0.0f && z > *zb)
            {
                color_t color = {
                    (unsigned char) (l1*(float)c1.r + l2*(float)c2.r + l3*(float)c3.r),
                    (unsigned char) (l1*(float)c1.g + l2*(float)c2.g + l3*(float)c3.g),
                    (unsigned char) (l1*(float)c1.b + l2*(float)c2.b + l3*(float)c3.b),
                    (unsigned char) (l1*(float)c1.a + l2*(float)c2.a + l3*(float)c3.a),
                };
                *XI_PIXEL(xi, x, y) = color;
                *zb = z;
            }
        }
    }
}


typedef struct
{
    int vertices_count;
    int faces_count;
    v3f *vertices;
    int *faces;
} model_t;

#include "build/utah_teapot.c"


int main(void)
{
    ximage_t img = { .width=WIDTH, .height=HEIGHT, .data=canvas };

    xscale =  (float)WIDTH / 2.0f;
    yscale = -(float)HEIGHT/ 2.0f;
    xoffset = (float)WIDTH / 2.0f;
    yoffset = (float)HEIGHT/ 2.0f;


    int   fps   = 120;
    float speed = 36.0f;

    float angle = 0.0f;
    float dt    = 1.0f / (float)fps;
    int seconds = (int)(360.0f / speed);

    static char header[100] = {0};
    static unsigned char rgb[WIDTH*HEIGHT*3] = {0};


    v3f pos = { 0.0f, -1.5f, 5.0f };
    model_t *model = &utah_teapot;


    for (int second = 0; second < seconds; ++second)
    {
        for (int frame = 0; frame < fps; ++frame)
        {
            for (int i = 0; i < WIDTH*HEIGHT; ++i) { buffer[i] = 0.0f; }

            xi_fill_bg(&img, XI_RGB(23, 22, 22));

            angle += speed*dt;
            float c = xi_cosf(d2r(angle));
            float s = xi_sinf(d2r(angle));

            for (int face = 0; face < model->faces_count; ++face)
            {
                v3f world1 = model->vertices[model->faces[face*3 + 0]];
                v3f world2 = model->vertices[model->faces[face*3 + 1]];
                v3f world3 = model->vertices[model->faces[face*3 + 2]];

                world1 = v3f_add(rotate(world1, c, s), pos);
                world2 = v3f_add(rotate(world2, c, s), pos);
                world3 = v3f_add(rotate(world3, c, s), pos);

                v2i screen1 = to_screen(to_ndc(world1));
                v2i screen2 = to_screen(to_ndc(world2));
                v2i screen3 = to_screen(to_ndc(world3));

                xi_draw_triangle_3z3c(&img, buffer,
                    screen1.x, screen1.y,
                    screen2.x, screen2.y,
                    screen3.x, screen3.y,
                    1.0f/world1.z, 1.0f/world2.z, 1.0f/world3.z,
                    XI_RGB(255, 0, 0),
                    XI_RGB(0, 255, 0),
                    XI_RGB(0, 0, 255)
                );
            }

            int header_len = snprintf(header, sizeof(header), "P6\n%d %d\n255\n", img.width, img.height);
            fwrite(header, 1, (size_t)header_len, stdout);

            for (int i = 0; i < img.width*img.height; ++i)
            {
                rgb[i*3 + 0] = img.data[i].r;
                rgb[i*3 + 1] = img.data[i].g;
                rgb[i*3 + 2] = img.data[i].b;
            }
            fwrite(rgb, 1, WIDTH*HEIGHT*3, stdout);
        }
    }

    return 0;
}
