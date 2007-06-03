# my designed  program

Uploaded by junzhesun on 2007-06-03 02:08:00 (rating 0 out of 5)

## Summary

/********毕业设计---基于ATmega32l单片机的高压防暴电机控制器的电路程序设计********/


//*****设计学生：ZB电气051班 孙俊哲


//**************编译环境AVR STUDIO 编程语言GCC


//***时间：2007年4月  

#include  

#include  

#include  

#include  

#include  

#include  

#define uchar unsigned char  

#define uint unsigned int  

#define FREQ 8 /***系统时钟 单位MHZ **/  

#define Vadd pc0 /******定义参数修改按键包括：电压，电流，功率，极对数******/  

#define Vsub pc1  

#define Iadd pc2  

#define Isub pc3  

#define Wadd pc4  

#define Wsub pc5  

#define Padd pc6  

#define Psub pc7  

uchar tempL,tempH;  

uchar flag1=1; /**启动电机时标志位*/  

uchar flag2=1; /**停电机的标志位*/ 


uchar an[5]={0,0,0,0,0}; /******************定义显示缓冲区***********/  

uchar bn[5]={0,0,0,0,0};  

uchar pn[5]={0,0,0,0,0};  

uchar sp[5]={0,0,0,0,0};  

uchar qn[4]={0,0,0,0};


static uchar g\_1AdValue=0; //检测启动电压  

static uchar g\_2AdValue=0; //检测线电压  

static uchar g\_3AdValue=0; //检测相电压  

static uchar g\_4AdValue=0; //线电流  

static uchar g\_5AdValue=0; //相电流


// **********进入程序区****


//延时子程序毫秒级  

void DelayMs(uint ms)  

