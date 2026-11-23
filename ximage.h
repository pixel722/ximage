#ifndef XIMAGE_H
#define XIMAGE_H


typedef struct
{
    unsigned char r;
    unsigned char g;
    unsigned char b;
    unsigned char a;
} color_t;

typedef struct
{
    int width, height;
    color_t *data;
} ximage_t;


void xi_fill_bg(ximage_t *xi, color_t color);
void xi_put_pixel(ximage_t *xi, int x, int y, color_t color);
void xi_draw_rect(ximage_t *xi, int x0, int y0, int width, int height, color_t color);
void xi_draw_circle(ximage_t *xi, int x0, int y0, int r, color_t color);
void xi_draw_triangle(ximage_t *xi, int x1, int y1, int x2, int y2, int x3, int y3, color_t color);
void xi_draw_triangle_3c(ximage_t *xi, int x1, int y1, int x2, int y2, int x3, int y3, color_t c1, color_t c2, color_t c3);
void xi_draw_triangle_3z3c(ximage_t *xi, float *zbuffer, int x1, int y1, int x2, int y2, int x3, int y3, float z1, float z2, float z3, color_t c1, color_t c2, color_t c3);
void xi_draw_texture(ximage_t *xi, ximage_t *texture, int x0, int y0, int width, int height);

int xi_maxi(int a, int b);
int xi_mini(int a, int b);

float xi_maxf(float a, float b);
float xi_minf(float a, float b);

int   xi_absi(int a);
float xi_absf(float a);

float xi_d2r(float deg);
float xi_sinf(float x);
float xi_cosf(float x);


#define XI_NULL ((void *)0)
#define XI_PI  3.141592653589793f

#define XI_RGBA(R, G, B, A) (color_t){ .r=(R), .g=(G), .b=(B), .a=(A) }
#define XI_RGB(R, G, B) (color_t){ .r=(R), .g=(G), .b=(B), .a=255 }

#ifndef XI_ASSERT
    #define XI_ASSERT(cond, msg)
#endif // #ifndef XI_ASSERT

#ifndef XI_ASSERT_VA
    #define XI_ASSERT_VA(cond, msg, ...)
#endif // #ifndef XI_ASSERT_VA

#ifdef XI_DEBUG
    color_t *xi_pixel_debug(ximage_t *xi, int x, int y, char *file, int line)
    {
        XI_ASSERT_VA((x) >= 0 && (x) < (xi)->width, "x out of bounce: %d [%d, %d), in %s:%d", x, 0, xi->width, file, line);
        XI_ASSERT_VA((y) >= 0 && (y) < (xi)->height, "y out of bounce: %d [%d, %d), in %s:%d", y, 0, xi->height, file, line);
        return xi->data + y*xi->width + x;
    }
    #define XI_PIXEL(xi, x, y) xi_pixel_debug(xi, x, y, __FILE__, __LINE__)
#else
    #define XI_PIXEL(xi, x, y) ((xi)->data + (y)*(xi)->width + (x))
#endif


// ================================================================================
#ifdef XIMAGE_IMPLEMENTATION


static inline color_t blend_colors(color_t fg, color_t bg)
{
    color_t color = {
        .r = (unsigned char)((fg.r*fg.a + bg.r*(255 - fg.a))/255),
        .b = (unsigned char)((fg.b*fg.a + bg.b*(255 - fg.a))/255),
        .g = (unsigned char)((fg.g*fg.a + bg.g*(255 - fg.a))/255),
        .a = bg.a
    };
    return color;
}


// ==================== MATH ==================== //

int xi_maxi(int a, int b)
{
    return a > b? a : b;
}

int xi_mini(int a, int b)
{
    return a < b? a : b;
}

float xi_maxf(float a, float b)
{
    return a > b? a : b;
}

float xi_minf(float a, float b)
{
    return a < b? a : b;
}

int xi_absi(int a)
{
    return a >= 0? a : -a;
}

float xi_absf(float a)
{
    return a >= 0.0f? a : -a;
}

float xi_d2r(float deg)
{
    return deg*(float)XI_PI/180.0f;
}

float xi_sinf(float x)
{
    while (x >  XI_PI) x -= 2.0f*XI_PI;
    while (x < -XI_PI) x += 2.0f*XI_PI;

    if (x >  XI_PI/2.0f) x =  XI_PI - x;
    if (x < -XI_PI/2.0f) x = -XI_PI - x;

    float y = x - x*x*x/6.0f + x*x*x*x*x/120.0f - x*x*x*x*x*x*x/5040.0f + x*x*x*x*x*x*x*x*x/362880.0f;

    return y;
}

