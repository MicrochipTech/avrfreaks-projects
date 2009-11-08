function [light,temp,volt,date,intv] = loggerplot(file)
%
%
%fd = serial('COM1','Baudrate',9600); fopen(fd);
fd = fopen(file);
shdr = fread(fd,1,'uint16'); % skip A5A5 header
date = fread(fd,6,'uint8'); %read start date; yy,mm,dy,hh,mm,ss
intv = fread(fd,1,'uint16'); %get log interval (seconds)
ehdr = fread(fd,1,'uint16'); % skip A5A5 header
hdr = hex2dec('a5a5'); % expected header delimiters
if (shdr ~= hdr || ehdr ~= hdr) 
    disp('*** Bad header ***'); 
end;

% load measurements (assumes only one dataset per file)
data = fread(fd,inf,'uint16');
last = find(data == 65535,1)-1;
disp([num2str(last),' data points read.']);
fclose(fd);

% parse measurements
light = data(1:3:last);
temp  = data(2:3:last);
volt  = data(3:3:last);

% convert temp to deg. F
tempF = raw2Fahrenheit(temp);

% Use time on X-axis
date = date' + [2000 0 0 0 0 0];
T = (1:length(tempF))*datenum(0,0,0,0,0,intv);
dnum = datenum(date) + T;
startDate = datestr(dnum(1)); disp(['Start: ',startDate]); 
endDate = datestr(dnum(end)); disp(['End:   ',endDate]);

s = sprintf('Log beginning on %02d/%02d/%02d at %02d:%02d:%02d',...
    date(2),date(3),date(1),date(4:6));

% find sunrise and sunset times
sunrise(light,dnum);
% find time of lowest temp.
lowTemp(tempF,dnum);

% plot temperature
figure(1);
clf;
plot(dnum,tempF);
set(gca,'Xtick',dnum(1):(1/24):dnum(end));
grid;
set(gca,'XMinorTick','on','YMinorTick','on');
datetick('x',15,'keepticks');
title(s);
xlabel('Time');
ylabel('Temperature, deg F');

% Plot light level
figure(2);
clf;
plot(dnum,(690-light)/6.9);
set(gca,'Xtick',dnum(1):(1/24):dnum(end));
grid;
set(gca,'XMinorTick','on','YMinorTick','on');
datetick('x',15,'keepticks');
title(s);
xlabel('Time');
ylabel('Light reading');

% % Plot voltage level
% figure(3);
% clf;
% plot(dnum,5*volt/1024);
% grid;
% datetick('x',15,'keepticks');
% title(s);
% xlabel('Time');
% ylabel('Volts');
% set(gca,'XMinorTick','on','YMinorTick','on');

end

function Tf = raw2Fahrenheit(X)
%
beta = 4250;
Tz = 273;
Ta = 298;
Tc = beta./(log(X./(1024-X)) + beta/Ta) - Tz;
Tf = 1.8*Tc+32; % deg C to deg F
return;
end

function [r s] = sunrise(light, dnum)
L1 = (light < 689);
L2 = (L1(1:end-1)-L1(2:end));
r = (L2 < 0);
r(end+1)=0;
s = (L2 > 0);
s(end+1)=0;

%sunrise and set times
n = sum(s);
disp('  Sunset                 Sunrise');
disp([datestr(dnum(s)),repmat(' ',n),datestr(dnum(r))]);

end

function [r s] = lowTemp(tempF, dnum)
dT = (dnum(end)-dnum(1));
per = ceil(dT);
stp = fix(length(tempF)/per);
dt = 1;
disp('  Temperature High/Lows');
for i=1:per
    j = (i-1)*stp;
    k = min(j+stp,length(tempF));
    j = j + dt;
    lowT = min(tempF(j:k));
    hiT  = max(tempF(j:k));
    L = find(tempF(j:k)==lowT,1)+j;
    H = find(tempF(j:k)==hiT,1)+j;
    disp([' Low of ',num2str(lowT),' on ',datestr(dnum(L))]);
    disp(['High of ',num2str(hiT),' on ',datestr(dnum(H))]);
    dt = 1000;
end

end