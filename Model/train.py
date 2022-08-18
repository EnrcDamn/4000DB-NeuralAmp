import torch
from torch import nn
from torch.utils.data import DataLoader
from model import Network

name = 'test'
in_file = 'data/ac30_test1_in_FP32.wav'
out_file = 'data/ac30_test1_out_FP32.wav'
epochs = 60
batch_size = 4096
test_size = 0.2

learning_rate = 0.0005 
conv1d_strides = 3
conv1d_filters = 36
hidden_units= 96

def create_data_loader(train_data, batch_size):
    train_data_loader = DataLoader(train_data, batch_size=batch_size)
    return train_data_loader



if __name__ == "__main__":
    if torch.cuda.is_available():
        device = "cuda"
    else:
        device = "cpu"
    print(f"Using {device} device")

    # data = 
    train_data_loader = create_data_loader(train_data=data, batch_size=batch_size)

    # build model
    model = Network().to(device)

    # instantiate loss funcion and optimiser
    loss_fn = nn.CrossEntropyLoss()
    optimiser = torch.optim.Adam(model.parameters(),
                                 lr = learning_rate)

    # train model
    train(
        model,
        train_data_loader,
        loss_fn,
        optimiser,
        device,
        epochs
    )

    # save the model
    torch.save(model.state_dict(), "model.pth")
    print("Trained model is stored at model.pth")