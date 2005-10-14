// convert.cpp : Defines the entry point for the console application.
//

#include <stdafx.h>
#include <stdio.h>
#include <afx.h>

#include "string.h"
#include <iostream>
#include <fstream.h>
#include <math.h>


#define LINE_IGNORE 1
#define BigTimeGap  60		//60 secs

struct element
{
	long lat;
	long lng;
	unsigned long time;
	unsigned char marker;
};

class GPSProcess
{
  public:

	GPSProcess();
	void Process(CString filename, int filter,int winSample, int winSize);

  private:

    bool GPSProcess::ReadFile (CString fname);
	bool GetLine(ifstream inStrm, CString &theLine);
	CString UTC(unsigned long);
	char LatitudeI(long v);
	char LongitudeI(long v);
	CString GenerateGPRMC(struct element data);
	double LatLong(long v);
	bool WindowData(unsigned int idx,int winSample, int winSize);
	unsigned char Checksum(CString line);
	CString OutFilename(CString infile);

  private:

	struct element data[30000];

	unsigned long elements;
	unsigned long ignored;

};

//-----------------------------------------------------------------------------

int main(int argc, char* argv[])
{
	
	if (argc < 3)
	{
		cerr << "\nUsage "<< argv[0] << " <input file> <filter value> [<Window Sample> <Window Size>]\n\n";
		cerr << "Filter value: results in every <filter value> being output\n";
		cerr << "              eg, a filter of 5 keeps only 1 in 5 values.\n";
		cerr << "              0 value is no filtering\n";
		cerr << "If <Window Sample> number of sucessive elements are within <Window Size>\n";
		cerr << "lag/long units they are excluded from output. size=10 is approx .7m\n";
		cerr << "defaults to sample=10, size=40\n";
		return 1;
	}
	int winSamp = 10;
	int winSize = 40;
	if (argc==5)
	{
		winSamp = atoi(argv[3]);
		winSize = atoi(argv[4]);
	}
	else
		cerr <<" No Window Sample & Size, defaults to " << winSamp <<" "<< winSize << "\n";

	GPSProcess processor;

	processor.Process(argv[1], atoi(argv[2]), winSamp, winSize);

	return 0;
}
//-----------------------------------------------------------------------------

GPSProcess::GPSProcess()
{
	elements=0;
	ignored=0;
}
//-----------------------------------------------------------------------------
//filter is every nth item to output. 0 is no filter.
//eg filter = 5 would output every 5th value.

void GPSProcess::Process(CString filename, int filter, int winSample, int winSize)
{
	ofstream outStrm;
	unsigned long lastTime;
	int  filterCount =0;			//num points filtered since last output
	int kept=0;

	// read file into  buffer

	if (!ReadFile(filename))
		return;

	//open output file

	outStrm.open(OutFilename(filename), ofstream::trunc   );

	if (!outStrm)
	{
		cerr << "Failed to open out file";
		return;
	}


	// start the processing

	lastTime = data[0].time;

	for (int i=0;i<elements;i++)
	{
		cerr  << GenerateGPRMC(data[i]) <<"  ";

		if (data[i].marker & LINE_IGNORE)		//if ignore set, dont output line
		{
			cerr << " skipped \n";
			filterCount =99999;				//force output after run of ignore's.
			continue;
		}

		bool windowed = WindowData(i,winSample, winSize);	//look @ i+1 to window size, set ignore flg if all in window

		if ((data[i].time - lastTime) > BigTimeGap )
			cerr << "Big Time Gap " << (data[i].time - lastTime) << " secs\n";
		lastTime = data[i].time;
	
		filterCount++;

		// if fileterd out through windowing, then always output the current line (1st of window)
		// if reached filterCount number of points then output line & reset counter.

		if (windowed || ( filterCount >= filter))
		{
			cerr << "****\n";

			outStrm << GenerateGPRMC(data[i]) <<"\xD\xA";
			kept++;
			filterCount=0;
		}
		else
		{
			cerr <<"filterd\n" ;
		}
	}

	outStrm.flush();
	outStrm.close();

	cerr <<"\ntot=" <<elements << " ign=" << ignored << " kept=" << kept <<'\n';

}
//-----------------------------------------------------------------------------
// create outfilename.
// use basename and add ".nmea"

CString GPSProcess::OutFilename(CString infile)
{

	int idx =infile.ReverseFind('.');
	if (idx != -1)
		infile = infile.Left(idx);

	infile +=".nmea";

	cerr <<"output filename: "<< infile <<"\n";
	return infile;
}

//-----------------------------------------------------------------------------

bool GPSProcess::ReadFile (CString fname)
{
	ifstream inStrm;
	CString line;

	inStrm.open(fname,ifstream::nocreate);
	if (!inStrm)
	{
		cerr << "failed to open input file";
		return false;
	}

	while (true)
	{
		if (!GetLine(inStrm,line))
			break;
		data[elements].lat = atol(line.Mid(8));
		data[elements].lng = atol(line.Mid(17));
		data[elements].time= atol(line.Mid(1));
		data[elements].marker=0;
		elements++;
	}

	inStrm.close();
	return true;

}

