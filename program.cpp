//============================================================================
// Name        : program.cpp
// Author      : Bo Yu
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
#include <vector>
#include <algorithm>
using namespace std;
#include <math.h>
#include <sys/time.h>	// gettimeofday()

//
// Global variables.
//

const int DISK_REQUEST_SIZE = 10000;
const int MAX_DISK_TRACK = 65536;	// 4096 * 8 * 2

int rand_int ( int max )
{
	return rand() % max;
}

int rand_int ( int min, int max )
{
	return rand_int ( max - min ) + min;
}

int Fibonacci ( int number )
{
	if ( number == 0 || number == 1 )
	{
		return number;
	}
	else
	{
		return Fibonacci ( number - 1 ) + Fibonacci ( number - 2 );
	}
}

static struct timeval s_tvStart;

void StartTimer()
{
	gettimeofday ( &s_tvStart, NULL );
}

double GetTimer()
{
	struct timeval tvFinish;
	long sec, usec;

	gettimeofday ( &tvFinish, NULL );
	sec = tvFinish.tv_sec - s_tvStart.tv_sec;
	usec = tvFinish.tv_usec - s_tvStart.tv_usec;

	return ( 1e+3*sec + 1e-3*usec );	// millisec
}

//
// Driver generated variables.
// Simulate track access in three scenarios : sequential access, localize access, random access.
//

vector<int> g_vecDiskTracksForIoBoundWithSequentialAccess (DISK_REQUEST_SIZE);
vector<int> g_vecDiskTracksForBurstyIoWithLocalizeAccess (DISK_REQUEST_SIZE);
vector<int> g_vecDiskTracksForCpuBoundWithRandomAccess (DISK_REQUEST_SIZE);

//
// Driver related functions.
//

void SimulateIoBoundWithSequentialAccess ()
{
	for ( int i = 0; i < DISK_REQUEST_SIZE; i++ )
	{
		g_vecDiskTracksForIoBoundWithSequentialAccess [i] = i+1000;
	}
}

//
//	|-----|     |-----|     |-----|
//	|	  |     |     |     |     |
//	|-----|-----|-----|-----|-----| tracks
//

void SimulateBurstyIoWithLocalizeAccess ()
{
	int iOffset = g_vecDiskTracksForBurstyIoWithLocalizeAccess.size() / 5;
	int iIndent = 0;

	// Beautiful code~~~
	for ( unsigned int i = 0; i < g_vecDiskTracksForBurstyIoWithLocalizeAccess.size(); i++ )
	{
		// Every bursty offset differ 1000.
		if ( i % iOffset == 0 )
			iIndent += 1000;

		g_vecDiskTracksForBurstyIoWithLocalizeAccess[i] = ( rand_int ( iIndent-50, iIndent+50 ) );
	}

}

void SimulateCpuBoundWithRandomAccess ()
{
	for ( int i = 0; i < DISK_REQUEST_SIZE; i++ )
	{
		g_vecDiskTracksForCpuBoundWithRandomAccess [i] = (rand() % MAX_DISK_TRACK);
	}
}

void PrintRequestsForSequ ()
{
	cout << "PRINT REQUESTS FOR SEQUENTIAL ACCESS\n";
	for ( unsigned int i = 0; i < g_vecDiskTracksForIoBoundWithSequentialAccess.size(); i++ )
		cout << "[ " << i << " ] REQUEST TRACK = " << g_vecDiskTracksForIoBoundWithSequentialAccess[i] << endl;
}

void PrintRequestsForLoca ()
{
	cout << "PRINT REQUESTS FOR LOCALIZE ACCESS\n";
	for ( unsigned int i = 0; i < g_vecDiskTracksForBurstyIoWithLocalizeAccess.size(); i++ )
		cout << "[ " << i << " ] REQUEST TRACK = " << g_vecDiskTracksForBurstyIoWithLocalizeAccess[i] << endl;
}

void PrintRequestsForRand ()
{
	cout << "PRINT REQUESTS FOR RANDOM ACCESS\n";
	for ( unsigned int i = 0; i < g_vecDiskTracksForCpuBoundWithRandomAccess.size(); i++ )
		cout << "[ " << i << " ] REQUEST TRACK = " << g_vecDiskTracksForCpuBoundWithRandomAccess[i] << endl;
}

//
// FCFS output variables.
//

vector<double> g_vecFCFSSeekTimeForSequentialAccess (DISK_REQUEST_SIZE);
vector<double> g_vecFCFSSeekTimeForLocalizeAccess (DISK_REQUEST_SIZE);
vector<double> g_vecFCFSSeekTimeForRandomAccess (DISK_REQUEST_SIZE);

vector<double> g_vecFCFSIoTimeForSequentialAccess (DISK_REQUEST_SIZE);
vector<double> g_vecFCFSIoTimeForLocalizeAccess (DISK_REQUEST_SIZE);
vector<double> g_vecFCFSIoTimeForRandomAccess (DISK_REQUEST_SIZE);

vector<double> g_vecFCFSDistanceForSequentialAccess (DISK_REQUEST_SIZE);
vector<double> g_vecFCFSDistanceForLocalizeAccess (DISK_REQUEST_SIZE);
vector<double> g_vecFCFSDistanceForRandomAccess (DISK_REQUEST_SIZE);

double g_dFCFSTotalSeekTimeForSequentialAccess = 0;
double g_dFCFSTotalSeekTimeForLocalizeAccess = 0;
double g_dFCFSTotalSeekTimeForRandomAccess = 0;

double g_dFCFSTotalIoTimeForSequentialAccess = 0;
double g_dFCFSTotalIoTimeForLocalizeAccess = 0;
double g_dFCFSTotalIoTimeForRandomAccess = 0;

