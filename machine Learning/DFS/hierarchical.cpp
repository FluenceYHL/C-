#include "hierarchical.h"
#include <QDebug>
#include <functional>
#include <algorithm>
#include <fstream>
#include <cmath>
#include <queue>
#include <assert.h>
#include <QMessageBox>
#include "scopeguard.h"
#include <QDebug>

// 宽度优先搜索
void YHL::Hierarchical::BFS (const int u) {
    std::queue<int> Q;
    Q.push (u);
    while(!Q.empty ()) {
        auto front = Q.front ();
        Q.pop ();
        for(int i = 0;i < this->len; ++i) {
            if(color[i] == 0 and maps[front][i] not_eq 0) {
                color[i] = color[u]; // 这里颜色 u front 都可以
                clusters[color[i]].emplace_back(i);
                Q.push (i);
            }
        }
    }
}

// 深度优先搜索
void YHL::Hierarchical::DFS(const int u)
{
    for(int i = 0;i < this->len; ++i) {
        if(color[i] == 0 and maps[u][i] not_eq 0) {
            color[i] = color[u];
            clusters[color[i]].emplace_back(i);
            DFS (i);
        }
    }
}

// 初始化矩阵
void YHL::Hierarchical::initDis(const double threshold) {

    this->maps.assign (len, std::vector<int>());
    for(auto& it : maps)
        it.assign (len, 0);

    for(int i = 0;i < len; ++i) {
        for(int j = 0;j < len; ++j) {  // j < i
            if(i == j) continue;
            auto distance = getDistance (dataSet[i], dataSet[j]);
            if(distance - threshold <= 0)
                 maps[i][j] = 1;
        }
    }
}

const YHL::Hierarchical::ansType YHL::Hierarchical::getClusters
        (const double threshold) {
    if(this->len == 0)
        this->readData (path);  // 注意第二次聚类, 过去数据的清理
    this->mergeSet (threshold);
/*
    this->initDis (threshold);
    // DFS 深度优先搜索, 也可以 BFS
    int cnt = 0;
    for(int i = 0;i < len; ++i) {
        if(this->color[i] == 0) {
            color[i] = ++cnt;  // 设定为一个类别
            clusters[cnt].emplace_back(i);
            BFS(i);
        }
    }
*/
    // 后期处理离群点
    this->len = 0;
    auto it = clusters.begin ();
    while(it not_eq clusters.end ()) {
        if(it->second.size () < 5)
            it = clusters.erase (it);
        else ++it;
    }
    return this->clusters;
}

// 并查集三连
void YHL::Hierarchical::initFather() {
    for(int i = 0;i < this->len; ++i) {
        this->father.emplace_back(i);
        this->rank.emplace_back(1);
        this->color.emplace_back(0); // 区分
    }
}
int YHL::Hierarchical::find(const int u) {
    return u == this->father[u] ? u : this->father[u] = find(this->father[u]);
}
void YHL::Hierarchical::mergeClusters(const int i, const int j) {
    int x = find(i);
    int y = find(j);
    if(x == y) return;
    if(rank[x] > rank[y])
        std::swap(x, y);
    rank[y] += rank[y];
    father[x] = y;
}

void YHL::Hierarchical::mergeSet(const double threshold)
{
    for(int i = 0;i < len; ++i) {
        for(int j = 0;j < len; ++j) {  // j < i
            if(i == j) continue;
            auto distance = getDistance (dataSet[i], dataSet[j]);
            if(distance - threshold <= 0)
                 mergeClusters (i, j);
        }
    }
    // 合并结束后, 更新祖先
    for(int i = 0;i < len; ++i)
        father[i] = find(i);
    for(int i = 0;i < len; ++i)
        clusters[father[i]].emplace_back(i);
}

// 读取数据
void YHL::Hierarchical::readData(const std::string &_path)
{
    this->path = _path;
    this->len = 0;
    this->dataSet.clear ();
    this->father.clear ();
    this->clusters.clear ();
    this->color.clear ();
    this->maps.clear ();

    std::ifstream in(path.c_str());
    ON_SCOPE_EXIT([&]{ in.close(); });
    in >> this->len;
    double x = 0.00, y = 0.00;
    for(int i = 0;i < this->len; ++i) {
        in >> x >> y;
        this->dataSet.emplace_back(x, y);
    }
    this->initFather();
}

const YHL::Hierarchical::oneCluster &YHL::Hierarchical::getDataSet() const
{
    return this->dataSet;
}

double YHL::getDistance(const YHL::point &a, const YHL::point &b) {
    return sqrt(pow(a.x - b.x, 2) + pow(a.y - b.y, 2));}

YHL::point::point(const double _x, const double _y)
    : x(_x), y(_y){}
bool YHL::point::operator==(const YHL::point &rhs) const {
    return (this->x - rhs.x) < 1e-3 and (this->y - rhs.y) < 1e-3;
}
