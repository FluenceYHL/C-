#include "fcm.h"
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
        return std::sqrt ((a.x - b.x) * (a.x - b.x) + (a.y - b.y) * (a.y - b.y));
    }

    std::vector<double> getRandVector(const int C) {
        static std::default_random_engine e(time(0));
        std::uniform_real_distribution<double> a(0.2, 0.4);
        std::vector<double> one;
        double sum = 0.00;
        for(int i = 0;i < C - 1; ++i) {
            double cur = a(e);
            sum += cur;
            one.emplace_back (cur);
        }
        one.emplace_back (1 - sum);
        for(int i = 0;i < C; ++i)
            qDebug() << one[i];
        return one;
    }

}

YHL::point YHL::FCM::getCenter(const int i)
{
    double denominator = 0.00;
    double numerator1 = 0.00, numerator2 = 0.00;
    for(int j = 0;j < this->len; ++j) {
        double common = std::pow(this->belongs[j][i], m);;
        denominator += common;
        numerator1 += this->dataSet[j].x * common;
        numerator2 += this->dataSet[j].y * common;
    }
    assert (std::fabs (denominator - 0) > 1e-4);
    return point(numerator1 / denominator, numerator2 / denominator);
}

double YHL::FCM::updateBelong(const int i, const int j)
{
    double ans = 0.00;
    double solid = getDistance (centers[i], dataSet[j]);
    for(int k = 0;k < this->C; ++k) {
        double rate = solid / getDistance (centers[k], dataSet[j]);
        ans += std::pow (rate, 2./(m - 1));
    }
    assert (std::fabs (ans - 0) > 1e-4);
    return 1./ans;
}

void YHL::FCM::cluster()
{
    this->init ();
    for(int t = 0;t < Times; ++t) {
        for(int i = 0;i < C; ++i) {
            this->centers[i] = this->getCenter(i);
        }
        for(int j = 0;j < this->len; ++j) {
            for(int i = 0;i < C; ++i) {
                this->belongs[j][i] = this->updateBelong(i, j);
            }
        }
    }
}

void YHL::FCM::init()
{
    this->centers.assign (C, point(0, 0));
    this->belongs.assign (this->len, std::vector<double>());
    for(int i = 0;i < len; ++i) {
        getRandVector (C).swap (this->belongs[i]);
    }
}

void YHL::FCM::loadFile(const std::string &fileName)
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

std::pair<QVector<double>, QVector<double> > YHL::FCM::getPoints() const
{
    std::pair<QVector<double>, QVector<double> > one;
    for(const auto& it : this->dataSet) {
        one.first.push_back (it.x);
        one.second.push_back (it.y);
    }
    return one;
}

std::tuple<double, double, double, double> YHL::FCM::getRange() const
{
    auto min_x = 1e12, min_y = 1e12;
    auto max_x = -1e12, max_y = -1e12;
    for(const auto& it : dataSet) {
        auto x = it.x;
        auto y = it.y;
        if(min_x > x) min_x = x;
        if(min_y > y) min_y = y;
        if(max_x < x) max_x = x;
        if(max_y < y) max_y = y;
    }
    return std::tuple<double, double, double, double>(min_x, min_y, max_x, max_y);
}

std::unordered_map<int, std::list<YHL::point> > YHL::FCM::getCluster()
{
    std::unordered_map<int, std::list<YHL::point> > one;
    this->cluster ();
    for(int j = 0;j < this->len; ++j) {
        auto max = 1e-12;
        int pos = 0;
        for(int i = 0;i < this->C; ++i) {
            if(this->belongs[j][i] > max) {
                max = belongs[j][i];
                pos = i;
            }
        }
        one[pos].emplace_back (this->dataSet[j]);
    }
    return one;
}










