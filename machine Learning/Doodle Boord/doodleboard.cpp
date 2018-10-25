#include "doodleboard.h"
#include "ui_doodleboard.h"
#include <QPainter>
#include <QPixmap>
#include <QDebug>
#include <QMessageBox>
#include <fstream>
#include <QTimer>
#include <limits>
#include "scopeguard.h"

doodleBoard::doodleBoard(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::doodleBoard)
{
    ui->setupUi(this);
    ui->result->setStyleSheet("background-color:#DDDDDD");
    this->setWindowTitle ("YHL 涂鸦板");
    pix = QPixmap(380, 500);
    pix.fill (Qt::white);
    this->initSave ();
}

doodleBoard::~doodleBoard()
{
    delete ui;
    ui = nullptr;
    std::ofstream out("./other/save.txt", std::ios::trunc);
    YHL::ON_SCOPE_EXIT([&]{ out.close (); });
    out << this->book.size () << "\n";
    for(const auto& it : this->book) {
        out << it.first << " " << it.second << "\n";
    }
}

void doodleBoard::initSave()
{
    this->numDialog = std::make_unique<number>();
    connect (this->numDialog.get (), &number::send_number, [&](const int arg){
        QString path = "/.png";
        path.insert (0, QString::number (arg));
        path.insert (2, QString::number (this->book[arg]));
        pix.save (path);
        ++this->book[arg];
    });
    std::ifstream in("./other/save.txt");
    YHL::ON_SCOPE_EXIT([&]{ in.close (); });
    int Size;
    in >> Size;
    int obj, times;
    for(int i = 0;i < Size; ++i) {
        in >> obj >> times;
        this->book.emplace (obj, times);
    }
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
        static long long cnt = 0;
        if(++cnt % 10 == 0) {
            qDebug() << "( " << endPoint.x ()<< " , " << endPoint.y () << " )";
            this->travels.emplace_back(endPoint);
            if(cnt >= std::numeric_limits<long long>::max ())
                cnt = 0;
        }
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
    this->travels.clear ();
}

void doodleBoard::on_recognize_clicked()
{
    YHL::ON_SCOPE_EXIT([&]{
        this->travels.clear ();
    });
}

void doodleBoard::on_save_button_clicked()
{
    assert (this->numDialog);
    numDialog->show ();
}
