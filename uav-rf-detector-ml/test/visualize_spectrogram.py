import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
from scipy.signal import spectrogram


# # Step 1: Create a sample signal (sine wave)
# sampling_rate = 1000  # Sampling rate in Hz
# T = 1.0 / sampling_rate  # Time interval between samples
# t = np.linspace(0.0, 1.0, sampling_rate)  # Time vector (1 second duration)
# freq1 = 50  # Frequency of the first sine wave (Hz)
# freq2 = 120  # Frequency of the second sine wave (Hz)
# signal = np.sin(2.0 * np.pi * freq1 * t) + 0.5 * np.sin(2.0 * np.pi * freq2 * t)

# Step 1: Read CSV file into a DataFrame
# Assuming the CSV has a column 'signal' with your signal data.
df = pd.read_csv('C:/Users/user/Downloads/Processed_CardRF/Processed_CardRF/Train/UAV_Controller/DJI_PHANTOM.csv', header=None)


# Step 2: Extract the signal from the DataFrame (modify 'signal' column if necessary)
#df = pd.to_numeric(df['DJI_PHANTOM'], errors='coerce')
#df = df.dropna(subset=['DJI_PHANTOM'])
df = df.iloc[:, :-3]

signal = df.values[:1024 * 100].flatten()


sample_rate = 1024
fft_signal = np.fft.fft(signal)
fft_freqs = np.fft.fftfreq(len(signal), 1/sample_rate)
print(fft_freqs)

# Step 3: Plot the spectrum (magnitude)
plt.figure(figsize=(10, 6))
plt.plot(fft_freqs[:len(fft_freqs) // 2], np.abs(fft_signal[:len(fft_signal) // 2]))  # Only plot the positive frequencies
plt.title("Frequency Spectrum")
plt.xlabel("Frequency (Hz)")
plt.ylabel("Magnitude")
plt.grid(True)
plt.show()

# Step 3: Generate the spectrogram
# frequencies, times, Sxx = spectrogram(signal, fs=1024)  # fs is the sample rate, adjust as needed

# Debug: Print the shapes of the arrays to verify they match
#print(f"frequencies shape: {frequencies.shape}")
#print(f"times shape: {times.shape}")
#print(f"Sxx shape: {Sxx.shape}")


# Step 4: Plot the spectrogram
# plt.pcolormesh(times, frequencies, 10 * np.log10(Sxx), shading='gouraud')
# plt.ylabel('Frequency [Hz]')
# plt.xlabel('Time [sec]')
# plt.title('Spectrogram of the Signal')
# plt.colorbar(label='Power/Frequency (dB/Hz)')
# plt.show()
# rng = np.random.default_rng()
# fs = 10e3
# N = 1e5
# amp = 2 * np.sqrt(2)
# noise_power = 0.01 * fs / 2
# time = np.arange(N) / float(fs)
# mod = 500*np.cos(2*np.pi*0.25*time)
# carrier = amp * np.sin(2*np.pi*3e3*time + mod)
# noise = rng.normal(scale=np.sqrt(noise_power), size=time.shape)
# noise *= np.exp(-time/5)
# x = carrier + noise

# print(x)
# #print(x)

# # Построение спектрограммы
# frequencies, times, Sxx = spectrogram(x, fs)

# # Визуализация спектрограммы
# plt.pcolormesh(times, frequencies, 10 * np.log10(Sxx))
# plt.ylabel('Частота [Гц]')
# plt.xlabel('Время [с]')
# plt.title('Спектрограмма')
# plt.colorbar(label='Мощность (дБ)')
# plt.show()