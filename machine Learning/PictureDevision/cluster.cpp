#include "cluster.h"
#include <QRgb>
#include <QDebug>
#include <cmath>

namespace {
    double getDistance(const std::vector<double>& a, const std::vector<double>& b){
        int l = a.size ();
        int r = b.size ();
        assert (l == r);
        double ans = 0.00;
//        for(int i = 0;i < l; ++i)
//            ans += std::pow (a[i] - b[i], 2);
        ans += std::pow (a[0] * 10.00 / 361 - b[0] * 10.00 / 361, 2);
        ans += std::pow (a[1] * 10.00 / 231 - b[1] * 10.00 / 231, 2);
        ans += std::pow (a[2] * 10.00 / 256 - b[2] * 10.00 / 256, 2);
        ans += std::pow (a[3] * 10.00 / 256 - b[3] * 10.00 / 256, 2);
        ans += std::pow (a[4] * 10.00 / 256 - b[4] * 10.00 / 256, 2);
        return std::sqrt (ans);
    }
}

int Cluster::find(const int u)
{
    return u == this->father[u] ? u : this->father[u] = find(this->father[u]);
}

void Cluster::merge(const int i, const int j)
{
    int lhs = this->find (i);
    int rhs = this->find (j);
    if(lhs == rhs) return;
    if(rank[lhs] < rank[rhs])
        std::swap (lhs, rhs);
    rank[lhs] += rank[rhs];
    father[rhs] = lhs;
}

void Cluster::initFather()
{
    this->father.reserve (this->len);
    for(int i = 0;i < this->len; ++i)
        this->father.emplace_back (i);
    this->rank.assign (this->len, 1);
}

void Cluster::initGragh(const double threshold)
{
    qDebug () << "len  :  " << len;
    qDebug () << "threshold  :  " << threshold;
    for(int i = 0;i < this->len; ++i) {
        for(int j = 0;j < this->len; ++j) {
            if(i == j) continue;
            double distance = getDistance (collect[i], collect[j]);
            if(distance < threshold) {
                this->merge(i, j);
            }
        }
    }
    for(int i = 0;i < this->len; ++i)
        this->father[i] = find(i);
}

void Cluster::loadPicture(QImage * const image)
{
    static constexpr double N = 0.60;
    static constexpr double M = 1 - N;
    QRgb *oneLine = nullptr;
    const int height = image->height ();
    const int width = image->width ();
    for(int i = 0;i < height; ++i) {
        oneLine = (QRgb*)image->scanLine (i);   // 还可以设置比重 N, M 距离为主还是颜色为主
        for(int j = 0;j < width; ++j) {
            std::vector<double> point;
            point.emplace_back (j);
            point.emplace_back (i);
            point.emplace_back (qRed (oneLine[j]));
            point.emplace_back (qGreen (oneLine[j]));
            point.emplace_back (qBlue (oneLine[j]));
            this->collect.emplace_back (std::move(point));
        }
    }
    this->len = height * width;
    this->initFather ();
}

std::unordered_map<int, std::list<std::vector<double> > > Cluster::getCluster(const double threshold)
{
    std::unordered_map<int, std::list<std::vector<double> > > ans;
    this->initGragh (threshold);
    for(int i = 0;i < this->len; ++i) {
        ans[father[i]].emplace_back (this->collect[i]);
    }
    qDebug() << "suze  :  " << ans.size ();
    return ans;
}




