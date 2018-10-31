#include "visualization.h"
#include "ui_visualization.h"
#include "fcm.h"
#include <cstdlib>
#include <ctime>
#include <QDebug>
#include <QMessageBox>

namespace {

    void logCall(const QString& message) {
        QMessageBox::warning (nullptr, "警告!", message, QMessageBox::Yes);
    }

    QColor getColor() {
        static QList<QColor> colors{ Qt::red, Qt::black, Qt::blue, Qt::yellow, Qt::green, Qt::gray };
        if(!colors.empty ()) {
            auto one = colors.front ();
            colors.pop_front ();
            return one;
        }
        int a = rand() % 255, b = rand() % 255, c = rand() % 255;
        return qRgb(a, b, c);
    }

}

visualization::visualization(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::visualization)
{
    ui->setupUi(this);
    ui->K->setText ("3");
    srand (time(nullptr));
    ui->threshold->setText(QString::number (0.001));
}

visualization::~visualization()
{
    delete ui;
    ui = nullptr;
}

void visualization::myPaint(const visualization::funType &fun,
                            const QColor &color)
{
    ui->Coordinate->xAxis->setLabel ("x 横坐标");
    ui->Coordinate->yAxis->setLabel ("y 纵坐标");
    ui->Coordinate->addGraph (ui->Coordinate->xAxis, ui->Coordinate->yAxis);
    // 设置画笔
    QPen pen;
    pen.setColor (color);
    pen.setWidth (3);
    // 绘制散点
    QCPGraph *curGragh = ui->Coordinate->addGraph ();
    curGragh->setPen (pen);
    curGragh->setLineStyle (QCPGraph::lsNone);
    curGragh->setScatterStyle (QCPScatterStyle(QCPScatterStyle::ssCircle, 2));
    curGragh->setData (fun.first, fun.second, false);
    ui->Coordinate->replot ();
    // 保存图片
    QString path = QApplication::applicationDirPath () + "/" + "dataSet.png";
    ui->Coordinate->savePng (path);
}

void visualization::myRange()
{
    auto one = this->data->getRange ();
    ui->Coordinate->xAxis->setRange (std::floor (std::get<0>(one)) - 1, std::ceil (std::get<2>(one)) + 1);
    ui->Coordinate->yAxis->setRange (std::floor (std::get<1>(one)) - 1, std::ceil (std::get<3>(one)) + 1);  // 排序
}

void visualization::on_pushButton_3_clicked()
{
    this->close ();
}

void visualization::closeEvent(QCloseEvent *)
{
    this->close ();
}

void visualization::on_choose_data_clicked()
{

    QString fileName = QFileDialog::getOpenFileName(this, tr("选择文件"), ".",tr("Text Files(*.txt)"));
    auto null_one = std::make_unique<YHL::FCM>();
    this->data.swap(null_one);
    assert (this->data);
    this->data->loadFile (fileName.toStdString ());
    auto points = this->data->getPoints ();

    ui->Coordinate->clearGraphs ();
    this->myRange ();
    this->myPaint (points, Qt::red);
}

void visualization::on_K_mean_clicked()
{
    if(!this->data) {
        logCall ("尚未选择文件!"); return;
    }
    auto one = this->data->getCluster ();
    ui->Coordinate->clearGraphs ();
    qDebug () << "now";
    qDebug () << "Size  :  " << one.size ();
    for(const auto& it : one) {
        QVector<double> xVec, yVec;
        for(const auto& r : it.second) {
            xVec << r.x;
            yVec << r.y;
        }
        qDebug () << "一部分" << it.second.size ();
        auto color = getColor ();
        this->myPaint (funType(std::move(xVec), std::move(yVec)), color);
    }
}







