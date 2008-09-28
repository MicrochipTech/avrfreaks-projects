function main
 clc
 t = 0:100;
 y = sin(2*pi*t/100)
 size(y)
 y = y-min(y);
 y = y/max(y);
 y1 = [0];
 for i = 1:100
   y1 = [y1 y/max(y)];
 end

 y = y * 1000;
 
 
 
 steps = 8;
 ymin = min(y);
 ymax = max(y);
 
 for i = 1:length(y)
     z(i) = quantizer (y(i), steps, ymin, ymax);
 end
 
 z = z - min(z);
 z = z/max(z) * (2^steps-1);
 z = floor(z);

 
fid = fopen('sindata.c', 'w');
disp('#include <avr/pgmspace.h>');
cs = sprintf('unsigned char sindata[%d] PROGMEM =', length(z)); 
fprintf(fid, '%s\r\n', cs);
fprintf(fid, '{\r\n');

disp(cs)
disp('{');

n = length(z);
each_time = 10;
ctr = 0;

while (n>each_time)
    n = n-each_time ;
    for i = 1:each_time
        ctr = ctr+1;
        if i == 1
            ss = sprintf('%d ', (z(ctr)));
            
        else
            ss = sprintf('%s, %d', ss, (z(ctr)));
        end
    end
    fprintf(fid, '%s,\r\n', ss);
    disp(ss)
end

for i = 1:n
   ctr = ctr+1;
   if i == 1
        ss = sprintf('%d', z(ctr));
   else
        ss = sprintf('%s, %d', ss, z(ctr));
   end
end
disp(ss)
fprintf(fid, '%s,\r\n', ss);
disp('}');
fprintf(fid, '};\r\n');
%  disp('Howdy')
y = [0]
for i = 1:1000
   y = [y z/max(z)];
end
%       sound(y, 50000)
y
fclose(fid)
stem(z, '.')
end


function y = quantizer (x, steps, ymin, ymax)
    
    %ymax = 3;
    %ymin = -3;
    delta = (ymax-ymin)/(2^steps-1);
    y=ymin;
    for i = 0:2^steps-1
        if (x <= (ymin + i*delta))
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