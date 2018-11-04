#include <iostream>
#include <algorithm>
#include <functional>
#include <fstream>
#include <vector>
#include <string>
#include <cmath>
#include <tuple>
#include <list>
#include <assert.h>
#include "scopeguard.hpp"
#define PI std::acos(-1)

namespace {
	template<typename T>
	std::pair<double, double> getMean(const std::vector<T>& container) {
		std::pair<double, double> one;
		for(const auto& it : container) {
			one.first += it.height;
			one.second += it.weight;
		}
		const int len = container.size();
		one.first = 1.00 * one.first / len;
		one.second = 1.00 * one.second / len;
		return one;
	}

	template<typename T>
	std::pair<double, double> getVariance(const std::vector<T>& container, 
			const std::pair<double, double>& means) {
		std::pair<double, double> one;
		for(const auto& it : container) {
			one.first += std::pow(it.height - means.first, 2);
			one.second += std::pow(it.weight - means.second, 2);
		}
		const int len = container.size();
		one.first = std::sqrt(1.00 * one.first / len);
		one.second = std::sqrt(1.00 * one.second / len);
		return one;
	}

	template<typename T>
	double getCommon(const std::vector<T> container, const std::pair<double, double>& means) {
		double ans = 0.00;
		for(const auto& it : container) {
			ans += (it.height - means.first) * (it.weight - means.second);
		}
		const int len = container.size();
		return ans * 1.00 / len;
	}

	inline bool isEqual(const char a, const char b) {
		return a == b || std::tolower(a) == b;
	}
}

class Bayers {
	struct person {
		double height;
		double weight;
		std::string type;
		person(const double _h, const double _w) : height(_h), weight(_w) {}
		person(const double _h, const double _w, const std::string& _type) 
			: height(_h), weight(_w), type(_type) {}
		void display() const {
			std::cout << height << "\t" << weight << "\n";
		}
	};
private:
	std::vector< person > male;
	std::vector< person > female;
	std::pair<double, double> maleMean, femaleMean;
	std::pair<double, double> maleVariance, femaleVariance;
	double commonDelta1, commonDelta2;
private:
	void caculate() {
		this->maleMean = getMean<person>(this->male);
		this->femaleMean = getMean<person>(this->female);
		this->maleVariance = getVariance<person>(this->male, this->maleMean);
		this->femaleVariance = getVariance<person>(this->female, this->femaleMean);
		this->commonDelta1 = getCommon<person>(this->male,this->maleMean);
		this->commonDelta2 = getCommon<person>(this->female, this->femaleMean);
		// std::cout << "男生协方差  :  " << commonDelta1 << "\n";
		// std::cout << "女生协方差  :  " << commonDelta2 << "\n";
	}
	const double getBoys(const double height, const double weight) const {
		double ans = 0.00;
		double denomirator = this->maleVariance.first * this->maleVariance.second - commonDelta1 * commonDelta1;
		// std::cout << "denomirator  :  " << denomirator << "\n";
		std::pair<double, double> x;
		x.first = height - this->maleMean.first;
		x.second = weight - this->maleMean.second;
		std::pair<double, double> reverse;
		reverse.first = x.first * this->maleVariance.second - x.second * this->commonDelta1;
		reverse.second = x.second * this->maleVariance.first - x.first * this->commonDelta1;
		ans = reverse.first * x.first + reverse.second * x.second;
		ans = 1.00 * ans / denomirator;
		return -0.50 * (ans + std::log(std::fabs(denomirator)));
	}
	const double getGirls(const double height, const double weight) const {
		double ans = 0.00;
		double denomirator = this->femaleVariance.first * this->femaleVariance.second - commonDelta2 * commonDelta2;
		std::pair<double, double> x;
		x.first = height - this->femaleMean.first;
		x.second = weight - this->femaleMean.second;
		std::pair<double, double> reverse;
		reverse.first = x.first * this->femaleVariance.second - x.second * this->commonDelta2;
		reverse.second = x.second * this->femaleVariance.first - x.first * this->commonDelta2;
		ans = reverse.first * x.first + reverse.second * x.second;
		ans = 1.00 * ans / denomirator;
		return -0.50 * (ans + std::log(std::fabs(denomirator)));
	}
	const char decision(const double height, const double weight) const {
		static constexpr double M = 0.50;
		static constexpr double F = 1.00 - M;
		double lhs = this->getBoys(height, weight) + std::log(M);
		double rhs = this->getGirls(height, weight) + std::log(F);
		// std::cout << "lhs  :  " << lhs << "\n";
		// std::cout << "rhs  :  " << rhs << "\n";
		return lhs > rhs ? 'M' : 'F';
	}
public:
	void loadFile(){
		std::ifstream in("male.TXT");
		YHL::ON_SCOPE_EXIT([&]{
			in.close();
		});
		assert(in);
		int len;
		in >> len;
		double height, weight;
		for(int i = 0;i < len; ++i) {
			in >> height >> weight;
			this->male.emplace_back(height, weight);
		}
		in.close();
		in.open("female.TXT");
		assert(in);
		in >> len;
		for(int i = 0;i < len; ++i) {
			in >> height >> weight;
			this->female.emplace_back(height, weight);
		}
		this->caculate();
	}
	void testFile(const std::string& fileName) {
		std::ifstream in(fileName.c_str());
		YHL::ON_SCOPE_EXIT([&]{
			in.close();
		});
		assert(in);
		int len;
		in >> len;
		double height, weight;
		std::string type;
		int cnt = 0;
		for(int i = 0;i < len; ++i) {
			in >> height >> weight >> type;
			if(!isEqual(this->decision(height, weight), type[0]))
				++cnt;
			// break;
		}
		std::cout << "错误个数  :  " << cnt << "\n";
		std::cout << "错误率    :  " << cnt * 1.00 / len << "\n\n\n";
	}
};

int main() {
	Bayers one;
	one.loadFile();
	std::cout << "测试集 1  : \n";
	one.testFile("test1.txt");
	std::cout << "测试集 2  : \n";
	one.testFile("test2.txt");
	return 0;
}