import h5py
import matplotlib.pyplot as plt
import numpy as np

file_path = "C:/Users/user/Downloads/Compressed/MAVICPRO_10M_FLYING00030.mat"
rf_signal = None

with h5py.File(file_path, 'r') as f:
    # Check if Channel_1 is a group and explore its contents
    if isinstance(f['Channel_1'], h5py.Group):
        print("Channel_1 is a group. It contains:")
        for key in f['Channel_1'].keys():
            print(f" - {key}: Shape = {f['Channel_1'][key].shape}, Data type = {f['Channel_1'][key].dtype}")
    else:
        print("Channel_1 is a dataset.")
    
    # Access the RF signal data
    rf_signal = f['Channel_1/Data'][0]  # Use [0] to extract the 1D array

sample_rate = 20e9  # 20 GHz
# Perform Fourier Transform
fft_result = np.fft.fft(rf_signal)
fft_magnitude = np.abs(fft_result)  # Get the magnitude
freqs = np.fft.fftfreq(len(rf_signal), d=1/sample_rate)

# Only take the positive frequencies
positive_freqs = freqs[:len(freqs)//2] / 1e9
positive_magnitude = fft_magnitude[:len(freqs)//2]

plt.plot(positive_freqs, positive_magnitude)
plt.title("RF Signal Spectrum")
plt.xlabel("Frequency (GHz)")
plt.ylabel("Magnitude")
plt.show()