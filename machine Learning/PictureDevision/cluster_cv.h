#ifndef CLUSTER_CV_H
#define CLUSTER_CV_H

#include <QMainWindow>
#include <QImage>
#include <memory>
#include <thread>

namespace Ui {
class cluster_cv;
}

class cluster_cv : public QMainWindow
{
    Q_OBJECT

public:
    explicit cluster_cv(QWidget *parent = nullptr);
    ~cluster_cv();

private slots:
    void on_close_button_clicked();

    void on_choose_button_clicked();

    void on_cluster_button_clicked();

    void on_save_button_clicked();

private:
    Ui::cluster_cv *ui;
    QImage image;
    QImage result;
    QPixmap pix;
    std::thread clusterThread;
};

#endif // CLUSTER_CV_H
