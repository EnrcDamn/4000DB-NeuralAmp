# Neural Reel Saturator

Guitar plugin made with JUCE, using a neural network to reproduce the pre-amp section of my old Akai 4000DB reel-to-reel tape machine.

The training was made using the [GuitarML](https://github.com/GuitarML/Automated-GuitarAmpModelling) Automated Amp Modelling submodule with a multi-parameterization model. This repository is an implementation of the paper ["Real-Time Guitar Amplifier Emulation with Deep Learning"](https://www.mdpi.com/2076-3417/10/3/766/htm).

For real-time inference within the plugin, [RTNeural](https://github.com/jatinchowdhury18/RTNeural) was used.

Taking inspiration from Neural DSP products, the goal of the project is to develop a full working plugin using neural networks in a similar fashion, to model a highly non-linear amplification circuit.

# How To Use

## Pre-processing

