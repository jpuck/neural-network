# Neural Network PHP Extension

This neural network was written in C++ by [Dr. Mike Gashler][1]
and is released under the [CC0 license][2].
The native PHP module is compiled using [PHP-CPP][3].

## Dependencies

Install build utilities. (Ubuntu e.g.)

    sudo apt-get install build-essential php7.0 php7.0-dev -y

Install [PHP-CPP][3].

    git clone https://github.com/CopernicaMarketingSoftware/PHP-CPP.git
    cd PHP-CPP
    make
    sudo make install

## Installation

Build, install, and enable the module.

    make
    sudo make install
    sudo phpenmod jpuck-neural-network

## Usage

See the tests for more detailed usage.

```php
<?php

// intialize with the number of inputs,
// number of nodes per hidden layer (must have at least 1),
// and lastly the number of outputs.
// e.g. 3 inputs, 1 hidden layer with 16 nodes, and 2 outputs
$nn = new jpuck\NeuralNetwork(3, 16, 2);

// train the network with some features, labels, and a bias.
$inputs = [
    0.12,
    -0.38,
    0.77,
];
$outputs = [
    0.54,
    -0.25,
];
$nn->refine($inputs, $outputs, 0.02);

// get a prediction
$prediction = $nn->predict(...$inputs);
```

[1]:https://github.com/mikegashler
[2]:http://creativecommons.org/publicdomain/zero/1.0/
[3]:https://github.com/CopernicaMarketingSoftware/PHP-CPP