double g_dFCFSTotalDistanceForSequentialAccess = 0;
double g_dFCFSTotalDistanceForLocalizeAccess = 0;
double g_dFCFSTotalDistanceForRandomAccess = 0;

//
// FCFS related functions.
//

//
// Seek time 	: the time necessary to move the disk arm to the desired cylinder.
// Formula   	: (4 e -3) msec * | TrackX - TrackY | + 1 msec
//

void FCFS ()
{
	double dTrackPrev;
	unsigned int i;
	double dIoTime = 0;

	//
	// For sequential access.
	//

	dTrackPrev = 0;
	for ( i = 0; i < g_vecFCFSSeekTimeForSequentialAccess.size(); i++ )
	{
		// Calculate seek time.
		g_vecFCFSSeekTimeForSequentialAccess[i] = (4e-3) * fabs(dTrackPrev - g_vecDiskTracksForIoBoundWithSequentialAccess[i]) + 1;

		// simulate do some IO operation.
		StartTimer();
		Fibonacci (10);
		dIoTime = GetTimer();

		// Calculate I/O time.
		g_vecFCFSIoTimeForSequentialAccess[i] = g_vecFCFSSeekTimeForSequentialAccess[i] + 0.5 * ( (60 * 1e+3) / 7200 ) + dIoTime;

		// Calculate Distance.
		g_vecFCFSDistanceForSequentialAccess[i] = fabs (g_vecDiskTracksForIoBoundWithSequentialAccess[i] - dTrackPrev );

		dTrackPrev = g_vecDiskTracksForIoBoundWithSequentialAccess[i];
	}

	//
	// For localize access.
	//

	dTrackPrev = 0;
	for ( i = 0; i < g_vecFCFSSeekTimeForLocalizeAccess.size(); i++ )
	{
		// Calculate seek time.
		g_vecFCFSSeekTimeForLocalizeAccess[i] = (4e-3) * fabs(dTrackPrev - g_vecDiskTracksForBurstyIoWithLocalizeAccess[i]) + 1;

		// simulate do some IO operation.
		StartTimer();
		Fibonacci (10);
		dIoTime = GetTimer();

		// Calculate I/O time.
		g_vecFCFSIoTimeForLocalizeAccess[i] = g_vecFCFSSeekTimeForLocalizeAccess[i] + 0.5 * ( (60 * 1e+3) / 7200 ) + dIoTime;

		// Calculate distance.
		g_vecFCFSDistanceForLocalizeAccess[i] = fabs ( g_vecDiskTracksForBurstyIoWithLocalizeAccess[i] - dTrackPrev );

		dTrackPrev = g_vecDiskTracksForBurstyIoWithLocalizeAccess[i];
	}

	//
	// For random access.
	//

	dTrackPrev = 0;
	for ( i = 0; i < g_vecFCFSSeekTimeForRandomAccess.size(); i++ )
	{
		// Calculate seek time.
		g_vecFCFSSeekTimeForRandomAccess[i] = (4e-3) * fabs(dTrackPrev - g_vecDiskTracksForCpuBoundWithRandomAccess[i]) + 1;

		// simulate do some IO operation.
		StartTimer();
		Fibonacci (10);
		dIoTime = GetTimer();

		// Calculate I/O time.
		g_vecFCFSIoTimeForRandomAccess[i] = g_vecFCFSSeekTimeForRandomAccess[i] + 0.5 * ( (60 * 1e+3) / 7200 ) + dIoTime;

		// Calculate distance.
		g_vecFCFSDistanceForRandomAccess[i] = fabs ( g_vecDiskTracksForCpuBoundWithRandomAccess[i] - dTrackPrev );

		dTrackPrev = g_vecDiskTracksForCpuBoundWithRandomAccess[i];
	}
}

//
// Total time to complete the given I/O operations include: seektime + rotational latency + I/O
// Formula : Total I/O = Seek time + 0.5*(60*1e+3/7200)msec + I/O
//

