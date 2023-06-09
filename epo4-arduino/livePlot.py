import time
import pandas as pd
import matplotlib.pyplot as plt
from pySerialTransfer import pySerialTransfer as txfer
import csv

def import_all(COMport='/dev/ttyACM0'):
    print("Importing Sensor Data \n")
    try:
        link = txfer.SerialTransfer(COMport, baud=115200)

        open_link = link.open()
        print(f"Link is open: {open_link}")

        header = ['TimeStamp', 'ECG Data', 'GSR Data', 'PPG_Red Data', 'PPG_IR Data']
        data = [0, 0, 0, 0, 0]

        output_file = open('SensorData.csv', 'w', newline='')
        writer = csv.writer(output_file)
        writer.writerow(header)

        # Create a pandas DataFrame to store the data
        df = pd.DataFrame(columns=header)

        time.sleep(3)
        print(f"link status: {link.status}")
        print('Stop data collection by keyboard interrupt (ctrl+c)')

        plt.ion()  # Turn on interactive mode for real-time plotting
        fig, ax = plt.subplots()
        lines = ax.plot([], [])
        ax.legend(header[1:])  # Exclude TimeStamp from the legend
        ax.set_xlabel('Time')
        ax.set_ylabel('Sensor Data')

        while True:
            if link.available():
                recSize = 0

                data[0] = link.rx_obj(obj_type='L', start_pos=recSize)
                recSize += txfer.STRUCT_FORMAT_LENGTHS['L']

                data[1] = link.rx_obj(obj_type='H', start_pos=recSize)
                recSize += txfer.STRUCT_FORMAT_LENGTHS['H']

                data[2] = link.rx_obj(obj_type='H', start_pos=recSize)
                recSize += txfer.STRUCT_FORMAT_LENGTHS['H']

                data[3] = link.rx_obj(obj_type='I', start_pos=recSize)
                recSize += txfer.STRUCT_FORMAT_LENGTHS['I']

                data[4] = link.rx_obj(obj_type='I', start_pos=recSize)
                recSize += txfer.STRUCT_FORMAT_LENGTHS['I']

                writer.writerow(data)
                df.loc[len(df)] = data[1:]  # Append the data to the DataFrame

                # Plot the data in real-time
                ax.clear()
                for i, line in enumerate(lines):
                    line.set_data(df['TimeStamp'], df[header[i+1]])
                ax.relim()
                ax.autoscale_view(True, True, True)
                fig.canvas.draw()

            elif link.status < 0:
                if link.status == txfer.CRC_ERROR:
                    print('ERROR: CRC_ERROR')
                elif link.status == txfer.PAYLOAD_ERROR:
                    print('ERROR: PAYLOAD_ERROR')
                elif link.status == txfer.STOP_BYTE_ERROR:
                    print('ERROR: STOP_BYTE_ERROR')
                else:
                    print('ERROR: {}'.format(link.status))

    except KeyboardInterrupt:
        link.close()
        output_file.close()
        print("Serial connection & output_file closed. \nDon't forget to rename SensorData to prevent overwriting!")

