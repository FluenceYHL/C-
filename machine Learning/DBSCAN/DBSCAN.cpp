#include "DBSCAN.h"
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
}

YHL::DBSCAN::DBSCAN(const int _minPts, const double _radius)
    : minPts(_minPts), radius(_radius)
{}

void YHL::DBSCAN::initCluster()
{
    this->color.assign (this->len, -1);
    this->territory.assign (this->len, std::vector<int>());
    for(int i = 0;i < this->len; ++i) {
        for(int j = 0;j < this->len; ++j) {
            if(i == j)
                continue;
            if(getDistance (this->dataSet[i], this->dataSet[j]) <= radius)
                this->territory[i].emplace_back (j);
        }
        if(this->territory[i].size () < this->minPts)
            std::vector<int>().swap (this->territory[i]);
    }
}

void YHL::DBSCAN::makecluster()
{
    int cnt = 0;
    for(int i = 0;i < this->len; ++i) {
        if(color[i] == -1) {
            if(this->territory[i].empty ()) {
                color[i] = -2;
                continue;
            }
            color[i] = ++cnt;
            this->clusters[color[i]].emplace_back(this->dataSet[i]);
            DFS(i);
        }
    }
}

void YHL::DBSCAN::DFS(const int u)
{
    if(this->territory[u].empty ())
        return;
    for(const auto& it : this->territory[u]) {
        if(color[it] < 0) {
            color[it] = color[u];
            this->clusters[color[it]].emplace_back(this->dataSet[it]);
            DFS(it);
        }
    }
}

void YHL::DBSCAN::clear()
{
    for(int i = 0;i < this->len; ++i)
        this->color[i] = -1;
    this->clusters.clear ();
}

const std::unordered_map<int, std::list<YHL::point> > &
    YHL::DBSCAN::getCluster()
{
    qDebug () << "半径  :  " << this->radius;
    qDebug () << "个数  :  " << this->minPts;
    this->clear (); qDebug () << "OK1\n";
    this->makecluster ();qDebug () << "OK2\n";
    return this->clusters;
}

std::pair<QVector<double>, QVector<double> > YHL::DBSCAN::getEfficiency()
{
    return this->efficiency;
}

void YHL::DBSCAN::loadFile(const std::string &fileName)
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
    this->initCluster ();
}

std::pair<QVector<double>, QVector<double> > YHL::DBSCAN::getPoints() const
{
    std::pair<QVector<double>, QVector<double> > one;
    for(const auto& it : this->dataSet) {
        one.first.push_back (it.features[0]);
        one.second.push_back (it.features[1]);
    }
    return one;
}



