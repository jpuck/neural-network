// ----------------------------------------------------------------
// The contents of this file are distributed under the CC0 license.
// See http://creativecommons.org/publicdomain/zero/1.0/
// ----------------------------------------------------------------

#ifndef NEURALNET_H
#define NEURALNET_H

#include <vector>
#include "matrix.h"

class Rand;


/// An class used by the NeuralNet class
class Layer
{
public:
	Matrix m_weights; // cols = in, rows = out
	std::vector<double> m_bias;
	std::vector<double> m_net;
	std::vector<double> m_activation;
	std::vector<double> m_error;

	Layer(size_t inputs, size_t outputs);

	void init(Rand& rand);
	void feed_forward(const std::vector<double>& in);
	void backprop(const Layer& from);
	void update_weights(const std::vector<double>& alpha, double learning_rate);
};




/// A multi-layer perceptron neural network
class NeuralNet
{
public:
	Rand& m_rand;
	std::vector<Layer*> m_layers;


	NeuralNet(Rand& r);
	NeuralNet(const NeuralNet& other);
	virtual ~NeuralNet();

	/// Initializes each layer with small random values
	void init();

	/// Present one pattern to refine this NeuralNet
	void refine(const std::vector<double>& feature, const std::vector<double>& label, double learning_rate);

	/// Train the NeuralNet
	void train(const Matrix& features, const Matrix& labels);

	/// Feed an input vector through this neural network to compute a predicted output vector
	const std::vector<double>& forward_prop(const std::vector<double>& in);

protected:
	void compute_output_layer_error_terms(const std::vector<double>& target);
	void backpropagate();
	void descend_gradient(const std::vector<double>& in, double learning_rate);
};


#endif // NEURALNET_H
