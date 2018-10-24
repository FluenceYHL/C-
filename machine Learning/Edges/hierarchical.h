#ifndef HIERARCHICAL_H
#define HIERARCHICAL_H
#include <iostream>
#include <vector>
#include <memory>
#include <string>
#include <cmath>
#include <list>
#include <unordered_map>
#include <set>

namespace YHL {

    struct point {
        double x, y;
        point(const double _x, const double _y);
        bool operator==(const point& rhs) const;
    };
    double getDistance(const point& a, const point& b);

    class Hierarchical {

        using oneCluster = std::vector<point>;
        using ansType = std::unordered_map<int, std::list<int> >;
        struct Node {
            int end, next;
            Node(const int _end, const int _next)
                : end(_end), next(_next) {}
        };
    private:
        int len = 0;
        oneCluster dataSet;
        int k;
        std::vector<Node> edges;
        std::vector<int> head;
        std::vector<int> color;
        std::unordered_map<int, std::list<int> > clusters;
        std::string path;
    private:
        void DFS(const int u);
        void BFS(const int u);
        void initDis(const double threshold);
    public:
        void readData(const std::string& _path = "hierarchical.txt");
        const ansType getClusters(const double threshold);
        const oneCluster& getDataSet() const;
    };
}



#endif // HIERARCHICAL_H
