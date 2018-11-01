#include <iostream>
#include <algorithm>
#include <functional>
#include <fstream>
#include <string>
#include <memory>
#include <cmath>
#include <list>
#include <vector>
#include <tuple>
#include <map>
#include <set>
#include <assert.h>
#include <unordered_map>
#include <unordered_set>
#include "scopeguard.h"

namespace {
	using oneLine = std::shared_ptr< std::vector< std::string > >;
	using dataType = std::list< oneLine >;

	struct treeNode {
		std::string type;
		std::shared_ptr<treeNode> child[2];
		treeNode(const std::string& _type) 
			: type(_type) {}
	};

	inline void padding(const int depth) {
		for(int i = 0;i < depth * 4; ++i)
			std::cout << " ";
	}
}

std::tuple< std::vector< std::string >, dataType > loadFile(const std::string& fileName) {
	std::ifstream in(fileName.c_str());
	YHL::ON_SCOPE_EXIT([&]{
		in.close();
	});
	int labelNum = 0, itemlNum = 0;
	in >> labelNum;
	std::string input;
	std::vector< std::string > label;
	for(int i = 0;i < labelNum; ++i) {
		in >> input;
		label.emplace_back(input);
	}
	dataType curData;
	in >> itemlNum;
	for(int i = 0;i < itemlNum; ++i) {
		auto one = std::make_shared< std::vector< std::string > >();
		for(int j = 0;j <= labelNum; ++j) {
			in >> input;
			one->emplace_back(input);
		}
		curData.emplace_back(one);
	}
	return std::make_tuple< std::vector< std::string >, dataType >(std::move(label), std::move(curData));
}

std::set< std::string > getSet(const dataType& curData, const int pos) {
	std::set< std::string > book;
	for(const auto& it : curData) {
		assert(it);
		book.emplace((*it)[pos]);
	}
	return book;
}

std::list< std::string > getLabel(const dataType& curData, const int pos) {
	std::list< std::string > one;
	for(const auto& it : curData) {
		assert(it);
		one.emplace_back((*it)[pos]);
	}
	return one;
}

double getGini(const std::list< std::string >& one) {
	if(one.empty())
		return 0.00;
	std::unordered_map<std::string, int> book;
	for(const auto& it : one)
		++book[it];
	double ans = 0.00;
	const int len = one.size();
	for(const auto& it : book) { 
		ans += std::pow(it.second * 1.00 / len, 2);
	}
	return 1.00 - ans;
}

double sigmaGini(const std::list< std::string >& cur, const std::string& it,
				 const std::list< std::string >& ans_list) {
	std::list < std::string > lhs, rhs;
	auto l = cur.begin();
	auto r = ans_list.begin();
	for(;l not_eq cur.end(), r not_eq ans_list.end(); ++l, ++r) { 
		if(*l == it)
			lhs.emplace_back(*r);
		else
			rhs.emplace_back(*r);
	}
	const int len = cur.size();
	return (lhs.size() * 1.00 / len) * getGini(lhs) + (rhs.size() * 1.00 / len) * getGini(rhs);
}

std::pair<double, std::string> getLost(const std::list< std::string >& cur, 
		const std::list< std::string >& ans_list) {
	std::pair<double, std::string> one;
	std::unordered_set< std::string > book;
	for(const auto& it : cur)
		book.emplace(it);
	one.first = 1e9;
	for(const auto& it : book) {
		double lost = sigmaGini(cur, it, ans_list); 
		if(lost < one.first) {
			one.first = lost;
			one.second = it;
		}
	}
	return one;
}

std::pair<int, std::string> getBest(const dataType& curData, 
		const std::vector< std::string >& label) {
	int labelSize = label.size();
	auto ans_list = getLabel(curData, labelSize);

	auto min = 1e9;
	std::pair<int, std::string> one;
	for(int i = 0;i < labelSize; ++i) {
		auto cur = getLabel(curData, i);
		auto lost = getLost(cur, ans_list);  // first 损失, second 例如学生，和其他
		if(lost.first < min) {
			min = lost.first;
			one.first = i;
			one.second = lost.second;
		}
	}
	return one;
}

std::pair<dataType, dataType> spilt(const dataType& curData, 
		const std::pair<int, std::string>& best) {
	std::pair<dataType, dataType> one;
	for(const auto& it : curData) {
		assert(it);
		if((*it)[best.first] == best.second) 
			one.first.emplace_back(it);
		else
			one.second.emplace_back(it);
		it->erase(it->begin() + best.first);
	}
	return one;
}

std::string getMost(const std::list< std::string >& one) {
	std::unordered_map<std::string, int> book;
	for(const auto& it : one) 
		++book[it];
	auto max = 1e-12;
	std::string ans;
	for(const auto& it : book) {
		if(it.second > max) {
			max = it.second;
			ans = it.first;
		}
	}
	return ans;
}

std::shared_ptr<treeNode> buildTree(const dataType &curData, 
		std::vector< std::string >& label) {
	int labelSize = label.size();
	auto result = getSet(curData, labelSize);
	if(result.size() == 1)
		return std::make_shared<treeNode>(*result.begin());

	if(labelSize == 1) {
		auto most = getMost(getLabel(curData, 1));
		return std::make_shared<treeNode>(most);
	}

	auto best = getBest(curData, label); // 哪个特征，特征的什么值 
	auto feature = label[best.first];
	label.erase(label.begin() + best.first);

	auto root = std::make_shared<treeNode>(feature + " 是否为 " + best.second + " ?");

	auto l_label = label, r_label = label;
	auto spiltData = spilt(curData, best);
	root->child[0] = buildTree(spiltData.first, l_label);
	root->child[1] = buildTree(spiltData.second, r_label);
	return root;
}

void visit(treeNode *const root, int depth) {
	assert(root);
	std::cout << root->type;
	if(!root->child[0] and !root->child[1])
		return;
	static constexpr char choice[2][10] = {"是 : ", "否　: "};
	for(int i = 0;i < 2; ++i) {
		std::cout << "\n\n";  
		padding(depth);
		std::cout << choice[i];
		visit(root->child[i].get(), depth + 1);
		padding(depth);
	}
}

int main() {
	auto ans = loadFile("dataSet_2.txt");
	auto root = buildTree(std::get<1>(ans), std::get<0>(ans));
	visit(root.get(), 1);
	return 0;
}