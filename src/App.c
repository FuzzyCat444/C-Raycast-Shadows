#include "App.h"

#include <math.h>
#include <stdlib.h>

App* App_Create(int width, int height, Raster*(*loadRaster)(const char*)) {
    App* app = (App*) malloc(sizeof(App));
    
    app->screen = Raster_Create(width, height);
    
    app->leftPressed = 0;
    app->rightPressed = 0;
    app->downPressed = 0;
    app->upPressed = 0;
    app->spacePressed = 0;
    app->mousePressed = 0;
    app->mouseMoving = 0;
    app->mousePos = (Vector) { 0.0, 0.0 };
    
    app->playerPos = (Vector) { 10.5, 10.5 };
    app->playerSpeed = 5.0;
    app->playerRadius = 0.45;
    app->playerCol = (Color) { 180, 0, 0 };
    app->playerLightCol = (Color) { 255, 255, 64 };
    
    app->map = Map_Create(20, 20);
    Map_Clear(app->map, 0);
    Map_Borders(app->map, 1);
    
    app->lightsSize = 3;
    app->lights = (Light**) malloc(app->lightsSize * sizeof(Light*));
    app->lights[0] = Light_Create((Vector) { 0.0, 0.0 }, 
        app->playerLightCol);
    app->lights[1] = Light_Create((Vector) { 0.0 , 0.0 }, 
        (Color) { 64, 255, 225 });
    app->lights[2] = Light_Create((Vector) { 15.5 , 14.5 }, 
        (Color) { 255, 64, 255 });
    app->numLightRays = 4000;
    app->lightRays = createLightRays(app->numLightRays);
    
    app->polygonsSize = 0;
    app->polygonsCapacity = 1;
    app->polygons = (Polygon**) malloc(sizeof(Polygon*));
    app->renderPolygons = (RenderPolygon**) malloc(sizeof(RenderPolygon*));
    
    Tile* mapTiles = app->map->tiles;
    int mapWidth = app->map->width;
    int mapHeight = app->map->height;
    int mapIdx = 0;
    srand(0);
    for (int y = 0; y < mapHeight; y++) {
        for (int x = 0; x < mapWidth; x++) {
            if (rand() % 5 == 0 && !(x == 3 && y == 3)) {
                mapTiles[mapIdx] = 1;
            }
            if (mapTiles[mapIdx] > 0) {
                Polygon* tilePoly = Polygon_Create();
                Polygon_PushPoint(tilePoly, (Vector) { x, y });
                Polygon_PushPoint(tilePoly, (Vector) { x + 1, y });
                Polygon_PushPoint(tilePoly, (Vector) { x + 1, y + 1 });
                Polygon_PushPoint(tilePoly, (Vector) { x, y + 1 });
                App_AddPolygon(app, tilePoly);
            }
            mapIdx++;
        }
    }
    
    app->time = 0.0;
    
    app->quit = 0;
    
    return app;
}

void App_Destroy(App* app) {
    if (app == NULL)
        return;
    
    Raster_Destroy(app->screen);
    
    Map_Destroy(app->map);
    
    for (int i = 0; i < app->lightsSize; i++) {
        Light_Destroy(app->lights[i]);
    }
    free(app->lights);
    free(app->lightRays);
    
    for (int i = 0; i < app->polygonsSize; i++) {
        Polygon_Destroy(app->polygons[i]);
        RenderPolygon_Destroy(app->renderPolygons[i]);
    }
    free(app->polygons);
    free(app->renderPolygons);
    
    free(app);
}

void App_AddPolygon(App* app, Polygon* polygon) {
    int size = app->polygonsSize;
    int capacity = app->polygonsCapacity;
    
    int index = size++;
    app->polygons[index] = polygon;
    app->renderPolygons[index] = RenderPolygon_Create(polygon);
    
    if (size == capacity) {
        capacity *= 2;
        app->polygons = (Polygon**) realloc(app->polygons, 
            capacity * sizeof(Polygon*));
        app->renderPolygons = (RenderPolygon**) realloc(app->renderPolygons, 
            capacity * sizeof(RenderPolygon*));
    }
    
    app->polygonsSize = size;
    app->polygonsCapacity = capacity;
}

