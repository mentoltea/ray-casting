//gcc .\main.c -L ./lib/ -lraylib -lopengl32 -lgdi32 -lwinmm
//gcc .\main.c -lraylib -lgdi32 -lwinmm
//gcc .\main.c -lraylib -lopengl32 -lgdi32 -lwinmm
#include <raylib.h>
#include <stdlib.h>
#include "vector.h"
#include <math.h>

const int WINX = 600;
const int WINY = 400;

typedef struct Player {
    Vector2 point;
    float angle; // 0 - 360
} Player;

typedef struct Obtacle {
    Vector2 point;
    float sizex, sizey;
} Obtacle;

float absf(float x) {
    if (x>=0) return x;
    return -x;
}

bool in_obtacle(Vector2 p, Obtacle* obt) {
    if (p.x >= obt->point.x && p.x < obt->point.x + obt->sizex)
        if (p.y >= obt->point.y && p.y < obt->point.y + obt->sizey) return true;
    return false;
}

bool in_obtacles(Vector2 p, Obtacle* vec) {
    vector_metainfo meta = vec_meta(vec);
    for (int i=0; i<meta.length; i++) {
        if (in_obtacle(p, vec+i)) return true;
    }
    return false;
}

Vector2 ray_intersect(Obtacle* vec, Vector2 start, float angle, float limit, float step) {
    Vector2 edge = start;
    float stepx = cos((double)angle/180 * PI);
    float stepy = sin((double)angle/180 * PI);

    int dx, dy;
    while (!in_obtacles(edge, vec) && edge.x < WINX && edge.x >= 0 && edge.y < WINY && edge.y >=0) {
        edge.x += stepx;
        edge.y += stepy;
        dx = edge.x - start.x;
        dy = edge.y - start.y;
        if (dx*dx + dy*dy >= limit) break;
    }
    return edge;
}


int main(int argc, char** argv) {
    InitWindow(WINX, WINY, "Ray casting");
    SetTargetFPS(60);
    float hview_angle = 50;
    float delta = 1;
    int N = 2*hview_angle/delta + 1 + 2;
    int limit = 200000;

    Player player;
    player.point.x = WINX-50;
    player.point.y = WINY-50;
    player.angle = 180;

    Obtacle* obts = new_vec(sizeof(Obtacle), 10); // vector
    Obtacle obt;
    obt.point.x = 30; obt.point.y = 30;
    obt.sizex = 100; obt.sizey = 50;
    obts = vec_add(obts, &obt);

    obt.point.x = 100; obt.point.y = 150;
    obt.sizex = 100; obt.sizey = 50;
    obts = vec_add(obts, &obt);

    Vector2* pts = malloc(sizeof(Vector2)*(N));
    float angle;
    Vector2 mouse;
    double dx, dy, d;
    while (!WindowShouldClose()) {
        if (IsKeyDown(KEY_W)) {
            player.point.y -= 2;
        } else if (IsKeyDown(KEY_S)) {
            player.point.y += 2;
        }

        if (IsKeyDown(KEY_A)) {
            player.point.x -= 2;
        } else if (IsKeyDown(KEY_D)) {
            player.point.x += 2;
        }

        mouse = GetMousePosition();
        // printf("%f %f\n", modf)
        if (mouse.x != player.point.x && mouse.y != player.point.y) {
            dx = mouse.x - player.point.x;
            dy = mouse.y - player.point.y;
            d = sqrt(dx*dx + dy*dy);
            player.angle = acos(dx/d)/PI*180;
            
            if (dy<0) player.angle *= -1;
        }
        // printf("%d\n", player.angle);
        angle = player.angle + hview_angle;
        pts[0] = player.point;
        for (int i=1; i<=N-2; i++) {
            angle -= delta;
            pts[i] = ray_intersect(obts, player.point, angle, limit, 0.3);
        }
        pts[N-1] = player.point;

        BeginDrawing();
        ClearBackground((Color){50,50,50,255});

        DrawFPS(0, 0);

        // DrawLineEx(player.point, pts[1], 1, (Color){120,120,120,255});
        // DrawLineEx(pts[1], pts[N-1], 1, (Color){120,120,120,255});
        // DrawLineEx(player.point, pts[N-1], 1, (Color){120,120,120,255});
        DrawTriangleFan(pts, N, (Color){120,120,120,255});
        // DrawTriangle(pts[0], pts[1], pts[N-1], (Color){120,120,120,255});
        // printf("%f %f | %f %f | %f %f\n", player.point.x, player.point.y, pts[0].x, pts[0].y, pts[N-1].x, pts[N-1].y);
        EndDrawing();
    }
    CloseWindow();

    delete_vec(obts);
    free(pts);
    return 0;
}