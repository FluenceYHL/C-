#include "test_plot.h"
#include "ui_test_plot.h"
#include <QVector>

test_plot::test_plot(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::test_plot)
{
    ui->setupUi(this);

    constexpr int range = 101;
    QVector<double> x(range), y(range);
    for(int i = 0;i < range; ++i) {
        x[i] = i/5 - 10;
        y[i] = x[i] * x[i] * x[i];
    }
    ui->Coordinate->addGraph ();
    ui->Coordinate->graph (0)->setData (x, y);
    ui->Coordinate->xAxis->setLabel ("x");
    ui->Coordinate->yAxis->setLabel ("y");
    ui->Coordinate->xAxis->setRange (-11, 11);
    ui->Coordinate->yAxis->setRange (-1100, 1100);
}

test_plot::~test_plot()
{
    delete ui;
    ui = nullptr;
}
