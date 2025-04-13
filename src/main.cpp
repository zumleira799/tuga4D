#include <Application/BaseApp.h>

using namespace tuga4d::Application;
int main() {
    BaseApp* app = new BaseApp();
    app->Run();
    delete app;
    return EXIT_SUCCESS;
}