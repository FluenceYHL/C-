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
#include <assert.h>
#include "scopeguard.h"
using point = std::tuple<double, double>;
using oneCluster = std::vector<point>;

void print(const std::vector<oneCluster>& clusters) {
	for(const auto& it : clusters) {
		std::cout << "\n\n*******************\n\n";
		for(const auto& r : it) {
			std::cout << "( " << std::get<0>(r) << " , ";
			std::cout << std::get<1>(r) << " )\n";
		}
	}
}

// 读取文件内容
std::vector< point > readData(const std::string& path) {  // std::unique_ptr
	std::ifstream in(path.c_str());
	YHL::ON_SCOPE_EXIT([&]{
		in.close();
		std::cout << "数据集文件已关闭\n\n";
	});
	auto items = 0;
	in >> items;
	auto x = 0.00, y = 0.00;
	std::vector< point > dataSet;
	for(int i = 0;i < items; ++i) {
		in >> x >> y;
		dataSet.emplace_back(std::make_tuple<double, double>(std::move(x), std::move(y)));
	}
	// for(const auto& it : dataSet)
	// 	std::cout << std::get<0>(it) << "\t" << std::get<1>(it) << "\n";
	return dataSet;
}

// 计算两个点之间的距离, 在这里选择的是欧氏距离
inline double getDistance(const point& a, const point& b) {
	return sqrt(pow(std::get<0>(a) - std::get<0>(b), 2) + 
				pow(std::get<1>(a) - std::get<1>(b), 2));
}

// 在这些簇中心点 centers 中, one 这个点选离自己最近的一个,返回这个最近的中心店
const int getLabel(const point& one, const oneCluster& centers) {
	// 计算 one 每一个 cluster 中心的距离, 返回距离最近的那个 cluster
	auto Min = 1e6;
	int label = -1, centerSize = centers.size();
	for(int i = 0;i < centerSize; ++i) {
		auto ans = getDistance(centers[i], one);
		if(ans < Min) {
			Min = ans;
			label = i;
		}
	}
	return label;
}

// 给定一个簇,计算簇的中心,在这里选择的是 x, y 均值点
point getCenter(const oneCluster& one) {
	double mean_x, mean_y = 0.00;
	for(const auto& it : one) {
		mean_x += std::get<0>(it);  // 取横坐标
		mean_y += std::get<1>(it);  // 取纵坐标
	}
	int scale = one.size();
	return std::make_tuple<double, double>(mean_x / scale, mean_y / scale);
}

// 给定聚类结果 clusters, 和这些簇的中心 centers,预估聚类效果,方式多样
const double getEvaluate(const std::vector<oneCluster>& clusters,
						 const oneCluster& centers) {
	double ans = 0;
	int lSize = clusters.size(), rSize = centers.size(); // 一个簇对应一个中心点
	assert(lSize == rSize);
	for(int i = 0;i < lSize; ++i) {
		// it 代表一个簇, 计算这个簇每一个点 和 "虚拟"中心点的距离(中心点可能不在簇中,毕竟求的是均值所在)
		int oneSize = clusters[i].size();
		for(int k = 0;k < oneSize; ++k) {
			ans += getDistance(clusters[i][k], centers[i]);  // 第 i 个簇的每个点, 计算和这个簇的中心点的距离
		}
	}
	return ans;
}

using answerType = std::tuple<std::vector<oneCluster>, oneCluster, double>;
// 给定数据集 dataSet, 聚成 k 类, 阈值 thresholdValue(预估差 < 阈值 就结束)

answerType K_means(const oneCluster& dataSet, const int k, 
			 const double thresholdValue) {
	// 还可以预处理,删掉极端点
	auto dataSize = dataSet.size();
	assert(k <= dataSize); // 如果聚类数 > 数据量,这是错误的
	oneCluster centers;
	// 先选定 k 个随机的中心点
	std::vector<int> book(k, 0);
	srand(time(nullptr));
	for(int i = 0;i < k; ++i) {
		auto j = rand() % dataSize;
		while(book[j] == 1) 
			j = rand() % dataSize;
		centers.emplace_back(dataSet[j]);
	}

	// clusters 存储的每一个元素都是一个簇, 预先分配 K 个簇的空间
	std::vector< oneCluster > clusters;
	clusters.assign(k, oneCluster());

	double oldValue = 1.00, newValue = 1.00; int cnt = 0;

	while(true) {
		std::cout << "\n\n********** 第 " << ++cnt << "  次聚类 ************\n\n";

		// 每个点找出离它最近的中心点, 放在第 label 个簇中
		for(const auto& it : dataSet) {
			auto label = getLabel(it, centers); 
			assert(0 <= label and label < k);
			clusters[label].emplace_back(it);
		}
		// print(clusters);

		// 重新计算每个簇的中心点
		for(int i = 0;i < k; ++i) {
			centers[i] = getCenter(clusters[i]);
			std::cout << "第 " << i + 1 << " 个簇的中心点是  :  ";   // 这些信息也可以返回
			std::cout << "( " << std::get<0>(centers[i]) << " , " << std::get<1>(centers[i]) << " )\n";
		}

		// 重新衡量这次的最小函数值
		oldValue = newValue;  // 先存储上次的最小均方差之和
		newValue = getEvaluate(clusters, centers);
		std::cout << "oldValue  :  " << oldValue << "\n";
		std::cout << "newValue  :  " << newValue << "\n";
		if(abs(oldValue - newValue) / newValue < thresholdValue) // 如果变化小于阈值,就结束 也可以是差
			return std::make_tuple<std::vector<oneCluster>, oneCluster>
					(std::move(clusters), std::move(centers), newValue); // NVO

		// 每次聚类,得到的聚类都是不一样的,所以上次的记录要清空
		for(auto &it : clusters) 
			it.clear();
	}
	return std::make_tuple<std::vector<oneCluster>, oneCluster>
		(std::vector<oneCluster>(), oneCluster(), 0.00);
}