void PrintFCFSData ()
{
	unsigned int i = 0;

	//
	// SEEK TIME.
	//

	cout << "PRINT FCFS SEEK TIME FOR SEQUENTIAL ACCESS.\n";
	for ( i = 0; i < g_vecFCFSSeekTimeForSequentialAccess.size(); i++ )
	{
		cout << "[ " << i << " ] SEEK TIME = " << g_vecFCFSSeekTimeForSequentialAccess[i] << " millisec "<< endl;
		g_dFCFSTotalSeekTimeForSequentialAccess += g_vecFCFSSeekTimeForSequentialAccess[i];
	}
	cout << "TOTAL = " << g_dFCFSTotalSeekTimeForSequentialAccess << endl;
	cout << endl;

	cout << "PRINT FCFS SEEK TIME FOR LOCALIZE ACCESS.\n";
	for ( i = 0; i < g_vecFCFSSeekTimeForLocalizeAccess.size(); i++ )
	{
		cout << "[ " << i << " ] SEEK TIME = " << g_vecFCFSSeekTimeForLocalizeAccess[i] << " millisec " << endl;
		g_dFCFSTotalSeekTimeForLocalizeAccess += g_vecFCFSSeekTimeForLocalizeAccess[i];
	}
	cout << "TOTAL = " << g_dFCFSTotalSeekTimeForLocalizeAccess << endl;
	cout << endl;

	cout << "PRINT FCFS SEEK TIME FOR RANDOM ACCESS.\n";
	for ( i = 0; i < g_vecFCFSSeekTimeForRandomAccess.size(); i++ )
	{
		cout << "[ " << i << " ] SEEK TIME = " << g_vecFCFSSeekTimeForRandomAccess[i] << " millisec " << endl;
		g_dFCFSTotalSeekTimeForRandomAccess += g_vecFCFSSeekTimeForRandomAccess[i];
	}
	cout << "TOTAL = " << g_dFCFSTotalSeekTimeForRandomAccess << endl;
	cout << endl;

	//
	// I/O TIME
	//

	cout << "PRINT FCFS I/O OPERATION TIME FOR SEQUENTIAL ACCESS.\n";
	for ( i = 0; i < g_vecFCFSIoTimeForSequentialAccess.size(); i++ )
	{
		cout << "[ " << i << " ] I/O OPERATION TIME = " << g_vecFCFSIoTimeForSequentialAccess[i] << " millisec "<< endl;
		g_dFCFSTotalIoTimeForSequentialAccess += g_vecFCFSIoTimeForSequentialAccess[i];
	}
	cout << "TOTAL = " << g_dFCFSTotalIoTimeForSequentialAccess << endl;
	cout << endl;

	cout << "PRINT FCFS I/O OPERATION TIME FOR LOCALIZE ACCESS.\n";
	for ( i = 0; i < g_vecFCFSIoTimeForLocalizeAccess.size(); i++ )
	{
		cout << "[ " << i << " ] LOCALIZE ACCESS TIME = " << g_vecFCFSIoTimeForLocalizeAccess[i] << " millisec "<< endl;
		g_dFCFSTotalIoTimeForLocalizeAccess += g_vecFCFSIoTimeForLocalizeAccess[i];
	}
	cout << "TOTAL = " << g_dFCFSTotalIoTimeForLocalizeAccess << endl;
	cout << endl;

	cout << "PRINT FCFS I/O OPERATION TIME FOR RANDOM ACCESS.\n";
	for ( i = 0; i < g_vecFCFSIoTimeForRandomAccess.size(); i++ )
	{
		cout << "[ " << i << " ] RANDOM ACCESS TIME = " << g_vecFCFSIoTimeForRandomAccess[i] << " millisec "<< endl;
		g_dFCFSTotalIoTimeForRandomAccess += g_vecFCFSIoTimeForRandomAccess[i];
	}
	cout << "TOTAL = " << g_dFCFSTotalIoTimeForRandomAccess << endl;
	cout << endl;

	//
	// DISTANCE.
	//

	cout << "PRINT FCFS DISTANCE FOR SEQUENTIAL ACCESS.\n";
	for ( i = 0; i < g_vecFCFSDistanceForSequentialAccess.size(); i++ )
	{
		cout << "[ " << i << " ] DISTANCE = " << g_vecFCFSDistanceForSequentialAccess[i] << endl;
		g_dFCFSTotalDistanceForSequentialAccess += g_vecFCFSDistanceForSequentialAccess[i];
	}
	cout << "TOTAL = " << g_dFCFSTotalDistanceForSequentialAccess << endl;
	cout << endl;

	cout << "PRINT FCFS DISTANCE FOR LOCALIZE ACCESS.\n";
	for ( i = 0; i < g_vecFCFSDistanceForLocalizeAccess.size(); i++ )
	{
		cout << "[ " << i << " ] DISTANCE = " << g_vecFCFSDistanceForLocalizeAccess[i] << endl;
		g_dFCFSTotalDistanceForLocalizeAccess += g_vecFCFSDistanceForLocalizeAccess[i];
	}
	cout << "TOTAL = " << g_dFCFSTotalDistanceForLocalizeAccess << endl;
	cout << endl;

	cout << "PRINT FCFS DISTANCE FOR RANDOM ACCESS.\n";
	for ( i = 0; i < g_vecFCFSDistanceForRandomAccess.size(); i++ )
	{
		cout << "[ " << i << " ] DISTANCE = " << g_vecFCFSDistanceForRandomAccess[i] << endl;
		g_dFCFSTotalDistanceForRandomAccess += g_vecFCFSDistanceForRandomAccess[i];
	}
	cout << "TOTAL = " << g_dFCFSTotalDistanceForRandomAccess << endl;
	cout << endl;
}

//
// SSTF variables.
//

vector<double> g_vecSSTFSeekTimeForSequentialAccess (DISK_REQUEST_SIZE);
vector<double> g_vecSSTFSeekTimeForLocalizeAccess (DISK_REQUEST_SIZE);
vector<double> g_vecSSTFSeekTimeForRandomAccess (DISK_REQUEST_SIZE);

vector<double> g_vecSSTFIoTimeForSequentialAccess (DISK_REQUEST_SIZE);
vector<double> g_vecSSTFIoTimeForLocalizeAccess (DISK_REQUEST_SIZE);
vector<double> g_vecSSTFIoTimeForRandomAccess (DISK_REQUEST_SIZE);

vector<double> g_vecSSTFDistanceForSequentialAccess (DISK_REQUEST_SIZE);
vector<double> g_vecSSTFDistanceForLocalizeAccess (DISK_REQUEST_SIZE);
vector<double> g_vecSSTFDistanceForRandomAccess (DISK_REQUEST_SIZE);

double g_dSSTFTotalSeekTimeForSequentialAccess = 0;
double g_dSSTFTotalSeekTimeForLocalizeAccess = 0;
double g_dSSTFTotalSeekTimeForRandomAccess = 0;

double g_dSSTFTotalIoTimeForSequentialAccess = 0;
double g_dSSTFTotalIoTimeForLocalizeAccess = 0;
double g_dSSTFTotalIoTimeForRandomAccess = 0;

double g_dSSTFTotalDistanceForSequentialAccess = 0;
double g_dSSTFTotalDistanceForLocalizeAccess = 0;
double g_dSSTFTotalDistanceForRandomAccess = 0;

