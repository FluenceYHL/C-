#ifndef FCM_H
#define FCM_H
#include <iostream>
#include <vector>
#include <QPoint>
#include <tuple>
#include <QVector>
#include <list>
#include <unordered_map>
#include <QDebug>

namespace YHL {

    struct point {
        std::vector<double> features;
        point() = default;
        point(const double x, const double y) {
            this->features.emplace_back (x);
            this->features.emplace_back (y);
        }
        point(const std::vector<double>& rhs) : features(rhs){}
        void display() const {
            qDebug() << features[0] << " , " << features[1];
        }
    };

    class SOM {
        static constexpr int Iters = 1000;
        static constexpr double N = 10;
        static constexpr double M = 12;
    private:
        int len = 0;
        double elta, radius;
        point winner;
        std::vector<point> oldData;
        std::vector<point> dataSet;
        std::vector< std::vector< point > > weights;

        std::unordered_map<int, std::list<point> > clusters;
        std::pair< QVector<double>, QVector<double> > efficiency;
    private:
        void makeCluster(const double threshold);
        void initCluster();
        void getWinner(const int u);
        void updateElta(const int t);
        void updateNeibors(const int u);
        void collect();
    public:
        SOM() = default;
        SOM(SOM&&) = default;
        void loadFile(const std::string& fileName);
        std::pair<QVector<double>, QVector<double> > getPoints() const;
        std::unordered_map<int, std::list<point> >
            getCluster(const double threshold, const double _radius);
        std::pair<QVector<double>, QVector<double> > getEfficiency();
    };

}

#endif // FCM_H
