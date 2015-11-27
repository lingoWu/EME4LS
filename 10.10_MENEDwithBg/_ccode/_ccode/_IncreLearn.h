#ifndef INCRELEARN_H
#define INCRELEARN_H

#include "_Declaration.h"
#include "_Parameter.h"
#include "_Model.h"
#include "_Corpus.h"
#include "_Initer.h"
#include "_GibbsSampler.h"
#include "_Util.h"
#include <vector>

namespace IncreLearn
{
	int selectMode = 3;
	int increamentMaxIter = 5;
	int sampleMaxIterEnt = 5;
	int sampleMaxIterIndic = 20;
	int indicatorMaxIter = 200;
	int increTimes = 0;

	vector< int> unlabededSet;
	vector< int> labededSet;

	ofstream confidenceLog;

	void saveModel()
	{
		cout<<"  ..."<<endl;
		string fname;
		stringstream ss;
		ss<<increTimes;
		ss>> fname;
		string fn = Parameter::getParameter ("iter_result_folder")+getSeparator()+"model_#"+fname;
		Model::outputModel (fn);
		cout<<'#'<<increTimes<<" finish saving Model"<<endl;
	}

	bool learnOnce()
	{
		// \/ get the confidenceLog name
		string fname;
		stringstream ss;
		ss<<increTimes;
		ss>> fname;
		string fn = Parameter::getParameter ("iter_result_folder")+getSeparator()+"confidenceLog_#"+fname;
		time_t t = time(0); 
		char NowTimeStr[64]; 
		strftime( NowTimeStr, sizeof(NowTimeStr), "_%Y-%m-%d_%H.%M.%S",localtime(&t) );
		fname = fn + NowTimeStr + ".txt";
		confidenceLog.open (fname.c_str (),ios::out);

		// \/first step: selection
// 		int * ddebug = Model::mentions[0].entityOptions;
		for (Corpus::PseudoDocs::size_type i = 0;i<Corpus::pseudoDocs.size ();++i)
		{
			Corpus::PseudoDoc & pd = Corpus::pseudoDocs[i];
			if (!pd.test) continue;
// 			confidenceLog<<"bs"<<endl;//begin select debug
			if (pd.isToLearn(selectMode))
			{
				pd.trans2use ();
				unlabededSet.push_back (i);
// 				assert(ddebug == Model::mentions[0].entityOptions);
			}
// 			confidenceLog<<"es"<<endl;//end select debug
		}

		printTime ();
		cout<<'#'<<increTimes<<"finishing selecting "<<unlabededSet.size()<<" words"<<endl;
		if (unlabededSet.size() == 0)
		{
			cout<<"select no words"<<endl;
			return false;
		}

		// \/second step: train 
		for (int i=0;i<sampleMaxIterEnt;++i)
		{
			cout<<i<<'.';
			GibbsSampler::sampleOnce (unlabededSet,false);
		}
		for (int i=0;i<sampleMaxIterIndic;++i)
		{
			cout<<i<<'.';
			GibbsSampler::sampleOnce (unlabededSet,true);
		}

		
		printTime ();
		saveModel();

		// \/third step: add
		for (vector<Corpus::PseudoDoc *>::size_type t=0;t<unlabededSet.size();++t)
		{
			Corpus::pseudoDocs[unlabededSet[t]].labeled = true;
			labededSet.push_back (unlabededSet[t]);
		}
		unlabededSet.clear ();
		cout<<"used data:"<<labededSet.size()<<" -- unused data:"<<Corpus::pseudoDocs.size () - labededSet.size()<<endl;
		cout<<'#'<<increTimes<<"finishing adding to labededSet"<<endl;

		confidenceLog.close();
		return true;
	}

	void learn()
	{
		printTime ();
		for (int i=0; i < indicatorMaxIter ; ++i)
		{
			cout<<i<<'.';
			GibbsSampler::sampleOnce (labededSet,true);
		}

		printTime ();
		increTimes = -1;
		saveModel();
		cout<<"Finishing indicator Sampling"<<endl;

		for (increTimes = 0; increTimes < increamentMaxIter ;++increTimes)
		{
			if (!learnOnce())
			{
				break;
			}
		}

		printTime ();
		cout<<"Finishing Learning"<<endl;
	}

}

namespace Corpus
{
	bool PseudoDoc::isToLearn (int selectMode)
	{
		static float d = 0;
		static float sum = 0;
		static double sumEntPrblts;
		static double maxEntPrblts;
		static double p;
		static int sub;
		static int entid;

		if (selectMode == 1) //on the contrary, it's harder to accomplished
		{
			
		}
		else if (selectMode == 2)
		{
			
		}
		else if (selectMode == 3)
		{
			getEntsPrblts ();
			sub = 0;
			sumEntPrblts = 0;
			maxEntPrblts = 0;
			assert(ent_sz);
			for (unsigned int i=0;i<ent_sz;++i)
			{
				p = prblts[i];

				sumEntPrblts += p;
				if (p>maxEntPrblts) 
				{
					maxEntPrblts=p;
					sub = i;
				}
			}
// 			cout<<maxExpPrblts<<'/'<<sumEntPrblts<<endl;
			IncreLearn::confidenceLog<<maxEntPrblts/sumEntPrblts<<'\t'<<Model::getMention(windowStr[menIndex]).entityOptions[sub]<<endl;
			if (Model::getMention(windowStr[menIndex]).entityOptions[sub] >0 && Model::getMention(windowStr[menIndex]).entityOptions[sub] < int(Model::entityNum) && maxEntPrblts/sumEntPrblts > Model::eta) 
			{
				return true;
			}
		}
		else if (selectMode == 4)
		{
			if ( size () <= 1) return false;

			getEntsPrblts ();
			sub = 0;
			sumEntPrblts = 0;
			maxEntPrblts = 0;
			assert(ent_sz);
			for (unsigned int i=0;i<ent_sz;++i)
			{
				if (prblts[i]>maxEntPrblts) 
				{
					maxEntPrblts=prblts[i];
					sub = i;
				}
			}
			sum = 0;
			for (unsigned i=0;i<size ();++i)
			{
				if (i == menIndex) continue;

// 				d = Model::getEntity (BACKGROUND).getWordCnt (windowStr[i])+
// 					Model::getEntity (Model::getMention (windowStr[menIndex]).entityOptions[sub]).getWordCnt (windowStr[i]);

				d = (float)(Model::getCxtCnt (windowStr[i]));

				if (d < 0.1) sum += 1;
				else sum += Model::getEntity (BACKGROUND).getWordCnt (windowStr[i]) / d;
			}
			entid = Model::getMention(windowStr[menIndex]).entityOptions[sub];
			IncreLearn::confidenceLog<<sum/(size () - 1)<<'\t'<<entid<<endl;

			return entid >0 && entid < int(Model::entityNum) && sum/(size () - 1) < Model::eta;
		}
		return false;
	}
}

#endif
