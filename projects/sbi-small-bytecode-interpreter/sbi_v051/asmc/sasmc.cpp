#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <algorithm>
#include <iostream>
#include <fstream>
#include "tokenizer.h"

#define VERSION_STR "0.4"

using namespace std;
using std::string;
using std::cout;
using std::endl;

typedef unsigned char byte;

/*
	SBI Format
*/
#define HEADER_0						0xAA
#define HEADER_1						0x4B
//#define HEADER_1					0x3B // Old version
//#define HEADER_1					0x2B // Old version
//#define HEADER_1					0x1B // Old version

#define LABELSECTION				0xA3
#define INTERRUPTSSECTION		0xB3

#define SEPARATOR						0xB7

#define FOOTER_0						0x3A
#define FOOTER_1						0xF0

/*
	SBI instructions
*/
#define assign							0x01
#define move								0x02
#define add									0x10
#define sub									0x11
#define mul									0x12
#define div									0x13
#define incr								0x20
#define decr								0x21
#define inv									0x22
#define tob									0x23
#define cmp									0x30
#define high								0x31
#define low									0x32
#define label								0x40
#define jump								0x41
#define cmpjump							0x42
#define	_ret								0x43
#define debug								0x50
#define debugstr						0x51
#define error								0x52
#define sint								0x60
#define _int								0x61
#define exit								0xFF

#define varid								0x04
#define value								0xF4

/*
	Program global variables
*/
fstream f; // .SBI
fstream fp; // .SBI.DT
int linen = 1;
byte labelsn = 0;
int labels[256];
byte interruptsn = 0;
int interrupts[256];
long progln=0;
char* inname;
char* outname;
char* prgname;

/*
	Command-line arguments parsing functions
*/
char* getCmdOption(char ** begin, char ** end, const std::string & option)
{
    char ** itr = std::find(begin, end, option);
    if (itr != end && ++itr != end)
    {
        return *itr;
    }
    return 0;
}

bool cmdOptionExists(char** begin, char** end, const std::string& option)
{
    return std::find(begin, end, option) != end;
}

/*
	SBI writing routines
*/
void beginsbi(void)
{
	char buf[2] = { HEADER_0, HEADER_1 };
	f.write(buf, 2);
}

void endsbi(void)
{
	char buf[2] = { FOOTER_0, FOOTER_1 };
	f.write(buf, 2);
}

void sbiwb(byte b)
{
	char buf[1] = { b };
	f.write(buf, 1);
}

void sbiwi(int addr)
{
	char buf[2] = {(addr & 0xFF), (addr >> 8)};
	f.write(buf, 2);
}

void wb(byte b)
{
	char buf[1] = { b };
	fp.write(buf, 1);
	progln++;
}

void wsbi(void)
{
	beginsbi();
	
	int headln = 2 + 1 + 1 + (labelsn * 2) + 1 + 1 + 1 + (interruptsn * 2) + 1; // HEADER(2) + LABELSECTION + LABELNUM + LABELDATA + SEPARATOR + INTERRUPTSSECTION + INTERRUPTSN + INTERRUPTSDATA + SEPARATOR
	sbiwb(LABELSECTION);
	sbiwb(labelsn);
	
	byte n;
	for (n=0; n<labelsn; n++)
		sbiwi(headln + labels[n]);
	
	sbiwb(SEPARATOR);
	
	sbiwb(INTERRUPTSSECTION);
	sbiwb(interruptsn);
	
	for (n=0; n<interruptsn; n++)
		sbiwi(headln + interrupts[n]);
	
	sbiwb(SEPARATOR);
	
	fp.close();
	fp.open(prgname, ios::in | ios::binary);
	
	fp.seekg(0);
	char buff[progln];
  fp.read(buff, progln);
  
  f.write(buff, progln);
	
	endsbi();
}

