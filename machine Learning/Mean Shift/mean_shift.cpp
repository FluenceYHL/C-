#include "mean_shift.h"
#include "scopeguard.h"
#include <fstream>
#include <string>
#include <random>
#include <ctime>
#include <cmath>
#include <QVector>
#include <QDebug>

namespace {
    double getDistance(const YHL::point& a, const YHL::point& b) {
        double ans = 0.00;
        int len = a.features.size ();
        for(int i = 0;i < len; ++i) {
            ans += std::pow (a.features[i] - b.features[i], 2);
        }
        return std::sqrt (ans);
    }

    int getMaxIndex(const std::unordered_map<int, int>& container) {
        auto max = -1e12;
        int index = 0;
        for(const auto& it : container) {
            if(max < it.second) {
                max = it.second;
                index = it.first;
            }
        }
        return index;
    }
}

void YHL::Mean_Shift::clear()
{
    std::vector< std::unordered_map<int, int> >().swap (color);
    this->clusters.clear ();
}

void YHL::Mean_Shift::initCluster()
{
    this->color.assign (this->len, std::unordered_map<int, int>());
}

void YHL::Mean_Shift::merge(const YHL::point &u, const int cnt)
{
    auto min = 1e12;
    int best;
    for(const auto& it : this->centers) {
        double distance = getDistance (it.second, u);
        if(distance < min) {
            min = distance;
            best = it.first;
        }
    }
    constexpr double dis = 2;
    if(min < dis) {/**/}
    this->centers.emplace (cnt, u);
}

// u 点漂移 (属于簇 cnt)
void YHL::Mean_Shift::shift(const int u, const int cnt)
{
    constexpr int Iter = 10;
    point center = this->dataSet[u];
    for(int i = 0;i < Iter; ++i) {
        std::vector<int> territory;
        for(int j = 0;j < this->len; ++j) {
            if(j == u) continue;
            if(getDistance (center, dataSet[j]) < radius) {
                territory.emplace_back (j);
                ++this->color[j][cnt];
            }
        }
        auto pre = center;
        const int l = territory.size ();
        const int r = center.features.size ();
        for(const auto& it : territory) {
            for(int j = 0;j < r; ++j) {
                double res = this->dataSet[it].features[j] - center.features[j];
                center.features[j] += res * 1.00 / l;
            }
        }
        double distance = getDistance (pre, center);
        if(distance < threshold)
            break;
    }
    // this->merge (center, cnt);
}
/*
 * 1 while  还有点未被染色
 * 2    从未染色的点中随机选一个点
 * 3    找到这个点 半径范围 radius 内的所有点
 * 4    for 点 in 圆周
 * 5        标记被簇 cnt 访问一次（染色）
 * 6    计算圆周所有点的合力，对中心点产生的位移（可以采用高斯核函数）
 * 7    重复 2-6 ，直到中心点偏移 < 某个阈值
 * 8
 * 9 每个点，被哪个簇访问的次数最大，就把这个点归到这个簇
 */

void YHL::Mean_Shift::makecluster()
{
    int cnt = 0;
    for(int i = 0;i < this->len; ++i) {
        if(this->color[i].empty ())
            this->shift (i, ++cnt);
    }
    for(int i = 0;i < this->len; ++i) {
        auto index = getMaxIndex (color[i]);
        this->clusters[index].emplace_back (this->dataSet[i]);
    }
}

const std::unordered_map<int, std::list<YHL::point> > &
    YHL::Mean_Shift::getCluster(const double _radius, const double _threshold)
{
    this->radius = _radius;
    this->threshold = _threshold;
    this->clear ();
    this->initCluster ();
    this->makecluster ();
    return this->clusters;
}

void YHL::Mean_Shift::loadFile(const std::string &fileName)
{
    std::ifstream in(fileName.c_str ());
    ON_SCOPE_EXIT ([&]{
        in.close ();
    });
    assert (in);
    in >> this->len;
    double x, y;
    for(int i = 0;i < len; ++i) {
        in >> x >> y;
        this->dataSet.emplace_back (x, y);
    }
}

std::pair<QVector<double>, QVector<double> > YHL::Mean_Shift::getPoints() const
{
    std::pair<QVector<double>, QVector<double> > one;
    for(const auto& it : this->dataSet) {
        one.first.push_back (it.features[0]);
        one.second.push_back (it.features[1]);
    }
    return one;
}

std::pair<QVector<double>, QVector<double> > YHL::Mean_Shift::getEfficiency()
{
    return this->efficiency;
}



