#ifndef MEAN_SHIFT_H
#define MEAN_SHIFT_H
#include <iostream>
#include <vector>
#include <QPoint>
#include <tuple>
#include <QVector>
#include <list>
#include <unordered_map>

namespace YHL {

    struct point {
        std::vector<double> features;
        point() = default;
        point(const double x, const double y) {
            this->features.emplace_back (x);
            this->features.emplace_back (y);
        }
        point(const std::vector<double>& rhs) : features(rhs){}
    };

    class Mean_Shift {
        static constexpr double m = 2;
        static constexpr int Times = 10;
    private:
        int len = 0;
        double radius;   // 半径
        double threshold; // 两个簇 < 这个距离就合并
        std::vector<point> dataSet;
        std::vector< std::unordered_map<int, int> > color;  // i 这个点属于簇 j 的次数
        std::unordered_map<int, point> centers;
        std::unordered_map<int, std::list<point> > clusters;
        std::pair< QVector<double>, QVector<double> > efficiency;
    private:
        void merge(const point& u, const int cnt);
        void shift(const int u, const int cnt);
        void initCluster();
        void makecluster();
        void clear();
    public:
        Mean_Shift() = default;
        Mean_Shift(Mean_Shift&&) = default;
        void loadFile(const std::string& fileName);
        std::pair<QVector<double>, QVector<double> > getPoints() const;
        const std::unordered_map<int, std::list<point> >&
            getCluster(const double _radius, const double _threshold);
        std::pair<QVector<double>, QVector<double> > getEfficiency();
    };

}

#endif // FCM_H
