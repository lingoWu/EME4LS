#ifndef DECLARATION_H
#define DECLARATION_H

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <map>
#include <cstdlib>
#include <vector>

//#define NDEBUG
#include <assert.h>

#define BACKGROUND 0
#define NOTRANS -1
//##null_one model or null_mention is optional.
//#define WORDOUT -5
#define STARTTAG -1
//<s>
#define ENDTAG -2
//<-s>
// /\ the above four value is fixed //  [8/29/2015 By WuShan]

/*
#ifdef linux
#define ARRSZ 200
//the size of ArrayEntity
#else
#define ARRSZ 100
//the size of ArrayEntity
#endif
*/

#define TESTSZ 3
#define ADJUSTPART 100

using namespace std;


namespace Model
{
	extern unsigned int entityNum;
	extern unsigned int mentionNum;
	extern unsigned int contextNum;

};

namespace Corpus
{
	extern string corpus_remark;
	extern string corpus_time;
	extern unsigned int ent_sz;
	extern vector<double> prblts;
// 	extern bool debugg;

}

namespace GibbsSampler
{
// 	extern int output_interval;	 //the interval to output model parameters
// 	extern int burnt_out_times;	 //the interval to burn out
// 	extern int total_times;	 //the total interval in burn-in period
	extern bool is_output;	    //whether output during gibbs sampling
};

namespace IncreLearn
{
	extern int selectMode;
	extern int increamentMaxIter;
	extern int sampleMaxIterEnt ;
	extern int sampleMaxIterIndic ;
	extern int indicatorMaxIter;
	extern vector< int> unlabededSet;
	extern vector< int> labededSet;
}

#endif

