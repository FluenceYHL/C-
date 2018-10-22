#include "visualization.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    visualization w;
    w.show();

    return a.exec();
}
