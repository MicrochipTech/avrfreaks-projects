/**************************************************************/
/*                  ̰ʳ����Ϸ  V2.0                          */
/*          Ӳ��m64 + 128*64ͼ��Һ����ks0108��8M RC           */
/*  ������AVRSstudio4.12 SP4 + WINAVR20060421 -00�Ż�         */
/*  ���ߣ�Bozai(���䲨)                                       */
/*  ����: 2006-12-31  3:30                                    */
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
//����   //key
#define KEY_RIGHT  PF1
#define KEY_LEFT   PF0
#define KEY_UP     PF2
#define KEY_DOWN   PF3

//ȫ���ٶ�   //defination of speed
#define SPEED 20000



//��¼��ת��Ľṹ�壬����
//�����״���β���ķ�������β������ǰ���׵ķ���
//a list is used to record the every turns
struct NODE
{
	unsigned char direction;	//����   //direction
	unsigned char cnt;			//�ڴ˷�����ǰ���ĵ���  //the count of the dot in this section
	struct Node *next;			//ָ����һ���ڵ�  //next node
};

//��¼��״̬�Ľṹ��
//record the state of the snake
struct SNAKE
{
	unsigned char Head_X;		//��ͷ������//coordinate of the head
	unsigned char Head_Y;
	unsigned char Tail_X;		//��β������//coordinate of the tail
	unsigned char Tail_Y;
	unsigned char Direction;	//�ƶ�����//current dirction of the head
#define TO_RIGHT 0x01
#define TO_LEFT  0x02
#define TO_UP    0x03
#define TO_DOWN  0x04
	unsigned char Level;		//��Ϸ����// game level
#define L0 0x00
#define L1 0x01
#define L2 0x02
#define L3 0x03
#define L4 0x04
#define L5 0x05
#define L6 0x06
#define L7 0x07
	unsigned int  Dot_Cnt;	//������//the length of body
	unsigned char Next_X;	//Ŀ���//coordinate of the destination
	unsigned char Next_Y;
	struct NODE * p;		//����β���ƶ������������//the head of the list,the head is the earlist turn
} Snake_Data;//�����ṹ�����//global struct variable

unsigned char Hit_Self(unsigned char x,unsigned char y);//ԭ��//prototype

//��ʱ��1��ʼ��//timer 1 initialization , offer the seed of the random()
void Timer1_Initial()
{
 TCNT1H=0x00;
 TCNT1L=0x00;
 TCCR1A=0x03;//ctc mode  use icr1
 TCCR1B=0x01;//system clock;
 ICR1H=0xff;
 ICR1L=0xff;
}

/*********** ���ĸ�������Ϊһ���� *************/
//�ܹ��ߴ� Ϊ 64 * 32
// x��Χ0~63 y��Χ0~31 valΪ���ֵ 1��0

//use four pixel as a dot,that means 128*64pixels have 64*32 dots
void OneDot(unsigned char x,unsigned char y,unsigned char val)//fill the four pixels with 1 or 0
{
	OnePixel(x<<1,y<<1,val);
	OnePixel(x<<1,(y<<1)+1,val);
	OnePixel((x<<1)+1,y<<1,val);
	OnePixel((x<<1)+1,(y<<1)+1,val);
}

//�����ʼ��,x y Ϊ��ʼ����,lengthΪ����,levelΪ��Ϸ����
//Initialization of the snake, (x,y)means the start place
//length means the default length of the snake
//level is the speed of the game
void Snake_Ini(unsigned char x,unsigned char y,unsigned char length,unsigned char level)
{
	unsigned char i;
	unsigned int position;
	Snake_Data.Head_X = x + length - 1;	//��ͷ  //head of the snake
	Snake_Data.Head_Y = y;
	Snake_Data.Tail_X = x;				//��β  //tail of the snake
	Snake_Data.Tail_Y = y;
	Snake_Data.Direction = TO_RIGHT;	//Ĭ�Ϸ���  //directon
	Snake_Data.Level = level;			//��Ϸ����  //level
	Snake_Data.Dot_Cnt = length;		//������  //length
	ClearDisBuf(0x00);					//����      //clear display buffer
	for(i = 0;i<length;i++)OneDot(x+i,y,1);//���   //fill the snake
	while(1)						//ȷ��Ŀ���    //creat a destinate dot use function rand()
	{
		srand(TCNT1);
		position = rand()>>4;			//���������  //creat dot
		if(Hit_Self(position%64,position/64))continue;//��������е㴦�����²���  //if the dot is not locate at blank spce
		OneDot(position%64,position/64,1);	//���  //fill the new dot
		Snake_Data.Next_X = position%64;//����Ŀ���   //set the coordinate of the new dot
		Snake_Data.Next_Y = position/64;
		break;
	}
	Snake_Data.p = malloc(4); 			 	//��̬�����ڴ�  //malloc
	Snake_Data.p->direction = TO_RIGHT;		//����   //direction
	Snake_Data.p->cnt = length;				//����   //length
	Snake_Data.p->next = 0;					//����һ�ڵ�   //no next node
	UpdateDis();							//ˢ��   //refresh the screen
}

