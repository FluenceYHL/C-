#include <iostream>
#include <fstream>
#include <ctime>
#include <random>
#include "scopeguard.h"

int main() {
	std::ofstream out("hierarchical.txt", std::ios::trunc);
	YHL::ON_SCOPE_EXIT([&]{ out.close(); });
	int num = 30;
	out << num << "\n";

	std::default_random_engine e(time(0));
	std::uniform_real_distribution<double> a(0, 10);
	
	for(int i = 0;i < num; ++i)
		out << a(e) << " " << a(e) << "\n";
	return 0;
}