{ uint i;  

 for(i=0;i
 for(j=0;j<10;j++)  

 { ;  

 }


 }


//十六进制转十进制程序  

 Change(uchar *cp)  

{  

 uint val;


 val = 0;  

 while (*cp)  

 { cp=cp+2; /***************去掉‘0X’字符**************************/


 if (*cp>= 'a' && *cp<= 'f')  

 val = val * 16 + *cp - 'a' + 10;  

 else if (*cp>= 'A' && *cp<= 'F')  

 val = val * 16 + *cp- 'A' + 10;  

 else if (*cp>= '0' && *cp<= '9')  

 val = val * 16 + *cp- '0';  

 else  

 break;  

 cp++;  

 }  

 return val;


 }


//T1/C1的捕获中断  

SIGNAL(SIG\_INPUT\_CAPTURE1)  

{  

 TCNT1H=0;  

 TCNT1L=0; //计数器清零,以便记录下一次脉冲(写16位寄存器时先写高8位/后写低8位  

 tempL=ICR1L;  

 tempH=ICR1H; //记录16位的捕捉数据 (读16位寄存器时先读低8位/后读高8位)  

 ICR1H=0;  

 ICR1L=0; //清空捕捉数据


 }


//外部中断0  

SIGNAL(SIG\_INTERRUPT0)  

 {  

 flag2=0;


 }  

//检测控制器工作电压ADC中断


 SIGNAL(SIG\_ADC)  

 { g\_1AdValue=ADCL;  

 g\_1AdValue=(uint)(ADCH<<8); /******右对齐****/  

 if(g\_1AdValue>=0x1cc&&g\_1AdValue<=0x3aa) /***在电压2.7-5.5伏范围内***/  

 { flag1=0;  

 ADIE=0;  

 }


 }


 // 显示子程序  

 // 初始化：写指令到MC14489


void WCdisplay(uint comm)  

 {  

 int i;  

 int x;  

 PORTB&=~\_BV(PB2); /********片选信号置底**/  

 PORTB&=~\_BV(PB1); /***CLOCK信号置底**/  

 for(i=0;i<8;i++)  

 { x=comm&0x80;  

 if(x==0)  

 PORTB&=~\_BV(PB3);  

 else  

 PORTB|=\_BV(PB3);  

 PORTB|=\_BV(PB1); /***CLOCK信号置高***/  

 comm=comm<<1;  

 PORTB&=~\_BV(PB1); /***CLOCK信号置底***/


 }  

 PORTB|=\_BV(PB2); /****片选信号置高****/  

 }


//*写数据到MC14489  

void WDdisplay(uint *disdata1,uint *disdata2,uint *disdata3,uint *disdata4,uint *disdata5)  

 {  

 uint mid;  

 int i,j;  

 int x;  

 PORTB&=~\_BV(PB2); /********片选信号置底**/  

 PORTB&=~\_BV(PB1); /***CLOCK信号置底**/  

 for(j=0;j<5;j++)  

 {  

 mid=disdata1[j];  

 for(i=0;i<8;i++)  

 {  

 x=mid&0x80;  

 if(x==0)  

 PORTB&=~\_BV(PB3);  

 else  

 PORTB|=\_BV(PB3);  

 PORTB|=\_BV(PB1); /***CLOCK信号置高***/  

 mid=mid<<1;  

 PORTB&=~\_BV(PB1); /***CLOCK信号置底***/  

 }


 }  

 PORTB&=~\_BV(PB1); /***CLOCK信号置底**/  

 for(j=0;j<5;j++)  

 {  

 mid=disdata2[j];  

 for(i=0;i<8;i++)  

 {  

 x=mid&0x80;  

 if(x==0)  

 PORTB&=~\_BV(PB3);  

 else  

 PORTB|=\_BV(PB3);  

 PORTB|=\_BV(PB1); /***CLOCK信号置高***/  

 mid=mid<<1;  

 PORTB&=~\_BV(PB1); /***CLOCK信号置底***/


 }  

 }  

 PORTB&=~\_BV(PB1); /***CLOCK信号置底**/  

 for(j=0;j<5;j++)  

 {  

 mid=disdata3[j];  

 for(i=0;i<8;i++)  

 {  

 x=mid&0x80;  

 if(x==0)  

 PORTB&=~\_BV(PB3);  

 else  

 PORTB|=\_BV(PB3);  

 PORTB|=\_BV(PB1); /***CLOCK信号置高***/  

 mid=mid<<1;  

 PORTB&=~\_BV(PB1); /***CLOCK信号置底***/


 }  

 }  

 PORTB&=~\_BV(PB1); /***CLOCK信号置底**/  

 for(j=0;j<5;j++)  

 {  

 mid=disdata4[j];  

 for(i=0;i<8;i++)  

 {  

 x=mid&0x80;  

 if(x==0)  

 PORTB&=~\_BV(PB3);  

 else  

 PORTB|=\_BV(PB3);  

 PORTB|=\_BV(PB1); /***CLOCK信号置高***/  

 mid=mid<<1;  

 PORTB&=~\_BV(PB1); /***CLOCK信号置底***/  

 }


 }  

 PORTB&=~\_BV(PB1); /***CLOCK信号置底**/  

 for(j=0;j<5;j++)  

 {  

 mid=disdata5[j];  

 for(i=0;i<8;i++)  

 {  

 x=mid&0x80;  

 if(x==0)  

 PORTB&=~\_BV(PB3);  

 else  

 PORTB|=\_BV(PB3);  

 PORTB|=\_BV(PB1); /***CLOCK信号置高***/  

 mid=mid<<1;  

 PORTB&=~\_BV(PB1); /***CLOCK信号置底***/


 } 


 }  

 PORTB|=\_BV(PB2); /****片选信号置高****/  

 }


//判断是否有键按下，有键安下返回1


 uchar KEYPRESSED(void)  

 {  

 uchar key;  

 key=PINC;  

 key=key|0x00;  

 if(key!=0x00)  

 return 1;


 }  

 /*************************************************************************************************************************************************************/ 


/**************************************************主*函*数******************************************************************************************************/  

void main(void)  

 {  

 uchar key=0,volt=300,current=30,power=50,pole=2;  

 unsigned long data;  

 uint a,b,c,d,s,p,q,speed; /**转换成十进制的数值包括，电压，功率等**/  

 uint a1,a2,a3,c1,c2,b1,b2,b3,d1,d2,p1,p2,s1,s2,sp1,sp2,sp3,sp4,q1,q2,q3;  

 float factor;  

 PORTA=0; /*******定义PA口为输入状态，无上拉****/  

 DDRA=0;  

 PORTB=0x00; /***定义PB口为输出口********/  

 DDRB=0xff;  

 PORTC=0; /**********定义PC口为键盘输入口，无上拉***************/  

 DDRC=0;  

 PORTD&=~\_BV(PD2); /*********使能外部中断INT0****/  

 DDRD=0;  

 GICR=(1<<INT0);  

 MCUCR=((1<<ISC01)|(1<<ISC01)); /********上升沿触发****/  

 TCCR1B=0xc5; //T/C1=1024分频,上升沿触发捕捉,启动噪声抑制功能  

 TIMSK|=1<<5; //捕捉中断使能


 ADMUX=0x40; /************PA0通道，内部AVCC 5V参考电压*******/  

 ADCSRA=\_BV(ADEN)|\_BV(ADIE)|\_BV(ADIF)|\_BV(ADPS1)|\_BV(ADPS2|\_BV(ADATE)); /***使能中断允许，自由模式，64分频*********/  

 ADCSRA|=\_BV(ADSC); /**自由模式开始转换*********/  

 sei();  

 do {


 } while(flag1);  

 if(g\_1AdValue>=0x1cc&&g\_1AdValue<=0x3aa) /***在电压2.7-5.5伏范围内***/  

 {  

 PORTB|=\_BV(PB0); /*********如果电压符合要求，则启动电机***/  

 DelayMs(10);  

 PORTB&=~\_BV(PB0);


 }  

 do { // 假设在硬件电路中，电压，电流都是按一定比例转换的  

 //检测线电压  

 ADMUX=0xc1; /************PA1通道，6v参考电压*******/  

 ADCSRA=\_BV(ADEN)|\_BV(ADIE)|\_BV(ADIF)|\_BV(ADPS1)|\_BV(ADPS2); /***使能中断允许，自由模式，64分频*********/  

 ADCSRA|=\_BV(ADSC); /**自由模式开始转换*********/  

 DelayMs(10);  

 g\_2AdValue=ADCL;  

 g\_2AdValue=(uint)(ADCH<<8);  

 a=Change((uchar)g\_2AdValue);  

 a=a*(volt/6); /**转换成实际的电压值**/  

 a3=a%10;  

 a2=a/10%10;  

 a1=a/100;


 //检测相电压  

 ADMUX=0xc2; /************PA2通道，6v参考电压*******/  

 ADCSRA=\_BV(ADEN)|\_BV(ADIE)|\_BV(ADIF)|\_BV(ADPS1)|\_BV(ADPS2); /***使能中断允许，自由模式，64分频*********/  

 ADCSRA|=\_BV(ADSC); /**自由模式开始转换*********/  

 DelayMs(10);  

 g\_3AdValue=ADCL;  

 g\_3AdValue=(uint)(ADCH<<8);  

 b=Change((uchar)g\_3AdValue);  

 b=b*(volt/10); /**线电压是相电压的1.732倍**/  

 b3=a%10;  

 b2=a/10%10;  

 b1=a/100; 


 //检测线电流  

 ADMUX=0xc3; /************PA3通道，6v参考电压*******/  

 ADCSRA=\_BV(ADEN)|\_BV(ADIE)|\_BV(ADIF)|\_BV(ADPS1)|\_BV(ADPS2); /***使能中断允许，自由模式，64分频*********/  

 ADCSRA|=\_BV(ADSC); /**自由模式开始转换*********/  

 DelayMs(10);  

 g\_4AdValue=ADCL;  

 g\_4AdValue=(uint)(ADCH<<8);  

 c=Change((uchar)g\_4AdValue);  

 c=c*(current/6); /**得到实际电流值**/  

 c2=c%10;  

 c1=c/10;


//检测相电流  

 ADMUX=0xc4; /************PA4通道，6v参考电压*******/  

 ADCSRA=\_BV(ADEN)|\_BV(ADIE)|\_BV(ADIF)|\_BV(ADPS1)|\_BV(ADPS2); /***使能中断允许，自由模式，64分频*********/  

 ADCSRA|=\_BV(ADSC); /**自由模式开始转换*********/  

 DelayMs(10);  

 g\_5AdValue=ADCL;  

 g\_5AdValue=(uint)(ADCH<<8);  

 d=Change((uchar)g\_5AdValue);  

 d=d*(current/10); /**得到实际电流值**/  

 d2=d%10;  

 d1=d/10;


//检测速度  

 data=tempH*256+tempL; //读取最近的捕捉时间(计数器的值)  

 speed=8000000/1024 *60/data; //每分钟的转数；在8M的时钟下，单片机每分钟将计数468750次，假设计数到46875时来了一个外部脉冲，那么单片机就会把这个这个46875数据记录下来，那么468750/46875＝10，就是说单片机计数到468750时，将会有10个脉冲出现，所以，频率就＝10次/分钟  

 sp4=speed%10;  

 sp3=speed/10%10;  

 sp2=speed/100%10;  

 sp1=speed/1000;


//视在功率,有功功率，无功功率,功率因数计算  

 factor=power/(volt*current);  

 p=3*b*d*factor;  

 q=3*b*d*sin(acos(factor));  

 s=3*b*d;  

 p1=p%10;  

 p2=p/10;  

 q1=q%10;  

 q2=q/10;  

 s1=s%10;  

 s2=s/10;  

//显示电机的各项参数  

 an[0]=a1,an[1]=a2,an[2]=a3,an[3]=c1,an[4]=c2;  

 bn[0]=b1,bn[1]=b2,bn[2]=b3,bn[3]=d1,bn[4]=d2;  

 pn[0]=p1,pn[1]=p2,pn[2]=s1,pn[3]=s2,pn[4]=pole;  

 sp[0]=sp1,sp[1]=sp2,sp[2]=sp3,sp[3]=sp4,sp[4]=q1;  

 qn[0]=q2,qn[1]=q3;


 WCdisplay(0x0ff);  

 WDdisplay(an,bn,pn,sp,qn); 


//高电压故障报警，低电压故障预警，高电流故障预警  

 if((a>380&&a<200)||(c>50&&c<15))  

 PORTB|=\_BV(PB4); //用PB4口输出高电平进行报警  

 DelayMs(1000); //延时1秒  

 PORTB&=~\_BV(PB4);  

//参数设置  

 if(KEYPRESSED()==1)  

 {  

 DelayMs(10); /*************延时消抖***/  

 if(KEYPRESSED()==1)  

 {  

 key=PINC;  

 key=key|0x00; /***************判断键值并操作改变数据*************/  

 if(key==0x01)  

 {  

 volt++;  

 }  

 else if(key==0x02)  

 {  

 volt--;  

 }  

 else if(key==0x04)  

 {  

 current++;


 }  

 else if(key==0x08)  

 {  

 current--;


 }  

 else if(key==0x10)  

 {  

 power++;


 }  

 else if(key==0x20)  

 {  

 power--; 


 }  

 else if(key==0x40)  

 {  

 pole++;


 }  

 else if(key==0x80)  

 {  

 pole--;


 }


 break;


 }


 break;


 }  

 else if(KEYPRESSED()!=1)  

 {  

 DelayMs(10);  

 if(KEYPRESSED()!=1)  

 break;  

 }


 }while(flag2);


 //停电机  

 PORTB&=~\_BV(PB5); /********上升沿触发，停电机******/  

 DelayMs(10);  

 PORTB|=\_BV(PB5);  

}

## Compilers

- WinAVR (GNU GCC)

## Tags

- Complete code
- WinAVR (GNU GCC)
