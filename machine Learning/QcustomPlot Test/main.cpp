#include "test_plot.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    test_plot w;
    w.show();

    return a.exec();
}
