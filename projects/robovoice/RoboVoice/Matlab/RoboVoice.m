
function dummy

clc;
Fs=8000;
%x=sin(2*pi*Fc*[1:N]/Fs);
[x,Fswav]=wavread('holly.wav');
N=length(x);
x=resample(x,Fs,Fswav);
x=x';
% wavplay(x,Fs);
% figure(1);sigspec(x);

%z=echoo(x,1000,0.5);
%z=reverb2(x,100,0.9);
%z=flange(Fs,0.005,x,0.1);%0.05, 0.05
z=vibrato(Fs,0.005,x,0.1);
%z=SquareRingModulator(x,Fs,1000);
%z=ringModulator(x,Fs,390);
soundsc(z,Fs);
clear;

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
function [y]=vibrato(fs, v, y, r)
%[y] = vibrato(fs, v, y, r)
%z=vibrato(Fs,0.005,x,0.1);     %Mad fool
%z=vibrato(Fs,0.005,x,0.05);
%z=vibrato(Fs,0.005,x,0.2);     %Sad, almost weening sound
%z=vibrato(Fs,0.010,x,0.2);
%z=vibrato(Fs,0.020,x,0.2);     %Strange accent

%
%   This is a basic vibrato effect.
%
%      fs = Sample rate
%      v = Variation. Depth [s] from 0s to 0.015s (amplitude of timedelay)
%      y = Input audio signal. This should be a row vector.
%      r = Rate. [s] (1/frequency of time delay)
%
%   Example:
%     
%      >>y = flange(fs,0.002,x,0.5);
%
%
%   See also WAVREAD and SOUND
%
%Version 1.0
%Coded by: Stephen G. McGovern, date: 08.03.03
%Nice effect to be expected when v is equal to r
md= ceil(v*fs);     %md = depth of delay in nummber of samples (peak to peak)
n=1:length(y)+md;
v=round(v*fs);
z=zeros(1,md);
m=max(abs(y));
y=[z y z];
rr=2*pi/round(fs*r);%calculate rate of change to mix the samples
b=round((v/2)*(1-cos(rr.*n))); %1-cos, because we always take past samples, not future ones.
y=y(n+md-b);
m=m/max(abs(y));
y=m*y;
plot(n/fs,b/fs);
ylabel('Time varying function [s]');
xlabel('Time of sound sample [s]');






function z=echoo(x,delay, factor)
%Echo
% delay=1000;
% factor=.95;
z=[x zeros(1,delay)] +factor*[zeros(1,delay) x];


function z=reverb(x,delay,factor)
%Reverb
%delay=100, factor=0.9 for robot voice

z=[x zeros(1,delay)];
for i=1+delay:length(x)+delay;
    z(i)=z(i)+z(i-delay)*factor;
end

function z=reverb2(x,delay,factor)
%Reverb
%delay=100, factor=0.9 for robot voice

z=[x zeros(1,delay)];
for i=1+delay:length(x)+delay;
    z(i)=x(i-delay)+factor*z(i-delay);
end


% %Chorus
% % Fc=0.1;
% % factor=0.8;
% % factorD=200;
% % sum=200;
% % delay=floor(factorD*(cos(2*pi*Fc/Fs*[1:length(x)+factorD]))+sum);
% % z=zeros(1,length(delay));
% % for i=1:length(x);
% %     if i-delay(i)>0 & i-delay(i)<length(x)
% %         z(i)=x(i)+factor*x(i-delay(i));
% %     end
% % end
% 
% 

function [y]=flange(fs, v, y, r)
%[y] = flange(fs, v, y, r)
%
%   This is a basic flanging effect.
%
%      fs = Sample rate
%      v = Variation. Depth [s] from 0s to 0.015s (amplitude of timedelay)
%      y = Input audio signal. This should be a row vector.
%      r = Rate. [s] (frequency of time delay)
%
%   Example:
%     
%      >>y = flange(fs,0.002,x,0.5);
%
%
%   See also WAVREAD and SOUND
%
%Version 1.0
%Coded by: Stephen G. McGovern, date: 08.03.03
%Nice effect to be expected when v is equal to r
md= ceil(v*fs);
n=1:length(y)+md;
v=round(v*fs);
z=zeros(1,md);
m=max(abs(y));
y=[z y z];
rr=2*pi/round(fs*r);
b=round((v/2)*(1-cos(rr.*n)));
y=y(n+md)+y(n+md-b);
m=m/max(abs(y));
y=m*y;
% plot(n/fs,b/fs);
% ylabel('Time varying function [s]');
% xlabel('Time of sound sample [s]');
% 

function [y]=SquareRingModulator(x,Fs,f)
%This function implements a ring modulator with a squarewave instead of a cosine function.
%y:     Output sample sequence
%x:     Input sample sequence
%Fs:    Sample frequency [Hz]
%f:     Frequency of the modulating squarewave.
%
%   Example: z=roboVoice(x,Fs,80);
%
if f>Fs/2 
    error('f must be smaller than Fs/2!');
end
len=length(x);
y=zeros(1,len);
for i=0:Fs/f:len-Fs/2/f
    for j=1:Fs/2/f
        y(round(i+j))=1;
    end
end
%plot(y(1:200))
y=y.*x;

function [y]=ringModulator(x,Fs,f)
%Ring modulator
y=cos(2*pi*f/Fs*[1:length(x)]);
y=y.*x;
