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

	double getGauth(const double u, const double delta, const double x) {
		double index = -0.50 * std::pow((x - u) / delta, 2);
		return 1.00 / (std::sqrt(2 * PI) * delta) * std::exp(index);
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
private:
	void caculate() {
		this->maleMean = getMean<person>(this->male);
		this->femaleMean = getMean<person>(this->female);
		this->maleVariance = getVariance<person>(this->male, this->maleMean);
		this->femaleVariance = getVariance<person>(this->female, this->femaleMean);
	}
	char decisionWeight(const double x) const {
		static constexpr double M = 0.50;
		static constexpr double F = 0.50;
		double lhs = M * getGauth(this->maleMean.second, this->maleVariance.second, x);
		double rhs = F * getGauth(this->femaleMean.second, this->femaleVariance.second, x);
		return lhs > rhs ? 'M' : 'F';
	}
	char decisionHeight(const double x) const {
		static constexpr double M = 0.50;
		static constexpr double F = 0.50;
		double lhs = M * getGauth(this->maleMean.first, this->maleVariance.first, x);
		double rhs = F * getGauth(this->femaleMean.first, this->femaleVariance.first, x);
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
		int heightCnt = 0, weightCnt = 0;
		for(int i = 0;i < len; ++i) {
			in >> height >> weight >> type;
			if(!isEqual(this->decisionHeight(height), type[0]))
				++heightCnt;
			if(!isEqual(this->decisionWeight(weight), type[0]))
				++weightCnt;
		}
		std::cout << "身高预测错误个数  :  " << heightCnt << "\t错误率 " << heightCnt * 1.00 / len << "\n";
		std::cout << "体重预测错误个数  :  " << weightCnt << "\t错误率 " << weightCnt * 1.00 / len << "\n\n\n";
	}
};

int main() {
	Bayers one;
	one.loadFile();
	std::cout << "\n\n测试集 1  : \n";
	one.testFile("test1.txt");
	std::cout << "测试集 2  : \n";
	one.testFile("test2.txt");
	return 0;
}