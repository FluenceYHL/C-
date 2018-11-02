#ifndef FCM_H
#define FCM_H
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

    class DBSCAN {
        static constexpr double m = 2;
        static constexpr int Times = 10;
    private:
        int len = 0;
        int minPts;
        double radius;
        std::vector<int> color;
        std::vector<point> dataSet;
        std::vector< std::vector<int> > territory; // 领土
        std::pair< QVector<double>, QVector<double> > efficiency;
        std::unordered_map<int, std::list<point> > clusters;
    private:
        void initCluster();
        void makecluster();
        void DFS(const int u);
        void clear();
    public:
        DBSCAN(const int _minPts, const double _radius);
        DBSCAN(DBSCAN&&) = default;
        void loadFile(const std::string& fileName);
        std::pair<QVector<double>, QVector<double> > getPoints() const;
        const std::unordered_map<int, std::list<point> >&
            getCluster();
        std::pair<QVector<double>, QVector<double> > getEfficiency();
    };

}

#endif // FCM_H
