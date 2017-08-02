<?php

use PHPUnit\Framework\TestCase;
use jpuck\NeuralNetwork;

class NeuralNetworkTest extends TestCase
{
    public function test_instantiates_object()
    {
        $this->assertInstanceOf(NeuralNetwork::class, new NeuralNetwork(3,16,2));
    }

    public function test_can_train_and_predict()
    {
        $nn = new NeuralNetwork(3,16,2);

        // train
        for ($i = 0; $i < 100000; $i++)
        {
            $in[0] = $this->getSmallFloat();
            $in[1] = $this->getSmallFloat();
            $in[2] = $this->getSmallFloat();

            $out[0] = ($in[0] + $in[1] + $in[2]) / 3.0;
            $out[1] = ($in[0] * $in[1] - $in[2]);

            $nn->refine($in, $out, 0.02);
        }

        // test
        $sse = 0.0;
        $testPatterns = 100;
        for ($i = 0; $i < $testPatterns; $i++)
        {
            $in[0] = $this->getSmallFloat();
            $in[1] = $this->getSmallFloat();
            $in[2] = $this->getSmallFloat();

            $prediction = $nn->predict(...$in);

            $out[0] = ($in[0] + $in[1] + $in[2]) / 3.0;
            $out[1] = ($in[0] * $in[1] - $in[2]);

            $err0 = $out[0] - $prediction[0];
            $err1 = $out[1] - $prediction[1];
            $sse += ($err0 * $err0) + ($err1 * $err1);
        }

        $rmse = sqrt($sse/$testPatterns);

        $this->assertLessThan(0.05, $rmse);
    }

    public function getSmallFloat()
    {
        // between 0 and 1
        return (float) mt_rand() / (float) mt_getrandmax();
    }
}
