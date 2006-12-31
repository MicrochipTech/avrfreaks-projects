/**************************************************************/
/*                  贪食蛇游戏  V2.0                          */
/*          硬件m64 + 128*64图形液晶（ks0108）8M RC           */
/*  环境：AVRSstudio4.12 SP4 + WINAVR20060421 -00优化         */
/*  作者：Bozai(章其波)                                       */
/*  日期: 2006-12-31  3:30                                    */
/**************************************************************/
/*            SNAKE  GAME V2.0                                */
/*  Author: Zhang Qibo                                        */
/*  E-mail: sudazqb@163.com                                   */
/*  MSN   : zhangqibo_1985@hotmail.com                        */
/*  Date  : 2006-12-31                                        */
/**************************************************************/

#include<avr/io.h>
#include<avr/pgmspace.h>
#include"LCD/LCD.H"
#include<stdlib.h>
//按键   //key
#define KEY_RIGHT  PF1
#define KEY_LEFT   PF0
#define KEY_UP     PF2
#define KEY_DOWN   PF3

//全局速度   //defination of speed
#define SPEED 20000



//记录蛇转向的结构体，链表
//链表首代表尾部的方向，链表尾部代表当前蛇首的方向
//a list is used to record the every turns
struct NODE
{
	unsigned char direction;	//方向   //direction
	unsigned char cnt;			//在此方向上前进的点数  //the count of the dot in this section
	struct Node *next;			//指向下一个节点  //next node
};

//记录蛇状态的结构体
//record the state of the snake
struct SNAKE
{
	unsigned char Head_X;		//蛇头的坐标//coordinate of the head
	unsigned char Head_Y;
	unsigned char Tail_X;		//蛇尾的坐标//coordinate of the tail
	unsigned char Tail_Y;
	unsigned char Direction;	//移动方向//current dirction of the head
#define TO_RIGHT 0x01
#define TO_LEFT  0x02
#define TO_UP    0x03
#define TO_DOWN  0x04
	unsigned char Level;		//游戏级别// game level
#define L0 0x00
#define L1 0x01
#define L2 0x02
#define L3 0x03
#define L4 0x04
#define L5 0x05
#define L6 0x06
#define L7 0x07
	unsigned int  Dot_Cnt;	//蛇身长度//the length of body
	unsigned char Next_X;	//目标点//coordinate of the destination
	unsigned char Next_Y;
	struct NODE * p;		//表明尾部移动方向的链表首//the head of the list,the head is the earlist turn
} Snake_Data;//声明结构体变量//global struct variable

unsigned char Hit_Self(unsigned char x,unsigned char y);//原形//prototype

//定时器1初始化//timer 1 initialization , offer the seed of the random()
void Timer1_Initial()
{
 TCNT1H=0x00;
 TCNT1L=0x00;
 TCCR1A=0x03;//ctc mode  use icr1
 TCCR1B=0x01;//system clock;
 ICR1H=0xff;
 ICR1L=0xff;
}

/*********** 以四个像素作为一个点 *************/
//总共尺寸 为 64 * 32
// x范围0~63 y范围0~31 val为填充值 1或0

//use four pixel as a dot,that means 128*64pixels have 64*32 dots
void OneDot(unsigned char x,unsigned char y,unsigned char val)//fill the four pixels with 1 or 0
{
	OnePixel(x<<1,y<<1,val);
	OnePixel(x<<1,(y<<1)+1,val);
	OnePixel((x<<1)+1,y<<1,val);
	OnePixel((x<<1)+1,(y<<1)+1,val);
}

//蛇身初始化,x y 为起始坐标,length为长度,level为游戏级别
//Initialization of the snake, (x,y)means the start place
//length means the default length of the snake
//level is the speed of the game
void Snake_Ini(unsigned char x,unsigned char y,unsigned char length,unsigned char level)
{
	unsigned char i;
	unsigned int position;
	Snake_Data.Head_X = x + length - 1;	//蛇头  //head of the snake
	Snake_Data.Head_Y = y;
	Snake_Data.Tail_X = x;				//蛇尾  //tail of the snake
	Snake_Data.Tail_Y = y;
	Snake_Data.Direction = TO_RIGHT;	//默认方向  //directon
	Snake_Data.Level = level;			//游戏级别  //level
	Snake_Data.Dot_Cnt = length;		//蛇身长度  //length
	ClearDisBuf(0x00);					//清屏      //clear display buffer
	for(i = 0;i<length;i++)OneDot(x+i,y,1);//填充   //fill the snake
	while(1)						//确定目标点    //creat a destinate dot use function rand()
	{
		srand(TCNT1);
		position = rand()>>4;			//产生随机数  //creat dot
		if(Hit_Self(position%64,position/64))continue;//如果落在有点处则重新产生  //if the dot is not locate at blank spce
		OneDot(position%64,position/64,1);	//填充  //fill the new dot
		Snake_Data.Next_X = position%64;//设置目标点   //set the coordinate of the new dot
		Snake_Data.Next_Y = position/64;
		break;
	}
	Snake_Data.p = malloc(4); 			 	//动态申请内存  //malloc
	Snake_Data.p->direction = TO_RIGHT;		//方向   //direction
	Snake_Data.p->cnt = length;				//点数   //length
	Snake_Data.p->next = 0;					//无下一节点   //no next node
	UpdateDis();							//刷新   //refresh the screen
}

