#include "visualization.h"
#include "ui_visualization.h"
#include "DBSCAN.h"
#include <cstdlib>
#include <ctime>
#include <QDebug>
#include <QMessageBox>

namespace {

    QList<QColor> colors{ Qt::red, Qt::black, Qt::blue, Qt::yellow, Qt::green, Qt::gray };

    void logCall(const QString& message) {
        QMessageBox::warning (nullptr, "警告!", message, QMessageBox::Yes);
    }

    QColor getColor() {
        if(!colors.isEmpty ()) {
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
    ui->K->setText ("1");
    srand (time(nullptr));
    ui->threshold->setText(QString::number (5));
    ui->Coordinate->setInteractions (QCP::iRangeDrag | QCP::iRangeZoom);
    ui->Coordinate->legend->setVisible (true);
    ui->Coordinate->legend->setFont (QFont("Helvetica", 9));
}

visualization::~visualization()
{
    delete ui;
    ui = nullptr;
}

void visualization::myPaint(const visualization::funType &fun,
                            const QColor &color, const QString& name)
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
    curGragh->setName (name);
    ui->Coordinate->replot ();
    // 保存图片
    QString path = QApplication::applicationDirPath () + "/" + "dataSet.png";
    ui->Coordinate->savePng (path);
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
    if(ui->K->text ().isEmpty ()) {
        logCall ("聚类数目不能为空"); return;
    }
    auto radius = ui->K->text ().toDouble ();
    auto minPts = ui->threshold->text ().toInt ();
    auto null_one = std::make_unique<YHL::DBSCAN>(minPts, radius);
    this->data.swap(null_one);
    assert (this->data);
    this->data->loadFile (fileName.toStdString ());
    auto points = this->data->getPoints ();

    ui->Coordinate->clearGraphs ();
    this->myPaint (points, Qt::red, "YHL_C++");
}

void visualization::on_K_mean_clicked()
{
    if(!this->data) {
        logCall ("尚未选择文件!"); return;
    }
    qDebug () << "isEmpty () 之前";
    if(ui->threshold->text ().isEmpty ()) {
        logCall ("阈值不能为空"); return;
    }
    qDebug () << "isEmpty () 之后";
    QStringList names{ "老人", "小孩", "青年", "中年", "婴儿", "人妖", "天使", "恶魔", "战士", "魔法师", "道士", "菩萨"};
    auto one = this->data->getCluster ();
    ui->Coordinate->clearGraphs ();
    for(const auto& it : one) {
        QVector<double> xVec, yVec;
        for(const auto& r : it.second) {
            xVec << r.features[0];
            yVec << r.features[1];
        }
        auto color = getColor ();
        this->myPaint (funType(std::move(xVec), std::move(yVec)), color, names.front ());
        names.pop_front ();
    }
    ui->Coordinate->replot ();
}

void visualization::on_pushButton_5_clicked()
{
    if(!this->data) {
        logCall ("尚未选择文件!"); return;
    }
    auto one = this->data->getEfficiency ();
    if(one.second.empty ()) {
        logCall ("请先聚类"); return;
    }
    QCustomPlot *gragh = new QCustomPlot();
    this->litters.emplace_back (gragh);
    gragh->addGraph (gragh->xAxis, gragh->yAxis);
    gragh->graph (0)->setName ("随着迭代次数增加，误差函数的变化");
    gragh->setInteractions (QCP::iRangeDrag | QCP::iRangeZoom);
    gragh->legend->setVisible (true);
    gragh->xAxis->setRange (0, 30);
    gragh->yAxis->setRange (800, 2500);
    gragh->xAxis->setLabel ("迭代次数");
    gragh->yAxis->setLabel ("损失函数");
    gragh->show ();

    auto curGragh = gragh->addGraph ();
    curGragh->setPen (QPen(getColor (), 6));
    curGragh->setData (one.first, one.second, false);
    curGragh->setName ("损失函数曲线");
    gragh->replot ();
}


