clc
clear all
close all

fi_1 = fopen('grammi_1_nosmoke.txt','r');
d = fscanf(fi_1, '%d');

t = d(1:2:end);
x = d(2:2:end);

figure(1);
plot(t, x);

figure(2);
W = 8;
N = W;
FW = W/4-1;
DET = [];
subplot(2,2,4);
plot(t, x);
xlim([t(1) t(end)]);
for i=1:length(x)-W-1
    subplot(2,2,1);
    plot(t(1+i:1+i+W), x(1+i:1+i+W));
    xlim([t(1+i) t(1+i+W)]);
    %ylim([- 10]);
    f = fftshift(fft(x(1+i:1+i+W), N))/N;
    subplot(2,2,3);
    plot(2/N:2/N:1-2/N, abs(f(length(f)/2+2:end)));
    ylim([0 10]);
    subplot(2,2,2);
    DET = [DET sum(abs(f(length(f)/2+2:length(f)/2+2+FW-1)))/FW];
    plot(DET);
    pause(0.01);
end
