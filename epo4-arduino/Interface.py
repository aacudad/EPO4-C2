import pandas as pd
import ImportECG        #Import sensor data
import plotData         #Plotting retreived data
import sys, getopt      #Command line arguments



def main(argv):
   try:
      opts, args = getopt.getopt(argv,"hi:p:",["help","import=","plot="])
   except getopt.GetoptError:
      print ('Interface.py [-h] or [--help] for more information')
      sys.exit(2)
    
   for opt, arg in opts:
      if (opt == '-h' or opt == '--help'):
         print('The following additional arguments can be passed:')
         print('-i <COM-port> or --import <COM-port>                only import ECG, GSR and respiratory data to ECGdata.csv')
         print('-p <input_file.csv> or --plot <inputfile.csv>       only plot the data currently saved in input_file.csv')
         sys.exit()
      elif opt in ("-i", "--import"):
         ImportECG.import_all(arg)
      elif opt in ("-p", "--plot"):
         if(".csv" in arg):
            plotData.plot_data(arg)
         else:
            print('Inputfile must be of type .csv')
      else:
         print('No valid options and arguments passed')
         assert False, "unhandled option"
        



if __name__ == "__main__":
   main(sys.argv[1:])
