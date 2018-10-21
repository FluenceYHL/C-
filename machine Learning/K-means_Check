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
/*数据集如下:
100
6.19955 -5.26633
7.87373 -3.8204
0.61807 0.762981
6.43234 -3.6369
-5.85391 -5.43082
7.08321 -5.19481
6.86895 -3.53223
3.29556 0.341203
0.938501 2.85453
6.28384 -3.60953
-4.63535 -5.00783
6.15297 -3.56158
-4.60264 -4.38616
6.18538 -4.93968
-3.01818 -4.37128
7.90486 -4.08041
1.98598 1.5456
3.81914 2.39515
6.61393 -5.54269
6.89759 -3.75722
-4.82522 -5.31712
-3.32756 -5.52602
3.10531 0.921524
1.7831 3.52953
-5.53331 -4.61417
-4.90964 -4.68944
-4.02845 -4.64469
7.61457 -3.67537
7.73959 -3.86293
7.45996 -5.60111
-5.93719 -3.25585
3.94458 2.21061
1.42386 2.92192
0.394443 2.69613
-5.63353 -3.72033
1.99513 1.46544
6.83905 -3.76809
7.83477 -4.66631
7.99092 -5.91342
6.4897 -5.07989
1.68916 2.30713
0.420631 3.24445
3.37946 3.66761
-4.95966 -5.59546
-3.81355 -4.81734
7.05449 -4.44915
-4.07432 -3.43555
-3.97694 -5.71253
-4.67445 -4.67368
1.78029 3.85688
-4.8239 -5.44721
7.05074 -3.58508
7.46771 -4.89391
-4.75616 -4.486
-5.41263 -4.33762
3.69699 3.17947
-4.75922 -4.64986
-5.17244 -3.25915
7.00785 -3.12782
6.10821 -4.52301
-10.5604 5.66912
-13.8728 5.75677
12.7397 -13.0812
10.546 14.0689
10.3282 -13.8149
13.514 -13.2951
-4.4688 11.8042
11.9705 -8.19039
-7.5209 -14.8589
-14.909 -2.1882
-13.6263 10.3529
6.60029 5.23191
11.0351 -9.95958
3.02972 -1.69328
-5.35044 -10.1864
-1.49357 9.0281
2.47058 4.66674
-0.455121 -9.1013
-4.98722 -7.51184
-12.2767 -8.59729
14.1895 -14.0159
8.83532 -13.9819
-12.9488 -4.04028
7.0246 -11.736
-4.53117 -4.15246
-3.24859 13.1903
-1.9325 6.87654
12.3352 8.92354
-4.64407 -6.26415
-3.75641 -7.22711
14.1448 -9.15947
-10.6374 -3.02221
6.64652 -13.0971
-5.55078 14.7923
5.24819 14.6907
-13.5771 4.99954
-7.22672 1.05592
-14.5553 11.44
-6.54046 12.5044
5.92822 -5.7061

*/
