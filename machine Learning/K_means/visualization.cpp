#include "visualization.h"
#include "ui_visualization.h"
#include "k_means.h"
#include <QDebug>

visualization::visualization(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::visualization)
{
    ui->setupUi(this);
}

visualization::~visualization()
{
    delete ui;
    ui = nullptr;
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
    auto null_one = std::make_unique<YHL::K_means>(fileName.toStdString ());
    this->data.swap(null_one);
    assert (this->data);

    // 读取文件数据
    auto &dataSet = this->data->getDataSet ();
    QVector<double> xVec, yVec;
    auto min_x = 1e12, min_y = 1e12;
    auto max_x = -1e12, max_y = -1e12;
    for(const auto& it : dataSet) {
        auto x = std::get<0>(it);
        auto y = std::get<1>(it);
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
}

void visualization::on_K_mean_clicked()
{
    qDebug() << "开始聚类\n";
    auto &ans = this->data->getCluster ();
    this->data->display ();

    auto evaluate = std::get<2>(ans);
    ui->evaluate->setText(QString::number (evaluate));

    auto &clusters = std::get<0>(ans);
    ui->Coordinate->clearGraphs ();
    QVector<QColor> colors = { Qt::red, Qt::blue, Qt::green, Qt::yellow, Qt::black, Qt::gray };
    assert(static_cast<int>(clusters.size()) <= colors.size());
    int colorCnt = -1;
    for(const auto& it : clusters) {
        QVector<double> xVec, yVec;
        // 当前 it 是一个聚类
        for(const auto& r : it) {
            xVec << std::get<0>(r);
            yVec << std::get<1>(r);
        }
        ui->Coordinate->addGraph (ui->Coordinate->xAxis, ui->Coordinate->yAxis);
        // 设置画笔
        QPen pen;
        pen.setColor (colors[++colorCnt]);
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







