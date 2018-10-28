#include <iostream>
#include <functional>
#include <cstring>
#include <fstream>
#include <memory>
#include <string>
#include <vector>
#include <mutex>
#include <map>
#include <set>
#include <unordered_map>
#include <unordered_set>
#include <assert.h>
#include <cstdlib>
#include <random>
#include <ctime>
#include <cmath>
#include <sys/unistd.h>
#include "scopeguard.h"

namespace {
	using randType = std::uniform_real_distribution<double>;

	inline double getRand(const double edge){
		static std::default_random_engine e(time(0));
    	randType a(-edge, edge);
    	return a(e);
	};

	inline double sigmoid(const double x) {
		return 1.00 / (1.00 + std::exp(-x));
	}

	inline double dSigmoid(const double y) {
		return y * (1.00 - y);
	}
}

namespace YHL {

	class BPNN {
		static constexpr int inputSize = 784;
		static constexpr int hideSize = 100;
		static constexpr int outputSize = 10;
		static constexpr int layer = 3;
		static constexpr double rate = 0.35;

		using matrix = std::vector< std::vector<double> >;
	private:
		// layer - 1 层权值矩阵
		matrix weights[3];
		// 数据集和答案
		matrix dataSet;
		matrix answers;
		// 两层之间的向量
		std::vector<double> output[3];
		std::vector<double> delta[2];
		std::vector<double> threshold[2];

		double target[10];

		void initWeights() {
			for(int i = 0;i < inputSize; ++i) {
				this->weights[0].emplace_back(std::vector<double>());
				for(int j = 0;j < hideSize; ++j)
					this->weights[0][i].emplace_back(getRand(0.5));
			}
			for(int i = 0;i < hideSize; ++i) {
				this->weights[1].emplace_back(std::vector<double>());
				for(int j = 0;j < outputSize; ++j)
					this->weights[1][i].emplace_back(getRand(0.5));
			}
		}

		void initOutput() {
			output[0].assign(inputSize, 0.00);
			output[1].assign(hideSize, 0.00);
			output[2].assign(outputSize, 0.00);  // 这种初始化方式和下面的相比
		}

		void initDelta() {
			for(int i = 0;i < hideSize; ++i)
				this->delta[0].emplace_back(0.00);
			for(int i = 0;i < outputSize; ++i)
				this->delta[1].emplace_back(0.00);
		}

		void initThreshold() {
			for(int i = 0;i < hideSize; ++i)
				this->threshold[0].emplace_back(getRand(0.5));
			for(int i = 0;i < outputSize; ++i)
				this->threshold[1].emplace_back(getRand(0.5));
		}

		double getError() {
			double ans = 0;
			for(int i = 0;i < outputSize; ++i) 
				ans += std::pow(output[2][i] - target[i], 2);
			return ans * 0.50;
		}

		void loadTrain() {}

		void forwardDrive() {
			for(int j = 0;j < hideSize; ++j) {
				double res = 0.00;
				for(int i = 0;i < inputSize; ++i)
					res += this->weights[0][i][j] * output[0][i];
				this->output[1][j] = sigmoid(res - this->threshold[0][j]);
			}
			for(int k = 0;k < outputSize; ++k) {
				double res = 0.00;
				for(int j = 0;j < hideSize; ++j)
					res += this->weights[1][j][k] * output[1][j];
				this->output[2][k] = sigmoid(res - this->threshold[1][k]);
			}
		}

		void backPropagate() {
			for(int i = 0;i < outputSize; ++i) {
				auto O = this->output[2][i];
				this->delta[1][i] = (O - target[i]) * dSigmoid(O);	
			}
			for(int k = 0;k < outputSize; ++k) {
				double gradient = delta[1][k];
				for(int j = 0;j < hideSize; ++j)
					this->weights[1][j][k] -= rate * output[1][j] * gradient;
				this->threshold[1][k] -= rate * gradient;
			}
			for(int j = 0;j < hideSize; ++j) {
				auto res = 0.00;
				for(int k = 0;k < outputSize; ++k)  // 每个输出神经元的梯度, 和相联的边
					res += this->weights[1][j][k] * delta[1][k];
				auto O = this->output[1][j];
				this->delta[0][j] = dSigmoid(O) * res;
			}
			for(int j = 0;j < hideSize; ++j) {
				double gradient = delta[0][j];
				for(int i = 0;i < inputSize; ++i)
					this->weights[0][i][j] -= rate * output[0][i] * gradient;
				this->threshold[0][j] -= rate * gradient;
			}
		}

	public:
		BPNN() {
			this->initWeights();
			this->initOutput();
			this->initDelta();
			this->initThreshold();
		}

		void train() {
			this->loadTrain();
			std::ifstream image("./trainSet/train-images.idx3-ubyte", std::ios::binary);
			std::ifstream label("./trainSet/train-labels.idx1-ubyte", std::ios::binary);
			ON_SCOPE_EXIT([&]{
				image.close();
				label.close();
			});
			assert(image and label);
			char head[100];
			image.read(head, sizeof(char) * 16);
			label.read(head, sizeof(char) * 8);

			std::cout << "开始读取文件\n";
			char image_buf[784];
			char label_buf;
			while(!image.eof() and !label.eof()) {
				image.read((char*)&image_buf, sizeof(char) * 784);
				label.read((char*)&label_buf, sizeof(char) * 1);

				for(int i = 0;i < 10; ++i) 
					this->target[i] = 0.00;
				target[(unsigned int)label_buf] = 1.00;

				for(int i = 0;i < 784; ++i) {
					double value = (unsigned int)image_buf[i] < 128 ? 0.00 : 1.00;
					output[0][i] = value;
				}

				this->forwardDrive();
				std::cout << "error  :  " << this->getError() << "\n";
				if(this->getError() > 1e-5)
					this->backPropagate();
			}
		}

		const int recognize(const std::vector<double>& input) {
			const int len = input.size();
			assert(len == inputSize);
			for(int i = 0;i < inputSize; ++i)
				this->output[0][i] = input[i];
			this->forwardDrive();
			auto min = 1e12;
			int pos = 0;
			for(int i = 0;i < outputSize; ++i) {
				if(min > output[2][i]) {
					min = output[2][i];
					pos = i;
				}
			}
			return pos;
		}

		~BPNN() {
			return;
			std::ofstream out("weights.txt", std::ios::trunc);
			ON_SCOPE_EXIT([&]{ 
				out.close(); 
			});
			assert(out);
			for(const auto& it : this->weights) {
				out << it.size() << "\n";
				for(const auto& l : it) {
					out << l.size() << "\n";
					for(const auto r : l)
						out << r << " ";
					out << "\n";
				}
			}
			std::cout << "矩阵备份完毕\n";
		}

		void loadFile(const std::string& fileName) {
			std::ifstream in(fileName.c_str());
			ON_SCOPE_EXIT([&]{
				in.close();
			});
			assert(in);
			int l, r;
			for(int i = 0;i < 2; ++i) {
				in >> l;
				for(int j = 0;j < l; ++j) {
					in >> r;
					double value;
					for(int k = 0;k < r; ++k) {
						in >> value;
						this->weights[i][j][k] = value;
					}
				}
			}
			std::cout << "加载矩阵完毕\n";
		}
	};

}

int main() {
	YHL::BPNN one;
	// one.train();
	return 0;
}