/*
	Compilation functions
*/
int pline(string command, int argn, vector<string>& args)
{
	byte argt[8];
	byte argv[8];
	int i=0;
	int p=0;
	for (i=0; i<8; i++)
	{
		argt[i]=value;
		argv[i]=0;
	}
	for (i=0; i<argn; i++)
	{
		if ((args[i][0]=='_')&&(args[i][1]=='t'))
		{
			string s=args[i].substr(2);
			argt[p]=varid;
			argv[p]=atoi(s.c_str());
			p++;
		} else {
			argt[p]=value;
			argv[p]=atoi(args[i].c_str());
			p++;
		}
	}
	if (command.compare("assign")==0)
	{
		if (argn!=2) { printf("%i: (%s) Wrong number of parameters\n", linen, command.c_str()); return 1; }
		if ((argt[0]!=varid)||(argt[1]!=value)) { printf("%i: (%s) Wrong type of parameters\n", command.c_str(), linen); return 1; }
		wb(assign);
		wb(argv[0]);
		wb(argv[1]);
		return 0;
	}
	if (command.compare("move")==0)
	{
		if (argn!=2) { printf("%i: (%s) Wrong number of parameters\n", linen, command.c_str()); return 1; }
		if ((argt[0]!=varid)||(argt[1]!=varid)) { printf("%i: (%s) Wrong type of parameters\n", command.c_str(), linen); return 1; }
		wb(move);
		wb(argv[0]);
		wb(argv[1]);
		return 0;
	}
	if (command.compare("add")==0)
	{
		if (argn!=3) { printf("%i: (%s) Wrong number of parameters\n", linen, command.c_str()); return 1; }
		if (argt[2]!=varid) { printf("%i: (%s) Wrong type of parameters\n", command.c_str(), linen); return 1; }
		wb(add);
		wb(argt[0]);
		wb(argv[0]);
		wb(argt[1]);
		wb(argv[1]);
		wb(argv[2]);
		return 0;
	}
	if (command.compare("sub")==0)
	{
		if (argn!=3) { printf("%i: (%s) Wrong number of parameters\n", linen, command.c_str()); return 1; }
		if (argt[2]!=varid) { printf("%i: (%s) Wrong type of parameters\n", command.c_str(), linen); return 1; }
		wb(sub);
		wb(argt[0]);
		wb(argv[0]);
		wb(argt[1]);
		wb(argv[1]);
		wb(argv[2]);
		return 0;
	}
	if (command.compare("mul")==0)
	{
		if (argn!=3) { printf("%i: (%s) Wrong number of parameters\n", linen, command.c_str()); return 1; }
		if (argt[2]!=varid) { printf("%i: (%s) Wrong type of parameters\n", command.c_str(), linen); return 1; }
		wb(mul);
		wb(argt[0]);
		wb(argv[0]);
		wb(argt[1]);
		wb(argv[1]);
		wb(argv[2]);
		return 0;
	}
	if (command.compare("div")==0)
	{
		if (argn!=3) { printf("%i: (%s) Wrong number of parameters\n", linen, command.c_str()); return 1; }
		if (argt[2]!=varid) { printf("%i: (%s) Wrong type of parameters\n", command.c_str(), linen); return 1; }
		wb(div);
		wb(argt[0]);
		wb(argv[0]);
		wb(argt[1]);
		wb(argv[1]);
		wb(argv[2]);
		return 0;
	}
	if (command.compare("incr")==0)
	{
		if (argn!=1) { printf("%i: (%s) Wrong number of parameters\n", linen, command.c_str()); return 1; }
		if (argt[0]!=varid) { printf("%i: (%s) Wrong type of parameters\n", command.c_str(), linen); return 1; }
		wb(incr);
		wb(argv[0]);
		return 0;
	}
	if (command.compare("decr")==0)
	{
		if (argn!=1) { printf("%i: (%s) Wrong number of parameters\n", linen, command.c_str()); return 1; }
		if (argt[0]!=varid) { printf("%i: (%s) Wrong type of parameters\n", command.c_str(), linen); return 1; }
		wb(decr);
		wb(argv[0]);
		return 0;
	}
	if (command.compare("inv")==0)
	{
		if (argn!=1) { printf("%i: (%s) Wrong number of parameters\n", linen, command.c_str()); return 1; }
		if (argt[0]!=varid) { printf("%i: (%s) Wrong type of parameters\n", command.c_str(), linen); return 1; }
		wb(inv);
		wb(argv[0]);
		return 0;
	}
	if (command.compare("tob")==0)
	{
		if (argn!=1) { printf("%i: (%s) Wrong number of parameters\n", linen, command.c_str()); return 1; }
		if (argt[0]!=varid) { printf("%i: (%s) Wrong type of parameters\n", command.c_str(), linen); return 1; }
		wb(tob);
		wb(argv[0]);
		return 0;
	}
	if (command.compare("cmp")==0)
	{
		if (argn!=3) { printf("%i: (%s) Wrong number of parameters\n", linen, command.c_str()); return 1; }
		if (argt[2]!=varid) { printf("%i: (%s) Wrong type of parameters\n", command.c_str(), linen); return 1; }
		wb(cmp);
		wb(argt[0]);
		wb(argv[0]);
		wb(argt[1]);
		wb(argv[1]);
		wb(argv[2]);
		return 0;
	}
	if (command.compare("high")==0)
	{
		if (argn!=3) { printf("%i: (%s) Wrong number of parameters\n", linen, command.c_str()); return 1; }
		if (argt[2]!=varid) { printf("%i: (%s) Wrong type of parameters\n", command.c_str(), linen); return 1; }
		wb(high);
		wb(argt[0]);
		wb(argv[0]);
		wb(argt[1]);
		wb(argv[1]);
		wb(argv[2]);
		return 0;
	}
	if (command.compare("low")==0)
	{
		if (argn!=3) { printf("%i: (%s) Wrong number of parameters\n", linen, command.c_str()); return 1; }
		if (argt[2]!=varid) { printf("%i: (%s) Wrong type of parameters\n", command.c_str(), linen); return 1; }
		wb(low);
		wb(argt[0]);
		wb(argv[0]);
		wb(argt[1]);
		wb(argv[1]);
		wb(argv[2]);
		return 0;
	}
	if (command.compare("label")==0)
	{
		if (argn!=1) { printf("%i: (%s) Wrong number of parameters\n", linen, command.c_str()); return 1; }
		if (argt[0]!=value) { printf("%i: (%s) Wrong type of parameters\n", command.c_str(), linen); return 1; }
		labels[argv[0]] = progln;
		if ((argv[0]+1) > labelsn) labelsn = argv[0]+1;
		return 0;
	}
	if (command.compare("sig")==0)
	{
		if (argn!=1) { printf("%i: (%s) Wrong number of parameters\n", linen, command.c_str()); return 1; }
		if (argt[0]!=value) { printf("%i: (%s) Wrong type of parameters\n", command.c_str(), linen); return 1; }
		interrupts[argv[0]] = progln;
		if ((argv[0]+1) > interruptsn) interruptsn = argv[0]+1;
		return 0;
	}
	if (command.compare("jump")==0)
	{
		if (argn!=2) { printf("%i: (%s) Wrong number of parameters\n", linen, command.c_str()); return 1; }
		if (argt[1]!=value) { printf("%i: (%s) Wrong type of parameters\n", command.c_str(), linen); return 1; }
		wb(jump);
		wb(argt[0]);
		wb(argv[0]);
		wb(argv[1]);
		return 0;
	}
	if (command.compare("cmpjump")==0)
	{
		if (argn!=4) { printf("%i: (%s) Wrong number of parameters\n", linen, command.c_str()); return 1; }
		if (argt[3]!=value) { printf("%i: (%s) Wrong type of parameters\n", command.c_str(), linen); return 1; }
		wb(cmpjump);
		wb(argt[0]);
		wb(argv[0]);
		wb(argt[1]);
		wb(argv[1]);
		wb(argt[2]);
		wb(argv[2]);
		wb(argv[3]);
		return 0;
	}
	if (command.compare("ret")==0)
	{
		if (argn!=0) { printf("%i: (%s) Wrong number of parameters\n", linen, command.c_str()); return 1; }
		wb(_ret);
		return 0;
	}
	if (command.compare("debug")==0)
	{
		if (argn!=1) { printf("%i: (%s) Wrong number of parameters\n", linen, command.c_str()); return 1; }
		wb(debug);
		wb(argt[0]);
		wb(argv[0]);
		return 0;
	}
	if (command.compare("error")==0)
	{
		if (argn!=1) { printf("%i: (%s) Wrong number of parameters\n", linen, command.c_str()); return 1; }
		wb(error);
		wb(argt[0]);
		wb(argv[0]);
		return 0;
	}
	if (command.compare("sint")==0)
	{
		if (argn!=1) { printf("%i: (%s) Wrong number of parameters\n", linen, command.c_str()); return 1; }
		wb(sint);
		wb(argt[0]);
		wb(argv[0]);
		return 0;
	}
	if (command.compare("int")==0)
	{
		wb(_int);
		for (int i=0; i<8; i++)
		{
			wb(argt[i]);
			wb(argv[i]);
		}
		return 0;
	}
	if (command.compare("exit")==0)
	{
		if (argn!=0) { printf("%i: (%s) Wrong number of parameters\n", linen, command.c_str()); return 1; }
		wb(exit);
		return 0;
	}
}

