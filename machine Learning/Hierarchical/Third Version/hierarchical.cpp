#include "hierarchical.h"
#include <QDebug>
#include <functional>
#include <algorithm>
#include <fstream>
#include <cmath>
#include <assert.h>
#include "scopeguard.h"

double YHL::getDistance(const YHL::point &a, const YHL::point &b) {
    return sqrt(pow(a.x - b.x, 2) + pow(a.y - b.y, 2));
}

YHL::point::point(const double _x, const double _y)
    : x(_x), y(_y)
{}

bool YHL::point::operator==(const YHL::point &rhs) {
    return (this->x - rhs.x) < 1e-3 and (this->y - rhs.y) < 1e-3;
}

void YHL::Hierarchical::initDis() {
    for(int i = 0;i < this->len; ++i) {
        for(int j = 0;j < i; ++j) {
                if(i == j) continue;
                auto distance = getDistance(this->dataSet[i], this->dataSet[j]);
                this->dis[i].emplace(j, distance);
        }
    }
}

void YHL::Hierarchical::initFather() {
    for(int i = 0;i < this->len; ++i)
        this->father.emplace_back(i);
}

int YHL::Hierarchical::find(const int u) {
    return u == this->father[u] ? u : this->father[u] = find(this->father[u]);
}

void YHL::Hierarchical::mergeClusters(const int i, const int j) {
    int x = find(i);
    int y = find(j);
    if(x == y) return;
    this->father[x] = y;
    // x 所在簇归并到 y, x 这个点就算消失了
    this->dis[x].clear();
    for(auto& it : this->dis) {
        auto res = it.second.find(Pair(x, 0));
        if(res not_eq it.second.end())
            it.second.erase(res);
    }
}

void YHL::Hierarchical::readData(const std::string &path) {
    this->PATH = path;
    this->len = 0;
    this->dataSet.clear ();
    this->father.clear ();
    this->clusters.clear ();
    this->dis.clear ();

    std::ifstream in(path.c_str());
    ON_SCOPE_EXIT([&]{ in.close(); });
    in >> this->len;
    double x = 0.00, y = 0.00;
    for(int i = 0;i < this->len; ++i) {
        in >> x >> y;
        this->dataSet.emplace_back(x, y);
    }
    this->initFather();
    this->initDis(); // 构造相似度矩阵
}

const YHL::Hierarchical::ansType YHL::Hierarchical::getClusters(const double threshold)
{
    if(this->len > 0)
        this->readData (this->PATH);
    this->getCluster (threshold);
    std::list< std::list<point> > ans;
    for(const auto& it : this->clusters) {
        std::list<point> null_one;
        // 其实在这里就可以直接删掉一些离群点
        if(it.second.size () < 5)
            continue;
        for(const auto r : it.second)
            null_one.emplace_back(this->dataSet[r]);
        ans.emplace_back(std::move(null_one));
    }
    return ans;
}

void YHL::Hierarchical::getCluster(const double threshold) {
    /*
       1、把每个样本归为一类，计算每两个类之间的距离，也就是样本与样本之间的相似度；
       2、寻找各个类之间最近的两个类，把他们归为一类（这样类的总数就少了一个）；
       3、重新计算新生成的这个类与各个旧类之间的相似度；
       4、重复2和3直到所有样本点都归为一类，结束
       5. 逐一 father = find(i)
    */
    while(true) {
        auto minInf = 1e9;
        int aPos = -1, bPos = -1;
        for(const auto& it : this->dis) {
            // it.first 某个点, it.second 集合, 和其他点的距离
            if(it.second.empty())
                continue;
            auto head = *it.second.begin();
            auto eachMin = head.dis;
            if(minInf > eachMin) {
                minInf = eachMin;
                aPos = it.first;
                bPos = head.u;
            }
        }
        //qDebug () << "最近距离是 :  " << minInf;
        if(minInf > threshold) break;
        //qDebug () << "最近的两个点是  :  " << aPos << "  " << bPos << "\n";
        this->mergeClusters(aPos, bPos);
    }
    this->makeClusters();
}

void YHL::Hierarchical::makeClusters()
{
    //qDebug () << "\n合并结束  :  \n";
    for(int i = 0;i < len; ++i)
        this->father[i] = find(i);
    for(int i = 0;i < len; ++i) {
        //qDebug () << i << "  father  is  :  " << this->father[i];

        // clusters[this->father[i]].emplace_back(i);
        auto category = this->father[i];
        auto res = this->clusters.find(category);
        if(res == this->clusters.end()) {
            std::list<int> null_one { i };
            this->clusters.emplace(category, std::move(null_one));
        }
        else res->second.emplace_back(i);
    }
    //qDebug () << "最终聚类结果是 : \n";
    for(const auto& it : this->clusters) {
        //qDebug () << "分类标号 " << it.first << "  :  ";
        //for(const auto r : it.second)
            //qDebug () << r << "  ";
        //qDebug () << "";
    }
}

const YHL::Hierarchical::oneCluster &YHL::Hierarchical::getDataSet() const
{
    return this->dataSet;
}
