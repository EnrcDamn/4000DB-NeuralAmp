import torch
from torch import nn
from torch.utils.data import DataLoader
from model import Network
import numpy as np
import data_processing

IN_FILE = 'input_FP32.wav'
TARGET_FILE = 'output_FP32.wav'
EPOCHS = 60
BATCH_SIZE = 4096

learning_rate = 0.0005 
conv1d_strides = 3
conv1d_filters = 36
hidden_units = 96

def error_to_signal(y, y_pred):
    """
    Error to signal ratio with pre-emphasis filter:
    https://www.mdpi.com/2076-3417/10/3/766/htm
    """
    y, y_pred = pre_emphasis_filter(y), pre_emphasis_filter(y_pred)
    return (y - y_pred).pow(2).sum(dim=2) / (y.pow(2).sum(dim=2) + 1e-10)

def pre_emphasis_filter(x, coeff=0.95):
    return torch.cat((x[:, :, 0:1], x[:, :, 1:] - coeff * x[:, :, :-1]), dim=2)

def train_dataloader():
    return DataLoader(
        train_data,
        shuffle=True,
        batch_size=BATCH_SIZE,
        num_workers=4,
    )

def training_step(self, batch, batch_idx):
    x, y = batch
    y_pred = self.forward(x)
    loss = error_to_signal(y[:, :, -y_pred.size(2) :], y_pred).mean()
    logs = {"loss": loss}
    return {"loss": loss, "log": logs}

def train(model, data_loader, loss_fn, optimiser, device, epochs):
    for i in range(epochs):
        print(f"Epoch: {i+1}")
        training_step(model, data_loader, loss_fn, optimiser, device)
        print("--------------")
    print(f"Completed training for {i+1} epochs")


if __name__ == "__main__":
    if torch.cuda.is_available():
        device = "cuda"
    else:
        device = "cpu"
    print(f"Using {device} device...")

    data = data_processing.Dataset(in_filename=IN_FILE, out_filename=TARGET_FILE)
    x, y = data.process()

    train_dl = train_dataloader(train_data=x, batch_size=BATCH_SIZE)
    model = Network(
    )