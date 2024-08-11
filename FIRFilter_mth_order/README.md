## Results:

https://github.com/user-attachments/assets/41df8d0a-4ff4-4dcc-8f6f-39a0ce30f3c7



## Matlab/Octave code finding FIR coefficient:
Change ' N ' (filter order/length) to get respective FIR coefficients for required size:
```octave
clc
clear
close all

N = 5      % Filter order/length (odd number)
fs = 1000;   % sampling frequency
To = 1/fs;
wc = 200;   % Cuttoff frequency
n = 0:N-1;   % Time index

% Ideal impulse response
hd = (wc * To) * sinc( wc * (n-(N-1)/2) * To) % FIR filter coefficients
w = hamming(N)'; % Hamming window
h = hd .* w     % smoothed FIR filter coefficients

% Plot the filter coefficients
figure;
subplot(3,1,1);
stem(0:N-1, h);
title('FIR Filter Coefficients');
xlabel('n');
ylabel('h[n]');

% Calculate the frequency response
[H, f] = freqz(h, 1, 1024);

% Plot the magnitude response
subplot(3,1,2);
plot(f/pi, abs(H));
title('Magnitude Response of FIR Filter');
xlabel('Normalized Frequency (\times \pi rad/sample)');
ylabel('Magnitude');
grid on;


H_dB = 20 * log10(abs(H));
subplot(3,1,3);
plot(f/pi, H_dB);
title('Magnitude Response of FIR Filter (in dB)');
xlabel('Normalized Frequency (\times \pi rad/sample)');
ylabel('Magnitude (dB)');
grid on;

pause;

``` 

# Theory behind designing an FIR Filter:
## Steps:
 1. Find infinite impulse response $g_{d}[k]$ usinginverse fourier transform for an FIR filter with desired frequency response $G_{D(iw)}$ :
 
 ```math
g_{d}[k] = \frac{1}{2\pi}\int^{+\omega_{0}/2}_{-\omega_{0}/2} G_{D}(iw).e^{i\omega T_{o}k}d\omega
```

2. Use desired frequency response $G_{D(iw)}$ (e.g. for an ideal low-pass filter):
![image](https://github.com/user-attachments/assets/05de75f7-ec59-4e12-9189-4ee8f1359de7)

 ```math

g_{d}[k] = \frac{1}{2\pi}\int^{+\omega_{e}}_{-\omega_{e}} 1.e^{i\omega T_{o}k}d\omega
 ```

 ```math

g_{d}[k] = \frac{\sin(\omega_{e}kT_{0})}{\pi kT_{0}}
 ```
or
 ```math

g_{d}[k] = \frac{\omega_{e}}{\pi}sinc(\omega_{e}kT_{0})
 ```
If  $f_{e} = \frac{\omega_{e}}{2\pi}$, then
 ```math
g_{d}[k] = 2f_{e} *sinc\left(2 \pi f_{e}T_0k\right)
 ```
Let $n$ = $[0, 1, ...., N-1]$ then $k$ = $\left( n-\frac{N-1}{2} \right)$,
if $N$ is odd:
 ```math
g_{d}[n] = 2f_{e}  T_{0}*sinc\left(f_{e} T_{0} \left[n-\frac{N-1}{2} \right]\right)
 ```
if $N$ is even:
 ```math
g_{d}[n] = 2f_{e}  T_{0}*sinc\left(f_{e} T_{0} \left[n-\frac{N}{2} \right]\right) \qquad for \quad n\neq \frac{N}{2}
 ```
and,
 ```math
	g_{d}[n] = 2*f_{e}T_{0} \qquad for \quad n = \frac{N}{2}
 ```
where, 
	$sinc = \frac{\sin(x)}{x}$
	$N$ = length/order of the filter
	$f_{e}$ = cut-off freq.
	$f_{samp}$ = sampling frequency
![image](https://github.com/user-attachments/assets/baf406b4-56b2-43b1-ab04-30b1cec5d546)


Similarly, for other types:
![image](https://github.com/user-attachments/assets/c6cb8649-fb5e-4fdf-87d8-bacbe7b34fea)


3. Reduce the error by truncating the infinite impulse response $g_{d}[k]$ by windowing(e.g. hamming window $w[n]$):
 ```math
g[n] = g_{d}[n].w[n]
 ```
where, 
$g[k]$ = coefficients of the FIR filter

ref: https://www.staff.ncl.ac.uk/oliver.hinton/eee305/Chapter4.pdf

