#include "cluster_cv.h"
#include "ui_cluster_cv.h"
#include <QMessageBox>
#include <QFileDialog>
#include <QImage>
#include <QPoint>
#include <QPainter>
#include <QPixmap>
#include <QDebug>
#include "cluster.h"

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

cluster_cv::cluster_cv(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::cluster_cv)
{
    ui->setupUi(this);
    ui->threshold->setText ("20");
    ui->origin_picture->setStyleSheet("background-color:#DDDDDD");
    ui->handled_picture->setStyleSheet("background-color:#DDDDDD");
}

cluster_cv::~cluster_cv()
{
    delete ui;
    ui = nullptr;
}

void cluster_cv::on_close_button_clicked()
{
    this->close ();
}

void cluster_cv::on_choose_button_clicked()
{
    auto fileName = QFileDialog::getOpenFileName(this, tr("选择图像"), "", tr("Images (*.png *.bmp *.jpg *.tif *.GIF )"));
    if(fileName.isEmpty ())
        return;
    if(!this->image.load (fileName)) {
        logCall ("打开图片失败 !"); return;
    }
    ui->origin_picture->setScaledContents (true);
    ui->origin_picture->setPixmap (QPixmap::fromImage (this->image));
}

void cluster_cv::on_cluster_button_clicked()
{
    if(this->image.isNull ()) {
        logCall ("请先选择图片 !"); return;
    }
    auto threshold = ui->threshold->text ();
    if(threshold.isEmpty()) {
        logCall ("请输入阈值"); return;
    }
    auto one = std::make_unique<Cluster>();
    assert (one);
    one->loadPicture (&this->image);
    auto ans = one->getCluster(threshold.toDouble ());
    logCall ("计算完毕");
    double height = this->image.height ();
    double width = this->image.width ();
    this->result = this->image.copy ();
    for(const auto& it : ans) {
        auto color = getColor ();
        qDebug() << "这部分  :  " << it.second.size ();
        qDebug () << "color" << color;
        for(const auto& r : it.second) {
            this->result.setPixelColor (r[0], r[1], color);
        }
    }
    ui->handled_picture->setPixmap (QPixmap::fromImage (this->result));
    ui->handled_picture->setScaledContents(true);
    this->result.save ("./results/YHL.png");
}

void cluster_cv::on_save_button_clicked()
{
    this->image.save ("./results/YHL.png");
}
