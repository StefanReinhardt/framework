#include <iostream>
#include "Application.h"






int main(int argc, char ** argv)
{
    Application app(argc, argv);
    app.setOrganizationName("app");
    app.setApplicationName("app");
    return app.exec();
}
