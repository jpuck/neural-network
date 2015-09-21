// This file is licensed under the CC0 license (See http://creativecommons.org/publicdomain/zero/1.0/).
// And just to make sure you get the idea, it is also licensed under the WTFPL (See http://en.wikipedia.org/wiki/WTFPL).

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <exception>
#include <string>
#include <memory>
#include <vector>
#include <cmath>
#include "error.h"
#include "string.h"
#include "rand.h"
#include "matrix.h"
#include "neuralnet.h"

using std::cout;
using std::cerr;
using std::string;
using std::auto_ptr;
using std::vector;

void run_demo()
{
	// Make a neural network
	Rand rand(0);
	NeuralNet nn(rand);
	nn.m_layers.push_back(new Layer(3, 16));
	nn.m_layers.push_back(new Layer(16, 2));
	nn.init();

	// Train it to approximate some simple functions
	cout << "Training...\n";
	vector<double> in;
	in.resize(3);
	vector<double> out;
	out.resize(2);
	for(int i = 0; i < 100000; i++)
	{
		in[0] = rand.uniform();
		in[1] = rand.uniform();
		in[2] = rand.uniform();
		out[0] = (in[0] + in[1] + in[2]) / 3.0;
		out[1] = (in[0] * in[1] - in[2]);
		nn.refine(in, out, 0.02);
	}

	// Test it
	cout << "Testing...\n";
	double sse = 0.0;
	int testPatterns = 100;
	for(int i = 0; i < testPatterns; i++)
	{
		in[0] = rand.uniform();
		in[1] = rand.uniform();
		in[2] = rand.uniform();
		const vector<double>& prediction = nn.forward_prop(in);
		out[0] = (in[0] + in[1] + in[2]) / 3.0;
		out[1] = (in[0] * in[1] - in[2]);
		double err0 = out[0] - prediction[0];
		double err1 = out[1] - prediction[1];
		sse += (err0 * err0) + (err1 * err1);
	}
	double rmse = std::sqrt(sse / testPatterns);
	if(rmse < 0.05)
		cout << "Passed.\n";
	else
		cout << "Failed!!! Got " << to_str(rmse) << "\n";

}

int main(int argc, char *argv[])
{
	enableFloatingPointExceptions();
	int ret = 1;
	try
	{
		run_demo();
		ret = 0;
	}
	catch(const std::exception& e)
	{
		cerr << "An error occurred: " << e.what() << "\n";
	}
	cout.flush();
	cerr.flush();
	return ret;
}
