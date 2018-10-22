#include <iostream>
#include <fstream>
#include <ctime>
#include <random>
#include "scopeguard.h"

int main() {
	std::ofstream out("k-means_circle_2.txt", std::ios::trunc);
	YHL::ON_SCOPE_EXIT([&]{ out.close(); });
	int num = 2500;
	out << num << "\n";

	std::default_random_engine e(time(0));
	std::uniform_real_distribution<double> a(-3, 3);
	std::uniform_real_distribution<double> b(-10, 10);
	std::uniform_real_distribution<double> c(-20, 20);

	// 要制造 1800 个点
	while(true) {
		static int sum = 0;
		int choice = rand() % 3;
		if(choice == 0) {
			static int cnt = 0;
			if(++cnt <= 100)
				out << a(e) << " " << a(e) << "\n";
		}
		if(choice == 1) {
			static int cnt = 0;
			if(++cnt <= 700) {
				auto l = b(e), r = b(e);
				while(l * l + r * r < 36 || l * l + r * r > 100)
					l = b(e), r = b(e);
				out << l << " " << r << "\n";
			}
		}
		if(choice == 2) {
			static int cnt = 0;
			if(++cnt <= 1400) {
				auto l = c(e), r = c(e);
				while(l * l + r * r < 196 || l * l + r * r > 400)
					l = c(e), r = c(e);
				out << l << " " << r << "\n";
			}
		}
		if(++sum == 2200)
			break;
	}

	std::uniform_real_distribution<double> d(-20, 20); // 剩下的是大范围内随机, 1, 2, 3, 4象限都有
	for(int i = 0; i < 300; ++i)
		out << d(e) << " " << d(e) << "\n";
	return 0;
}