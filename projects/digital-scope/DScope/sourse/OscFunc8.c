#include <delay.h>
#include <string.h>
#include <math.h>




#define Set(x)  LCD_CPORT.x=1
#define Clr(x)  LCD_CPORT.x=0
#define Set2(x,y)  LCD_CPORT|=((1<<x)+(1<<y));
#define Clr2(x,y)  LCD_CPORT&=0xff-((1<<x)+(1<<y));

#define LCD_RST   0
#define LCD_CD    1
#define LCD_CE    2
#define LCD_RD    3
#define LCD_WR    4

#define LCD_CPORT PORTB
#define LCD_DPORT PORTD
#define LCD_DPIN  PIND
#define LCD_DDDR  DDRD
#define dly_const(x) Dly=x;

#define Key1 (KeyStatus>>3)&1
#define Key2 (KeyStatus>>4)&1
#define Key3 (KeyStatus>>5)&1

#define KeyPort (PINC & 0x38)  


const char flash conv[]={163,165,167,169,171,173,175,177,179,181,183,185,187,189,191,193,195,197,199,0,0,0,0,0,0,0,0,0,0,0,0,95,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51,52,53,54,55,56,57,58,59,60,61,62,161,97,65,66,67,68,69,70,71,72,73,74,75,76,77,78,79,80,81,82,83,84,85,86,87,88,89,90,91,92,93,94,0,0,0,0,0,0,0,0,0,0,0,0,0};
const flash char gridh[5]={0x08,0x02,0x00,0x80,0x20};  //10
const flash char MenuStr[]="TVtDG   ";
const flash char Msg[5][9]={"Time.div","Volt.div","Trig.Lev","DC/AC   ","Grid    "};
const flash char VoltStr[7][6]={"  1 v","0.5 v","0.1 v","50 mv","10 mv"," 5 mv"," 1 mv"};

unsigned char KeyStatus,mn=0,VoltDiv=0,TrigLevel=128;
unsigned char grid_mode=3;
bit DC_AC=0;  //dc=0  ac=1

void Cmd2(unsigned char cmd,unsigned int d);
void Cmd1(unsigned char cmd,unsigned char d);
void Cmd(unsigned char cmd);
void DataWR(unsigned char d);
void initLCD();
void ChkS();
void lcdstrf(char flash *st);
void lcdstr(char  *st);
void putpixel(unsigned char x, unsigned char y);
void GraphicCls();
void TextCls();
void Grid();
void Demo();
//unsigned char DataRD();

//////////////////////////////////////////////////////////////////////
void delay()
{ 
  register unsigned int i;
  for(i=0;i<Dly;i++);
}
/////////////////////////////////
void readkey()
{    
  static unsigned char back=0;
  unsigned char cur,des;
  
  cur=KeyPort;
  KeyStatus=(0xff-cur) & back;
  back=cur;
  
  
  des=(KeyStatus>>3)&0x03;
  des|=(mn<<4)&0xf0;
  
  if(Key3==1)
  {
    mn++;
    mn%=5;   
    
    Cmd(0x9F);//on& blink
    Cmd(0xA1);//2line
    Cmd2(0x21,mn);

  }
   
//"TVtDG   ";   
  switch(des)
  {      
  //mn==0   TimeDiv
    case 0x01: Dly+=100; break;
    case 0x02: Dly-=100; break;
  //mn==1   voltdiv
    case 0x11: 
      if(VoltDiv<7) VoltDiv++; 
      PORTD=(PORTD & 0xC7)+VoltDiv<<3; 
      break;
    case 0x12: 
      if(VoltDiv>0) VoltDiv--;  
      PORTD=(PORTD & 0xC7)+VoltDiv<<3;
      break;
  //mn==2   trig lev
    case 0x21: TrigLevel++; break;
    case 0x22: TrigLevel--; break;    
  //mn==3   Ac-Dc
    case 0x31: 
    case 0x32: DC_AC=1-DC_AC; break;    
  //mn==4   Grid
    case 0x41: grid_mode++; grid_mode%=4; break;
    case 0x42: grid_mode--; grid_mode%=4; break;
      
  };


  Cmd2(0x24,0x0808);
  lcdstrf(Msg[mn]);
    
}

