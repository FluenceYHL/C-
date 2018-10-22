#include "k_means.h"
#include <functional>
#include <algorithm>
#include <fstream>
#include <cstdlib>
#include <ctime>
#include <memory>
#include <cmath>
#include <array>
#include <list>
#include <assert.h>
#include "scopeguard.h"
#include <QDebug>

YHL::K_means::K_means(const std::string &path)
{
    this->readData (path);
    this->checkException (3);
    qDebug() << "离群点检测结束";
}

oneCluster &YHL::K_means::getDataSet()
{
    return this->dataSet;
}

void YHL::K_means::readData(const std::string &path)
{
    std::ifstream in(path.c_str());
    YHL::ON_SCOPE_EXIT([&]{
        in.close();
        qDebug () << "数据集文件已关闭";
    });
    auto items = 0;
    in >> items;
    auto x = 0.00, y = 0.00;
    for(int i = 0;i < items; ++i) {
        in >> x >> y;
        this->dataSet.emplace_back(std::make_tuple<double, double>(std::move(x), std::move(y)));
    }
    for(const auto& it : dataSet)
        qDebug () << std::get<0>(it) << "\t" << std::get<1>(it);
}

void YHL::K_means::checkException(const double thresholdValue)
{
    this->ans = this->getCluster (3, 1e-3);
    qDebug () << "\n************** 开始离群点检测 ***************";
    auto &clusters = std::get<0>(ans);
    auto &centers = std::get<1>(ans);
    int lSize = static_cast<int>(clusters.size());
    int rSize = static_cast<int>(centers.size());
    assert(lSize == rSize);
    for(int i = 0;i < lSize; ++i) {
        qDebug() << "1";
        std::vector<double> arr;
        int r = static_cast<int>(clusters[i].size()); qDebug() << "2";
        // 求得这个簇中每个点和中心的距离
        for(int j = 0;j < r; ++j) {
            arr.emplace_back(getDistance(clusters[i][j], centers[i]));
        }qDebug() << "3";
        // 求得中位数的大小
        auto temp = arr;
        std::sort(temp.begin(), temp.end()); qDebug() << "4";
        int midSize = temp.size() >> 1;
        qDebug() << "midSize  :  " << midSize;
        qDebug() << "temp     :  " << temp.size ();
        auto edge = temp[midSize]; qDebug() << "5";

        // 与中位数的比值 > 阈值的点在 数据集 中删除
        std::vector<point> ans, target;
        qDebug () << "第 " << i + 1 << " 个簇中 : ";
        for(int j = 0;j < r; ++j) {
            if(arr[j] / edge > thresholdValue) {
                qDebug () << "舍弃 ( " << std::get<0>(clusters[i][j]) << " , " << std::get<1>(clusters[i][j]) << " )";
                target.emplace_back(clusters[i][j]);
            }
        }
        // 将要删除的目标找到,然后逐个删除
        for(auto &it : target) {
            auto beg = dataSet.begin();
            while(beg not_eq dataSet.end()) {
                if(std::get<0>(*beg) == std::get<0>(it) and
                   std::get<1>(*beg) == std::get<1>(it)) {
                    beg = dataSet.erase(beg);
                }
                else ++beg;
            }
        }
        qDebug () << "\n数据集长度  :  " << dataSet.size() << "\n\n";
    }
    qDebug() << "出来了";
}

