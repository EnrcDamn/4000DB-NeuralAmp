from distutils import extension
import os
import torch
from torch.utils.data import Dataset

import os
from scipy import signal
from scipy.io import wavfile
import numpy as np
import math
import warnings

class Dataset:
    def __init__(self, in_filename, out_filename='', data_dir='Model/Data/'):
        self.data_dir = data_dir
        self.in_filename = in_filename
        self.out_filename = out_filename

    def process(self):
        in_rate, in_data = self.load_file(os.path.join(self.data_dir, self.in_filename))
        in_data = self.audio_converter(in_data)
        # assert in_rate == out_rate, "in_data and out_data must have same sample rate"
        # in_data = normalize(in_data)
        in_data = self.audio_splitter(in_data)
        return in_data

    def load_file(self, filename):
        try:
            file_rate, file_data = wavfile.read(filename)
            return file_rate, file_data
        except FileNotFoundError:
            print(["File Not Found At: " + filename])
            return

    def audio_converter(self, audio):
        '''
        Converts raw binary int16 data to float32. 
        Floating point audio data is normalized to the range of [-1.0, 1.0],
        which can be done by scaling the -32768 to 32767 int range.
        '''
        if audio.dtype == 'int16':
            return audio.astype(np.float32, order='C') / 32768.0
        elif audio.dtype == 'float32':
            return audio
        else:
            print('Unimplemented audio data type conversion...')

    def normalize(self, data):
        '''
        Normalizes data to the range of [-1.0, 1.0].
        Unused at the moment.
        '''
        data_max = max(data)
        data_min = min(data)
        data_norm = max(data_max,abs(data_min))
        return data / data_norm

    def audio_splitter(self, audio):
        '''
        Splits audio.
        Lambda function will put 70% of audio in the first split (train set)
        and 85% in the second one (validation set).
        '''
        split = lambda d: np.split(d, [int(len(d) * 0.7), int(len(d) * 0.85)])
        slices = {}
        slices["x_train"], slices["x_valid"], slices["x_test"] = split(audio)
        slices["mean"], slices["std"] = slices["x_train"].mean(), slices["x_train"].std()
        # standardize
        for key in "x_train", "x_valid", "x_test":
            slices[key] = (slices[key] - slices["mean"]) / slices["std"]
        return slices


if __name__ == "__main__":
    data = Dataset(in_filename='input_FP32.wav')
    data.process()