// ----------------------------------------------------------------
// The contents of this file are distributed under the CC0 license.
// See http://creativecommons.org/publicdomain/zero/1.0/
// ----------------------------------------------------------------

#include "neuralnet.h"
#include "error.h"
#include "string.h"
#include "rand.h"
#include <math.h>
#include <cmath>

using std::vector;




double activation(double x)
{
	if(x >= 700.0) // Don't trigger a floating point exception
		return 1.0;
	if(x < -700.0) // Don't trigger a floating point exception
		return -1.0;
	return tanh(x);
}

double activationDerivative(double net, double activation)
{
	return 1.0 - (activation * activation);
}




Layer::Layer(size_t inSize, size_t outSize)
{
	m_weights.setSize(outSize, inSize);
	m_bias.resize(outSize);
	m_net.resize(outSize);
	m_activation.resize(outSize);
	m_error.resize(outSize);
}


void Layer::init(Rand& rand)
{
	double dev = std::max(0.3, 1.0 / m_weights.cols());
	for(size_t i = 0; i < m_weights.rows(); i++)
	{
		vector<double>& row = m_weights[i];
		for(size_t j = 0; j < m_weights.cols(); j++)
		{
			row[j] = dev * rand.normal();
		}
	}
	for(size_t j = 0; j < m_weights.rows(); j++)
	{
		m_bias[j] = dev * rand.normal();
	}
}

double dotProduct(const vector<double>& a, const vector<double>& b)
{
	vector<double>::const_iterator itA = a.begin();
	vector<double>::const_iterator itB = b.begin();
	double d = 0.0;
	while(itA != a.end())
		d += *(itA++) * *(itB++);
	return d;
}

void Layer::feed_forward(const vector<double>& in)
{
	for(size_t i = 0; i < m_weights.rows(); i++)
	{
		m_net[i] = dotProduct(in, m_weights[i]) + m_bias[i];
		m_activation[i] = activation(m_net[i]);
	}
}

void Layer::backprop(const Layer& from)
{
	for(size_t i = 0; i < m_weights.rows(); i++)
	{
		double e = 0.0;
		for(size_t j = 0; j < from.m_weights.rows(); j++)
			e += from.m_weights[j][i] * from.m_error[j] * activationDerivative(m_net[i], m_activation[i]);
		m_error[i] = e;
	}
}

void Layer::update_weights(const vector<double>& in, double learning_rate)
{
	for(size_t j = 0; j < m_weights.rows(); j++)
	{
		for(size_t i = 0; i < m_weights.cols(); i++)
			m_weights[j][i] += learning_rate * m_error[j] * in[i];
		m_bias[j] += learning_rate * m_error[j];
	}
}








NeuralNet::NeuralNet(Rand& r)
: m_rand(r)
{
}

NeuralNet::NeuralNet(const NeuralNet& other)
: m_rand(other.m_rand)
{
	throw Ex("Big objects should generally be passed by reference, not by value.");
}

// virtual
NeuralNet::~NeuralNet()
{
	for(size_t i = 0; i < m_layers.size(); i++)
		delete(m_layers[i]);
}

void NeuralNet::init()
{
	for(size_t i = 0; i < m_layers.size(); i++)
		m_layers[i]->init(m_rand);
}

void NeuralNet::refine(const std::vector<double>& feature, const std::vector<double>& label, double learning_rate)
{
	forward_prop(feature);
	compute_output_layer_error_terms(label);
	backpropagate();
	descend_gradient(feature, learning_rate);
}

// virtual
void NeuralNet::train(const Matrix& features, const Matrix& labels)
{
	if(features.rows() != labels.rows())
		throw Ex("mismatching feature and label rows");
	init();

	// Make a list of indexes
	size_t* indexes = new size_t[features.rows()];
	for(size_t i = 0; i < features.rows(); i++)
		indexes[i] = i;

	double learning_rate = 0.1;
	for(size_t i = 0; i < 500; i++)
	{
		// Shuffle the indexes
		for(size_t j = features.rows() - 1; j > 0; j--)
			std::swap(indexes[j], indexes[m_rand.next(j)]);

		// Do one epoch of training
		for(size_t j = 0; j < features.rows(); j++)
		{
			size_t index = indexes[j];
			refine(features[index], labels[index], learning_rate);
		}

		// Decay the learning rate
		learning_rate *= 0.997;
	}
}

const std::vector<double>& NeuralNet::forward_prop(const std::vector<double>& in)
{
	m_layers[0]->feed_forward(in);
	for(size_t i = 1; i < m_layers.size(); i++)
		m_layers[i]->feed_forward(m_layers[i - 1]->m_activation);
	return m_layers[m_layers.size() - 1]->m_activation;
}

void NeuralNet::compute_output_layer_error_terms(const std::vector<double>& target)
{
	Layer& output_layer = *m_layers[m_layers.size() - 1];
	for(size_t i = 0; i < target.size(); i++)
		output_layer.m_error[i] = (target[i] - output_layer.m_activation[i]) * activationDerivative(output_layer.m_net[i], output_layer.m_activation[i]);
}

void NeuralNet::backpropagate()
{
	for(size_t i = m_layers.size() - 1; i > 0; i--)
		m_layers[i - 1]->backprop(*m_layers[i]);
}

void NeuralNet::descend_gradient(const vector<double>& in, double learning_rate)
{
	const vector<double>* pActivation = &in;
	for(size_t i = 0; i < m_layers.size(); i++)
	{
		m_layers[i]->update_weights(*pActivation, learning_rate);
		pActivation = &m_layers[i]->m_activation;
	}
}

