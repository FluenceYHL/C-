#include "visualization.h"
#include "ui_visualization.h"
#include <set>
#include <cstdlib>
#include <ctime>
#include <QDebug>
#include <QMessageBox>
#include <QColorDialog>
#include "hierarchical.h"
#include "scopeguard.h"

Visualization::Visualization(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Visualization)
{
    ui->setupUi(this);
    ui->threshold->setText ("1.42");
    this->setWindowTitle ("层次聚类 C++");
    srand(static_cast<unsigned int>(time(nullptr)));
}

Visualization::~Visualization()
{
    delete ui;
    ui = nullptr;
}

void Visualization::display(QColor color)
{
    // 设置画笔
    QPen pen;
    pen.setColor (color);
    pen.setWidth (4);
    // 绘制散点
    QCPGraph *curGragh = ui->Coordinate->addGraph ();
    curGragh->setPen (pen);
    curGragh->setLineStyle (QCPGraph::lsNone);
    curGragh->setScatterStyle (QCPScatterStyle(QCPScatterStyle::ssCircle, 2));
    curGragh->setData (xVec, yVec, false);

    ui->Coordinate->replot ();
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
    xVec.clear ();
    yVec.clear ();
    auto &dataSet = this->data->getDataSet ();
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

    this->display (Qt::red);
    // 保存图片
    ui->Coordinate->savePng ("./dataSet.png");
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

    ui->Coordinate->clearGraphs ();
    ui->Coordinate->addGraph (ui->Coordinate->xAxis, ui->Coordinate->yAxis);

    QList<QColor> colors = { Qt::red, Qt::blue, Qt::green, Qt::yellow, Qt::black, Qt::gray };

    auto &dataSet = this->data->getDataSet ();
    xVec.clear ();
    yVec.clear ();
    for(const auto& it : clusters) {
        YHL::ON_EXIT_SCOPE([&]{
            xVec.clear (); yVec.clear ();
        });
        // 当前 it 是一个聚类
        for(const auto& r : it.second) {
            xVec << dataSet[r].x;
            yVec << dataSet[r].y;
        }
        this->display (getColor (colors));
    }
    ui->Coordinate->replot ();
    // 保存图片
    ui->Coordinate->savePng ("./clusters.png");
}

void Visualization::on_result_button_clicked()
{

}

void Visualization::on_check_button_clicked()
{
    if(!this->data) {
        QMessageBox::warning (this, "警告!", "尚未选择文件", QMessageBox::Yes);
        return;
    }
    auto others = this->data->getOthers ();
    if(others.empty ()) {
        QMessageBox::warning (nullptr, "警告", "请先聚类", QMessageBox::Yes);
        return;
    }
    auto &dataSet = this->data->getDataSet ();

    ui->Coordinate->clearGraphs ();
    ui->Coordinate->addGraph (ui->Coordinate->xAxis, ui->Coordinate->yAxis);
    // 要注意,范围已经被上面的清理了
    do {
        YHL::ON_EXIT_SCOPE([&]{
            xVec.clear (); yVec.clear ();
        });
        auto len = dataSet.size ();
        for(int i = 0;i < len; ++i) {
            if(others.find (i) == others.end ()) {
                xVec << dataSet[i].x;
                yVec << dataSet[i].y;
            }
        }
        this->display (Qt::gray);
    } while(0);
    do {
        YHL::ON_EXIT_SCOPE([&]{
            xVec.clear (); yVec.clear ();
        });
        for(const auto& it : others) {
            xVec << dataSet[it.first].x;
            yVec << dataSet[it.first].y;
        }
        this->display (Qt::red);
    } while(0);
    // 保存图片
    ui->Coordinate->savePng ("./others.png");
}