//����Ƿ������Լ�   //if the snake hit itself  1 hit 0 not hit
unsigned char Hit_Self(unsigned char x,unsigned char y)
{
	return ReadPixel(x<<1,y<<1);
}

//���̳�ʼ��   //keys initialization
void Key_Ini()
{
	DDRF |= 0x00;
	PORTF |= 0xff;
}

//����ɨ��    //key scan  return the direction
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

//�ƶ�����    // move the snake
unsigned char Move()
{
	unsigned int position;
	unsigned char x,y;
	struct NODE * temp;//��ʱ�ڵ�  //temporary node

	//�ж��Ƿ�ײ�����ܵ�ǽ //gameover
	//judge if it hit the wall
	if(Snake_Data.Direction == TO_RIGHT && Snake_Data.Head_X == 63)return 1;
	else if(Snake_Data.Direction == TO_LEFT && Snake_Data.Head_X == 0)return 1;
	else if(Snake_Data.Direction == TO_UP && Snake_Data.Head_Y == 0)return 1;
	else if(Snake_Data.Direction == TO_DOWN && Snake_Data.Head_Y == 31)return 1;

	//���ݷ�����x y��ֵ
	//set x,y according the direction
	x = Snake_Data.Head_X;
	y = Snake_Data.Head_Y;
	if(Snake_Data.Direction == TO_RIGHT)x++;
	else if(Snake_Data.Direction == TO_LEFT)x--;
	else if(Snake_Data.Direction == TO_UP)y--;
	else if(Snake_Data.Direction == TO_DOWN)y++;

	temp = Snake_Data.p;
	while(temp->next != 0)temp = temp->next;//�ҵ�����β  //find the last node (the last turn of the snake)
	temp->cnt += 1;							//�ѵ�ֵ��1   //add the cnt  

	//�������Ŀ���,������
	//if the snake hit the destination
	if(x == Snake_Data.Next_X && y == Snake_Data.Next_Y)
	{
		//OneDot(x,y,1);
		Snake_Data.Dot_Cnt += 1;	//���1   //length += 1
		Snake_Data.Head_X = x;		//����ͷ  //set the new coordinate of the head
		Snake_Data.Head_Y = y;

		while(1)                   //���¼�����Ŀ���   //reculculate the destinate dot
		{
			srand(TCNT1);         //use TCNT1 as the seed
			position = rand()>>4;
			if(Hit_Self(position%64,position/64))continue;
			OneDot(position%64,position/64,1);
			Snake_Data.Next_X = position%64;
			Snake_Data.Next_Y = position/64;
			break;
		}
		UpdateDis();			//ˢ��      //refresh
		return 0;				//��������0 //return 0 normal
	}
	//��������Լ�
	//if hit itself
	else if(Hit_Self(x,y) && x != Snake_Data.Tail_X && y != Snake_Data.Tail_Y)return 1;//gameover
	//�����ƶ�
	//normal movement
	else
	{
		OneDot(x,y,1);			//��ǰ�ƶ�  //move ahead
		Snake_Data.Head_X = x;	//������    //set the new head coordinate
		Snake_Data.Head_Y = y;
		OneDot(Snake_Data.Tail_X,Snake_Data.Tail_Y,0);//�����һ��  //clear the tail
		UpdateDis();									//ˢ��  //refresh

		if(Snake_Data.p->cnt == 1)						//������Ǹöε����һ����  //if the dot is the last of this turn
		{
			temp = Snake_Data.p->next;					//����ڵ�   //save node
			free(Snake_Data.p);							//ɾ��       //delete the node
			Snake_Data.p = temp;						//���¸�ֵ   //reset
		}
		else            //����öμ�1   //otherwise decrement the count of this turn
		{
			Snake_Data.p->cnt -= 1;
		}
		//set the new tail according the direction
		switch(Snake_Data.p->direction)//���ݸöεķ���ȷ��β�������ĸ�����
		{
			case TO_RIGHT:Snake_Data.Tail_X += 1;break;
			case TO_LEFT :Snake_Data.Tail_X -= 1;break;
			case TO_UP   :Snake_Data.Tail_Y -= 1;break;
			case TO_DOWN :Snake_Data.Tail_Y += 1;break;
			default:break;
		}
	}
	return 0;//��������0  //return 0 normally
}

