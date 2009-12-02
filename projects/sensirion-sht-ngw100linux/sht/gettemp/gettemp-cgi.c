// Userspace test program

#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <stdio.h>
#define SOFT_RESET 10
#define GET_TEMP 20
#define GET_HUMI 30
#define SHT_OPERATIONS 70
struct sht_operations {
        int action;
        unsigned int data;
};


int main() {
	int fd;
	int r;
	unsigned int data=0;
	const float C1=-4.0;       // for 12 Bit
	const float C2= 0.0405;    // for 12 Bit
	const float C3=-0.0000028; // for 12 Bit
	const float T1=0.01;       // for 14 Bit @ 5V
	const float T2=0.00008;    // for 14 Bit @ 5V
	float rh;      // rh:      Humidity [Ticks] 12 Bit
	float t;    // t:       Temperature [Ticks] 14 Bit
	float rh_lin;              // rh_lin:  Humidity linear
	float rh_true;             // rh_true: Temperature compensated humidity
	float t_C;                 // t_C   :  Temperature [ C]
	float t_F;                 // t_F   :  Temperature [ F]


	fd = open("/dev/sht", O_RDWR);
	if( fd == -1) {
		printf("open error...\n");
		exit(1);
	}

        struct sht_operations p = {GET_TEMP, data};
        r = ioctl(fd, SHT_OPERATIONS, &p);
        if(r < 0) {
                perror("ioctl");
                exit(1);
        }
//	printf("Temp data:%d\n", r);
	t=(float)r;

        struct sht_operations h = {GET_HUMI, data};
        r = ioctl(fd, SHT_OPERATIONS, &h);
        if(r < 0) {
                perror("ioctl");
                exit(1);
        }
//	printf("Humi data:%d\n", r);
        rh=(float)r;

	t_C=t*0.01 - 39.60;                  //calc. Temperature from ticks to [ C]
	t_F=t*0.018 - 39.28;
	rh_lin=C3*rh*rh + C2*rh + C1;     //calc. Humidity from ticks to [%RH]
	rh_true=(t_C-25)*(T1+T2*rh)+rh_lin;   //calc. Temperature compensated humidity [%RH]
	if(rh_true>100)rh_true=100;       //cut if the value is outside of
	if(rh_true<0.1)rh_true=0.1;       //the physical possible range

        printf("HTTP/1.0 200 OK\n");
        printf("Content-type: text/html\n\n");
        printf(
                        "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n"
                        "<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.1//EN\" \"http://www.w3.org/TR/xhtml11/DTD/xhtml11.dtd\">\n"
                        "<html xmlns=\"http://www.w3.org/1999/xhtml\">\n"
                        "<link rel=\"stylesheet\" type=\"text/css\" media=\"screen\" href=\"/webif.css\" />"
                        "<?xml-stylesheet href=\"/include/style.css\" type=\"text/css\" media=\"screen\" ?>"
                        "<head>\n"
                        "<meta http-equiv=\"refresh\" content=\"5; URL=/cgi-bin/gettemp.cgi\" />\n"
                        "<title>Humidity/Temperature</title>\n"
                        "</head>\n"
                        "<body style=\"margin: 30px; color: #C3C4D2; background-color: #213242;\">\n"
              );
        printf("<h1>Current Humidity and Temperature</h1>\n");
        printf("<p><a style=\"color: inherit;\" href=\"/\">Back to home...</a></p>\n");

        printf("<br />");
        printf("<p>");

		printf("Temp C:%f\n", t_C);
		printf("<br />");
		printf("Temp F:%f\n", t_F);
		printf("<br />");
//       printf("Humi:%f\n", rh_lin);
        printf("Humi:%f\n", rh_true);
        printf("<br />");

        printf("</p>\n");

        printf("<br />\n");


error:
        printf("</body></html>\n");


	close(fd);
	exit(0);
}

