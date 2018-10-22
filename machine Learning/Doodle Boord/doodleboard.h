#ifndef DOODLEBOARD_H
#define DOODLEBOARD_H

#include <QMainWindow>
#include <QMouseEvent>

namespace Ui {
class doodleBoard;
}

class doodleBoard : public QMainWindow
{
    Q_OBJECT

public:
    explicit doodleBoard(QWidget *parent = nullptr);
    ~doodleBoard();

protected:
    void paintEvent(QPaintEvent*);
    void mousePressEvent(QMouseEvent*);
    void mouseMoveEvent(QMouseEvent*);
    void mouseReleaseEvent(QMouseEvent*);
    void closeEvent(QCloseEvent*);

private slots:
    void on_close_button_clicked();

    void on_clear_button_clicked();

    void on_recognize_clicked();

private:
    Ui::doodleBoard *ui;
    QPixmap pix;
    QPoint lastPoint;
    QPoint endPoint;
};

#endif // DOODLEBOARD_H
