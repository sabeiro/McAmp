#!/usr/bin/env python
#coding: utf-8
from matplotlib import pyplot as plt
from numpy.lib import stride_tricks
import numpy as np
import pylab
import wave
import array
from scipy.io import wavfile
import scipy.io.wavfile as wav
import matplotlib.pyplot as plt

f = 10.
w = 2. * np.pi * f
time_interval = 100
samples = 5000
t = np.linspace(0, time_interval, samples)
y1 = np.sin(w * t )
y2 = np.sin(2. * w * t)

y1c = np.array(y1[:])
y2c = np.array(y2[:])

yc = np.concatenate([y1c,y2c])

Pxx, freqs, t, plot = pylab.specgram(
    yc,
    NFFT=128, 
    Fs=44100, 
    detrend=pylab.detrend_none,
    window=pylab.window_hanning,
    noverlap=int(128 * 0.5))

pylab.show()

pylab.close('all')
w1 = wave.open('Lodrone.wav')
w2 = wave.open('LodroneCorto.wav')
# hmmm, probably a better way to do this, scipy.io function?
x1 = np.array(array.array('h', w1.readframes(w1.getnframes())))
x2 = np.array(array.array('h', w2.readframes(w2.getnframes())))
x1 = x1 / (2.**(16-1))  # normalize
x2 = x2 / (2.**(16-1))  # normalize
Fs = 16000.
NFFT = int(Fs*0.005)  # 5ms window
noverlap = int(Fs*0.0025)
pylab.figure(1)
pylab.specgram(x1, NFFT=NFFT, Fs=Fs, noverlap=noverlap, 
               cmap=pylab.get_cmap('Greys'))
pylab.title('Full 1m15s example min noise')
pylab.figure(2)
pylab.specgram(x2, NFFT=NFFT, Fs=Fs, noverlap=noverlap, 
               cmap=pylab.get_cmap('Greys'))
pylab.title('Full 1m15s example more noise')
pylab.figure(3); n=2100*176;
pylab.specgram(x2[n:n+256*256], NFFT=NFFT, Fs=Fs, noverlap=noverlap, 
               cmap=pylab.get_cmap('Greys'))
pylab.title('Full ~4s example min noise')
pylab.figure(4); pylab.plot(x1[n:n+256*256])
pylab.show()


fs, frames = wavfile.read("Lodrone.wav")

channels = [
    np.array(frames[:, 0]),
    np.array(frames[:, 1])
]

# generate specgram
Pxx, freqs, t, plot = pylab.specgram(
    channels[0],
    NFFT=4096, 
    Fs=44100, 
    detrend=pylab.detrend_none,
    window=pylab.window_hanning,
    noverlap=int(4096 * 0.5))
pylab.show()



""" short time fourier transform of audio signal """
def stft(sig, frameSize, overlapFac=0.5, window=np.hanning):
    win = window(frameSize)
    hopSize = int(frameSize - np.floor(overlapFac * frameSize))
    # zeros at beginning (thus center of 1st window should be for sample nr. 0)
    samples = np.append(np.zeros(np.floor(frameSize/2.0)), sig)    
    # cols for windowing
    cols = np.ceil( (len(samples) - frameSize) / float(hopSize)) + 1
    # zeros at end (thus samples can be fully covered by frames)
    samples = np.append(samples, np.zeros(frameSize))
    
    frames = stride_tricks.as_strided(samples, shape=(cols, frameSize), strides=(samples.strides[0]*hopSize, samples.strides[0])).copy()
    frames *= win
    
    return np.fft.rfft(frames)    
    
""" scale frequency axis logarithmically """    
def logscale_spec(spec, sr=44100, factor=20.):
    timebins, freqbins = np.shape(spec)

    scale = np.linspace(0, 1, freqbins) ** factor
    scale *= (freqbins-1)/max(scale)
    scale = np.unique(np.round(scale))
    
    # create spectrogram with new freq bins
    newspec = np.complex128(np.zeros([timebins, len(scale)]))
    for i in range(0, len(scale)):
        if i == len(scale)-1:
            newspec[:,i] = np.sum(spec[:,scale[i]:], axis=1)
        else:        
            newspec[:,i] = np.sum(spec[:,scale[i]:scale[i+1]], axis=1)
    
    # list center freq of bins
    allfreqs = np.abs(np.fft.fftfreq(freqbins*2, 1./sr)[:freqbins+1])
    freqs = []
    for i in range(0, len(scale)):
        if i == len(scale)-1:
            freqs += [np.mean(allfreqs[scale[i]:])]
        else:
            freqs += [np.mean(allfreqs[scale[i]:scale[i+1]])]
    
    return newspec, freqs

""" plot spectrogram"""
def plotstft(audiopath, binsize=2**10, plotpath=None, colormap="jet"):
    samplerate, samples = wav.read(audiopath)
    s = stft(samples, binsize)
    
    sshow, freq = logscale_spec(s, factor=1.0, sr=samplerate)
    ims = 20.*np.log10(np.abs(sshow)/10e-6) # amplitude to decibel
    
    timebins, freqbins = np.shape(ims)
    
    plt.figure(figsize=(15, 7.5))
    plt.imshow(np.transpose(ims), origin="lower", aspect="auto", cmap=colormap, interpolation="none")
    plt.colorbar()

    plt.xlabel("time (s)")
    plt.ylabel("frequency (hz)")
    plt.xlim([0, timebins-1])
    plt.ylim([0, freqbins])

    xlocs = np.float32(np.linspace(0, timebins-1, 5))
    plt.xticks(xlocs, ["%.02f" % l for l in ((xlocs*len(samples)/timebins)+(0.5*binsize))/samplerate])
    ylocs = np.int16(np.round(np.linspace(0, freqbins-1, 10)))
    plt.yticks(ylocs, ["%.02f" % freq[i] for i in ylocs])
    
    if plotpath:
        plt.savefig(plotpath, bbox_inches="tight")
    else:
        plt.show()
        
    plt.clf()

plotstft("Lodrone.wav")
