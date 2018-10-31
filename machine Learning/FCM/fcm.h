#ifndef FCM_H
#define FCM_H
#include <iostream>
#include <vector>
#include <QPoint>
#include <tuple>
#include <list>
#include <unordered_map>

namespace YHL {

    struct point {
        double x, y;
        point(const double _x, const double _y) : x(_x), y(_y){}
    };

    class FCM {
        static constexpr double m = 2;
        static constexpr int Times = 10;
    private:
        int C = 3;
        int len = 0;
        std::vector<point> centers;
        std::vector< std::vector<double> > belongs;
        std::vector<point> dataSet;
    private:
        int getBestNum();
        point getCenter(const int i);
        double updateBelong(const int i, const int j);
        void cluster();
        void init();
    public:
        FCM() = default;
        FCM(FCM&&) = default;
        void loadFile(const std::string& fileName);
        std::pair<QVector<double>, QVector<double> > getPoints() const;
        std::tuple<double, double, double, double> getRange() const;
        std::unordered_map<int, std::list<point> > getCluster();
    };

}

#endif // FCM_H