/*
	Main
*/
int main (int argc, char** argv)
{
	bool silent=false;
	bool clean=false;
	
	if(cmdOptionExists(argv, argv+argc, "-i")==false)
	{
		printf("SASM Compiler\n ver %s by Gi@cky98\n\n", VERSION_STR);
		printf("Usage: %s -i input.sasm [-o output.sbi] [-s] [-cl]\n\n", (char*)argv[0]);
		printf("-i input.sasm\t\tSASM assembly file to compile\n");
		printf("-o output.sbi\t\tSBI output filename\n");
		printf("-s\t\t\tSilent mode\n");
		printf("-cl\t\t\tClean non-SBI files used during compilation\n");
		return 1;
	}

	for (byte i=0; i<255; i++) labels[i]=0;
	
	inname = getCmdOption(argv, argv + argc, "-i");
  fstream file(inname, ios::in);

  if(cmdOptionExists(argv, argv + argc, "-o"))
  	outname = getCmdOption(argv, argv + argc, "-o");
  else
  	outname = (char*)"out.sbi";
	
	char buf[128];
  strcpy(buf, outname);
  strcat(buf, ".prg");
 	prgname = (char*)buf;
	
 	f.open(outname, ios::out | ios::binary);
 	fp.open(prgname, ios::out | ios::binary);

  if (!file) { printf("Can't open .SASM file for reading!\n"); return 1; }
  if (!f) { printf("Can't open .SBI file for writing!\n"); return 1; }
  if (!fp) { printf("Can't open .SBI.PRG file for writing!\n"); return 1; }
  
  if(cmdOptionExists(argv, argv + argc, "-s")) silent = true;
  if(cmdOptionExists(argv, argv + argc, "-cl")) clean = true;
  
  if (silent==false) printf("SASMC %s :: Assembling %s :: Please wait... ", VERSION_STR, inname);
  
  string line;
	
	while(std::getline(file, line))
	{
    Tokenizer str;
    string token;
    string command;
    vector<string> tokens(16);
    int cnt = 0;
    int ret;
    
    if (line.find(';')!=-1) line = line.substr(0, line.find(';'));
    if (line!="")
    {
    str.set(line, " \t");
    
    while((token = str.next()) != "")
    {
      if (cnt==0) command = token; else tokens[cnt-1] = token;
      cnt++;
    }
    
    int nt = cnt-1;
    ret = pline(command.c_str(), nt, tokens);
    if (ret>0) { f.close(); fp.close(); file.close(); remove(outname); remove(prgname); return 1; }
		}
		
		linen++;
	}
	
	wsbi();
	
	if (silent==false) printf("Done!\n");
	
	f.close();
	fp.close();
	file.close();
	
	if (clean==true)
	{
		if (silent==false) printf("SASMC %s :: Cleaning %s :: Please wait... ", VERSION_STR, prgname);
		remove(prgname);
		if (silent==false) printf("Done!\n");
	}
	
  return 0;
}
