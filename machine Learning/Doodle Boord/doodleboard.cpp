#include "doodleboard.h"
#include "ui_doodleboard.h"
#include <QPainter>
#include <QPixmap>

doodleBoard::doodleBoard(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::doodleBoard)
{
    ui->setupUi(this);
    ui->result->setStyleSheet("background-color:#DDDDDD");
    this->resize (600, 500);
    this->setWindowTitle ("YHL 涂鸦板");
    pix = QPixmap(250, 500);
    pix.fill (Qt::white);
}

doodleBoard::~doodleBoard()
{
    delete ui;
    ui = nullptr;
}

void doodleBoard::paintEvent(QPaintEvent *)
{
    QPainter help(&pix);
    help.setPen (QPen(Qt::black, 6));
    help.drawLine (lastPoint, endPoint);
    lastPoint = endPoint;
    QPainter painter(this);
    painter.drawPixmap (0, 0, pix);
}

void doodleBoard::mousePressEvent(QMouseEvent *e)
{
    if(e->button () == Qt::LeftButton) {
        lastPoint = e->pos ();
    }
    endPoint = lastPoint;
}

void doodleBoard::mouseMoveEvent(QMouseEvent *e)
{
    if(e->buttons () & Qt::LeftButton) {
        endPoint = e->pos ();
        update ();
    }
}

void doodleBoard::mouseReleaseEvent(QMouseEvent *e)
{
    if(e->button () == Qt::LeftButton) {
        endPoint = e->pos ();
        update ();
    }
}

void doodleBoard::closeEvent(QCloseEvent *)
{
    this->close ();
}

void doodleBoard::on_close_button_clicked()
{
    this->close ();
}

void doodleBoard::on_clear_button_clicked()
{
    pix.fill (Qt::white);
    update ();
    lastPoint.setX (0); lastPoint.setY (0);
    endPoint.setX (0); endPoint.setY (0);
}

void doodleBoard::on_recognize_clicked()
{

}
