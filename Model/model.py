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
    def __init__(self):
        super().__init__()

        # 2 Conv1d blocks -> LSTM -> linear

        self.conv1 = nn.Conv1d(
            in_channels = 1,
            out_channels = 16,
            kernel_size = 3,
            stride = 1,
            padding = 0
            )

        self.conv2 = nn.Conv1d(
            in_channels = 16,
            out_channels = 32,
            kernel_size = 3,
            stride = 1,
            padding = 0
            )

        self.lstm = nn.LSTM(
            input_size = 32, 
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