//检测是否碰到自己   //if the snake hit itself  1 hit 0 not hit
unsigned char Hit_Self(unsigned char x,unsigned char y)
{
	return ReadPixel(x<<1,y<<1);
}

//键盘初始化   //keys initialization
void Key_Ini()
{
	DDRF |= 0x00;
	PORTF |= 0xff;
}

//键盘扫描    //key scan  return the direction
unsigned char Key_Scan(unsigned int n)
{
	unsigned char i = 0;
	while(n--)
	{
		if((PINF & (1<<KEY_RIGHT))==0)i = TO_RIGHT;
		else if((PINF & (1<<KEY_LEFT))==0)i = TO_LEFT;
		else if((PINF & (1<<KEY_UP))==0)i = TO_UP;
		else if((PINF & (1<<KEY_DOWN))==0)i = TO_DOWN;
	}
	return i;
}

//移动蛇身    // move the snake
unsigned char Move()
{
	unsigned int position;
	unsigned char x,y;
	struct NODE * temp;//临时节点  //temporary node

	//判断是否撞到四周的墙 //gameover
	//judge if it hit the wall
	if(Snake_Data.Direction == TO_RIGHT && Snake_Data.Head_X == 63)return 1;
	else if(Snake_Data.Direction == TO_LEFT && Snake_Data.Head_X == 0)return 1;
	else if(Snake_Data.Direction == TO_UP && Snake_Data.Head_Y == 0)return 1;
	else if(Snake_Data.Direction == TO_DOWN && Snake_Data.Head_Y == 31)return 1;

	//根据方向置x y的值
	//set x,y according the direction
	x = Snake_Data.Head_X;
	y = Snake_Data.Head_Y;
	if(Snake_Data.Direction == TO_RIGHT)x++;
	else if(Snake_Data.Direction == TO_LEFT)x--;
	else if(Snake_Data.Direction == TO_UP)y--;
	else if(Snake_Data.Direction == TO_DOWN)y++;

	temp = Snake_Data.p;
	while(temp->next != 0)temp = temp->next;//找到链表尾  //find the last node (the last turn of the snake)
	temp->cnt += 1;							//把点值加1   //add the cnt  

	//如果碰到目标点,即吃子
	//if the snake hit the destination
	if(x == Snake_Data.Next_X && y == Snake_Data.Next_Y)
	{
		//OneDot(x,y,1);
		Snake_Data.Dot_Cnt += 1;	//点加1   //length += 1
		Snake_Data.Head_X = x;		//置新头  //set the new coordinate of the head
		Snake_Data.Head_Y = y;

		while(1)                   //重新计算新目标点   //reculculate the destinate dot
		{
			srand(TCNT1);         //use TCNT1 as the seed
			position = rand()>>4;
			if(Hit_Self(position%64,position/64))continue;
			OneDot(position%64,position/64,1);
			Snake_Data.Next_X = position%64;
			Snake_Data.Next_Y = position/64;
			break;
		}
		UpdateDis();			//刷新      //refresh
		return 0;				//正常返回0 //return 0 normal
	}
	//如果碰到自己
	//if hit itself
	else if(Hit_Self(x,y) && x != Snake_Data.Tail_X && y != Snake_Data.Tail_Y)return 1;//gameover
	//正常移动
	//normal movement
	else
	{
		OneDot(x,y,1);			//向前移动  //move ahead
		Snake_Data.Head_X = x;	//置新首    //set the new head coordinate
		Snake_Data.Head_Y = y;
		OneDot(Snake_Data.Tail_X,Snake_Data.Tail_Y,0);//清最后一点  //clear the tail
		UpdateDis();									//刷新  //refresh

		if(Snake_Data.p->cnt == 1)						//如果已是该段的最后一个子  //if the dot is the last of this turn
		{
			temp = Snake_Data.p->next;					//保存节点   //save node
			free(Snake_Data.p);							//删除       //delete the node
			Snake_Data.p = temp;						//重新赋值   //reset
		}
		else            //否则该段减1   //otherwise decrement the count of this turn
		{
			Snake_Data.p->cnt -= 1;
		}
		//set the new tail according the direction
		switch(Snake_Data.p->direction)//根据该段的方向确定尾部移向哪个个点
		{
			case TO_RIGHT:Snake_Data.Tail_X += 1;break;
			case TO_LEFT :Snake_Data.Tail_X -= 1;break;
			case TO_UP   :Snake_Data.Tail_Y -= 1;break;
			case TO_DOWN :Snake_Data.Tail_Y += 1;break;
			default:break;
		}
	}
	return 0;//正常返回0  //return 0 normally
}

