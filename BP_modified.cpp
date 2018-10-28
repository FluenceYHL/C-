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
		static constexpr int layer = 3;
		static constexpr double rate = 0.24;
		using matrix = std::vector< std::vector<double> >;
	private:
		// layer - 1 层权值矩阵
		matrix weights[layer];
		// 数据集和答案
		matrix dataSet;
		matrix answers;
		// 两层之间的向量
		std::vector<double> output[layer];
		std::vector<double> delta[layer - 1];
		std::vector<double> threshold[layer - 1];

		const int inputSize;
		const int hideSize; 
		const int outputSize;
		std::vector<double> target;

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
			this->output[0].assign(inputSize, 0.00);
			this->output[1].assign(hideSize, 0.00);
			this->output[2].assign(outputSize, 0.00); 
		}

		void initDelta() {
			this->delta[0].assign(hideSize, 0.00);
			this->delta[1].assign(outputSize, 0.00);
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
				this->threshold[1][k] -= rate * 1 * gradient;
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
				this->threshold[0][j] -= rate * 1 * gradient;
			}
		}

		const int chooseBest() const {
			auto max = -1e12;
			int pos = 0;
			for(int i = 0;i < outputSize; ++i) {
				if(max < output[2][i]) {
					max = output[2][i];
					pos = i;
				}
			}
			return pos;
		}

	public:
		BPNN(const int l, const int m, const int r) : inputSize(l), hideSize(m), outputSize(r) {
			this->initWeights();
			this->initOutput();
			this->initDelta();
			this->initThreshold();
		}

		void train() {
			std::ifstream image("./trainSet/train-images.idx3-ubyte", std::ios::binary);
			std::ifstream label("./trainSet/train-labels.idx1-ubyte", std::ios::binary);
			ON_SCOPE_EXIT([&]{
				image.close();
				label.close();
			});
			assert(image and label);
			char head[20];
			image.read(head, sizeof(char) * 16);
			label.read(head, sizeof(char) * 8);

			std::cout << "开始读取文件\n";
			char image_buf[inputSize + 1];
			char label_buf;
			target.assign(outputSize, 0.00);
			while(!image.eof() and !label.eof()) {
				image.read((char*)&image_buf, sizeof(char) * inputSize);
				label.read((char*)&label_buf, sizeof(char) * 1);

				for(int i = 0;i < outputSize; ++i) 
					this->target[i] = 0.00;
				target[(unsigned int)label_buf] = 1.00;

				for(int i = 0;i < inputSize; ++i) {
					double value = (unsigned int)image_buf[i] < 128 ? 0.00 : 1.00;
					output[0][i] = value;
				}

				this->forwardDrive();
				std::cout << "error  :  " << this->getError() << "\n";
				if(this->getError() > 1e-7)
					this->backPropagate();
			}
		}

		const int recognize(const std::vector<double>& input) {

			const int len = input.size();
			assert(len == inputSize);
			for(int i = 0;i < inputSize; ++i)
				this->output[0][i] = input[i];

			this->forwardDrive();
			return this->chooseBest();
		}

		~BPNN() {
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

		void test() {
			double test_success_count = 0.00, test_num = 0.00;
			FILE *image_test;
			FILE *image_test_label;
			image_test = fopen("./trainSet/t10k-images.idx3-ubyte", "rb");
			image_test_label = fopen("./trainSet/t10k-labels.idx1-ubyte", "rb");
			if (image_test == NULL || image_test_label == NULL){
				std::cout << "can't open the file!" << std::endl;
				exit(0);
			}

			unsigned char image_buf[784];
			unsigned char label_buf[10];
			
			int useless[1000];
			fread(useless, 1, 16, image_test);
			fread(useless, 1, 8, image_test_label);

			while (!feof(image_test) && !feof(image_test_label)){
				memset(image_buf, 0, 784);
				memset(label_buf, 0, 10);
				fread(image_buf, 1, 784, image_test);
				fread(label_buf, 1, 1, image_test_label);

				//initialize the input by 28 x 28 (0,1)matrix of the images
				for (int i = 0; i < 784; i++){
					if ((unsigned int)image_buf[i] < 128){
						output[0][i] = 0;
					}
					else{
						output[0][i] = 1;
					}
				}

				//initialize the target output
				for (int k = 0; k < outputSize; k++){
					target[k] = 0;
				}
				int target_value = (unsigned int)label_buf[0];
				target[target_value] = 1;
				
				//get the ouput and compare with the targe
				this->forwardDrive();

				double max_value = -99999;
				int max_index = 0;
				for (int k = 0; k < outputSize; k++){
					if (output[2][k] > max_value){
						max_value = output[2][k];
						max_index = k;
					}
				}

				//output == target
				if (target[max_index] == 1){
					test_success_count ++;
				}
				
				test_num ++;

				if ((int)test_num % 1000 == 0){
					std::cout << "test num: " << test_num << "  success: " << test_success_count << std::endl;
				}
			}
			std::cout << std::endl;
			std::cout << "The success rate: " << test_success_count / test_num << std::endl;
		}
	};

}

int main() {
	YHL::BPNN one(784, 100, 10);
	one.train();
	one.test();
	return 0;
}