//
// SSTF functions.
//

//
// Strategy 	: Select the nearest request from the current track and mark it TREATED.
//
// Seek Time 	: the time necessary to move the disk arm to the desired cylinder.
// Formula   	: (4 e -3) msec * | TrackX - TrackY | + 1 msec
//
// Total time to complete the given I/O operations include: seektime + rotational latency + I/O
// Formula : Total I/O = Seek time + 0.5*(60*1e+3/7200)msec + I/O
//

enum RequestStatus { UNTREATED, TREATED };

void SSTF ()
{
	unsigned int i = 0;
	unsigned int j = 0;
	int iRequestCount = 0;
	int iShortestRequestLen = MAX_DISK_TRACK;
	int iShortestRequestIndex = 0;
	int iCurrentTrack = 0;
	double dIoTime = 0;

	//
	// Sequential Access.
	//

	// Each disk request maps one boolean element.
	vector<RequestStatus> vecDiskRequestStatus (DISK_REQUEST_SIZE);

	// Initialize them as false (UNTREATED).
	for ( i = 0; i < vecDiskRequestStatus.size(); i++ )
	{
		vecDiskRequestStatus[i] = UNTREATED;
	}

	// Counting request number.
	while ( iRequestCount < g_vecDiskTracksForIoBoundWithSequentialAccess.size() )
	{
		// Select the request with the least seek time from the current head position.
		for ( i = 0; i < g_vecDiskTracksForIoBoundWithSequentialAccess.size(); i++ )
		{
			if ( vecDiskRequestStatus[i] == UNTREATED &&
				 iShortestRequestLen > fabs(iCurrentTrack-g_vecDiskTracksForIoBoundWithSequentialAccess[i]) )
			{
				iShortestRequestLen = fabs(iCurrentTrack-g_vecDiskTracksForIoBoundWithSequentialAccess[i]);
				iShortestRequestIndex = i;
			}
		}

		// Calculate the seek time.
		g_vecSSTFSeekTimeForSequentialAccess [iShortestRequestIndex] = (4e-3)*fabs(iShortestRequestLen)+1;

		// Calculate the IO time.
		StartTimer();
		Fibonacci ( 10 );
		dIoTime = GetTimer();
		g_vecSSTFIoTimeForSequentialAccess [iShortestRequestIndex] =
				g_vecSSTFSeekTimeForSequentialAccess [iShortestRequestIndex] + 0.5 * ( (60 * 1e+3) / 7200 ) + dIoTime;

		// Calculate the distance.
		g_vecSSTFDistanceForSequentialAccess [iShortestRequestIndex] = iShortestRequestLen;

		// marked as TREATED.
		vecDiskRequestStatus[iShortestRequestIndex] = TREATED;

		// renew the current track position.
		iCurrentTrack = g_vecDiskTracksForIoBoundWithSequentialAccess[iShortestRequestIndex];

		iRequestCount++;

		iShortestRequestLen = MAX_DISK_TRACK;
	}

	//
	// Localize Access.
	//

	i = 0;
	j = 0;
	iRequestCount = 0;
	iShortestRequestLen = MAX_DISK_TRACK;
	iShortestRequestIndex = 0;
	iCurrentTrack = 0;
	dIoTime = 0;

	// Initialize them as false (UNTREATED).
	for ( i = 0; i < vecDiskRequestStatus.size(); i++ )
	{
		vecDiskRequestStatus[i] = UNTREATED;
	}

	// Counting request number.
	while ( iRequestCount < g_vecDiskTracksForBurstyIoWithLocalizeAccess.size() )
	{
		// Select the request with the least seek time from the current head position.
		for ( i = 0; i < g_vecDiskTracksForBurstyIoWithLocalizeAccess.size(); i++ )
		{
			if ( vecDiskRequestStatus[i] == UNTREATED &&
				 iShortestRequestLen > fabs(iCurrentTrack-g_vecDiskTracksForBurstyIoWithLocalizeAccess[i]) )
			{
				iShortestRequestLen = fabs(iCurrentTrack-g_vecDiskTracksForBurstyIoWithLocalizeAccess[i]);
				iShortestRequestIndex = i;
			}
		}

		// get the shortest one and calculate the seek time.
		g_vecSSTFSeekTimeForLocalizeAccess [iShortestRequestIndex] = (4e-3)*fabs(iShortestRequestLen)+1;

		// Calculate the IO time.
		StartTimer();
		Fibonacci ( 10 );
		dIoTime = GetTimer();
		g_vecSSTFIoTimeForLocalizeAccess [iShortestRequestIndex] =
				g_vecSSTFSeekTimeForLocalizeAccess [iShortestRequestIndex] + 0.5 * ( (60 * 1e+3) / 7200 ) + dIoTime;

		// Calculate the distance.
		g_vecSSTFDistanceForLocalizeAccess [iShortestRequestIndex] = iShortestRequestLen;

		// marked as TREATED.
		vecDiskRequestStatus[iShortestRequestIndex] = TREATED;

		// renew the current track position.
		iCurrentTrack = g_vecDiskTracksForBurstyIoWithLocalizeAccess[iShortestRequestIndex];

		iRequestCount++;

		iShortestRequestLen = MAX_DISK_TRACK;
	}

	//
	// Random Access.
	//

	i = 0;
	j = 0;
	iRequestCount = 0;
	iShortestRequestLen = MAX_DISK_TRACK;
	iShortestRequestIndex = 0;
	iCurrentTrack = 0;
	dIoTime = 0;

	// Initialize them as false (UNTREATED).
	for ( i = 0; i < vecDiskRequestStatus.size(); i++ )
	{
		vecDiskRequestStatus[i] = UNTREATED;
	}

	// Counting request number.
	while ( iRequestCount < g_vecDiskTracksForCpuBoundWithRandomAccess.size() )
	{
		// Select the request with the least seek time from the current head position.
		for ( i = 0; i < g_vecDiskTracksForCpuBoundWithRandomAccess.size(); i++ )
		{
			if ( vecDiskRequestStatus[i] == UNTREATED &&
				 iShortestRequestLen > fabs(iCurrentTrack-g_vecDiskTracksForCpuBoundWithRandomAccess[i]) )
			{
				iShortestRequestLen = fabs(iCurrentTrack-g_vecDiskTracksForCpuBoundWithRandomAccess[i]);
				iShortestRequestIndex = i;
			}
		}

		// get the shortest one and calculate the seek time.
		g_vecSSTFSeekTimeForRandomAccess [iShortestRequestIndex] = (4e-3)*fabs(iShortestRequestLen)+1;

		// Calculate the IO time.
		StartTimer();
		Fibonacci ( 10 );
		dIoTime = GetTimer();
		g_vecSSTFIoTimeForRandomAccess [iShortestRequestIndex] =
				g_vecSSTFSeekTimeForRandomAccess [iShortestRequestIndex] + 0.5 * ( (60 * 1e+3) / 7200 ) + dIoTime;

		// Calculate the distance.
		g_vecSSTFDistanceForRandomAccess [iShortestRequestIndex] = iShortestRequestLen;

		// marked as TREATED.
		vecDiskRequestStatus[iShortestRequestIndex] = TREATED;

		// renew the current track position.
		iCurrentTrack = g_vecDiskTracksForCpuBoundWithRandomAccess[iShortestRequestIndex];

		iRequestCount++;

		iShortestRequestLen = MAX_DISK_TRACK;
	}
}

