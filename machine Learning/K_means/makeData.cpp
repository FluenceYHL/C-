#include <iostream>
#include <fstream>
#include <ctime>
#include <random>
#include "scopeguard.h"

int main() {
	std::ofstream out("k-means(1).txt", std::ios::trunc);
	YHL::ON_SCOPE_EXIT([&]{ out.close(); });
	int num = 380;
	out << num << "\n";

	std::default_random_engine e(time(0));
	std::uniform_real_distribution<double> a(0, 4);
	std::uniform_real_distribution<double> b(6, 8);
	std::uniform_real_distribution<double> c(-3, -6);
	for(int i = 0;i < num - 80; ++i) {
		int choice = rand() % 3;
		switch(choice) {
			case 0 : {
				out << a(e) << " " << a(e) << "\n";  // 这一块比较集中,位于第一象限
				break;
			}
			case 1 : {
				out << b(e) << " " << c(e) << "\n";  // 这一块比较集中,位于第四象限
				break;
			}
			case 2 : {
				out << c(e) << " " << c(e) << "\n";  // 这一比较集中,位于第三象限
				break;
			}
		}
	}
	std::uniform_real_distribution<double> d(-10, 10); // 剩下的是大范围内随机, 1, 2, 3, 4象限都有
	for(int i = 0; i < 80; ++i)
		out << d(e) << " " << d(e) << "\n";
	return 0;
}