const answerType& YHL::K_means::getCluster
    (const int k, const double thresholdValue)
{

    // 还可以预处理,删掉极端点
    auto dataSize = dataSet.size();
    assert(k <= dataSize); // 如果聚类数 > 数据量,这是错误的
    oneCluster centers;
    // 先选定 k 个随机的中心点
    std::vector<int> book(k, 0);
    srand(time(nullptr));
    for(int i = 0;i < k; ++i) {
        auto j = rand() % dataSize;
        while(book[j] == 1)
            j = rand() % dataSize;
        centers.emplace_back(dataSet[j]);
    }

    // clusters 存储的每一个元素都是一个簇, 预先分配 K 个簇的空间
    std::vector< oneCluster > clusters;
    clusters.assign(k, oneCluster());

    double oldValue = 1.00, newValue = 1.00; int cnt = 0;

    while(true) {
        qDebug () << "\n********** 第 " << ++cnt << "  次聚类 ************\n";

        // 每个点找出离它最近的中心点, 放在第 label 个簇中
        for(const auto& it : dataSet) {
            auto label = getLabel(it, centers);
            qDebug() << "label  :  " << label << "\n";
            assert(0 <= label and label < k);
            clusters[label].emplace_back(it);
        }
        // print(clusters);

        // 重新计算每个簇的中心点
        for(int i = 0;i < k; ++i) {
            centers[i] = getCenter(clusters[i]);
            qDebug () << "第 " << i + 1 << " 个簇的中心点是  :  " << "( " << std::get<0>(centers[i]) << " , " << std::get<1>(centers[i]) << " )";
        }

        // 重新衡量这次的最小函数值
        oldValue = newValue;
        // 先存储上次的最小均方差之和
        newValue = getEvaluate(clusters, centers);
        qDebug () << "oldValue  :  " << oldValue;
        qDebug () << "newValue  :  " << newValue;
        if(std::abs(oldValue - newValue) / newValue < thresholdValue)
        // 如果变化小于阈值,就结束 也可以是差
        {
            this->ans = std::make_tuple<std::vector<oneCluster>, oneCluster>
                    (std::move(clusters), std::move(centers), newValue);
            return this->ans;
        }
        // 每次聚类,得到的聚类都是不一样的,所以上次的记录要清空
        for(auto &it : clusters)
            it.clear();
    }
}

void YHL::K_means::display() const
{
    auto &clusters = std::get<0>(ans);
    auto &centers = std::get<1>(ans);
    auto newValue = std::get<2>(ans);
    {
        for(const auto& it : clusters) {
            qDebug () << "\n*******************\n";
            for(const auto& r : it) {
                qDebug () << "( " << std::get<0>(r) << " , " << std::get<1>(r) << " )";
            }
        }
    }
    {
        qDebug () << "\n局部最优均方差之和是  :  " << newValue << "\n";
        int cnt = 0;
        for(const auto& it : centers) {
            qDebug () << "第 " << ++cnt << " 个簇的中心点是  :  " << "( " << std::get<0>(it) << " , " << std::get<1>(it) << " )\n";
        }
    }
}

double YHL::getDistance(const point &a, const point &b)
{
    return sqrt(pow(std::get<0>(a) - std::get<0>(b), 2) +
                pow(std::get<1>(a) - std::get<1>(b), 2));
}

int YHL::getLabel(const point &one, const oneCluster &centers)
{
    auto Min = 1e6;
    int label = -1, centerSize = static_cast<int>(centers.size());
    for(int i = 0;i < centerSize; ++i) {
        auto ans = getDistance(centers[i], one);
        if(ans < Min) {
            Min = ans;
            label = i;
        }
    }
    return label;
}

point YHL::getCenter(const oneCluster &one)
{
    double mean_x = 0.00, mean_y = 0.00;
    for(const auto& it : one) {
        mean_x += std::get<0>(it);  // 取横坐标
        mean_y += std::get<1>(it);  // 取纵坐标
    }
    int scale = static_cast<int>(one.size());
    if(scale == 0)  // 这里要特别注意
        scale = 1;
    return std::make_tuple<double, double>(mean_x / scale, mean_y / scale);
}

double YHL::getEvaluate(const std::vector<oneCluster>& clusters,
                                 const oneCluster& centers)
{
    double ans = 0;
    int lSize = static_cast<int>(clusters.size());
    int rSize = static_cast<int>(centers.size()); // 一个簇对应一个中心点
    assert(lSize == rSize);
    for(auto i = 0;i < lSize; ++i) {
        // it 代表一个簇, 计算这个簇每一个点 和 "虚拟"中心点的距离(中心点可能不在簇中,
        // 毕竟求的是均值所在)
        int oneSize = static_cast<int>(clusters[i].size());
        for(int k = 0;k < oneSize; ++k) {
            ans += getDistance(clusters[i][k], centers[i]);
            // 第 i 个簇的每个点, 计算和这个簇的中心点的距离
        }
    }
    return ans;
}