void PrintSSTFData ()
{
	unsigned int i = 0;

	//
	// SEEK TIME
	//

	cout << "PRINT SSFT SEEK TIME FOR SEQUENTIAL ACCESS\n";
	for ( i = 0; i < g_vecSSTFSeekTimeForSequentialAccess.size(); i++ )
	{
		cout << "[ " << i << " ] " << g_vecSSTFSeekTimeForSequentialAccess[i] << endl;
		g_dSSTFTotalSeekTimeForSequentialAccess += g_vecSSTFSeekTimeForSequentialAccess[i];
	}
	cout << "TOTAL = " << g_dSSTFTotalSeekTimeForSequentialAccess << endl << endl;

	cout << "PRINT SSFT SEEK TIME FOR LOCALIZE ACCESS\n";
	for ( i = 0; i < g_vecSSTFSeekTimeForLocalizeAccess.size(); i++ )
	{
		cout << "[ " << i << " ] " << g_vecSSTFSeekTimeForLocalizeAccess[i] << endl;
		g_dSSTFTotalSeekTimeForLocalizeAccess += g_vecSSTFSeekTimeForLocalizeAccess[i];
	}
	cout << "TOTAL = " << g_dSSTFTotalSeekTimeForLocalizeAccess << endl << endl;

	cout << "PRINT SSFT SEEK TIME FOR RANDOM ACCESS\n";
	for ( i = 0; i < g_vecSSTFSeekTimeForRandomAccess.size(); i++ )
	{
		cout << "[ " << i << " ] " << g_vecSSTFSeekTimeForRandomAccess[i] << endl;
		g_dSSTFTotalSeekTimeForRandomAccess += g_vecSSTFSeekTimeForRandomAccess[i];
	}
	cout << "TOTAL = " << g_dSSTFTotalSeekTimeForRandomAccess << endl << endl;

	//
	// IO TIME
	//

	cout << "PRINT SSFT IO TIME FOR SEQUENTIAL ACCESS\n";
	for ( i = 0; i < g_vecSSTFIoTimeForSequentialAccess.size(); i++ )
	{
		cout << "[ " << i << " ] " << g_vecSSTFIoTimeForSequentialAccess[i] << endl;
		g_dSSTFTotalIoTimeForSequentialAccess += g_vecSSTFIoTimeForSequentialAccess[i];
	}
	cout << "TOTAL = " << g_dSSTFTotalIoTimeForSequentialAccess << endl << endl;

	cout << "PRINT SSFT IO TIME FOR LOCALIZE ACCESS\n";
	for ( i = 0; i < g_vecSSTFIoTimeForLocalizeAccess.size(); i++ )
	{
		cout << "[ " << i << " ] " << g_vecSSTFIoTimeForLocalizeAccess[i] << endl;
		g_dSSTFTotalIoTimeForLocalizeAccess += g_vecSSTFIoTimeForLocalizeAccess[i];
	}
	cout << "TOTAL = " << g_dSSTFTotalIoTimeForLocalizeAccess << endl << endl;

	cout << "PRINT SSFT IO TIME FOR RANDOM ACCESS\n";
	for ( i = 0; i < g_vecSSTFIoTimeForRandomAccess.size(); i++ )
	{
		cout << "[ " << i << " ] " << g_vecSSTFIoTimeForRandomAccess[i] << endl;
		g_dSSTFTotalIoTimeForRandomAccess += g_vecSSTFIoTimeForRandomAccess[i];
	}
	cout << "TOTAL = " << g_dSSTFTotalIoTimeForRandomAccess << endl << endl;

	//
	// DISTANCE
	//

	cout << "PRINT SSFT DISTANCE FOR SEQUENTIAL ACCESS\n";
	for ( i = 0; i < g_vecSSTFDistanceForSequentialAccess.size(); i++ )
	{
		cout << "[ " << i << " ] " << g_vecSSTFDistanceForSequentialAccess[i] << endl;
		g_dSSTFTotalDistanceForSequentialAccess += g_vecSSTFDistanceForSequentialAccess[i];
	}
	cout << "TOTAL = " << g_dSSTFTotalDistanceForSequentialAccess << endl << endl;

	cout << "PRINT SSFT DISTANCE FOR LOCALIZE ACCESS\n";
	for ( i = 0; i < g_vecSSTFDistanceForLocalizeAccess.size(); i++ )
	{
		cout << "[ " << i << " ] " << g_vecSSTFDistanceForLocalizeAccess[i] << endl;
		g_dSSTFTotalDistanceForLocalizeAccess += g_vecSSTFDistanceForLocalizeAccess[i];
	}
	cout << "TOTAL = " << g_dSSTFTotalDistanceForLocalizeAccess << endl << endl;

	cout << "PRINT SSFT DISTANCE FOR RANDOM ACCESS\n";
	for ( i = 0; i < g_vecSSTFDistanceForRandomAccess.size(); i++ )
	{
		cout << "[ " << i << " ] " << g_vecSSTFDistanceForRandomAccess[i] << endl;
		g_dSSTFTotalDistanceForRandomAccess += g_vecSSTFDistanceForRandomAccess[i];
	}
	cout << "TOTAL = " << g_dSSTFTotalDistanceForRandomAccess << endl << endl;
}

