import argparse
from torch.utils.data import Dataset
import os
from scipy.io import wavfile
import numpy as np

IN_FILE = "4000DB-dry"
OUT_FILE = "4000DB-100"

class Dataset:
    def __init__(self, in_filename, out_filename, data_dir='Model/Data/'):
        self.data_dir = data_dir
        self.in_filename = in_filename
        self.out_filename = out_filename

    def process(self):
        in_rate, in_data = self.load_file(os.path.join(self.data_dir, self.in_filename+'.wav'))
        out_rate, out_data = self.load_file(os.path.join(self.data_dir, self.out_filename+'.wav'))
        assert in_rate == out_rate, "in_data and out_data must have same sample rate!"
        #in_data = self.audio_converter(in_data)
        #out_data = self.audio_converter(out_data)
        # in_data = normalize(in_data)
        x_subset = self.audio_splitter(in_data, 'x')
        y_subset = self.audio_splitter(out_data, 'y')
        self.save_wav('Model/processed/'+self.in_filename+'_train.wav', in_rate, x_subset['x_train'])
        self.save_wav('Model/processed/'+self.in_filename+'_test.wav', in_rate, x_subset['x_test'])
        self.save_wav('Model/processed/'+self.out_filename+'_train.wav', out_rate, y_subset['y_train'])
        self.save_wav('Model/processed/'+self.out_filename+'_test.wav', out_rate, y_subset['y_test'])
        return x_subset, y_subset

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

    def audio_splitter(self, audio, set_name):
        '''
        Splits audio.
        Lambda function will put 70% of audio in the first split (train set)
        and 85% in the second one (validation set).
        '''
        split = lambda d: np.split(d, [int(len(d) * 0.8), int(len(d) * 0.85)])
        slices = {}
        slices[set_name+"_train"], slices[set_name+"_valid"], slices[set_name+"_test"] = split(audio)
        slices["mean"], slices["std"] = slices[set_name+"_train"].mean(), slices[set_name+"_train"].std()
        # standardize
        # for key in set_name+"_train", set_name+"_valid", set_name+"_test":
        #     slices[key] = (slices[key] - slices["mean"]) / slices["std"]
        return slices
    
    def save_wav(self, name, rate, data):
        wavfile.write(name, rate, data.astype(np.float32))

if __name__ == "__main__":
    data = Dataset(in_filename=IN_FILE, out_filename=OUT_FILE)
    x, y = data.process()