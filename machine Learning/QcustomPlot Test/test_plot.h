#ifndef TEST_PLOT_H
#define TEST_PLOT_H

#include <QMainWindow>

namespace Ui {
class test_plot;
}

class test_plot : public QMainWindow
{
    Q_OBJECT

public:
    explicit test_plot(QWidget *parent = nullptr);
    ~test_plot();

private:
    Ui::test_plot *ui;
};

#endif // TEST_PLOT_H
