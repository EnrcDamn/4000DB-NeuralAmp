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
    def __init__(self, data_dir='../Data/', extensions=('input', 'target')):
        self.extensions = extensions if extensions else ['']
        self.subsets = {}
        assert type(data_dir) == str, "data_dir should be string,not %r" % {type(data_dir)}
        self.data_dir = data_dir
        self.data = {}
        self.frame_len = None
        self.fs = None

    # load a file of 'filename' into existing subset/s 'set_names', split fractionally as specified by 'splits',
    # if 'cond_val' is provided the conditioning value will be saved along with the frames of the loaded data
    def load_file(self, filename, set_names='train', splits=None, cond_val=None):
        # Assertions and checks
        if type(set_names) == str:
            set_names = [set_names]
        assert len(set_names) == 1 or len(set_names) == len(splits), "number of subset names must equal number of " \
                                                                     "split markers"
        assert [self.subsets.get(each) for each in set_names], "set_names contains subsets that don't exist yet"
        
        for i, ext in enumerate(self.extensions):
            try:
                fs, np_data = wavfile.read(filename)
                print(fs)
            except FileNotFoundError:
                print(["File Not Found At: " + self.data_dir + filename])
                return
            raw_audio = audio_converter(np_data)
            # Split the audio if the set_names were provided
            if len(set_names) > 1:
                raw_audio = audio_splitter(raw_audio, splits)
                for n, sets in enumerate(set_names):
                    self.subsets[set_names[n]] = self.add_data(raw_audio[n], fs, ext, cond_val)
            elif len(set_names) == 1:
                self.subsets[set_names[0]] = self.add_data(raw_audio, fs, ext, cond_val)
    
    # Add 'audio' data, in the data dictionary at the key 'ext', if cond_val is provided save the cond_val of each frame
    def add_data(self, data, fs, audio, ext):
        # if no 'ext' is provided, all the subsets data will be stored at the 'data' key of the 'data' dict
        ext = 'data' if not ext else ext
        # Frame the data and optionally create a tensor of the conditioning values of each frame
        framed_data = framify(data, self.frame_len)
        data = list(self.data[ext])
        self.data[ext] = (torch.cat((data[0], framed_data), 1),)
        return self.data[ext]

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


def audio_splitter(audio, split_markers):
    '''
    Splits audio.
    Each split marker determines the fraction of the total audio in that split, 
    i.e [0.75, 0.25] will put 75% in the first split and 25% in the second one.
    '''
    assert sum(split_markers) <= 1.0
    if sum(split_markers) < 0.999:
        warnings.warn("Sum of split markers is less than 1: some audio won't be included in the dataset")
    start = 0
    slices = []
    # convert split markers to samples: [0.75, 0.25] -> [75k, 25k] for audio[0:100k]
    split_samples = [int(marker * audio.shape[0]) for marker in split_markers]
    for n in split_samples:
        end = start + n
        slices.append(audio[start:end])
        start = end
        # return [ [0, ..., 75k], [75k+1, ..., 100k] ]
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