//������Ϸ   // Play game
void Play_Game()
{
	unsigned int cnt;										//������Ϸ�ٶ�  	//speed
	unsigned char direction;								//����          	//direction
	unsigned char current_direction = Snake_Data.Direction;//��ǰ���� 			//current direction
	struct NODE * temp;										//��ʱ�ڵ� ���ڽ���	//temporary node
	struct NODE * temp1;									//��ʱ�ڵ�
	switch(Snake_Data.Level)								//������Ϸ����������ʱ //set the speed according the level
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
		direction = Key_Scan(cnt);							//����ֵ		//read key
		if(direction!=0 && direction != current_direction)	//��ͬ����		//a new direction
		{
			//�ж��Ƿ�Ϊ��Ч���򣬲�����ֱ�ӵ�ͷ
			//judge if it is a valid turn, do not allowed turn back directly
			if((direction == TO_RIGHT && current_direction == TO_LEFT)||(direction == TO_LEFT && current_direction == TO_RIGHT)||(direction == TO_UP && current_direction == TO_DOWN)||(direction == TO_DOWN && current_direction == TO_UP));
			else
			{
				current_direction = direction;		//�ı䷽��			//change direction
				struct NODE *p = malloc(4);			//����һ���½ڵ�	//a new node
				p->direction = current_direction;	//�÷���			//set direction
				p->cnt = 0;							//����Ϊ0  ��move�����л���ֵ  //cnt=1 it will be set 1 in function Move()
				p->next = 0;						//�޺����ڵ�		//no next node
				temp = Snake_Data.p;				//�������׸���ʱֵ  //set the new list head
				while(temp->next != NULL)temp = temp->next;//�ҵ�����β //find the last node
				temp->next = p;						//����½ڵ�        //add the new node
			}
		}
		switch(direction)//���ݰ�������ȷ���ƶ����� �������෴�����ƶ�
		{				 //set the direction of the head
			case TO_RIGHT :if(Snake_Data.Direction != TO_LEFT)Snake_Data.Direction = TO_RIGHT;break;
			case TO_LEFT  :if(Snake_Data.Direction != TO_RIGHT)Snake_Data.Direction = TO_LEFT;break;
			case TO_UP    :if(Snake_Data.Direction != TO_DOWN)Snake_Data.Direction = TO_UP;break;
			case TO_DOWN  :if(Snake_Data.Direction != TO_UP)Snake_Data.Direction = TO_DOWN;break;
			default:break;
		}
		if(Move())
		{
			//��Ϸ�������ͷ��ڴ棬ɾ������
			//game over release the memory
			temp = Snake_Data.p;
			while(temp->next != 0)
			{
				temp1 = temp->next;
				free(temp);
				temp = temp1;
			}
			free(temp);
			return;//���ײ�����ܺ��Լ���gameover��  //game over
		}
	}	
}

int main()
{
	Timer1_Initial();	//��ʱ��1��ʼ��				//initialize timer1
	Key_Ini();			//��ʼ������				//initialize key
	InitialLcd();		//Һ������ʼ��				//initialize LCD
	while(1)
	{
		Snake_Ini(5,5,8,0);	//��Ϸ��ʼ�� β��(5,5)�� ����8 ����0	//initialize game
		Play_Game();		//��ʼ��Ϸ								//play game
	}	
}
