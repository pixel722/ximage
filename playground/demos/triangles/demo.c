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
#define HEIGHT 600
color_t canvas[WIDTH*HEIGHT];
float buffer[WIDTH*HEIGHT];


float d2r(float deg)
{
    return deg*(float)M_PI/180.0f;
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

    static char header[100] = {0};
    static unsigned char rgb[WIDTH*HEIGHT*3] = {0};

    v3f pos = { 0.0f, 0.0f, 5.0f };

    v3f blue1 = {  0.0f,  1.0f,  0.0f };
    v3f blue2 = {  1.0f, -1.0f, -1.0f };
    v3f blue3 = { -1.0f, -1.0f,  1.0f };

    v3f pink1 = {  0.0f,  1.0f,  0.0f };
    v3f pink2 = {  1.0f, -1.0f,  1.0f };
    v3f pink3 = { -1.0f, -1.0f, -1.0f };

    for (int second = 0; second < 10; ++second)
    {
        for (int frame = 0; frame < fps; ++frame)
        {
            for (int i = 0; i < WIDTH*HEIGHT; ++i) { buffer[i] = 0.0f; }

            xi_fill_bg(&img, XI_RGB(23, 22, 22));

            angle += speed*dt;
            float c = cosf(d2r(angle));
            float s = sinf(d2r(angle));

            {
                v3f world1 = v3f_add(rotate(blue1, c, s), pos);
                v3f world2 = v3f_add(rotate(blue2, c, s), pos);
                v3f world3 = v3f_add(rotate(blue3, c, s), pos);

                v2i screen1 = to_screen(to_ndc(world1));
                v2i screen2 = to_screen(to_ndc(world2));
                v2i screen3 = to_screen(to_ndc(world3));

                xi_draw_triangle_3z(&img, buffer,
                    screen1.x, screen1.y,
                    screen2.x, screen2.y,
                    screen3.x, screen3.y,
                    1.0f/world1.z, 1.0f/world2.z, 1.0f/world3.z,
                    XI_RGB(0, 0, 128)
                );

                xi_draw_triangle_3z(&img, buffer,
                    screen3.x, screen3.y,
                    screen2.x, screen2.y,
                    screen1.x, screen1.y,
                    1.0f/world3.z, 1.0f/world2.z, 1.0f/world1.z,
                    XI_RGB(0, 0, 128)
                );
            }

            {
                v3f world1 = v3f_add(rotate(pink1, c, s), pos);
                v3f world2 = v3f_add(rotate(pink2, c, s), pos);
                v3f world3 = v3f_add(rotate(pink3, c, s), pos);

                v2i screen1 = to_screen(to_ndc(world1));
                v2i screen2 = to_screen(to_ndc(world2));
                v2i screen3 = to_screen(to_ndc(world3));

                xi_draw_triangle_3z(&img, buffer,
                    screen1.x, screen1.y,
                    screen2.x, screen2.y,
                    screen3.x, screen3.y,
                    1.0f/world1.z, 1.0f/world2.z, 1.0f/world3.z,
                    XI_RGB(128, 0, 128)
                );

                xi_draw_triangle_3z(&img, buffer,
                    screen3.x, screen3.y,
                    screen2.x, screen2.y,
                    screen1.x, screen1.y,
                    1.0f/world3.z, 1.0f/world2.z, 1.0f/world1.z,
                    XI_RGB(128, 0, 128)
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
