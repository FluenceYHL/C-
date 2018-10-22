#include "doodleboard.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    doodleBoard w;
    w.show();

    return a.exec();
}