//
// SCAN variables.
//

vector<double> g_vecSCANSeekTimeForSequentialAccess (DISK_REQUEST_SIZE);
vector<double> g_vecSCANSeekTimeForLocalizeAccess (DISK_REQUEST_SIZE);
vector<double> g_vecSCANSeekTimeForRandomAccess (DISK_REQUEST_SIZE);

vector<double> g_vecSCANIoTimeForSequentialAccess (DISK_REQUEST_SIZE);
vector<double> g_vecSCANIoTimeForLocalizeAccess (DISK_REQUEST_SIZE);
vector<double> g_vecSCANIoTimeForRandomAccess (DISK_REQUEST_SIZE);

vector<double> g_vecSCANDistanceForSequentialAccess (DISK_REQUEST_SIZE);
vector<double> g_vecSCANDistanceForLocalizeAccess (DISK_REQUEST_SIZE);
vector<double> g_vecSCANDistanceForRandomAccess (DISK_REQUEST_SIZE);

double g_dSCANTotalSeekTimeForSequentialAccess = 0;
double g_dSCANTotalSeekTimeForLocalizeAccess = 0;
double g_dSCANTotalSeekTimeForRandomAccess = 0;

double g_dSCANTotalIoTimeForSequentialAccess = 0;
double g_dSCANTotalIoTimeForLocalizeAccess = 0;
double g_dSCANTotalIoTimeForRandomAccess = 0;

double g_dSCANTotalDistanceForSequentialAccess = 0;
double g_dSCANTotalDistanceForLocalizeAccess = 0;
double g_dSCANTotalDistanceForRandomAccess = 0;

//
// SCAN Algorithm related functions.
//

//
// Strategy		: In order to simplified the problem, we start process the requests after all of them have been produced.
//
// Seek Time 	: the time necessary to move the disk arm to the desired cylinder.
// Formula   	: (4 e -3) msec * | TrackX - TrackY | + 1 msec
//
// Total time to complete the given I/O operations : seektime + rotational latency + I/O
// Formula 		: Total I/O = Seek time + 0.5*(60*1e+3/7200)msec + I/O
//

void SCAN ()
{
	unsigned int i = 0;
	int iCurrentTrack = 0;
	double dIoTime = 0;

	//
	// Sequential access.
	//

	for ( i = 0; i < g_vecSCANSeekTimeForSequentialAccess.size(); i++ )
	{
		// Calculate the seek time.
		g_vecSCANSeekTimeForSequentialAccess[i] =
				(4e-3) * fabs(iCurrentTrack - g_vecDiskTracksForIoBoundWithSequentialAccess[i]) + 1;

		// Calculate the I/O time.
		StartTimer();
		Fibonacci ( 10 );
		dIoTime = GetTimer();
		g_vecSCANIoTimeForSequentialAccess [i] =
				g_vecSCANSeekTimeForSequentialAccess [i] + 0.5 * ( (60 * 1e+3) / 7200 ) + dIoTime;

		// Calculate the distance.
		g_vecSCANDistanceForSequentialAccess [i] =
				fabs(iCurrentTrack - g_vecDiskTracksForIoBoundWithSequentialAccess[i]);

		iCurrentTrack = g_vecDiskTracksForIoBoundWithSequentialAccess[i];
	}

	//
	// Localize access.
	//

	// Copy requests into a buffer vector.
	vector<int> vecTempRequests ( DISK_REQUEST_SIZE );
	for ( i = 0; i < g_vecDiskTracksForBurstyIoWithLocalizeAccess.size(); i++ )
		vecTempRequests [i] = g_vecDiskTracksForBurstyIoWithLocalizeAccess[i];

	// Sort the vector
	sort ( vecTempRequests.begin(), vecTempRequests.end() );

	iCurrentTrack = 0;

	for ( i = 0; i < g_vecSCANSeekTimeForLocalizeAccess.size(); i++ )
	{
		// Calculate the seek time.
		g_vecSCANSeekTimeForLocalizeAccess[i] =
				(4e-3) * fabs(iCurrentTrack - vecTempRequests[i]) + 1;

		// Calculate the I/O time.
		StartTimer();
		Fibonacci ( 10 );
		dIoTime = GetTimer();
		g_vecSCANIoTimeForLocalizeAccess [i] =
				g_vecSCANSeekTimeForLocalizeAccess [i] + 0.5 * ( (60 * 1e+3) / 7200 ) + dIoTime;

		// Calculate the distance.
		g_vecSCANDistanceForLocalizeAccess [i] = fabs(iCurrentTrack - vecTempRequests[i]);

		iCurrentTrack = vecTempRequests[i];
	}

	//
	// Random access.
	//

	// Copy requests into a buffer vector.
	for ( i = 0; i < g_vecDiskTracksForCpuBoundWithRandomAccess.size(); i++ )
		vecTempRequests [i] = g_vecDiskTracksForCpuBoundWithRandomAccess[i];

	// Sort the vector
	sort ( vecTempRequests.begin(), vecTempRequests.end() );

	iCurrentTrack = 0;

	for ( i = 0; i < g_vecSCANSeekTimeForRandomAccess.size(); i++ )
	{
		// Calculate the seek time.
		g_vecSCANSeekTimeForRandomAccess[i] =
				(4e-3) * fabs(iCurrentTrack - vecTempRequests[i]) + 1;

		// Calculate the IO time.
		StartTimer();
		Fibonacci ( 10 );
		dIoTime = GetTimer();
		g_vecSCANIoTimeForRandomAccess [i] =
				g_vecSCANSeekTimeForRandomAccess [i] + 0.5 * ( (60 * 1e+3) / 7200 ) + dIoTime;

		// Calculate the distance.
		g_vecSCANDistanceForRandomAccess [i] = fabs(iCurrentTrack - vecTempRequests[i]);

		iCurrentTrack = vecTempRequests[i];
	}
}

