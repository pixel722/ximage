#include "../../ximage.h"
#include "../engine.h"
extern ximage_t img;


typedef struct
{
    int vertices_count;
    int faces_count;
    v3f *vertices;
    int *faces;
} model_t;

#include "../build/utah_teapot.c"


static float speed = 36.0f;
static float angle = 0.0f;

static float xscale =  (float)WIDTH / 2.0f;
static float yscale = -(float)HEIGHT/ 2.0f;
static float xoffset = (float)WIDTH / 2.0f;
static float yoffset = (float)HEIGHT/ 2.0f;

static float zbuffer[WIDTH*HEIGHT];

static v3f pos = { 0.0f, -1.5f, 5.0f };
static model_t *model = &utah_teapot;


static void xi_draw_triangle_3z3c(
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
    float recD = 1.0f/D;

    z1 = 1.0f / z1;
    z2 = 1.0f / z2;
    z3 = 1.0f / z3;

    for (int y = ystart; y < yend; ++y)
    {
        for (int x = xstart; x < xend; ++x)
        {
            float X = (float)x + 0.5f;
            float Y = (float)y + 0.5f;
            float l1 = ((X3-X)*(Y2-Y) - (X2-X)*(Y3-Y)) * recD;
            float l2 = ((X1-X)*(Y3-Y) - (X3-X)*(Y1-Y)) * recD;
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


static v2f to_ndc(v3f world)
{
    return (v2f){ world.x / world.z, world.y / world.z };
}

static v2i to_screen(v2f ndc)
{
    return (v2i){ (int)(xscale*ndc.x + xoffset), (int)(yscale*ndc.y + yoffset) };
}

static v3f rotate(v3f v, float c, float s)
{
    return (v3f){
        v.x*c - v.z*s,
        v.y,
        v.x*s + v.z*c
    };
}

static v3f v3f_add(v3f a, v3f b)
{
    return (v3f){ a.x+b.x, a.y+b.y, a.z+b.z };
}


void draw_demo_c(float dt)
{
    //for (int i = 0; i < WIDTH*HEIGHT; ++i) { buffer[i] = FLT_MAX; }
    for (int i = 0; i < WIDTH*HEIGHT; ++i) { zbuffer[i] = 0.0f; }

    xi_fill_bg(&img, XI_RGB(23, 22, 22));

    angle += speed*dt;
    float c = cosf(d2r(angle));
    float s = sinf(d2r(angle));

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

        xi_draw_triangle_3z3c(&img, zbuffer,
            screen1.x, screen1.y,
            screen2.x, screen2.y,
            screen3.x, screen3.y,
            world1.z, world2.z, world3.z,
            XI_RGB(255, 0, 0),
            XI_RGB(180, 0, 0),
            XI_RGB(120, 0, 0)
        );
    }
}
