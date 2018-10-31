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
#include "scopeguard.h"
// 某一行数据的类型，用　shared_ptr 包装，内部是一个　vector, 因为横向数据少, 用　vector 直接删除效率也不会受到太大的影响
using lineType = std::shared_ptr< std::vector< std::string > >; 
//　数据集的数据结构：因为纵向数据很大，用　vector 会频繁处在移动的过程，所以采用　list 链表
// 每个链表元素都是一个　vector
using dataType = std::list< lineType >;

// std::unique_ptr< dataType > dataSet; // 之所以采用指针，是因为如果有百万千万条数据, 对栈的负担还是很大的
// std::vector< std::string > Labels;  //　存储特征名字（房子，年龄，有工作，信用度），用　vector 的原因是需要定位某个位置那一列数据

// 针对某一列计算信息熵
template<typename container, typename T>
double calcShanNon(const container& one) {
	std::map<T, int> book;
	std::set<T> Set;
	for(const auto& it : one) {
		// ++book[it], Set.emplace(it);
		Set.emplace(it);
		auto r = book.find(it);
		if(r not_eq book.end()) 
			++r->second;
		else 
			book.emplace(it, 1);
	}
	auto ans = 0.00;
	int len = one.size();
	for(const auto& it : Set) {
		double p = static_cast<double>(static_cast<double>(book[it]) / len);
		ans -= p * log2(p);
	}
	return ans;
}
inline double calcShanNon(const std::list<std::string>& one) {
	return calcShanNon<std::list<std::string>, std::string >(one);
}

// 计算某一特征下, 最终分类的条件信息熵
double calcCondition(const std::list< std::string >& curLabel, 
		const std::list< std::string >& base_list) {
	auto lSize = curLabel.size();
	auto rSize = base_list.size();
	assert(lSize == rSize);

	std::map< std::string, std::list< std::string > > book;

	auto l = curLabel.begin();
	auto r = base_list.begin();
	for(;l not_eq curLabel.end(), r not_eq base_list.end(); ++l, ++r) {
	// for(auto [l, r] = std::make_tuple(l.begin(), r.begin())) {
		// book[*l].emplace_back(*r);
		auto res = book.find(*l);
		if(res not_eq book.end())
			res->second.emplace_back(*r);
		else {
			std::list< std::string > null_list;
			null_list.emplace_back(*r);
			book.emplace(*l, std::move(null_list));
		}
	}
	auto len = lSize;
	double ans = 0.00;
	for(const auto& it : book) {
		auto rate = static_cast<double>(static_cast<double>(it.second.size()) / len);  // 这部分占的比例
		ans += rate * calcShanNon(it.second);   // 在这个部分中, 同意和不同意构成的不确定性
	}
	return ans;
}

// 获取某一特征那一列的数据, 例如获取＂是否有房子＂的所有数据
std::list< std::string > getLabel(const dataType& curData, const int i) {
	std::list< std::string > ans;
	for(const auto& it : curData) {     // 针对数据集每一行
		assert(it);
		ans.emplace_back((*it)[i]);	    // 这一行数据的第　i 个数据
	}
	return ans;							// 最终得到的就是整个数据集第　i 列所有数据
}

// 和上面类似，不过这里是获取某一特征有几种分类，例如年龄那一列，得到的是　＂老人＂＂青年＂＂中年＂
std::set< std::string > getSet(const dataType& curData, const int i) {
	std::set< std::string > ans;
	for(const auto& it : curData) {
		assert(it);
		ans.emplace((*it)[i]);        
	}
	return ans;                       
} 

