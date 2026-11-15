#include "../../ximage.h"
#include "../engine.h"
extern ximage_t img;


typedef struct
{
    v2f pos;
    v2f speed;
    color_t color;
} ball_t;

#define RADIUS 40
#define START_SPEED 500.0


static ball_t ball = {
    .pos   = (v2f){ WIDTH/2.0, HEIGHT/2.0 },
    .speed = (v2f){ START_SPEED, START_SPEED },
    .color = (color_t){ 255, 0, 0, 255 },
};

void draw_demo_a(float dt)
{
    xi_fill_bg(&img, XI_RGB(23, 22, 22));
    xi_draw_circle(&img, (int)ball.pos.x, (int)ball.pos.y, RADIUS, ball.color);

    ball.pos.x += ball.speed.x*dt;
    ball.pos.y += ball.speed.y*dt;

    if (ball.pos.x - RADIUS <= 0.0 || ball.pos.x + RADIUS >= WIDTH)
        ball.speed.x = -ball.speed.x;
    if (ball.pos.y - RADIUS <= 0.0 || ball.pos.y + RADIUS >= HEIGHT)
        ball.speed.y = -ball.speed.y;
}
