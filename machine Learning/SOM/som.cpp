#include "som.h"
#include "scopeguard.h"
#include <fstream>
#include <string>
#include <random>
#include <ctime>
#include <cmath>
#include <QVector>
#include <QDebug>

namespace {

    double getDistance(const YHL::point& a, const YHL::point& b) {
        double ans = 0.00;
        int len = a.features.size ();
        for(int i = 0;i < len; ++i) {
            ans += std::pow (a.features[i] - b.features[i], 2);
        }
        return std::sqrt (ans);
    }

    void makeRandom(std::vector<double>& matrix) {
        static std::default_random_engine e(time(0));
        std::uniform_real_distribution<double> a(-1, 1);
        for(auto& it : matrix) {
            it = a(e);
        }
    }

    void normalize(std::vector<double>& Vector) {
        double mo = 0.00;
        for(const auto& it : Vector) {
            mo += it * it;
        }
        mo = std::sqrt (mo);
        for(auto& it : Vector)
            it = it * 1.00 / mo;
    }

    double getCos(const std::vector<double>& a,
                        const std::vector<double>& b) {
        double ans = 0.00;
        const int l = a.size ();
        const int r = b.size ();
        assert (l == r);
        for(int i = 0;i < l; ++i) {
            ans += a[i] * b[i];
        }
        return ans;
    }

}

void YHL::SOM::initCluster()
{
    this->clusters.clear ();
    for(auto& it : this->weights) {
        for(auto & r : it) {
            makeRandom (r.features);
            normalize (r.features);
        }
    }
}

void YHL::SOM::getWinner(const int u)
{
    auto max = -1e12;
    for(const auto& n : this->weights) {
        for(const auto& m : n) {
            double distance = getCos (m.features, this->dataSet[u].features);
            if(distance > max) {  // std::function<double>()
                max = distance;
                winner = m;
            }
        }
    }
}

void YHL::SOM::updateElta(const int t)
{
    this->radius = 0.80 - (t + 1.00) * 0.75 / Iters;
    this->elta = 0.50 / (1 + t) * std::exp (-radius);
}

void YHL::SOM::updateNeibors(const int u)
{
    auto& input = this->dataSet[u];
    for(auto& n : this->weights) {
        for(auto& m : n) {
            auto distance = getCos (m.features, winner.features);
            if(distance > radius) {
                const int l = m.features.size ();
                for(int i = 0;i < l; ++i) {
                    m.features[i] += this->elta * distance * (input.features[i] - m.features[i]);
                }
                normalize (m.features);
            }
        }
    }
}

void YHL::SOM::collect()
{
    // 用并查集合并
}

std::unordered_map<int, std::list<YHL::point> >
    YHL::SOM::getCluster(const double threshold, const double _radius)
{
    this->radius = _radius;
    this->initCluster ();
    srand(time(nullptr));
    for(int t = 0;t < Iters; ++t) {
        int u = rand () % this->len;
        this->getWinner (u);
        this->updateNeibors (u);
        this->updateElta (t);
        if(this->elta < threshold)
            break;
    }
    this->collect ();
    return this->clusters;
}

void YHL::SOM::loadFile(const std::string &fileName)
{
    std::ifstream in(fileName.c_str ());
    ON_SCOPE_EXIT ([&]{
        in.close ();
    });
    assert (in);
    in >> this->len;
    double x, y;
    for(int i = 0;i < len; ++i) {
        in >> x >> y;
        this->oldData.emplace_back (x, y);
        this->dataSet.emplace_back (x, y);
        normalize (this->dataSet.back ().features);
    }
    this->weights.assign (N, std::vector<point>(M, point(0.00, 0.00)));
}

std::pair<QVector<double>, QVector<double> > YHL::SOM::getPoints() const
{
    std::pair<QVector<double>, QVector<double> > one;
    for(const auto& it : this->oldData) {
        one.first.push_back (it.features[0]);
        one.second.push_back (it.features[1]);
    }
    return one;
}

std::pair<QVector<double>, QVector<double> > YHL::SOM::getEfficiency()
{
    return this->efficiency;
}