// 求得当前数据集最大信息增益的特征
const int getBest(const dataType& curData, 
				  const std::vector< std::string >& labels) {   
	std::list< std::string > base_list;    // 存储当前数据集最终的分类 D
	for(const auto& it : curData) 
		base_list.emplace_back(it->back());      
	double base_ShanNon = calcShanNon(base_list); // 计算　H(D)

	double ans = -1e9;
	auto res = -1;
	auto featureSize = labels.size();     
	for(int i = 0;i < featureSize; ++i) {       // 遍历＂当前＂剩下的特征
		auto curLabel = getLabel(curData, i);   // 先获取这个特征这一列数据
		auto cur = base_ShanNon - calcCondition(curLabel, base_list); 
		/*
			针对这一特征值的数据，分成几部分，每一部分占的比例　＊ 这一部分对最终分类的不确定性的影响, 如果
			这一部分中分类很确定,例如全部是"同意",少数"拒绝",那么针对这种很确定的情况,信息熵很小,
			减小了最终分类的不确定性,使得最终分类的信息熵减小
			但是如果分类很不确定,例如一半"同意",一半"拒绝",这样很难分类,造成的不确定性也很大,信息熵也很大
			这个部分,增加了最终分类的不确定性
		*/
		// auto Ha = calcShanNon(curLabel);
		auto Ha = 1;
		cur /= Ha;
		if(cur > ans) {
			ans = cur; 
			res = i;
		}
	}
	assert(0 <= res and res <= featureSize);
	return res;
}

// "当前"数据集,位于 pos 这一列数据,label 这个部分的数据提取出来
// 例如年龄中，找到＂老人＂这个特征，和老人有关的数据
// 但是有一点很重要,要删除"老人"这一列的特征
dataType spilt(const dataType& curData, const int pos, 
			const std::string& label) {
	dataType ans;
	for(const auto& it : curData) {    // 遍历"当前"数据集的每一行      
		assert(it);
		if((*it)[pos] == label) {      // 如果这一行 pos 位置上是 "老人" 的数据
			(*it).erase((*it).begin() + pos); // 数据集要删除＂老人＂这一列数据,因为横向特征值少,用 vector 损失也不大
			ans.emplace_back(it);
		}
	}
	return ans;  // NVO
}

// 读取数据
using myTuple = std::tuple< std::unique_ptr<dataType>, std::vector< std::string > >;

myTuple readData() {
	std::ifstream in("dataSet.txt");
	YHL::ON_SCOPE_EXIT([&]{
		in.close();
		std::cout << "数据集文件已关闭\n\n";
	});

	auto labelNum = 0;    // 这块读取特征名字
	in >> labelNum;
	std::string input;
	std::vector< std::string > Labels;
	for(int i = 0;i < labelNum; ++i) {
		in >> input;
		Labels.emplace_back(input);
	}

	auto dataSet = std::make_unique<dataType>(); // 生成数据集
	assert(dataSet);

	auto lineNum = 0;     // 这块读取数据集
	in >> lineNum;
	for(int i = 0;i < lineNum; ++i) {
		lineType oneLine = std::make_shared< std::vector< std::string > >();
		for(int k = 0;k <= labelNum; ++k) {
			in >> input;
			oneLine->emplace_back(input);
		}
		dataSet->emplace_back(oneLine);
	}
	return myTuple(std::move(dataSet), std::move(Labels));
	//return std::make_tuple< std::unique_ptr<dataType>, std::vector< std::string > >(std::move(dataSet), std::move(Labels));
}

// 最终决策树的数据节点定义
struct treeNode {
	std::string type;   // 根据什么特征选 ?
	// map < first 选什么(是,否), second 子节点 >
	std::map< std::string, std::shared_ptr<treeNode> > childs;

	treeNode(const std::string& _type)
		: type(_type)
	{}
};

// 求最终分类中出现次数最多的 判断(例如大多选 同意, 或大多选 拒绝)
template< typename container, typename T>
T mostElement(const container& one) {
	std::map<T, int> book;
	for(const auto& it : one) {
		auto ans = book.find(it);
		if(ans == book.end()) 
			book.emplace(it, 1);
		else
			++ans->second;
	}
	auto times = -1;
	std::string res = "";
	for(const auto& it : book) {
		if(it.second > times) {
			times = it.second;
			res = it.first;
		}
	}
	return res;
}
inline std::string mostElement(const std::list<std::string>& one) {
	return mostElement<std::list<std::string>, std::string>(one);
}

