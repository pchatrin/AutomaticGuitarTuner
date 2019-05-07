[audio_in,audio_freq_sampl]=audioread('B1.m4a');
Length_audio=length(audio_in);
df=audio_freq_sampl/Length_audio;
frequency_audio=-audio_freq_sampl/2:df:audio_freq_sampl/2-df;
figure
FFT_audio_in=fftshift(fft(audio_in))/length(fft(audio_in));
plot(frequency_audio,abs(FFT_audio_in));
title('FFT of Input Audio');
xlabel('Frequency(Hz)');
ylabel('Amplitude');

[M,I] = max(abs(FFT_audio_in));
frequency_audio(I) 