float xi_cosf(float x)
{
    while (x >  XI_PI) x -= 2.0f*XI_PI;
    while (x < -XI_PI) x += 2.0f*XI_PI;

    if (x < 0.0f) x = -x;

    float y = 1 - x*x/2.0f + x*x*x*x/24.0f - x*x*x*x*x*x/720.f + x*x*x*x*x*x*x*x/40320.0f;

    return y;
}


// ==================== 2D PRIMITIVES ==================== //

void xi_fill_bg(ximage_t *xi, color_t color)
{
    for (int i = 0; i < xi->width*xi->height; ++i)
        xi->data[i] = color;
}

void xi_put_pixel(ximage_t *xi, int x, int y, color_t color)
{
    if (x >= 0 && x < xi->width && y >= 0 && y < xi->height)
    {
        *XI_PIXEL(xi, x, y) = color;
    }
}

void xi_draw_rect(ximage_t *xi, int x0, int y0, int width, int height, color_t color)
{
    int xstart = x0 >= 0? x0 : 0;
    int xend   = x0+width < xi->width? x0+width : xi->width;
    int ystart = y0 >= 0? y0 : 0;
    int yend   = y0+height < xi->height? y0+height : xi->height;

    for (int y = ystart; y < yend; ++y)
    {
        for (int x = xstart; x < xend; ++x)
        {
            *XI_PIXEL(xi, x, y) = color;
        }
    }
}

void xi_draw_circle(ximage_t *xi, int x0, int y0, int r, color_t color)
{
    if (r == 0)
    {
        xi_put_pixel(xi, x0, y0, color);
        return;
    }

    int xstart = x0-r >= 0? x0-r : 0;
    int xend   = x0+r < xi->width? x0+r+1 : xi->width;
    int ystart = y0-r >= 0? y0-r : 0;
    int yend   = y0+r < xi->height? y0+r+1 : xi->height;
    double rsq = (double)r * (double)r;

    for (int y = ystart; y < yend; ++y)
    {
        for (int x = xstart; x < xend; ++x)
        {
            double mx = (double)x - (double)x0 + 0.5;
            double my = (double)y - (double)y0 + 0.5;
            if (mx*mx + my*my <= rsq)
            {
                *XI_PIXEL(xi, x, y) = color;
            }
        }
    }
}

typedef struct
{
    int xstart;
    int xend;
    int ystart;
    int yend;

    float X1;
    float Y1;
    float X2;
    float Y2;
    float X3;
    float Y3;

    float D;

    float X;
    float Y;
    float l1;
    float l2;
    float l3;
} xi_triangle_t;

static inline int xi_init_triangle(xi_triangle_t *tri, ximage_t *xi, int x1, int y1, int x2, int y2, int x3, int y3)
{
    tri->xstart = xi_maxi(         0,  xi_mini(x1, xi_mini(x2, x3)));
    tri->xend   = xi_mini( xi->width,  xi_maxi(x1, xi_maxi(x2, x3)));
    tri->ystart = xi_maxi(         0,  xi_mini(y1, xi_mini(y2, y3)));
    tri->yend   = xi_mini(xi->height,  xi_maxi(y1, xi_maxi(y2, y3)));

    tri->X1 = (float)x1;
    tri->Y1 = (float)y1;
    tri->X2 = (float)x2;
    tri->Y2 = (float)y2;
    tri->X3 = (float)x3;
    tri->Y3 = (float)y3;

    tri->D = (tri->X3 - tri->X1)*(tri->Y2 - tri->Y1) - (tri->X2 - tri->X1)*(tri->Y3 - tri->Y1);

    if (xi_absf(tri->D) < 1.0e-12f)
    {
        return 0;
    }

    return 1;
}

static inline int xi_point_in_triangle(xi_triangle_t *tri, int x, int y)
{
    float X = (float)x + 0.5f;
    float Y = (float)y + 0.5f;
    tri->l1 = ((tri->X3 - X)*(tri->Y2 - Y) - (tri->X2 - X)*(tri->Y3 - Y)) / tri->D;
    tri->l3 = ((tri->X2 - X)*(tri->Y1 - Y) - (tri->X1 - X)*(tri->Y2 - Y)) / tri->D;
    tri->l2 = 1.0f - tri->l1 - tri->l3;

    if (tri->l1 >= 0.0f && tri->l2 >= 0.0f && tri->l3 >= 0.0f)
        return 1;

    return 0;
}