// 递归构建决策树
std::shared_ptr<treeNode> buildTree(const dataType& curData, 
									std::vector< std::string >& labels) {
	auto finalSize = labels.size();
	auto book = getSet(curData, finalSize);    
	if(book.size() == 1) 
		return std::make_shared<treeNode>(*book.begin()); // 如果剩下的最终分类都相同,例如都是"同意","拒绝"

	if(finalSize <= 1) {                                  // 如果最终只剩下一个特征,就不用分了,直接选最大概率的
		auto base_list = getLabel(curData, finalSize);    // 这里是 0 和 1 需要区分一下
		auto most = mostElement(base_list);
		return std::make_shared<treeNode>(most);
	}

	// 其实在这里可以剪枝,限制个数,限制高度,设定阈值

	int bestPos = getBest(curData, labels);     // 找出最大信息增益的特征
	auto bestLabel = labels[bestPos];           // 这个特征的名字
	auto thisLabel = getSet(curData, bestPos);  // 获取一个特征的几个部分，例如＂老人＂＂青年＂＂中年＂
	labels.erase(labels.begin() + bestPos); 	// 删除"有房子这一列数据

	auto root = std::make_shared<treeNode>(bestLabel);  // 生成当前节点

	for(const auto& it : thisLabel) {           // 针对这个最优特征的每个部分
		auto subData = spilt(curData, bestPos, it); // 分裂出这个部分的数据集,例如"老人"的所有数据
		auto subLabels = labels;                

		auto one = buildTree(subData, subLabels);// 这里只能用副本, 因为回溯到有房子其他分支，之前递归删掉的项目回复不过来
		root->childs.emplace(it, one);          // it 例如 "老人", one "怎么判断" 的节点
	}
	return root;
}    //{'有房子': {'是': '同意', '否': {'有工作': {'是': '同意', '否': '拒绝'}}}}


void visit(treeNode* root) {  
	assert(root);
	std::cout << root->type;
	if(root->childs.empty())
		return;
	std::cout << " { ";
	for(const auto& it : root->childs) {
		std::cout << " [ " << it.first << " : ";
		visit(it.second.get());
		std::cout << " ] ";
	}
	std::cout << "}";
}

std::string DFS(treeNode* const root, const std::map<std::string, std::string>& info) {
	assert(root);
	if(root->childs.empty())
		return root->type;

	auto res = info.find(root->type);    // 可以用一个 template
	assert(res not_eq info.end());
	auto choice = (*res).second;

	assert(root->childs.find(choice) not_eq root->childs.end());

	return DFS(root->childs[choice].get(), info);    // 可以改成非递归 while(root->childs.size())
}

void test(treeNode* const root) {
	std::ifstream in("test.txt");
	YHL::ON_SCOPE_EXIT([&]{
		in.close();
		std::cout << "\n\n测试文件已关闭\n\n";
	});

	auto testCase = 0, testItem = 0;
	in >> testCase >> testItem;

	std::string item, conditon;
	for(int i = 0;i < testCase; ++i) {
		std::map< std::string, std::string > info;
		for(int j = 0;j < testItem; ++j) {
			in >> item >> conditon;
			info.emplace(item, conditon);
		}
		std::cout << "\n\n判别结果  :  ";
		// std::cout << DFS(root, info) << "\n\n";
		auto judge = [&]{
			auto ptr = root;
			while(ptr->childs.size()) {
				auto choice = info[ptr->type];
				ptr = ptr->childs[choice].get();
			}
			return ptr->type;
		};
		std::cout << judge() << std::endl;
	}
}

int main() {
	auto tuple = readData();  // readData 应该返回一个 pair

	auto dataSet = std::move(std::get<0>(tuple));
	auto Labels = std::get<1>(tuple);

	auto root = buildTree(*dataSet, Labels);
	visit(root.get());

	test(root.get());
	return 0;
}

// 最重要的是,还缺少如何测试

// 哪里还可以用 ON_EXIT_SCOPE
// 两个 auto 可以存在同一个循环中吗
// 过长的 type 怎么简化