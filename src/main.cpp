// This file is licensed under the CC0 license (See http://creativecommons.org/publicdomain/zero/1.0/).
// And just to make sure you get the idea, it is also licensed under the WTFPL (See http://en.wikipedia.org/wiki/WTFPL).
#include <phpcpp.h>
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

using std::vector;

class NeuralNetwork : public Php::Base
{
    private:
        Rand rand;
        NeuralNet nn;
        size_t inputCount = 0;

    public:
        NeuralNetwork() : rand(0), nn(rand) {}

        virtual ~NeuralNetwork() = default;

        void __construct(Php::Parameters &params)
        {
            if (params.size() < 3)
            {
                Php::error << "Neural Network requires inputs, at least one hidden layer, and outputs." << std::flush;
                return;
            }

            inputCount = (int16_t) params[0];

            int16_t inputs, outputs;

            for (size_t i = 0; i < params.size() - 1; i++)
            {
                inputs = params[i];
                outputs = params[i+1];

                if (inputs < 1 || outputs < 1)
                {
                    Php::error << "Inputs and outputs must be at least 1." << std::flush;
                    return;
                }

                nn.m_layers.push_back(new Layer(inputs, outputs));
            }

            nn.init();
        }

        void refine(Php::Parameters &params)
        {
            nn.refine(params[0], params[1], params[2]);
        }

        Php::Value predict(Php::Parameters &params)
        {
            if (params.size() != inputCount)
            {
                throw Php::Exception("Parameter count doesn't match input count.");
            }

            vector<double> in;
            in.resize(params.size());

            for (size_t i = 0; i < params.size(); i++)
            {
                in[i] = params[i];
            }

            const vector<double>& prediction = nn.forward_prop(in);

            Php::Value array(prediction);

            return array;
        }
};

/**
 *  tell the compiler that the get_module is a pure C function
 */
extern "C" {

    /**
     *  Function that is called by PHP right after the PHP process
     *  has started, and that returns an address of an internal PHP
     *  strucure with all the details and features of your extension
     *
     *  @return void*   a pointer to an address that is understood by PHP
     */
    PHPCPP_EXPORT void *get_module()
    {
        // static(!) Php::Extension object that should stay in memory
        // for the entire duration of the process (that's why it's static)
        static Php::Extension extension("jpuck-neural-network", "1.0");

        // create a namespace
        Php::Namespace ns("jpuck");

        Php::Class<NeuralNetwork> nnet("NeuralNetwork");
        nnet.method<&NeuralNetwork::__construct> ("__construct", {
            Php::ByVal("a", Php::Type::Numeric),
            Php::ByVal("b", Php::Type::Numeric),
            Php::ByVal("c", Php::Type::Numeric)
            // TODO: figure out variadic type hints
        });
        nnet.method<&NeuralNetwork::refine> ("refine", {
            Php::ByVal("input", Php::Type::Array),
            Php::ByVal("output", Php::Type::Array),
            Php::ByVal("bias", Php::Type::Float)
        });
        nnet.method<&NeuralNetwork::predict> ("predict", {
            Php::ByVal("input", Php::Type::Float)
            // TODO: figure out variadic type hints
        });

        ns.add(std::move(nnet));

        // add the namespace to the extension
        extension.add(std::move(ns));

        // return the extension
        return extension;
    }
}
