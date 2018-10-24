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
        for(int k = head[front];k not_eq -1; k = edges[k].next) {
            auto v = edges[k].end;
            if(color[v] == 0) {
                color[v] = color[u];
                clusters[color[v]].emplace_back(v);
                Q.push (v);
            }
        }
    }
}

// 深度优先搜索
void YHL::Hierarchical::DFS(const int u)
{
    for(int k = head[u];k not_eq -1; k = edges[k].next) {
        auto v = edges[k].end;
        if(color[v] == 0) {
            color[v] = color[u];
            clusters[color[v]].emplace_back(v);
            DFS(v);
        }
    }
}

// 初始化矩阵
void YHL::Hierarchical::initDis(const double threshold) {
    this->head.assign (len, -1);
    this->color.assign (len, 0);
    for(int i = 0;i < len; ++i) {
        for(int j = 0;j < len; ++j) {  // j < i
            if(i == j) continue;
            auto distance = getDistance (dataSet[i], dataSet[j]);
            if(distance - threshold <= 0) {
                this->edges.emplace_back(j, head[i]);
                head[i] = k++;
            }
        }
    }
}

const YHL::Hierarchical::ansType YHL::Hierarchical::getClusters
        (const double threshold) {
    // 注意第二次聚类, 过去数据的清理
    if(this->len == 0)
        this->readData (path);
    // 初始化邻接表
    this->initDis (threshold);
    int cnt = 0;
    for(int i = 0;i < len; ++i) {
        if(color[i] == 0) {
            color[i] = ++cnt;  // 开始染色
            clusters[cnt].emplace_back(i);
            BFS(i);
        }
    }
    // 后期处理离群点
    auto it = clusters.begin ();
    while(it not_eq clusters.end ()) {
        if(it->second.size () < 5) {
            for(auto r : it->second) {
                others.emplace(r, 1);
            }
            it = clusters.erase (it);
        }
        else ++it;
    }
    // 为下次聚类判断是否要读文件做铺垫
    this->len = 0;
    return this->clusters;
}

const std::unordered_map<int, int>& YHL::Hierarchical::getOthers()
{
    return this->others;
}

// 读取数据
void YHL::Hierarchical::readData(const std::string &_path)
{
    this->path = _path;
    this->len = 0;
    this->dataSet.clear ();
    this->clusters.clear ();

    std::ifstream in(path.c_str());
    ON_SCOPE_EXIT([&]{ in.close(); });
    in >> this->len;
    double x = 0.00, y = 0.00;
    for(int i = 0;i < this->len; ++i) {
        in >> x >> y;
        this->dataSet.emplace_back(x, y);
    }
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