//-----------------------------------------------------------------------------

bool GPSProcess::GetLine(ifstream inStrm,CString &theLine)
{
	static char line[256];

	while (true)
	{
		inStrm.getline(line,sizeof(line));

		if ((line[0]=='*') || (line[0]=='>'))
		{
			theLine = line;
			return true;
		}

		if (inStrm.eof())
			return false;
	}
}
//-----------------------------------------------------------------------------
// returns true of following windowSample points are all in windowSize.
// the current point at [idx] isn't set to ignore, only the following WinSample elements.

bool GPSProcess::WindowData(unsigned int idx, int winSample, int winSize)
{

	int i;
	bool zeroMatch=true;
	int  zeroMatched=0;

	double lat0 =data[idx].lat;
	double lng0 =data[idx].lng;


	// look through next sample, if all inside window then set ignore flag.
	// current one is not ignored. 

	idx++;						//dont ignore current.
	
	for (i=0;i<winSample;i++)
	{
		if (idx+i > elements)
			break;

		double lat1 =data[idx+i].lat;
		double lng1 =data[idx+i].lng;

		double len = sqrt ( (lat0-lat1)*(lat0-lat1) + (lng0-lng1)*(lng0-lng1));
		cerr << (int)len << " ";

		//ignore run length of 0 differences

		if ((int)len>0)					
			zeroMatch=false;

		if (zeroMatch)
		{
			data[idx+i].marker|=LINE_IGNORE;
			zeroMatched++;
		}

		if (len > winSize)
			break;
	}
	
	// if all points in window, ignore them

	if (i==winSample)
	{
		for (int j=0;j<winSample;j++)
		{
			data[idx+j].marker |= LINE_IGNORE;				
		}
		ignored +=winSample;
		cerr << " wnd skip " <<winSample <<" ";
		return true;
	}

	if (zeroMatched>0)
	{
		cerr << " identicals skiped "<< zeroMatched <<" ";
		ignored +=zeroMatched;
	}

	return false;
}
//-----------------------------------------------------------------------------

//in eg
//>047875 54243843 -3119846 00001870 0  SD  27033  83725
// so -3119846 is -3.119846 degrees.
//
//out egs
//$GPRMC,222712.02,A,5414.6304,N,00307.1916,W,000.0,328.5,281204,004.2,W,A*38
//$GPRMC,222713.02,A,5414.6303,N,00307.1915,W,000.0,328.5,281204,004.2,W,A*3D
//$GPRMC,222714.02,A,5414.6304,N,00307.1915,W,000.0,328.5,281204,004.2,W,A*3D

CString GPSProcess::GenerateGPRMC(struct element data)
{

	CString msg;
	CString check;

	msg.Format("$GPRMC,%s,A,%09.4f,%c,%010.4f,%c,000.0,000.0,%s,000.0,W,A*",
				UTC(data.time),
				fabs(LatLong(data.lat)),
				LatitudeI(data.lat),
				fabs(LatLong(data.lng)),
				LongitudeI(data.lng),
				"010203");

	check.Format("%02X",Checksum(msg));
	msg += check;

	return msg;

}
//-----------------------------------------------------------------------------
// time in seconds to UTC hhmmss.ss  (.ss is always 0, as data resolution is 1 sec)
//make sec 0.5 as Anquet has a bug with times that are bang on the hour!!

CString GPSProcess::UTC(unsigned long time)
{
	CString s;

	int hrs = time /3600;
	int mins= (time % 3600) /60;
	int secs= time%60;
	
	s.Format("%02d%02d%02d.50",hrs,mins,secs);

	return s;
}
//-----------------------------------------------------------------------------

char GPSProcess::LatitudeI(long v)
{
	return v < 0 ? 'S' : 'N';
}
//-----------------------------------------------------------------------------

char GPSProcess::LongitudeI(long v)
{
	return v < 0 ? 'W' : 'E';
}
//-----------------------------------------------------------------------------
//data obtained from GPS unit was converted from dddmm.mmmm to ddd.dddddd (minutes to fractions of degree)
//it now needs to be converted back.ig degrees ddd.dddddd to dddmm.mmmm.

double GPSProcess::LatLong(long v)
{
	long r = v%1000000L;
	long m = r*6/10;
	m+=(v-r);
	return m/10000.0;
	
}
//-----------------------------------------------------------------------------

unsigned char  GPSProcess::Checksum(CString msg)
{
	int i =1;		// first index after $

	unsigned char check=msg[i++];	
	
	while (msg[i])
	{
		check ^= msg[i++];
		if (msg[i] == '*')			//excludes '*'
			break;
	}
	return check;
}