//进行游戏   // Play game
void Play_Game()
{
	unsigned int cnt;										//用于游戏速度  	//speed
	unsigned char direction;								//方向          	//direction
	unsigned char current_direction = Snake_Data.Direction;//当前方向 			//current direction
	struct NODE * temp;										//零时节点 用于交换	//temporary node
	struct NODE * temp1;									//零时节点
	switch(Snake_Data.Level)								//根据游戏级别设置延时 //set the speed according the level
	{
		case L0: cnt = SPEED/(L0+1);break;
		case L1: cnt = SPEED/(L1+1);break;
		case L2: cnt = SPEED/(L2+1);break;
		case L3: cnt = SPEED/(L3+1);break;
		case L4: cnt = SPEED/(L4+1);break;
		case L5: cnt = SPEED/(L5+1);break;
		case L6: cnt = SPEED/(L6+1);break;
		case L7: cnt = SPEED/(L7+1);break;
		default: cnt = SPEED/(L0+1);break;
	}
	while(1)
	{	
		direction = Key_Scan(cnt);							//读键值		//read key
		if(direction!=0 && direction != current_direction)	//不同方向		//a new direction
		{
			//判断是否为有效方向，不允许直接掉头
			//judge if it is a valid turn, do not allowed turn back directly
			if((direction == TO_RIGHT && current_direction == TO_LEFT)||(direction == TO_LEFT && current_direction == TO_RIGHT)||(direction == TO_UP && current_direction == TO_DOWN)||(direction == TO_DOWN && current_direction == TO_UP));
			else
			{
				current_direction = direction;		//改变方向			//change direction
				struct NODE *p = malloc(4);			//申请一个新节点	//a new node
				p->direction = current_direction;	//置方向			//set direction
				p->cnt = 0;							//点数为0  在move函数中会置值  //cnt=1 it will be set 1 in function Move()
				p->next = 0;						//无后续节点		//no next node
				temp = Snake_Data.p;				//把链表首给零时值  //set the new list head
				while(temp->next != NULL)temp = temp->next;//找到链表尾 //find the last node
				temp->next = p;						//添加新节点        //add the new node
			}
		}
		switch(direction)//根据按键方向确定移动方向 不可向相反方向移动
		{				 //set the direction of the head
			case TO_RIGHT :if(Snake_Data.Direction != TO_LEFT)Snake_Data.Direction = TO_RIGHT;break;
			case TO_LEFT  :if(Snake_Data.Direction != TO_RIGHT)Snake_Data.Direction = TO_LEFT;break;
			case TO_UP    :if(Snake_Data.Direction != TO_DOWN)Snake_Data.Direction = TO_UP;break;
			case TO_DOWN  :if(Snake_Data.Direction != TO_UP)Snake_Data.Direction = TO_DOWN;break;
			default:break;
		}
		if(Move())
		{
			//游戏结束，释放内存，删除链表
			//game over release the memory
			temp = Snake_Data.p;
			while(temp->next != 0)
			{
				temp1 = temp->next;
				free(temp);
				temp = temp1;
			}
			free(temp);
			return;//如果撞倒四周和自己就gameover了  //game over
		}
	}	
}

int main()
{
	Timer1_Initial();	//定时器1初始化				//initialize timer1
	Key_Ini();			//初始化按键				//initialize key
	InitialLcd();		//液晶屏初始化				//initialize LCD
	while(1)
	{
		Snake_Ini(5,5,8,0);	//游戏初始化 尾在(5,5)处 长度8 级别0	//initialize game
		Play_Game();		//开始游戏								//play game
	}	
}
