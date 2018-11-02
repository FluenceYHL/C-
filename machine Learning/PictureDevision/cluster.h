#ifndef CLUSTER_H
#define CLUSTER_H
#include <iostream>
#include <vector>
#include <unordered_map>
#include <QImage>

class Cluster {
private:
    std::vector< std::vector<double> > collect;
    std::vector<int> father;
    std::vector<int> rank;
    int len;
private:
    int find(const int u);
    void merge(const int i, const int j);
    void initFather();
    void initGragh(const double threshold);
public:
    Cluster() = default;
    ~Cluster() = default;
    void loadPicture(QImage *const image);
    std::unordered_map<int, std::list< std::vector<double> > >
        getCluster(const double threshold);
};

#endif // CLUSTER_H
