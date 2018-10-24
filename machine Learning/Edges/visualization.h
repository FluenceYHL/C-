#ifndef VISUALIZATION_H
#define VISUALIZATION_H

#include <QMainWindow>
#include <memory>
#include <qcustomplot.h>

namespace Ui {
class Visualization;
}
namespace YHL{
class Hierarchical;
}

class Visualization : public QMainWindow
{
    Q_OBJECT

public:
    explicit Visualization(QWidget *parent = nullptr);
    ~Visualization();

private:
    void display(QColor color);
private slots:
    void on_close_button_clicked();

    void on_choose_button_clicked();

    void on_begin_button_clicked();

    void on_result_button_clicked();

    void on_check_button_clicked();

private:
    Ui::Visualization *ui;
    std::unique_ptr<YHL::Hierarchical> data;
    void closeEvent(QCloseEvent*);
    QColor getColor(QList<QColor>& colors);
    QVector<double> xVec, yVec;
};

#endif // VISUALIZATION_H
