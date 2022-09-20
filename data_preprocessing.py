import os
from scipy.io import wavfile
import numpy as np

def collect_files(directory):
    files_list = []
    if not os.path.exists(directory):
        raise FileNotFoundError("Data folder not found: maybe wrong name?")
    for _, _, files in os.walk(directory):
        for file in files:
            if file[-4:] == ".wav":
                files_list.append(file)
    print("Files successfully collected; starting processing...\n")
    return files_list

def process(data_dir, files):
    out_dir = "./Data/processed/"
    if not os.path.exists(out_dir):
        os.mkdir(out_dir)
    for file in files:
        file = file[:-4] # parse filename from ".wav" extension
        rate, data = load_file(os.path.join(data_dir, file+'.wav'))
        data = audio_converter(data)
        data = normalize(data)
        splitted_data = audio_splitter(data)
        save_wav(os.path.join(out_dir, file+'_train.wav'), rate, splitted_data['data_train'])
        save_wav(os.path.join(out_dir, file+'_test.wav'), rate, splitted_data['data_test'])
    print("Processing successfully completed!\n")

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
    '''
    Normalizes data to the range of [-1.0, 1.0].
    Unused at the moment.
    '''
    data_max = max(data)
    data_min = min(data)
    data_norm = max(data_max,abs(data_min))
    return data / data_norm

def audio_splitter(data):
    '''
    Splits audio.
    Lambda function will put 70% of audio in the first split (train set)
    and 85% in the second one (validation set).
    '''
    split = lambda d: np.split(d, [int(len(d) * 0.8), int(len(d) * 0.85)])
    slices = {}
    slices["data_train"], slices["data_valid"], slices["data_test"] = split(data)
    slices["mean"], slices["std"] = slices["data_train"].mean(), slices["data_train"].std()
    # standardize
    # for key in "data_train", "data_valid", "data_test":
    #     slices[key] = (slices[key] - slices["mean"]) / slices["std"]
    return slices

def save_wav(name, rate, data):
    wavfile.write(name, rate, data.astype(np.float32))

if __name__ == "__main__":
    DATA_DIR='./Data/'
    files = collect_files(DATA_DIR)
    process(DATA_DIR, files)