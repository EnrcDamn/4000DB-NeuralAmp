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
        in_rate, in_data = load_file(os.path.join(self.data_dir, self.in_filename))
        in_data = audio_converter(in_data)
        # in_data = normalize(in_data) # normalize in conversion above
        # Split the audio if the set_names were provided
        in_data = audio_splitter(in_data)
        return in_data
    
    # Add 'audio' data, in the data dictionary at the key 'ext', if cond_val is provided save the cond_val of each frame
    def add_data(self, data, fs, audio, ext):
        # if no 'ext' is provided, all the subsets data will be stored at the 'data' key of the 'data' dict
        ext = 'data' if not ext else ext
        # Frame the data and optionally create a tensor of the conditioning values of each frame
        framed_data = framify(data, self.frame_len)
        data = list(self.data[ext])
        self.data[ext] = (torch.cat((data[0], framed_data), 1),)
        return self.data[ext]


def load_file(filename):
    try:
        file_rate, file_data = wavfile.read(filename)
        return file_rate, file_data
    except FileNotFoundError:
        print(["File Not Found At: " + filename])
        return

def audio_converter(audio):
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

def normalize(data):
    data_max = max(data)
    data_min = min(data)
    data_norm = max(data_max,abs(data_min))
    return data / data_norm

def audio_splitter(audio):
    '''
    Splits audio.
    Lambda function will put 70% of audio in the first split (train) and 85% in the second one (valid).
    '''
    split = lambda d: np.split(d, [int(len(d) * 0.7), int(len(d) * 0.85)])

    slices = {}
    slices["x_train"], slices["x_valid"], slices["x_test"] = split(audio)
    slices["mean"], slices["std"] = slices["x_train"].mean(), slices["x_train"].std()
    print(slices)
    return slices

# TODO: REVIEW FRAMIFY FUNCTION
# converts numpy audio into frames, and creates a torch tensor from them, frame_len = 0 just converts to a torch tensor
def framify(audio, frame_len = 4096):
    # If audio is mono, add a dummy dimension, so the same operations can be applied to mono/multichannel audio
    audio = np.expand_dims(audio, 1) if len(audio) == 1 else audio
    # Calculate the number of segments the training data will be split into in frame_len is not 0
    seg_num = math.floor(audio.size[0] / frame_len) if frame_len else 1
    # If no frame_len is provided, set frame_len to be equal to length of the input audio
    frame_len = audio.size if not frame_len else frame_len
    # Find the number of channels
    channels = 1
    # Initialise tensor matrices
    dataset = torch.empty((frame_len, seg_num))
    # Load the audio for the training set
    for i in range(seg_num):
        dataset[i, :] = torch.from_numpy(audio[i * frame_len:(i + 1) * frame_len])
    return dataset


if __name__ == "__main__":
    data = Dataset(in_filename='input_FP32.wav')
    data.process()