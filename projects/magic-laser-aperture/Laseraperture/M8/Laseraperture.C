// Target : M8// Crystal: 8Mhz
//  http://www.lotusinvention.cn/
//神奇的光圈Magic Laser aperture             	  Invention: ZH1110   2008.3.31

//PD3(INT1)光栅输入外中断2
//PB3(OC2)激光pwm输出
//PD2(INT0)	效果切换按钮

#include <iom8v.h>
#include <macros.h>


#define uchar unsigned char
#define uint unsigned int  

#define SET_BIT(io ,bit) (   io |=   (1<<bit) ) //置位example: SET_BIT(PORTA,0);SET_BIT(DDRA,0);  
#define CLR_BIT(io ,bit) (   io &= ~(1<<bit) ) //清位example: CLR_BIT(PORTA,0);CLR_BIT(DDRA,0);  
#define GET_BIT(pin,bit) ( pin &    (1<<bit) ) //取得位example: GET_BIT(PINA,0);
#define SET_IN( dir,bit) ( dir &= ~(1<<bit) ) //设置输入example: SET_IN(DDRA,0);
#define SET_OUT(dir,bit) ( dir |=   (1<<bit) ) //设置输出example: SET_OUT(DDRA,0);  


//正弦函数表
const uchar Sin_Table[256]={ 128, 131, 134, 137, 140, 144, 147, 150, 153, 156, 159, 162, 165, 168, 171, 174, 177, 179, 182, 185, 188, 191, 193, 196, 199, 201, 204, 206, 209, 211, 213, 216, 218, 220, 222, 224, 226, 228, 230, 232, 234, 235, 237, 239, 240, 241, 243, 244, 245, 246, 248, 249, 250, 250, 251, 252, 253, 253, 254, 254, 254, 255, 255, 255, 255, 255, 255, 255, 254, 254, 254, 253, 253, 252, 251, 250, 250, 249, 248, 246, 245, 244, 243, 241, 240, 239, 237, 235, 234, 232, 230, 228, 226, 224, 222, 220, 218, 216, 213, 211, 209, 206, 204, 201, 199, 196, 193, 191, 188, 185, 182, 179, 177, 174, 171, 168, 165, 162, 159, 156, 153, 150, 147, 144, 140, 137, 134, 131, 128, 125, 122, 119, 116, 112, 109, 106, 103, 100, 97, 94, 91, 88, 85, 82, 79, 77, 74, 71, 68, 65, 63, 60, 57, 55, 52, 50, 47, 45, 43, 40, 38, 36, 34, 32, 30, 28, 26, 24, 22, 21, 19, 17, 16, 15, 13, 12, 11, 10, 8, 7, 6, 6, 5, 4, 3, 3, 2, 2, 2, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 3, 3, 4, 5, 6, 6, 7, 8, 10, 11, 12, 13, 15, 16, 17, 19, 21, 22, 24, 26, 28, 30, 32, 34, 36, 38, 40, 43, 45, 47, 50, 52, 55, 57, 60, 63, 65, 68, 71, 74, 77, 79, 82, 85, 88, 91, 94, 97, 100, 103, 106, 109, 112, 116, 119, 122, 125 };           


uchar	Num_Effect=0; //效果
uint  pixel_pos=0;//绘制到了第几像素点
uint  pixel_pos_Relative;//第几像素点(相对1)
uint  pixel_pos_Relative_2;//第几像素点(相对2)

uint  time_pis=0;

uchar pixelclock;//绘制每个点的时间
uint  k=1000;//递增时间
int  kv;//递增速度

uint  k_2=1000;//递增时间
int  kv_2;

uint PWM_Brightness; //激光亮度
uint  PWM_Brightness_2;


void Delaypies(uint pis)  //延时N圈时间, 1pis=1圈
{
	time_pis=0;
	while(time_pis<pis){;}
}

//call this routine to initialize all peripherals
void init_devices(void)
{

	//########计算旋转一周的时间############
	TCCR1A=0X00;
	TCCR1B=0X03;//64分频
	//####################
		
	//#####定时器0,控制每个像素绘制时间######
	SET_BIT(TCCR0,CS01);//8分频	
	//####################
	
	
	//#####//PD3(INT1)光栅输入外中断1#########
	DDRD=0b00000000;   PORTD=0b00000100;  
	SET_BIT(SREG,7);  //开全局中断  
	SET_BIT(GICR,INT1);//允许外中断1
	SET_BIT(MCUCR,ISC11);//下降沿触发
	
	//#####//PD2(INT0)切换按钮中断0#########
	SET_BIT(GICR,INT0);//允许外中断0
	SET_BIT(MCUCR,ISC01);//下降沿触发
	//####################
	
	
	//#####定时器2/PB3(OC2)激光######
	DDRB=0b11111111;   PORTB=0b11111111;  
	TCCR2=0X02;//内部CK/64脉冲
	SET_BIT(TCCR2,WGM20);    //快速PWM方式
	SET_BIT(TCCR2,WGM21);
	SET_BIT(TCCR2,COM21); //减1匹配置位
	OCR2=PWM_Brightness;
	//####################
	DDRC=0xff;       
	PORTC=0x00;  
		
}


