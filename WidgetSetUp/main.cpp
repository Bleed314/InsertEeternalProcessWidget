#include "WidgetSetUpMainWindow.h"

#include <iostream>
#include <string>

#include "CustomWindow.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    WidgetSetUpMainWindow w;
    //w.show();

    //CustomWindow aa(&w);
    //aa.show();

    return a.exec();
}

