#ifndef INITER_H 
#define INITER_H

#include "_Declaration.h"
#include "_Parameter.h"
#include "_Corpus.h"
#include "_Model.h"
#include "_Util.h"
#include <fstream>
#include <ios>
#include <sstream>

namespace Initer
{
	ifstream infile ;
	void readCorpus(int mode = 0) //0-trainLabeledOrUnlabeled,1-test
	{
		int k = 0;
		if (mode == 0)
		{
			infile.open (Parameter::getParameter("intCorpus_").c_str (),ios::in);
			string line;
			Corpus::Sentence stc;
			char c = '\0' ;
			int t,t2;
			if (infile.fail ())
			{
				std::cerr << "Can't open the intCorpus_ "<<endl;
			}
			while (getline(infile, line))
			{
				if (line == "T")
				{
					infile>>Corpus::corpus_time;
				}
				else if (line == "R")
				{
					infile>>Corpus::corpus_remark;
				}
				else if (line == "P")
				{
					c = 'p';
					infile>>Model::mentionNum;
					infile>>Model::contextNum; //  [7/12/2015 by WuShan]
					infile>>Model::entityNum;
					cout<<Model::mentionNum<<endl;
					cout<<Model::contextNum<<endl;
					cout<<Model::entityNum<<endl;
				}
				else if (line == "Q")
				{
					c = 'q';
					Model::createMentions ();
					Model::createEntities ();
					if (IncreLearn::selectMode == 4)
						Model::createCxtWords ();
					cout<<"create Mentions and Entities successfully"<<endl;
				}
				else if (line=="S" )
				{
					c = 's';
					stc.push_back (STARTTAG); // <s> -1
				}
				else if (line=="B" )
				{
					c = 'b';
					stc.push_back (STARTTAG); // <s> -1
				}
				else if (line == "E")
				{
					c = 'e';
					stc.push_back (ENDTAG); //</s> -2
				}
				else if (line == "F")
				{
					c = 'f';
					stc.clear ();
				}
				else if (line == "H")
				{
					c = 'h';
					stc.push_back (ENDTAG); //</s> -2
					for (int t=1;t<(int)(stc.size ())-1;++t)
					{
						if(stc[t+1]<int(Model::mentionNum) && stc[t+1] >= 0)
						{
							Corpus::pseudoDocs.push_back (Corpus::PseudoDoc(stc,t+1));
						}
					}
					stc.clear ();
				}
				else if ( line == "K")
				{
					c = 'k';
					Model::arrangeMentions();
					if (Model::noUnlabel) return;
				}
				else
				{
					if (c == 's' || c == 'b')
					{
						istringstream(line.c_str ())>>t;
// 						assert(t<(int)(Model::contextNum));
						stc.push_back (t);
					}
					else if (c == 'e')
					{
						istringstream(line.c_str ())>>t>>t2;
						assert(stc[t+1]<int(Model::mentionNum));
						if (Model::null_one)
						{
							if (t2<=0) t2 = NOTRANS;
						}
						IncreLearn::labededSet.push_back(Corpus::pseudoDocs.size());
						Corpus::pseudoDocs.push_back (Corpus::PseudoDoc(stc,t+1,t2));
					}
				}
				++k;
				if(c == 'b'||c == 'h')
				{
#ifdef linux
					if (k % 10000000 == 0)
#else
					if (k % 100000 == 0)
#endif // Linux

					{
						cout<<k/10000<<"w lines unlabeled"<<endl;
					}
				}
				else if (k % 5000000 == 0)
				{
					cout<<k/10000<<"w lines"<<endl;
				}
			}
		}
		infile.close ();
	}

	void InitParameters(int& mode)
	{
		srand ((unsigned)(time (NULL)));
		Parameter::getParameter ("mode",mode);
		if (mode==0)
		{
			Parameter::getParameter ("window_size",Model::window_sz);
			Parameter::getParameter ("null_one",Model::null_one);
			Parameter::getParameter ("hasDefault",Model::hasDefault);
			Parameter::getParameter ("noUnlabel",Model::noUnlabel);
			Parameter::getParameter ("menCxt",Model::menCxt);
			Model::getHp ();
			Model::cnt_bg=Model::cnt_df=0;
// 			Parameter::getParameter ("train_output_interval",GibbsSampler::output_interval);
// 			Parameter::getParameter ("train_burnt_out_times",GibbsSampler::burnt_out_times);
// 			Parameter::getParameter ("train_total_times",GibbsSampler::total_times);
			Parameter::getParameter ("increamentMaxIter",IncreLearn::increamentMaxIter);
			Parameter::getParameter ("sampleMaxIterEnt",IncreLearn::sampleMaxIterEnt);
			Parameter::getParameter ("sampleMaxIterIndic",IncreLearn::sampleMaxIterIndic);
			Parameter::getParameter ("indicatorMaxIter",IncreLearn::indicatorMaxIter);
			Parameter::getParameter ("selectMode",IncreLearn::selectMode);
			
		}
		return;
	}
}
#endif