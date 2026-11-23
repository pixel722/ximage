#include "../../ximage.h"
#include "../engine.h"
extern ximage_t img;


typedef struct
{
    int vertices_count;
    int normales_count;
    int textures_count;
    int faces_count;

    v3f *vertices;
    v3f *normales;
    v2f *textures;
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


static v2f to_ndc(v3f world)
{
    return (v2f){ world.x / world.z, world.y / world.z };
}

static v2i to_screen(v2f ndc)
{
    return (v2i){ (int)(xscale*ndc.x + xoffset), (int)(yscale*ndc.y + yoffset) };
}

static v3f rotate_x(v3f v, float c, float s)
{
    return (v3f){
        v.x,
        v.y*c - v.z*s,
        v.y*s + v.z*c
    };
}

static v3f rotate_y(v3f v, float c, float s)
{
    return (v3f){
        v.x*c - v.z*s,
        v.y,
        v.x*s + v.z*c
    };
}

static v3f rotate_z(v3f v, float c, float s)
{
    return (v3f){
        v.x*c - v.y*s,
        v.x*s + v.y*c,
        v.z
    };
}

static v3f v3f_add(v3f a, v3f b)
{
    return (v3f){ a.x+b.x, a.y+b.y, a.z+b.z };
}


void draw_demo_c(float dt)
{
    for (int i = 0; i < WIDTH*HEIGHT; ++i) { zbuffer[i] = 0.0f; }

    xi_fill_bg(&img, XI_RGB(23, 22, 22));

    angle += speed*dt;
    float c = cosf(d2r(angle));
    float s = sinf(d2r(angle));

    for (int face = 0; face < model->faces_count; face += 9)
    {
        v3f world1 = model->vertices[model->faces[face + 0]];
        v3f world2 = model->vertices[model->faces[face + 3]];
        v3f world3 = model->vertices[model->faces[face + 6]];

        world1 = v3f_add(rotate_y(world1, c, s), pos);
        world2 = v3f_add(rotate_y(world2, c, s), pos);
        world3 = v3f_add(rotate_y(world3, c, s), pos);

        v2i screen1 = to_screen(to_ndc(world1));
        v2i screen2 = to_screen(to_ndc(world2));
        v2i screen3 = to_screen(to_ndc(world3));

        xi_draw_triangle_3z3c(&img, zbuffer,
            screen1.x, screen1.y,
            screen2.x, screen2.y,
            screen3.x, screen3.y,
            1.0f/world1.z, 1.0f/world2.z, 1.0f/world3.z,
            XI_RGB(255, 0, 0),
            XI_RGB(180, 0, 0),
            XI_RGB(120, 0, 0)
        );
    }
}