///////////////////////////////
void lcdchar(char ch)
{

/*
!"#$%&'()*+,-./
0123456789:;<=>?
@ABCDEFGHIJKLMNO
PQRSTUVWXYZ[\]^_
`abcdefghijklmno
pqrstuvwxyz{|}~ 
*/
  Cmd1(0xC0,conv[ch]-1);
}	
///////////////////////////////
void lcdstr(char *st)
{
  char i;
  for(i=0;i<strlen(st);i++)
    lcdchar(st[i]);
}
///////////////////////////////
void lcdstrf(char flash *st)
{
  char i;
  for(i=0;i<strlenf(st);i++)
    lcdchar(st[i]);
}
/////////////////////////////// 
void lineto(unsigned char x, unsigned char y)
{
  static unsigned char bx=0,by=0;
  float dx,dy,xx,yy,len;
  unsigned char l;
  //len=sqrt((x-bx)*(x-bx)+(y-by)*(y-by));
  
  x&=0x7f;
  y&=0x7f;

  xx=x; xx-=bx; xx*=xx;
  yy=y; yy-=by; yy*=yy;
  len=sqrt(xx+yy);
  
  if(len>0)
  {
    dx=(x-bx)/len;
    dy=(y-by)/len;
  
  
    for(l=0;l<len;l++)
    {                    
      xx=dx*l+bx;  
      yy=dy*l+by;
      putpixel(xx,yy);
    }
    bx=x;by=y;
  }
}
/////////////////////////////// 
void putpixel(unsigned char x, unsigned char y)
{               
  unsigned int temp;
  unsigned char temp2;
  x&=0x7f;
  temp=y & 0x007f;
  temp=(temp<<4) |  (x>>3);
  Cmd2(0x24,temp);
  temp2=(0xff - (x & 0x07)) | 0xf8;
  Cmd(temp2);
}
///////////////////////////////
void Grid()
{
  unsigned char x,y,ch,st[4];

  Cmd2(0x24,0x0000); 
  for(y=0;y<128;y++)
  {   
    for(x=0;x<16;x++)
    {
      ch=0x00;
      if((grid_mode&0x02)==0x02)
        if((y%10)==3) ch=gridh[x%5];
      if((grid_mode&0x01)==0x01)
      {
        if(y==63) ch^=0xff;
        if(x==8) ch^=0x80;
      } 
      if(y<8 || y>120) ch=0x00;
      Cmd1(0xC0,ch);
    }
  }
  Cmd2(0x24,0x0800);
  lcdstrf(MenuStr);
  Cmd2(0x24,0x08F0);
  lcdstrf("Td:10ms Vd:");
  lcdstrf(VoltStr[VoltDiv]);
  
  Cmd2(0x24,0x08F3);
  sprintf(st,"%3d",Dly>>4);
  lcdstr(st);
  lcdstrf("ms");
}
///////////////////////////////
void GraphicCls()
{
  unsigned int l;
// zero fill

  Cmd2(0x24,0x0000);
  for (l=0;l<0x800;l++)
    Cmd1(0xC0,0x00);

} 
///////////////////////////////
void TextCls()
{
  int l;
// zero fill
  Cmd2(0x24,0x0800);

  for (l=0;l<16*16;l++)
    Cmd1(0xC0,0x00);

} 
//////////////////////////
void initLCD()
{         
  LCD_DDDR=0x00;
  LCD_DPORT=0xFF;
  
  //LCD_CPORT=0*LCD_RST_C+LCD_CD_C+LCD_CE_C+LCD_RD_C+LCD_WR_C;
  LCD_CPORT=(1<<LCD_CD) | (1<<LCD_CE) | (1<<LCD_RD) |(1<<LCD_WR);
  // if conected other device to pin(5,6,7) .....
  delay_us(100);
  
  Set(LCD_RST);

//Graphic home addr
  Cmd2(0x42,0x0000);
//text home addr
  Cmd2(0x40,0x0800);
//text Area
  Cmd2(0x41,0x0010);
//Graphic Area
  Cmd2(0x43,0x0010);
//Mode OR=80  xor=81    ((((and=83))))not use
  Cmd(0x80);
//offset
  Cmd2(0x22,0x0002);
//Graphic&text
  Cmd(0x9C);
//cursor 8line 
  Cmd(0xA2);
//cursor pointer 0,0
  Cmd2(0x21,0x0000);



} 
/////////////////////////////////
void Demo()
{        
  unsigned char i;
  GraphicCls();
  TextCls();  
             
  Cmd2(0x24,0x0810);
  lcdstrf("The Name Of GOD ");  
  Cmd2(0x24,0x08A0);  
  lcdstrf("Please wait...  ");  
  lcdstrf("[              ]");    
  Cmd2(0x24,0x08B1);
  for(i=0;i<14;i++)
  {
    Cmd1(0xC0,0x03);
    delay_ms(2);
    if((i==4)||(i==9)) delay_ms(6);
  }               
  delay_ms(10);
  TextCls();  
             
}
/////////////////////////////////
void ChkS()
{
  unsigned char temp;
  
  LCD_DDDR=0x00;
  LCD_DPORT=0xFF;
   
  Set(LCD_CE);
  Set(LCD_CD);
  do
  {        
    Clr2(LCD_RD,LCD_CE);
    temp=LCD_DPIN;
    Set2(LCD_RD,LCD_CE);
    //delay_us(1);
  }while ((temp& 0x03)!=0x03);
}
/////////////////////////////////
void Cmd2(unsigned char cmd,unsigned int d)
{
  unsigned char tt;
  
  tt=d & 0x00ff;
  DataWR(tt);
  tt=d>>8;
  DataWR(tt);
  Cmd(cmd);
}
/////////////////////////////////
void Cmd1(unsigned char cmd,unsigned char d)
{
  DataWR(d);
  Cmd(cmd);
}
/////////////////////////////////
void Cmd(unsigned char cmd)
{
  ChkS();
  Set(LCD_CD);
  Clr2(LCD_WR,LCD_CE);
  LCD_DDDR=0xff;
  LCD_DPORT=cmd;
  delay_us(1);
  Set2(LCD_WR,LCD_CE);
}
/////////////////////////////////
void DataWR(unsigned char d)
{
  ChkS();
 
  Clr(LCD_CD);
  Clr2(LCD_WR,LCD_CE);
  LCD_DDDR=0xff;
  LCD_DPORT=d; 
  delay_us(1);  
  Set2(LCD_WR,LCD_CE);
  Set(LCD_CD);
}
/////////////////////////////////
/*unsigned char  DataRD()
{
  unsigned char j;
  ChkS();
  Clr(LCD_CD);
  Clr2(LCD_RD,LCD_CE);
  j=LCD_DPIN;                         
  Set2(LCD_RD,LCD_CE);
  Set(LCD_CD);
  return j;
}*/