#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <algorithm>
#include <iostream>
#include <fstream>
#include "tokenizer.h"

using namespace std;
using std::string;
using std::cout;
using std::endl;

typedef unsigned char byte;

/*
	SBI Format
*/
#define HEADER_0				0xAA
#define HEADER_1				0x2B
//#define HEADER_1				0x1B // Old version

#define FOOTER_0				0x3A
#define FOOTER_1				0xF0

/*
	SBI instructions
*/
#define assign					0x01
#define move						0x02
#define add							0x10
#define sub							0x11
#define mul							0x12
#define div							0x13
#define incr						0x20
#define decr						0x21
#define inv							0x22
#define tob							0x23
#define cmp							0x30
#define high						0x31
#define low							0x32
#define label						0x40
#define jump						0x41
#define cmpjump					0x42
#define	_ret						0x43
#define debug						0x50
#define debugstr				0x51
#define error						0x52
#define sint						0x60
#define _int						0x61
#define exit						0xFF

#define varid						0x04
#define value						0xF4

/*
	Program global variables
*/
fstream f;
int linen=1;
char* inname;
char* outname;

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

void wb(byte b)
{
	char buf[1] = { b };
	f.write(buf, 1);
}

void endsbi(void)
{
	char buf[2] = { FOOTER_0, FOOTER_1 };
	f.write(buf, 2);
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
		wb(label);
		wb(argv[0]);
		return 0;
	}
	if (command.compare("jump")==0)
	{
		if (argn!=1) { printf("%i: (%s) Wrong number of parameters\n", linen, command.c_str()); return 1; }
		wb(jump);
		wb(argt[0]);
		wb(argv[0]);
		return 0;
	}
	if (command.compare("cmpjump")==0)
	{
		if (argn!=3) { printf("%i: (%s) Wrong number of parameters\n", linen, command.c_str()); return 1; }
		wb(cmpjump);
		wb(argt[0]);
		wb(argv[0]);
		wb(argt[1]);
		wb(argv[1]);
		wb(argt[2]);
		wb(argv[2]);
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
		wb(exit);
		return 0;
	}
}

/*
	MAIN
*/
int main (int argc, char** argv)
{
	bool silent=false;
	
	if(cmdOptionExists(argv, argv+argc, "-i")==false)
	{
		printf("SASM Compiler\n ver 0.2 by Gi@cky98\n\n");
		printf("Usage: %s -i input.sasm [-o output.sbi] [-s]\n\n", (char*)argv[0]);
		printf("-i input.sasm\t\tSASM assembly file to compile\n");
		printf("-o output.sbi\t\tSBI output filename\n");
		printf("-s\t\t\tSilent mode\n");
		return 1;
	}
	
	inname = getCmdOption(argv, argv + argc, "-i");
  fstream file(inname, ios::in);
  
  if(cmdOptionExists(argv, argv + argc, "-o"))
  	outname = getCmdOption(argv, argv + argc, "-o");
  else
  	outname = (char*)"out.sbi";
 	
 	f.open(outname, ios::out | ios::binary);
	
  if (!file) { printf("Can't open SASM file for reading!\n"); return 1; }
  if (!f) { printf("Can't open SBI file for writing!\n"); return 1; }
  
  if(cmdOptionExists(argv, argv + argc, "-s")) silent = true;
  
  if (silent==false) printf("SASMC 0.2 :: Assembling %s :: Please wait... ", inname);
  
  beginsbi();
  
  string line;
	
	while(std::getline(file, line))
	{
    Tokenizer str;
    string token;
    string command;
    vector<string> tokens(16);
    int cnt = 0;
    int ret;
    
    str.set(line, " \t");
    
    while((token = str.next()) != "")
    {
      if (cnt==0) command = token; else tokens[cnt-1] = token;
      cnt++;
    }
    
    ret = pline(command.c_str(), cnt-1, tokens);
    if (ret>0) { f.close(); file.close(); remove(outname); return 1; }
		linen++;
	}
	
	if (silent==false) printf("Done!\n");
	
	f.close();
	file.close();
	
  return 0;
}
