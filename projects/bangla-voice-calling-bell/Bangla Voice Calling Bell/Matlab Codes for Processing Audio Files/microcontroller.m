function main
clc
clear all
% y1 = wavread('doyakore.wav');
% y1 = wavread('dohaiapnar.wav');
y1 = wavread('bhagen.wav');

Fsamp = 4E3;
y = resample(y1(:,1), Fsamp , 44.010E3); %take only one channel

% sound(y, Fsamp)
 size(y)
 y = y-min(y);
 y = y/max(y);
 y = y * 1000;
 
 
 
 steps = 8;
 ymin = min(y);
 ymax = max(y);
 
 for i = 1:length(y)
     z(i) = quantizer (y(i), steps, ymin, ymax);
 end
 
 z(find(z==max(z))) = z(find(z==max(z))) * .8;  
 z(find(z==max(z))) = z(find(z==max(z))) * .8;
 z = z/max(z);
 z = z - min(z);
 z = z/max(z) * (2^steps-1);
 z = floor(z);
 z(1:1000);
 
fid = fopen('newwavdata4.c', 'w');
disp('#include <avr/pgmspace.h>');
cs = sprintf('#define WAVDATALENGTH1 %d', length(z)); 
fprintf(fid, '%s\r\n', cs);
cs = sprintf('unsigned char wavdata[WAVDATALENGTH1] PROGMEM =', length(z)); 
fprintf(fid, '%s\r\n', cs);
fprintf(fid, '{\r\n');

disp(cs)
disp('{');

n = length(z);
each_time = 10;
ctr = 0;

while (n>each_time)
    n = n-each_time ;
    ss = '';
    for i = 1:each_time
        ctr = ctr+1;
        rrs =  z(ctr);%dec2hex(z(ctr));
            ss = sprintf('%s %d, ', ss, rrs);
    end
    fprintf(fid, '%s\r\n', ss);
    disp(ss)
end

for i = 1:n
   ctr = ctr+1;
   rrs = z(ctr); 
   if i == 1
        ss = sprintf(' %d', rrs);
   else
        ss = sprintf('%s, %d', ss, rrs);
   end
end
disp(ss)
fprintf(fid, '%s\r\n', ss);
disp('};');
fprintf(fid, '};\r\n');
%  disp('Howdy')
   sound(z/max(z) - .5, Fsamp)
   plot(z/max(z) - .5)
fclose(fid)
end


function y = quantizer (x, steps, ymin, ymax)
    
    %ymax = 3;
    %ymin = -3;
    delta = (ymax-ymin)/(2^steps - 1);
    y=ymin;
    for i = 0:2^steps-1
        if (x < (ymin + i*delta))
            l = (ymin + (i-1)*delta);
            u = (ymin + i*delta);
            a = (l+u)/2;
            if (x>a) 
                y = u;
            else 
                y = l;
            end
            break;
        end
    end
    
end