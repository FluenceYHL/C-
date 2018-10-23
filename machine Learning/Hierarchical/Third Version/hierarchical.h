#ifndef HIERARCHICAL_H
#define HIERARCHICAL_H
#include <iostream>
#include <vector>
#include <memory>
#include <string>
#include <cmath>
#include <list>
#include <map>
#include <set>

namespace YHL {

    struct point {
        double x, y;
        point(const double _x, const double _y);
        bool operator==(const point& rhs);
    };
    double getDistance(const point& a, const point& b);

    class Hierarchical {

        using oneCluster = std::vector<point>;
        using ansType = std::list< std::list<point> >;

        struct Pair {
            int u;
            double dis;
            Pair(const int _u, const double _dis)
            : u(_u), dis(_dis) {}
            bool operator<(const Pair& rhs) const {
                if(this->u == rhs.u) return false;
                return this->dis < rhs.dis;
            }
            bool operator==(const Pair& rhs) const {
                return this->u == rhs.u;
            }
        };

    private:
        int len = 0;
        oneCluster dataSet;
        std::vector< int > father;
        std::map<int, std::set<Pair> > dis;
        std::map<int, std::list<int> > clusters;
        std::string PATH;
    private:
        void initDis();
        void initFather();
        int find(const int u);
        void mergeClusters(const int i, const int j);
        void getCluster(const double threshold);
        void makeClusters();
    public:
        void readData(const std::string& path = "hierarchical.txt");
        const ansType getClusters(const double threshold);
        const oneCluster& getDataSet() const;
    };

    /*
        1. 改成整个簇和其他簇的平均距离, 这个其实,只要动态存储 cluster 就可以,找到并查集的中心即可, find
        2. 改成 std::map<int, std::set<double> >  差 < 1e-3, 避免大量的重复计算
        int main() {
            YHL::Hierarchical one;
            one.readData("hierarchical.txt");
            one.getCluster(3);
            return 0;
        }
    */

}



#endif // HIERARCHICAL_H