//停顿
void mPause(void)  
{
	Num_Effect=13;	Delaypies(10);
}

// //##main#####main######main######main#######
void main(void)  
{
	init_devices();
	
	Num_Effect=0;kv=-20;//开场效果测试
	Delaypies(77);
	
	while(1)
	{
		mPause();
		Num_Effect=1;kv=-5; //两道环
		Delaypies(100);
		Num_Effect=1;kv=5;
		Delaypies(100);	
		
		mPause();//停顿
		Num_Effect=2;kv=10; //单蛟
		Delaypies(120);
		
		mPause()  ; //停顿
		Num_Effect=3;kv=-10;
		Delaypies(150);
		
		Num_Effect=4;kv=10;kv_2=-6;    //双蛟,对向游
		Delaypies(150);
		
		mPause()  ;//停顿
		Num_Effect=6;kv=11;kv_2=-3; //正弦渐隐
		Delaypies(150);
		
		mPause()  ;//停顿		
		Num_Effect=7;kv=2;  //布尔钟
		Delaypies(150);
	}
}


//效果切换按钮
#pragma interrupt_handler reciv0:2
void reciv0(void)  
{
	//跳过	Delaypies()  STEP OVER Delaypies() 
	time_pis=60000;	
}


//外部中断1函数入口 旋转一周的时间
#pragma interrupt_handler reciv1:3
void reciv1(void)  
{
	CLR_BIT(TIMSK,TOIE0);//关像素定时0溢出中断
	pixelclock=TCNT1>>6; //512等分
	pixel_pos=0;
	TCNT1=0;
	
	k+=kv;
	if(k>20000)k-=512;
	if(k<1000)k+=512;
	
	k_2+=kv_2;
	if(k_2>20000)k_2-=512;
	if(k_2<1000)k_2+=512;
	
	time_pis++;
	
	TCNT0=0XFF-pixelclock+9;
	SET_BIT(TIMSK,TOIE0);//开像素绘制中断
	TIFR=0xff;
}

//#####定时器0溢出中断,像素绘制#####
#pragma  interrupt_handler pix_counter:10
void pix_counter(void)  
{
	TCNT0=0XFF-pixelclock+9;
	
	pixel_pos++;
	pixel_pos_Relative=( (k+pixel_pos)%512);
	pixel_pos_Relative_2=( (k_2+pixel_pos)%512);
	
	switch (Num_Effect)
	{
		
		////#####开场#####
	case 0:
		{
			if (pixel_pos_Relative>(pixel_pos))  
				PWM_Brightness=0xff;
			else
				PWM_Brightness=0x00;
		}
		break;
		
		////#####两道环#####
	case 1:
		{
			if ((pixel_pos_Relative%256)>210)  //512/3=170
				PWM_Brightness=0x00;
			else
				PWM_Brightness=0xff;
		}
		break;
		
		
		////#####单蛟#####
	case 2:
		{
			PWM_Brightness=0xff-pixel_pos_Relative/2;
			if(PWM_Brightness>250) PWM_Brightness=0;
		}
		break;
		
		//#####双蛟#####
	case 3:
		PWM_Brightness=(pixel_pos_Relative%256);//
		if(PWM_Brightness>240)PWM_Brightness=0;
		break;
		
		//#####双蛟,对向#####
	case 4:
		PWM_Brightness=0;
		PWM_Brightness_2=0;
		if (pixel_pos_Relative<256)PWM_Brightness=0xff-pixel_pos_Relative;//逆时针
		if (pixel_pos_Relative_2<256)PWM_Brightness_2=pixel_pos_Relative_2;//顺时针
		PWM_Brightness+=PWM_Brightness_2;
		if(PWM_Brightness>255)PWM_Brightness=255;
		
		break;
		
		
	case 6: ////#####渐隐#####
		PWM_Brightness=Sin_Table[(pixel_pos_Relative%256)]; 
		PWM_Brightness_2=Sin_Table[(pixel_pos_Relative_2%256)]; 
		
		PWM_Brightness+=PWM_Brightness_2;
		PWM_Brightness/=2;
		
		break;
		
		//// ##### 布尔钟#####
	case 7:
		{
			uint bs;
			bs=(uint)1<<(pixel_pos/64);
			bs<<=2;
			if(k&bs)PWM_Brightness=0; else PWM_Brightness=0xff;
		}
		break;
		
		
		//#####空白#####
	case 13:
		PWM_Brightness=0;
		break;
		
	}
		
	OCR2=PWM_Brightness;
	
}