import torch
import torch.nn as nn
import math


class Network(nn.Module):
    '''This is an implementation of the model from the paper:
    "Real-Time Guitar Amplifier Emulation with Deep Learning"
    https://www.mdpi.com/2076-3417/10/3/766/htm

    Uses a stack of two 1-D Convolutional layers, followed by LSTM, followed by 
    a Linear (fully connected) layer.
    '''
    def __init__(self, dilations, in_channels, out_channels, kernel_size,
                num_classes, num_layers, input_size, hidden_size, seq_length):
        super().__init__()
        self.num_classes = num_classes #number of classes
        self.num_layers = num_layers #number of layers
        self.input_size = input_size #input size
        self.hidden_size = hidden_size #hidden state
        self.seq_length = seq_length #sequence length

        # 2 x 1d conv blocks -> LSTM -> linear

        self.conv1 = nn.Conv1d(
            in_channels = in_channels,
            out_channels = out_channels * 2,
            kernel_size = kernel_size,
            stride = 1,
            padding = 0, 
            dilation = dilations
            )

        self.conv2 = nn.Conv1d(
            in_channels = in_channels,
            out_channels = out_channels * 2,
            kernel_size = kernel_size,
            stride = 1,
            padding = 0, 
            dilation = dilations
            )

        self.lstm = nn.LSTM(
            input_size = input_size, 
            hidden_size = hidden_size,
            num_layers = num_layers
            )

        self.linear = nn.Linear(
            hidden_size,
            128
            )

    
    def forward(self, input_data):
        x = self.conv1(input_data)
        x = self.conv2(x)
        x = self.lstm(x)
        out = self.linear(x)
        
        return out

if __name__ == "__main__":
    model = Network()
    