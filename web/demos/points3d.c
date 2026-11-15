#include "../../ximage.h"
#include "../engine.h"
extern ximage_t img;


static float xscale =  (float)WIDTH / 2.0f;
static float yscale = -(float)HEIGHT/ 2.0f;
static float xoffset = (float)WIDTH / 2.0f;
static float yoffset = (float)HEIGHT/ 2.0f;

static float position[] = { 0.0f, 0.0f, 5.0f };
static float points[][3] = {
 {  1.0f,  1.0f, -1.0f },
 {  1.0f, -1.0f, -1.0f },
 { -1.0f, -1.0f, -1.0f },
 { -1.0f,  1.0f, -1.0f },

 {  1.0f,  1.0f, 1.0f },
 {  1.0f, -1.0f, 1.0f },
 { -1.0f, -1.0f, 1.0f },
 { -1.0f,  1.0f, 1.0f },
};
static int points_count = sizeof(points) / sizeof(*points);

static float angle = 0.0f;
static float speed = 36.0f;


void draw_demo_b(float dt)
{
    xi_fill_bg(&img, XI_RGB(23, 22, 22));

    angle += speed*dt;
    float c = cosf(d2r(angle));
    float s = sinf(d2r(angle));

    for (int i = 0; i < points_count; ++i)
    {
        float world[] = { points[i][0], points[i][1], points[i][2] };
        float x = world[0];
        float z = world[2];
        world[0] =  x*c - z*s;
        world[2] =  x*s + z*c;

        world[0] += position[0];
        world[1] += position[1];
        world[2] += position[2];

        float ndc[] = { world[0]/world[2], world[1]/world[2] };
        int screen[] = {
            (int)(xscale*ndc[0] + xoffset),
            (int)(yscale*ndc[1] + yoffset)
        };

        xi_draw_circle(&img, screen[0], screen[1], 10, XI_RGB(255, 0, 0));
    }
}