void PrintSCANData ()
{
	unsigned int i = 0;

	//
	// SEEK TIME
	//

	cout << "PRINT SCAN SEEK TIME FOR SEQUENTIAL ACCESS\n";
	for ( i = 0; i < g_vecSCANSeekTimeForSequentialAccess.size(); i++ )
	{
		cout << "[ " << i << " ] " << g_vecSCANSeekTimeForSequentialAccess[i] << endl;
		g_dSCANTotalSeekTimeForSequentialAccess += g_vecSCANSeekTimeForSequentialAccess[i];
	}
	cout << "TOTAL = " << g_dSCANTotalSeekTimeForSequentialAccess << endl << endl;

	cout << "PRINT SCAN SEEK TIME FOR LOCALIZE ACCESS\n";
	for ( i = 0; i < g_vecSCANSeekTimeForLocalizeAccess.size(); i++ )
	{
		cout << "[ " << i << " ] " << g_vecSCANSeekTimeForLocalizeAccess[i] << endl;
		g_dSCANTotalSeekTimeForLocalizeAccess += g_vecSCANSeekTimeForLocalizeAccess[i];
	}
	cout << "TOTAL = " << g_dSCANTotalSeekTimeForLocalizeAccess << endl << endl;

	cout << "PRINT SCAN SEEK TIME FOR RANDOM ACCESS\n";
	for ( i = 0; i < g_vecSCANSeekTimeForRandomAccess.size(); i++ )
	{
		cout << "[ " << i << " ] " << g_vecSCANSeekTimeForRandomAccess[i] << endl;
		g_dSCANTotalSeekTimeForRandomAccess += g_vecSCANSeekTimeForRandomAccess[i];
	}
	cout << "TOTAL = " << g_dSCANTotalSeekTimeForRandomAccess << endl << endl;

	//
	// I/O TIME
	//

	cout << "PRINT SCAN IO TIME FOR SEQUENTIAL ACCESS\n";
	for ( i = 0; i < g_vecSCANIoTimeForSequentialAccess.size(); i++ )
	{
		cout << "[ " << i << " ] " << g_vecSCANIoTimeForSequentialAccess[i] << endl;
		g_dSCANTotalIoTimeForSequentialAccess += g_vecSCANIoTimeForSequentialAccess[i];
	}
	cout << "TOTAL = " << g_dSCANTotalIoTimeForSequentialAccess << endl << endl;

	cout << "PRINT SCAN IO TIME FOR LOCALIZE ACCESS\n";
	for ( i = 0; i < g_vecSCANIoTimeForLocalizeAccess.size(); i++ )
	{
		cout << "[ " << i << " ] " << g_vecSCANIoTimeForLocalizeAccess[i] << endl;
		g_dSCANTotalIoTimeForLocalizeAccess += g_vecSCANIoTimeForLocalizeAccess[i];
	}
	cout << "TOTAL = " << g_dSCANTotalIoTimeForLocalizeAccess << endl << endl;

	cout << "PRINT SCAN IO TIME FOR RANDOM ACCESS\n";
	for ( i = 0; i < g_vecSCANIoTimeForRandomAccess.size(); i++ )
	{
		cout << "[ " << i << " ] " << g_vecSCANIoTimeForRandomAccess[i] << endl;
		g_dSCANTotalIoTimeForRandomAccess += g_vecSCANIoTimeForRandomAccess[i];
	}
	cout << "TOTAL = " << g_dSCANTotalIoTimeForRandomAccess << endl << endl;

	//
	// DISTANCE
	//

	cout << "PRINT SCAN DISTANCE FOR SEQUENTIAL ACCESS\n";
	for ( i = 0; i < g_vecSCANDistanceForSequentialAccess.size(); i++ )
	{
		cout << "[ " << i << " ] " << g_vecSCANDistanceForSequentialAccess[i] << endl;
		g_dSCANTotalDistanceForSequentialAccess += g_vecSCANDistanceForSequentialAccess[i];
	}
	cout << "TOTAL = " << g_dSCANTotalDistanceForSequentialAccess << endl << endl;

	cout << "PRINT SCAN DISTANCE FOR LOCALIZE ACCESS\n";
	for ( i = 0; i < g_vecSCANDistanceForLocalizeAccess.size(); i++ )
	{
		cout << "[ " << i << " ] " << g_vecSCANDistanceForLocalizeAccess[i] << endl;
		g_dSCANTotalDistanceForLocalizeAccess += g_vecSCANDistanceForLocalizeAccess[i];
	}
	cout << "TOTAL = " << g_dSCANTotalDistanceForLocalizeAccess << endl << endl;

	cout << "PRINT SCAN DISTANCE FOR RANDOM ACCESS\n";
	for ( i = 0; i < g_vecSCANDistanceForRandomAccess.size(); i++ )
	{
		cout << "[ " << i << " ] " << g_vecSCANDistanceForRandomAccess[i] << endl;
		g_dSCANTotalDistanceForRandomAccess += g_vecSCANDistanceForRandomAccess[i];
	}
	cout << "TOTAL = " << g_dSCANTotalDistanceForRandomAccess << endl << endl;
}

