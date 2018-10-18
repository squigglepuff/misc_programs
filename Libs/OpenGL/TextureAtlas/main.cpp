#include <QApplication>
#include "window.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    CWindow wnd;
    wnd.show();
    return app.exec();
}
