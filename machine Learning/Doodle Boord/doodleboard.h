#ifndef DOODLEBOARD_H
#define DOODLEBOARD_H

#include <QMainWindow>
#include <QMouseEvent>
#include <memory>
#include <number.h>
#include <unordered_map>
#include <QTimerEvent>
#include <vector>

namespace Ui {
class doodleBoard;
}

class doodleBoard : public QMainWindow
{
    Q_OBJECT
public:
    explicit doodleBoard(QWidget *parent = nullptr);
    ~doodleBoard();

private:
    void initSave ();

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

    void on_save_button_clicked();

private:
    Ui::doodleBoard *ui;
    QPixmap pix;
    QPoint lastPoint;
    QPoint endPoint;
    std::vector<QPoint> travels;
    std::unique_ptr<number> numDialog;
    std::unordered_map<int, int> book;
};

#endif // DOODLEBOARD_H