void PrintTotalData ()
{
	cout << "-----------------------------\n";
	cout << "Print total data information.\n";
	cout << "-----------------------------\n";

	cout <<  "FCFS TOTAL SEEK TIME \n";
	cout << "SEQUENTIAL ACCESS : 	" << g_dFCFSTotalSeekTimeForSequentialAccess << endl;
	cout << "Localize access : 	" << g_dFCFSTotalSeekTimeForLocalizeAccess << endl;
	cout << "Random access : 	" << g_dFCFSTotalSeekTimeForRandomAccess << endl;

	cout << "FCFS TOTAL IO TIME \n";
	cout << "SEQUENTIAL ACCESS : 	" << g_dFCFSTotalIoTimeForSequentialAccess << endl;
	cout << "Localize access : 	" << g_dFCFSTotalIoTimeForLocalizeAccess << endl;
	cout << "Random access : 	" << g_dFCFSTotalIoTimeForRandomAccess << endl;

	cout << "FCFS TOTAL DISTANCE \n";
	cout << "SEQUENTIAL ACCESS : 	" << g_dFCFSTotalDistanceForSequentialAccess << endl;;
	cout << "Localize access : 	" << g_dFCFSTotalDistanceForLocalizeAccess << endl;
	cout << "Random access : 	" << g_dFCFSTotalDistanceForRandomAccess << endl << endl;

	cout << "SSTF TOTAL SEEK TIME\n";
	cout << "SEQUENTIAL ACCESS : 	" << g_dSSTFTotalSeekTimeForSequentialAccess << endl;
	cout << "LOCALIZE ACCESS : 	" << g_dSSTFTotalSeekTimeForLocalizeAccess << endl;
	cout << "RANDOM ACCESS : 	" << g_dSSTFTotalSeekTimeForRandomAccess << endl;

	cout << "SSTF TOTAL IO TIME\n";
	cout << "SEQUENTIAL ACCESS : 	" << g_dSSTFTotalIoTimeForSequentialAccess << endl;
	cout << "LOCALIZE ACCESS : 	" << g_dSSTFTotalIoTimeForLocalizeAccess << endl;
	cout << "RANDOM ACCESS : 	" << g_dSSTFTotalIoTimeForRandomAccess << endl;

	cout << "SSTF TOTAL DISTANCE\n";
	cout << "SEQUENTIAL ACCESS : 	" << g_dSSTFTotalDistanceForSequentialAccess << endl;
	cout << "LOCALIZE ACCESS : 	" << g_dSSTFTotalDistanceForLocalizeAccess << endl;
	cout << "RANDOM ACCESS : 	" << g_dSSTFTotalDistanceForRandomAccess << endl << endl;

	cout << "SCAN TOTAL SEEK TIME \n";
	cout << "SEQUENTIAL ACCESS : 	" << g_dSCANTotalSeekTimeForSequentialAccess << endl;
	cout << "LOCALIZE ACCESS : 	" << g_dSCANTotalSeekTimeForLocalizeAccess << endl;
	cout << "RANDOM ACCESS : 	" << g_dSCANTotalSeekTimeForRandomAccess << endl;

	cout << "SCAN TOTAL IO TIME \n";
	cout << "SEQUENTIAL ACCESS :	" << g_dSCANTotalIoTimeForSequentialAccess << endl;
	cout << "LOCALIZE ACCESS : 	" << g_dSCANTotalIoTimeForLocalizeAccess << endl;
	cout << "RANDOM ACCESS : 	" << g_dSCANTotalIoTimeForRandomAccess << endl;

	cout << "SCAN TOTAL DISTANCE \n";
	cout << "SEQUENTIAL ACCESS : 	" << g_dSCANTotalDistanceForSequentialAccess << endl;
	cout << "LOCALIZE ACCESS :	" << g_dSCANTotalDistanceForLocalizeAccess << endl;
	cout << "RANDOM ACCESS : 	" << g_dSCANTotalDistanceForRandomAccess << endl;
}

int main() {

	SimulateIoBoundWithSequentialAccess();
	SimulateBurstyIoWithLocalizeAccess();
	SimulateCpuBoundWithRandomAccess();

	//PrintRequestsForSequ();
	//PrintRequestsForLoca();
	//PrintRequestsForRand();

	FCFS();
	PrintFCFSData ();

	SSTF();
	PrintSSTFData();

	SCAN ();
	PrintSCANData();

	PrintTotalData();

	cout << "\nEnd of main().\n";

	return 0;
}
