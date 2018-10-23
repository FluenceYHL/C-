#include <iostream>
#include <functional>
#include <algorithm>
#include <fstream>
#include <cstdlib>
#include <ctime>
#include <vector>
#include <tuple>
#include <memory>
#include <string>
#include <cmath>
#include <array>
#include <list>
#include <map>
#include <assert.h>
#include "scopeguard.h"

namespace YHL {

	struct point {
		double x, y;
		point(const double _x, const double _y)
			: x(_x), y(_y)
		{}
		bool operator==(const point& rhs) {
			return (this->x - rhs.x) < 1e-3 and (this->y - rhs.y) < 1e-3;
		}
	};

	// 这个 getDistance 可以传递一个簇作为参数, 可以计算均值, 中值等
	inline double getDistance(const point& a, const point& b) {
		return sqrt(pow(a.x - b.x, 2) + pow(a.y - b.y, 2));
	}

	class Hierarchical {
		using oneCluster = std::vector<point>;
	private:
		int len;
		oneCluster dataSet;
		std::vector< int > father;
		std::map<int, std::list<int> > clusters;  // 还没用到
		std::vector< std::vector<double> > dis;

	private:

		void initDis() {
			this->dis.assign(this->len, std::vector<double>());
			for(auto& it : dis) 
				it.assign(this->len, 1e9);
			for(int i = 0;i < this->len; ++i) {
				for(int j = 0;j < i; ++j) {
					if(i == j) continue;
					auto distance = getDistance(this->dataSet[i], this->dataSet[j]);
					dis[i][j] = dis[j][i] = distance;
				}
			}
		}

		void initFather() {
			for(int i = 0;i < this->len; ++i)
				this->father.emplace_back(i);
		}
		int find(const int u) {
			return u == this->father[u] ? u : this->father[u] = find(this->father[u]);
		}

		void mergeClusters(const int i, const int j) {
			if(i == j) return;
			int x = find(i);
			int y = find(j);
			this->father[x] = y;  
			// x 所在簇归并到 y, x 这个点就算消失了
			for(int i = 0;i < this->len; ++i)
				this->dis[x][i] = this->dis[i][x] = 1e9;
		}

	public:
		void readData(const std::string& path = "hierarchical.txt") {
			std::ifstream in(path.c_str());
			ON_SCOPE_EXIT([&]{ in.close(); });
			in >> this->len;
			double x = 0.00, y = 0.00;
			for(int i = 0;i < this->len; ++i) {
				in >> x >> y;
				this->dataSet.emplace_back(x, y);
			}

			for(const auto& it : dataSet)
				std::cout << it.x << " " << it.y << "\n";
			std::cout << dataSet.size() << "\n";
			// 构造相似度矩阵
			this->initFather();
			this->initDis();
		}

		void getCluster(const double threshold) {
			/*
		       1、把每个样本归为一类，计算每两个类之间的距离，也就是样本与样本之间的相似度；
		       2、寻找各个类之间最近的两个类，把他们归为一类（这样类的总数就少了一个）；
		       3、重新计算新生成的这个类与各个旧类之间的相似度；
		       4、重复2和3直到所有样本点都归为一类，结束
			*/
			while(true) {
				// if(dataSet.front() == dataSet.back()) 
				auto minInf = 1e9;
				int aPos = -1, bPos = -1;
				for(int i = 0;i < this->len; ++i) 
					for(int j = 0;j < this->len; ++j) {
						if(minInf > dis[i][j]) {
							minInf = dis[i][j];
							aPos = i; bPos = j;
						}
					}
				std::cout << "最近距离是 :  " << minInf << "\n";
				if(minInf > threshold) break;
				std::cout << "最近的两个点是  :  " << aPos << "  " << bPos << "\n\n";
				// for(int i = 0;i < this->len; ++i) {
				// 	for(int j = 0;j < this->len; ++j)
				// 		std::cout << dis[i][j] << "\t";
				// 	std::cout << "\n";
				// }
				// std::cout << "\n\n";
				// 开始合并
				this->mergeClusters(aPos, bPos);
			}
			std::cout << "\n\n合并结束  :  \n\n";
			for(int i = 0;i < len; ++i)
				this->father[i] = find(i);
			for(int i = 0;i < len; ++i) {
				std::cout << i << "  father  is  :  " << this->father[i] << "\n";

				// clusters[this->father[i]].emplace_back(i);
				auto category = this->father[i];
				auto res = this->clusters.find(category);
				if(res == this->clusters.end()) {
					std::list<int> null_one { i };
					this->clusters.emplace(category, std::move(null_one));
				}
				else res->second.emplace_back(i);
			}
			std::cout << "最终聚类结果是 : \n\n";
			for(const auto& it : this->clusters) {
				std::cout << "分类标号 " << it.first << "  :  ";
				for(const auto r : it.second)
					std::cout << r << "  ";
				std::cout << "\n";
			}
		}
	};

}

int main() {
	YHL::Hierarchical one;
	one.readData("hierarchical.txt");
	one.getCluster(3);
	return 0;
}