void display(const answerType& ans) {
	auto &clusters = std::get<0>(ans);
	auto &centers = std::get<1>(ans);
	auto newValue = std::get<2>(ans);
	
	print(clusters);
	{
		std::cout << "\n\n局部最优均方差之和是  :  " << newValue << "\n\n";
		int cnt = 0;
		for(const auto& it : centers) {
			std::cout << "第 " << ++cnt << " 个簇的中心点是  :  ";
			std::cout << "( " << std::get<0>(it) << " , " << std::get<1>(it) << " )\n";
		}
	}
}

void K_means_Check(const answerType& ans, const double thresholdValue,
				   oneCluster& dataSet) {
	std::cout << "\n\n************** 开始离群点检测 ***************\n\n";

	auto &clusters = std::get<0>(ans);
	auto &centers = std::get<1>(ans);
	int lSize = clusters.size();
	int rSize = centers.size();
	assert(lSize == rSize);
	for(int i = 0;i < lSize; ++i) {
		std::vector<double> arr;
		auto r = clusters[i].size();
		// 求得这个簇中每个点和中心的距离
		for(int j = 0;j < r; ++j) {
			arr.emplace_back(getDistance(clusters[i][j], centers[i]));
		}
		// 求得中位数的大小
		auto temp = arr;
		std::sort(temp.begin(), temp.end());
		int midSize = temp.size() >> 1;
		auto edge = temp[midSize];

		// 与中位数的比值 > 阈值的点在 数据集 中删除
		std::vector<point> ans, target;
		std::cout << "\n\n第 " << i + 1 << " 个簇中 : \n\n";
		for(int j = 0;j < r; ++j) {
			if(arr[j] / edge > thresholdValue) {
				std::cout << "舍弃 ( " << std::get<0>(clusters[i][j]) << " , " << std::get<1>(clusters[i][j]) << " )\n";
				target.emplace_back(clusters[i][j]);
			}
		}
		// 将要删除的目标找到,然后逐个删除
		for(auto &it : target) {
			auto beg = dataSet.begin();
			// for(int j = 0;j < target.size(); ++j) {
			// 	if(std::get<0>(target[j]) == std::get<0>(it) and
			// 	   std::get<1>(target[j]) == std::get<1>(it)) {
			// 		// dataSet.back().swap(t);
			// 		// dataSet.pop_back();
			// 		dataSet.erase(beg + j);
			// 	}
			// }
			while(beg not_eq dataSet.end()) {
				if(std::get<0>(*beg) == std::get<0>(it) and
				   std::get<1>(*beg) == std::get<1>(it)) {
					beg = dataSet.erase(beg);
				}
				else ++beg;
			}
		}
		std::cout << "\n数据集长度  :  " << dataSet.size() << "\n\n";
	}
}


int main() {
	auto dataSet = readData("k-means_check.txt");
	auto ans = K_means(dataSet, 3, 1e-3);  // 第一次大致找出聚类, 然后找出离群点(人判断这个也是这个过程,必须先获得整体认识)
	display(ans);
	K_means_Check(ans, 2.5, dataSet);
	ans = K_means(dataSet, 4, 1e-3);
	display(ans);
	return 0;
}

/*  尚未解决的问题 :
	1. 只支持二维,而不支持三维或更高,需要模板元
	其实,实现多维也不难,只要把 std::tuple 中的点类型改成高维即可,例如 (x, (y, z))
	2. 尚未实现如何删除离群点, 即预处理
	3. 尚未可视化
	4. 尚未实现层次聚类版
	5. 未能实现多次聚类,取最佳值......
*/