void App_HandleEvent(App* app, AppEventType type, AppEvent event) {
    switch (type) {
    case APPEVENT_KEYPRESSED:
        if (event.key == APPEVENT_KEY_ESCAPE) {
            app->quit = 1;
        } else if (event.key == APPEVENT_KEY_LEFT) {
            app->leftPressed = 1;
        } else if (event.key == APPEVENT_KEY_RIGHT) {
            app->rightPressed = 1;
        } else if (event.key == APPEVENT_KEY_DOWN) {
            app->downPressed = 1;
        } else if (event.key == APPEVENT_KEY_UP) {
            app->upPressed = 1;
        } else if (event.key == APPEVENT_KEY_SPACE) {
            app->spacePressed = 1;
        }
        break;
    case APPEVENT_KEYRELEASED:
        if (event.key == APPEVENT_KEY_LEFT) {
            app->leftPressed = 0;
        } else if (event.key == APPEVENT_KEY_RIGHT) {
            app->rightPressed = 0;
        } else if (event.key == APPEVENT_KEY_DOWN) {
            app->downPressed = 0;
        } else if (event.key == APPEVENT_KEY_UP) {
            app->upPressed = 0;
        } else if (event.key == APPEVENT_KEY_SPACE) {
            app->spacePressed = 0;
        }
        break;
    case APPEVENT_MOUSEPRESSED:
        app->mousePressed = 1;
        app->mousePos.x = event.mouse.x;
        app->mousePos.y = event.mouse.y;
        break;
    case APPEVENT_MOUSERELEASED:
        app->mousePressed = 0;
        app->mousePos.x = event.mouse.x;
        app->mousePos.y = event.mouse.y;
        break;
    case APPEVENT_MOUSEMOVED:
        app->mouseMoving = 1;
        app->mousePos.x = event.mouse.x;
        app->mousePos.y = event.mouse.y;
        break;
    }
}

void App_Render(App* app, double delta) {
    double time = app->time;
    time += delta;
    
    Color playerCol = app->playerCol;
    
    Raster* screen = app->screen;
    int width = screen->width;
    int height = screen->height;
    Raster_Clear(screen, (Color) { 0, 0, 0 });
    
    Polygon** polygons = app->polygons;
    RenderPolygon** renderPolygons = app->renderPolygons;
    int polygonsSize = app->polygonsSize;
    
    // Update
    
    double speed = app->playerSpeed;
    double playerRadius = app->playerRadius;
    int leftPressed = app->leftPressed;
    int rightPressed = app->rightPressed;
    int downPressed = app->downPressed;
    int upPressed = app->upPressed;
    int spacePressed = app->spacePressed;
    int moving = leftPressed != rightPressed || downPressed != upPressed;
    int mouseMoving = app->mouseMoving;
    int mousePressed = app->mousePressed;
    Vector move = {
        speed * (rightPressed - leftPressed),
        speed * (downPressed - upPressed)
    };
    Vector playerPos = app->playerPos;
    
    const int stepsPerSec = 400;
    int numSteps = (int) (delta * stepsPerSec) + 1;
    double delta2 = delta / numSteps;
    for (int i = 0; i < numSteps; i++) {
        playerPos.x += move.x * delta2;
        playerPos.y += move.y * delta2;
        Vector newPlayerPos = collideCirclePolygons(playerPos, playerRadius, 
            polygons, polygonsSize);
        playerPos = newPlayerPos;
    }
    app->playerPos = playerPos;
    
    int lightsSize = app->lightsSize;
    Light** lights = app->lights;
    if (spacePressed) {
        lights[0]->color = app->playerLightCol;
    } else {
        lights[0]->color = (Color) {0};
    }
    lights[0]->position = playerPos;
    lights[1]->position = (Vector) { 5.0, 5.5 + 4.0 * sin(2.0 * time) };
    
    int numLightRays = app->numLightRays;
    Vector* lightRays = app->lightRays;
    Map* map = app->map;
    for (int i = 0; i < lightsSize; i++) {
        Light_Illuminate(lights[i], lightRays, 
            numLightRays, map);
    }
    
    View playerView = View_Create(
        (Vector) { width, height },
        playerPos, 5.0
    );
    
    // Render
    
    renderLights(screen, lights, lightsSize, 
        &playerView, 3, 5);
    
    for (int i = 0; i < polygonsSize; i++) {
        RenderPolygon_Render(renderPolygons[i], screen, 
            (Color) { 64, 64, 64 }, playerView.offset, playerView.scale);
    }
        
    app->mouseMoving = 0;
    
    app->time = time;
}

int App_Continue(App* app) {
    return !app->quit;
}
