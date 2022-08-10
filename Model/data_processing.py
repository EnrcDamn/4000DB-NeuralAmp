import os
import torch
from torch.utils.data import Dataset

import os
from scipy import signal
from scipy.io import wavfile
import numpy as np
import warnings


class Dataset:
    def __init__(self, data_dir):
        self.data_dir = data_dir
        pass


def audio_converter(audio):
    '''
    Converts raw binary int16 data to float32.
    Floating point audio data is normalized to the range of [-1.0, 1.0],
    which can be done by scaling the -32768 to 32767 int range.
    '''
    if audio.dtype == 'int16':
        return audio.astype(np.float32, order='C') / 32768.0
    else:
        print('unimplemented audio data type conversion...')


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
def framify(audio, frame_len):
    # If audio is mono, add a dummy dimension, so the same operations can be applied to mono/multichannel audio
    audio = np.expand_dims(audio, 1) if len(audio.shape) == 1 else audio
    # Calculate the number of segments the training data will be split into in frame_len is not 0
    seg_num = math.floor(audio.shape[0] / frame_len) if frame_len else 1
    # If no frame_len is provided, set frame_len to be equal to length of the input audio
    frame_len = audio.shape[0] if not frame_len else frame_len
    # Find the number of channels
    channels = audio.shape[1]
    # Initialise tensor matrices
    dataset = torch.empty((frame_len, seg_num, channels))
    # Load the audio for the training set
    for i in range(seg_num):
        dataset[:, i, :] = torch.from_numpy(audio[i * frame_len:(i + 1) * frame_len, :])
    return dataset



if __name__ == "__main__":

    NAME = 'test'
    IN_FILE = 'Model/Data/input_FP32.wav'
    OUT_FILE = 'Model/Data/output_FP32.wav'

    in_rate, in_data = wavfile.read(IN_FILE)
    out_rate, out_data = wavfile.read(OUT_FILE)

    if os.path.exists('models/' + NAME):
        raise Exception("A model with the same name already exists. Please choose a new name.")
    os.makedirs('models/' + NAME)