#include "App.h"

int main() {
    App app;
    if (!app.init()) return -1;
    app.run(); // Calls emscripten_set_main_loop internally
    return 0; // never reached
}