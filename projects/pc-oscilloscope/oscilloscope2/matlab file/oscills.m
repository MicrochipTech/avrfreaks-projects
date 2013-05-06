s = serial('COM1');
set(s,'Parity','none','DataBits',8);
s.BaudRate=115200 ;
%s.bufferinput=3
s.inputbuffersize=100000;
get(s,'Baudrate');
get(s,'status');
fopen(s)
s.ReadAsyncMode = 'manual';
instrfind
a=0;

hold on;
for i=[1:1:1000]
    a(i)= fscanf(s,'%f');
    plot(a);
end
%a=7;
plot(a)

fclose(s)
delete(s)
clear s