color_t xi_lerp_colors(xi_triangle_t *tri, color_t c1, color_t c2, color_t c3)
{
    color_t color = {0};

    color.r = (unsigned char) (tri->l1*(float)c1.r + tri->l2*(float)c2.r + tri->l3*(float)c3.r);
    color.g = (unsigned char) (tri->l1*(float)c1.g + tri->l2*(float)c2.g + tri->l3*(float)c3.g);
    color.b = (unsigned char) (tri->l1*(float)c1.b + tri->l2*(float)c2.b + tri->l3*(float)c3.b);
    color.a = (unsigned char) (tri->l1*(float)c1.a + tri->l2*(float)c2.a + tri->l3*(float)c3.a);

    return color;
}

void xi_draw_triangle(ximage_t *xi, int x1, int y1, int x2, int y2, int x3, int y3, color_t color)
{
    xi_triangle_t tri = {0};
    if (xi_init_triangle(&tri, xi, x1, y1, x2, y2, x3, y3) == 0)
        return;

    for (int y = tri.ystart; y < tri.yend; ++y)
        for (int x = tri.xstart; x < tri.xend; ++x)
            if (xi_point_in_triangle(&tri, x, y))
                *XI_PIXEL(xi, x, y) = color;
}

void xi_draw_triangle_3c(ximage_t *xi, int x1, int y1, int x2, int y2, int x3, int y3, color_t c1, color_t c2, color_t c3)
{
    xi_triangle_t tri = {0};
    if (xi_init_triangle(&tri, xi, x1, y1, x2, y2, x3, y3) == 0)
        return;

    for (int y = tri.ystart; y < tri.yend; ++y)
        for (int x = tri.xstart; x < tri.xend; ++x)
            if (xi_point_in_triangle(&tri, x, y))
                *XI_PIXEL(xi, x, y) = xi_lerp_colors(&tri, c1, c2, c3);
}

void xi_draw_triangle_3z(ximage_t *xi, float *zbuffer, int x1, int y1, int x2, int y2, int x3, int y3, float z1, float z2, float z3, color_t color)
{
    xi_triangle_t tri = {0};
    if (xi_init_triangle(&tri, xi, x1, y1, x2, y2, x3, y3) == 0)
        return;

    for (int y = tri.ystart; y < tri.yend; ++y)
        for (int x = tri.xstart; x < tri.xend; ++x)
            if (xi_point_in_triangle(&tri, x, y))
                if (tri.D < 0)
                {
                    float z = tri.l1*z1 + tri.l2*z2 + tri.l3*z3;
                    float *zb = zbuffer + y*xi->width + x;
                    if (z > *zb)
                    {
                        *XI_PIXEL(xi, x, y) = color;
                        *zb = z;
                    }
                }
}

void xi_draw_triangle_3z3c(ximage_t *xi, float *zbuffer, int x1, int y1, int x2, int y2, int x3, int y3, float z1, float z2, float z3, color_t c1, color_t c2, color_t c3)
{
    xi_triangle_t tri = {0};
    if (xi_init_triangle(&tri, xi, x1, y1, x2, y2, x3, y3) == 0)
        return;

    for (int y = tri.ystart; y < tri.yend; ++y)
        for (int x = tri.xstart; x < tri.xend; ++x)
            if (xi_point_in_triangle(&tri, x, y))
                if (tri.D < 0)
                {
                    float z = tri.l1*z1 + tri.l2*z2 + tri.l3*z3;
                    float *zb = zbuffer + y*xi->width + x;
                    if (z > *zb)
                    {
                        *XI_PIXEL(xi, x, y) = xi_lerp_colors(&tri, c1, c2, c3);
                        *zb = z;
                    }
                }
}

void xi_draw_texture(ximage_t *xi, ximage_t *texture, int x0, int y0, int width, int height)
{
    int xstart = x0 >= 0? x0 : 0;
    int xend   = x0+width < xi->width? x0+width : xi->width;
    int ystart = y0 >= 0? y0 : 0;
    int yend   = y0+height < xi->height? y0+height : xi->height;

    for (int y = ystart; y < yend; ++y)
    {
        for (int x = xstart; x < xend; ++x)
        {
            color_t color = *XI_PIXEL(texture,
                (int)((float)texture->width *(float)(x-x0)/(float)width),
                (int)((float)texture->height*(float)(y-y0)/(float)height)
            );
            *XI_PIXEL(xi, x, y) = color;
        }
    }
}


#endif // #ifdef XIMAGE_IMPLEMENTATION


#endif // #ifndef XIMAGE_H
