#ifndef VISUALIZATION_H
#define VISUALIZATION_H

#include <QMainWindow>
#include "qcustomplot.h"
#include <memory>

namespace Ui {
class visualization;
}

namespace YHL {
class SOM;
}

class visualization : public QMainWindow
{
    Q_OBJECT

    using funType = std::pair<QVector<double>, QVector<double> >;

public:
    explicit visualization(QWidget *parent = nullptr);
    ~visualization();

private:
    void myPaint(const funType& fun, const QColor& color, const QString& name);

private slots:
    void on_pushButton_3_clicked();

    void on_choose_data_clicked();

    void on_K_mean_clicked();

    void on_pushButton_5_clicked();

private:
    Ui::visualization *ui;
    std::list<QCustomPlot*> litters;
    std::unique_ptr<YHL::SOM> data;
    void closeEvent(QCloseEvent*);
};

#endif // VISUALIZATION_H
