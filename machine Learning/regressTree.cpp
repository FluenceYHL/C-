#include <iostream>
#include <functional>
#include <memory>
#include <fstream>
#include <cmath>
#include <string>
#include <vector>
#include <assert.h>
#include "scopeguard.h"

class regressTree {
private:
	std::vector<double> dataSet;
	int len;

	double getLost(const int l, const int r) {
		double average = 0.00;
		for(int i = l;i <= r; ++i) 
			average += this->dataSet[i];
		average /= (r - l + 1);
		//std::cout << "区间  [ " << l << " , " << r << " ]\n";
		double ans = 0.00;
		for(int i = l;i <= r; ++i) {
			auto cur = this->dataSet[i] - average;
			ans += cur * cur;
		}
		//std::cout << "最小损失  :  " << ans << "\n";
		return ans;
	}

	void DFS(const int l, const int r) {
		if(l >= r) return;
		// l < r
		int best = 0;
		double minLost = 1e9;
		for(int mid = l; mid < r; ++mid) {
			auto lhs = getLost(l, mid);
			auto rhs = getLost(mid + 1, r);
			//std::cout << "mid  :  " << mid << "\nlhs  :  " << lhs << "\n" << "rhs  :  " << rhs << "\n" << "cur  :  " << lhs + rhs << "\n";
			if(lhs + rhs < minLost) {
				minLost = lhs + rhs;
				best = mid;
			}
		}
		std::cout << "切分点  :  " << best << "\n";
		DFS(l, best);
		DFS(best + 1, r);
	}
public:
	void readData(const std::string& fileName) {
		std::ifstream in(fileName.c_str());
		assert(in);
		YHL::ON_SCOPE_EXIT([&]{ in.close(); });
		in >> this->len;
		std::vector<double>().swap(this->dataSet);
		this->dataSet.reserve(len + 1);
		this->dataSet.emplace_back(0);
		for(int i = 0;i < this->len; ++i) {
			double y;
			in >> y;
			this->dataSet.emplace_back(y);
		}
	}
	void makeRegress() {
		DFS(1, this->len);
	}
};

int main() {
	regressTree one;
	one.readData("regressTree.txt");
	one.makeRegress();
	return 0;
}