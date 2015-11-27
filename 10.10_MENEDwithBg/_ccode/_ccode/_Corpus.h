#ifndef CORPUS_H
#define CORPUS_H

#include "_Declaration.h"
#include "_Parameter.h"
#include "_Model.h"
#include <vector>
#include <map>
#include <set>
#include <algorithm>
#define MAX(a,b) (((a)>(b))?(a):(b))
#define MIN(a,b) (((a)<(b))?(a):(b))
/////
//this mainly contains the class:
//    Sentence but Sentences
//    PseudoDoc and PseudoDocs  [5/18/2015 by WuShan]
//	  pseudoDocs and testPseudoDocs [07/30/2015 by WuShan]
/////
namespace Corpus 
{
	typedef vector<int> Sentence;

	class PseudoDoc
	{
	private:
		void InitIndicator (int i);
		void randomInitEntity();
	public:
		bool test;					//the data is testing or unlabeled without using.
		int menIndex;				//test data has.
		vector<int> windowStr;		//test data has.
		bool labeled;
		int defaultEntity;
		vector<bool> windowTi;

		unsigned int size()
		{
			return windowStr.size ();
		}

		void getEntsPrblts();
		void SampleEntity(bool init=false);
		void SampleIndicator(int i);
		bool isToLearn(int selectMode = 3);
		double getIndicatorPrblt(int i);

		/////
		// para1: stc ref
		// para2: the index of mention
		// para3: the entity NO. of it 
		// this will only init the indicator (this construction is applied to labeled psedudoDoc)
		// (this didn't ext the WORDOUT context. )
		/////
		PseudoDoc (const vector<int> &stc,int Index,int ent):windowStr(stc.begin ()+MAX(0,Index-Model::window_sz),stc.begin ()+MIN((int)(stc.size()),Index+1+Model::window_sz)),menIndex(MIN(Model::window_sz,Index)),defaultEntity(ent)
		{
			test = false;
			labeled = true;
			++Model::getMention (windowStr[menIndex]).entityOptions_Cnt[ent];
			++Model::getEntity (ent).pseudoDocCnt;
			for (unsigned int i=0;i<windowStr.size ();++i)
			{
				if (i==menIndex)
				{
					windowTi.push_back (true);
// 					++Model::cnt_df;
// 					Model::getEntity (defaultEntity).addWordCnt (windowStr[i]);
				}
				// \/ Here also use Sampler to init.
				else  InitIndicator (i);
			}
			// \/ this kind 'this' is temporary variable.
			//IncreLearn::labededSet.push_back (this);  [8/9/2015 By ShanWu]
			return ;
		}

		/////
		// para1: stc ref
		// para2: the index of mention
		// this will init the indicators and entity (this construction is applied to unlabeled psedudoDoc)
		// (this needn't ext the WORDOUT context. )
		/////
		PseudoDoc (const vector<int> &stc,int Index,bool isTest = true):windowStr(stc.begin ()+MAX(0,Index-Model::window_sz),stc.begin ()+MIN((int)(stc.size()),Index+1+Model::window_sz)),menIndex(MIN(Model::window_sz,Index)),test(isTest)
		{
			if (!isTest)
			{
				// actually this part won't be called. 
				labeled = false;
				randomInitEntity();
				for (unsigned int i=0;i<windowStr.size ();++i)
				{
					if (i==menIndex)
					{
						windowTi.push_back (true);
// 						++Model::cnt_df;
// 						Model::getEntity (defaultEntity).addWordCnt (windowStr[i]);
					}
					// \/ Here also use Sampler to init.
					else InitIndicator (i);
				}
				return ;
			}
		}

		/////
		// transform this unused unlabeled pseudoDoc to used unlabeled data.
		// [8/5/2015 by ShanWu]
		/////
		void trans2use()
		{
			test = false;
			labeled = false;
			SampleEntity (true);
			for (unsigned int i=0;i<windowStr.size ();++i)
			{
				if (i==menIndex)
				{
					windowTi.push_back (true);
// 					++Model::cnt_df;
// 					Model::getEntity (defaultEntity).addWordCnt (windowStr[i]);
				}
				// \/ Here also use Sampler to init.
				else InitIndicator (i);
			}
// 			IncreLearn::unlabededSet.push_back (this);
		}


	};

	typedef vector<PseudoDoc> PseudoDocs;
	PseudoDocs pseudoDocs;

	string corpus_remark("NULL");
	string corpus_time("NULL");

}

#endif
