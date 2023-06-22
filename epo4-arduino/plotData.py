import matplotlib.pyplot as plt
import pandas as pd
import numpy as np

# Get rid of jump in TimeStamps
def ChronologicalTimeStamps(data):
    smallestTimeStamp = data['TimeStamp'].idxmin()
    CorrectedData = data[smallestTimeStamp:-1]
    return CorrectedData

def plot_data(inputfile = 'SensorData.csv'):

    importData = pd.read_csv(inputfile)

    # Get rid of jump in TimeStamps and skip first few rows of irrelevant values from sensor / arduino startup
    CorrectedData = ChronologicalTimeStamps(importData)[10:]

    # Plot the obtained sensor values in their respective subplots
    fig, axs = plt.subplots(3, 1, sharex=True)
    fig.suptitle('Sensor Readouts')

    # ECG sensor
    axs[0].plot(CorrectedData['TimeStamp']/1000, CorrectedData['ECG Data'], label='ECG')
    axs[0].set_ylabel('Amplitude')
    axs[0].set_title('Electrocardiogram')
    axs[0].legend()

    # EDA / GSR sensor
    axs[1].plot(CorrectedData['TimeStamp']/1000, CorrectedData['GSR Data'], label='EDA')
    axs[1].set_ylabel('Amplitude')
    axs[1].set_title('Electrodermal activity')
    axs[1].legend()

    # Pulse oximeter sensor
    axs[2].plot(CorrectedData['TimeStamp']/1000, CorrectedData['PPG_Red Data'], label='Red LED')
    axs[2].plot(CorrectedData['TimeStamp']/1000, CorrectedData['PPG_IR Data'], label='IR LED')
    axs[2].set_xlabel('Time (s)')
    axs[2].set_ylabel('Amplitude')
    axs[2].set_title('Photoplethysmogram')
    axs[2].legend()

    plt.show()

