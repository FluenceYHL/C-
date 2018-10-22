#ifndef K_MEANS_H
#define K_MEANS_H
#include <iostream>
#include <vector>
#include <tuple>
#include <memory>
#include <string>
using point = std::tuple<double, double>;
using oneCluster = std::vector<point>;
using answerType = std::tuple<std::vector<oneCluster>, oneCluster, double>;

namespace YHL {

    double getDistance(const point& a, const point& b);
    int getLabel(const point& one, const oneCluster& centers);
    point getCenter(const oneCluster& one);
    double getEvaluate(const std::vector<oneCluster>& clusters,
                       const oneCluster& centers);

    class K_means {
    private:
        answerType ans;
        oneCluster dataSet;
    public:
        K_means(const std::string& path = "k-means_check.txt");
        oneCluster& getDataSet();
        answerType& getCluster();
        void display () const;
        const answerType& getCluster(const int k, const double thresholdValue);
    private:
        void readData(const std::string& path);
        void checkException(const double thresholdValue);
    };

}

#endif // K_MEANS_H







