#ifndef APP_H
#define APP_H

#include "Raster.h"
#include "Vector.h"
#include "Polygon.h"
#include "View.h"
#include "Collision.h"
#include "Map.h"
#include "Light.h"

// Events should be translated from whatever library is used to run this app 
// for modularity. In this case, SDL.
typedef enum AppEventType {
    APPEVENT_KEYPRESSED,
    APPEVENT_KEYRELEASED,
    APPEVENT_MOUSEPRESSED,
    APPEVENT_MOUSERELEASED,
    APPEVENT_MOUSEMOVED
} AppEventType;

// Other event types may be added to this union, like mouse pressses.
typedef union AppEvent {
    enum {
        APPEVENT_KEY_ESCAPE,
        APPEVENT_KEY_LEFT,
        APPEVENT_KEY_RIGHT,
        APPEVENT_KEY_DOWN,
        APPEVENT_KEY_UP,
        APPEVENT_KEY_SPACE
    } key;
    struct {
        double x;
        double y;
    } mouse;
} AppEvent;

typedef struct App {
    // Main array of pixels to render to.
    Raster* screen;
    
    // Player control booleans
    int leftPressed;
    int rightPressed;
    int downPressed;
    int upPressed;
    int spacePressed;
    int mousePressed;
    int mouseMoving;
    Vector mousePos;
    
    Vector playerPos;
    double playerSpeed;
    double playerRadius;
    Color playerCol;
    Color playerLightCol;
    
    Map* map;
    
    Light** lights;
    int lightsSize;
    Vector* lightRays;
    int numLightRays;
    
    Polygon** polygons;
    RenderPolygon** renderPolygons;
    int polygonsSize;
    int polygonsCapacity;
    
    double time;
    
    // Boolean for whether the app should exit.
    int quit;
} App;

// This function initializes the application with a screen size.
// loadRaster is a pointer to a function that takes a filename and returns
// a raster image. The App does not care how this function is implemented.
App* App_Create(int width, int height, Raster*(*loadRaster)(const char*));

void App_Destroy(App* app);

void App_AddPolygon(App* app, Polygon* polygon);

// Library specific events (SDL, Allegro, GLFW etc.) should be converted
// to AppEvents.
void App_HandleEvent(App* app, AppEventType type, AppEvent event);

// Render/update function with delta passed in seconds.
void App_Render(App* app, double delta);

// Boolean for whether the application loop should continue.
int App_Continue(App* app);

#endif