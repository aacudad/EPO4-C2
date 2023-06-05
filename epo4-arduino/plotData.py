import matplotlib.pyplot as plt
import pandas as pd
import numpy as np

# Get rid of jump in TimeStamps
def ChronologicalTimeStamps(data):
    smallestTimeStamp = data['TimeStamp'].idxmin()
    CorrectedData = data[smallestTimeStamp:-1]
    return CorrectedData

def plot_data(inputfile = 'SensorData.csv'):

    importData = pd.read_csv(inputfile, usecols=[0, 3, 4])

    # Get rid of jump in TimeStamps
    CorrectedData = ChronologicalTimeStamps(importData)

    ecg_plot = CorrectedData.plot(x = 'TimeStamp', subplots = False)

    plt.show()

