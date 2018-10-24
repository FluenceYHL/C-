#include "visualization.h"
#include "ui_visualization.h"
#include <set>
#include <cstdlib>
#include <ctime>
#include <QDebug>
#include <QMessageBox>
#include <qcustomplot.h>
#include <QColorDialog>
#include "hierarchical.h"

Visualization::Visualization(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Visualization)
{
    ui->setupUi(this);
    ui->threshold->setText ("1");
    this->setWindowTitle ("层次聚类 C++");
    srand(static_cast<unsigned int>(time(nullptr)));
}

Visualization::~Visualization()
{
    delete ui;
    ui = nullptr;
}

void Visualization::on_close_button_clicked()
{
    this->close ();
}

void Visualization::closeEvent(QCloseEvent *)
{
    this->close ();
}

QColor Visualization::getColor(QList<QColor>& colors)
{
    QColor curColor;
    if(colors.isEmpty ()) {
        int a = rand() % 256, b = rand() % 256, c = rand() % 256;
        return qRgb(a, b, c);
    } else {
        curColor = colors.front ();
        colors.pop_front ();
        return curColor;
    }
}

void Visualization::on_choose_button_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("选择文件"), ".",tr("Text Files(*.txt)"));

    auto null_one = std::make_unique<YHL::Hierarchical>();
    this->data.swap(null_one);
    assert (this->data);
    this->data->readData (fileName.toStdString ());

    // 读取文件数据
    auto &dataSet = this->data->getDataSet ();
    QVector<double> xVec, yVec;
    auto min_x = 1e12, min_y = 1e12;
    auto max_x = -1e12, max_y = -1e12;
    for(const auto& it : dataSet) {
        auto x = it.x;
        auto y = it.y;
        xVec << x;
        yVec << y;
        if(min_x > x) min_x = x;
        if(min_y > y) min_y = y;
        if(max_x < x) max_x = x;
        if(max_y < y) max_y = y;
    }

    ui->Coordinate->clearGraphs ();
    ui->Coordinate->xAxis->setLabel ("x 横坐标");
    ui->Coordinate->yAxis->setLabel ("y 纵坐标");
    ui->Coordinate->xAxis->setRange (std::floor (min_x) - 1, std::ceil (max_x) + 1);
    ui->Coordinate->yAxis->setRange (std::floor (min_y) - 1, std::ceil (max_y) + 1);  // 排序

    ui->Coordinate->addGraph (ui->Coordinate->xAxis, ui->Coordinate->yAxis);
    // 设置画笔
    QPen pen;
    pen.setColor (Qt::red);
    pen.setWidth (4);
    // 绘制散点
    QCPGraph *curGragh = ui->Coordinate->addGraph ();
    curGragh->setPen (pen);
    curGragh->setLineStyle (QCPGraph::lsNone);
    curGragh->setScatterStyle (QCPScatterStyle(QCPScatterStyle::ssCircle, 2));
    curGragh->setData (xVec, yVec, false);
    ui->Coordinate->replot ();
    // 保存图片
    QString path = QApplication::applicationDirPath () + "/" + "dataSet.png";
    ui->Coordinate->savePng (path);
    this->setWindowTitle (fileName);
}

void Visualization::on_begin_button_clicked()
{
    if(!this->data) {
        QMessageBox::warning (this, "警告!", "尚未选择文件", QMessageBox::Yes);
        return;
    }
    if(ui->threshold->text ().isEmpty ()) {
        QMessageBox::warning (this, "警告!", "请输入 阈值", QMessageBox::Yes);
        return;
    }
    auto threshold = ui->threshold->text ().toDouble ();
    auto &clusters = this->data->getClusters (threshold);
    ui->clusters_num->setText (QString::number (clusters.size ()));

    QList<QColor> colors = { Qt::red, Qt::blue, Qt::green, Qt::yellow, Qt::black, Qt::gray };

    ui->Coordinate->clearGraphs ();
    auto &dataSet = this->data->getDataSet ();
    for(const auto& it : clusters) {
        QVector<double> xVec, yVec;
        // 当前 it 是一个聚类
        for(const auto& r : it.second) {
            xVec << dataSet[r].x;
            yVec << dataSet[r].y;
        }
        ui->Coordinate->addGraph (ui->Coordinate->xAxis, ui->Coordinate->yAxis);
        // 设置画笔
        QPen pen;
        pen.setColor (this->getColor (colors));
        pen.setWidth (4);
        // 绘制散点
        QCPGraph *curGragh = ui->Coordinate->addGraph ();
        curGragh->setPen (pen);
        curGragh->setLineStyle (QCPGraph::lsNone);
        curGragh->setScatterStyle (QCPScatterStyle(QCPScatterStyle::ssCircle, 2));
        curGragh->setData (xVec, yVec, false);
    }
    ui->Coordinate->replot ();
    // 保存图片
    QString path = QApplication::applicationDirPath () + "/" + "clusters.png";
    ui->Coordinate->savePng (path);
}

void Visualization::on_result_button_clicked()
{

}

void Visualization::on_check_button_clicked()
{

}
