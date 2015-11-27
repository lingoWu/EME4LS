#ifndef TEST_H 
#define TEST_H

#include "_Declaration.h"
#include "_Parameter.h"
#include "_Model.h"
#include "_Corpus.h"
#include "_Initer.h"
#include "_GibbsSampler.h"
#include "_IncreLearn.h"
#include "_Util.h"
#include <fstream>
#include <ios>
#include <sstream>

namespace Test
{
	static vector<string> testFilenames;
	static vector<Corpus::PseudoDocs> testPseudoDocs;

	void readTest()
	{
		split (Parameter::getParameter("intTestCorpus"),",",testFilenames);
		testPseudoDocs.resize (testFilenames.size ());

		string line;
		Corpus::Sentence stc;
		int t;
		char c = 'b';
		for(unsigned i=0;i<testFilenames.size ();++i)
		{
			ifstream testFile(testFilenames[i].c_str (),ios::in);
			if (testFile.fail ())
			{
				std::cerr << "Can't open the file "<<testFilenames[i]<<endl;
			}
			while (getline(testFile, line))
			{
				if (line == "B" )
				{
					stc.push_back (STARTTAG); // <s> -1
					c = 'b';
				}
				else if (line == "H")
				{
					stc.push_back (ENDTAG); //</s> -2
					for (int t=1;t<(int)(stc.size ())-1;++t)
					{
						if (/*stc[t]!=WORDOUT &&*/ stc[t]<int(Model::mentionNum))
						{
							testPseudoDocs[i].push_back (Corpus::PseudoDoc(stc,t));
						}
					}
					stc.clear ();
					c = 'h';
				}
				else if (c == 'b')
				{
					istringstream(line.c_str ())>>t;
					stc.push_back (t);
				}
			}
		}
	}

	void test()
	{
		//Corpus::debugg = true;
		string path_ = Parameter::getParameter ("test_result_folder")+getSeparator()+"resOf_";

		static double sumPrblts = 0; 
		for (unsigned i=0;i<testPseudoDocs.size ();++i)
		{
			time_t t = time(0);  char NowTimeStr[64]; strftime( NowTimeStr, sizeof(NowTimeStr), "%Y-%m-%d_%H.%M.%S_",localtime(&t) );
			string fname = path_ + NowTimeStr + testFilenames[i].substr(testFilenames[i].find_last_of(getSeparator())+1,-1);
			ofstream resFile(fname.c_str (),ios::out);
			cout<<"res file to "<<fname<<endl;
			assert(resFile);
			resFile.setf(ios::fixed);
			resFile<<"# Corpus remark: "<<Corpus::corpus_remark<<" & time: "<<Corpus::corpus_time<<endl;
			resFile<<"# Time Now: "<<NowTimeStr<<endl;
			resFile<<"# testFilename: "<<testFilenames[i]<<endl;
			// \/ outPut all model setting
			resFile<<"# window_sz "<<Model::window_sz<<endl;
			resFile<<"# null_one "<<Model::null_one<<endl;
			resFile<<"# hasDefault "<<Model::hasDefault<<endl;
			resFile<<"# |W|: contextNum"<<endl;
			resFile<<"# selectMode  "<<IncreLearn::selectMode<<endl;
			resFile<<"# noUnlabel  "<<Model::noUnlabel<<endl;
			resFile<<"# menCxt  "<<Model::menCxt<<endl;
			// \/ outPut all hyperParameters
			resFile<<"# alpha  "<<Model::alpha<<endl;
			resFile<<"# alpha_df  "<<Model::alpha_df<<endl;
			resFile<<"# lambda  "<<Model::lambda<<endl;
			resFile<<"# lambda_df  "<<Model::lambda_df<<endl;
			resFile<<"# beta  "<<Model::beta<<endl;
			resFile<<"# beta_df  "<<Model::beta_df<<endl;
			resFile<<"# beta_bg  "<<Model::beta_bg<<endl;
			resFile<<"# gamma_df  "<<Model::gamma_df<<endl;
			resFile<<"# gamma_bg  "<<Model::gamma_bg<<endl;
			resFile<<"# eta  "<<Model::eta<<endl;

			for (unsigned j=0;j<testPseudoDocs[i].size();++j)
			{
				Corpus::PseudoDoc & pd = testPseudoDocs[i][j];
				int men = pd.windowStr[pd.menIndex];
				pd.getEntsPrblts();
				sumPrblts = 0; 
				//sort (Corpus::prblts.begin (),Corpus::prblts.begin ()+Corpus::ent_sz,greater<double>());
				for (unsigned k=0;k<Corpus::ent_sz;++k) sumPrblts+=Corpus::prblts[k];
				resFile<<"T || "<<men;
				for (unsigned k=0;k<Corpus::ent_sz;++k)
				{
					int ent = Model::getMention (men).entityOptions[k];
					if (ent<0) ent = NOTRANS;
					resFile<<" || "<<ent
					<<':'<<Corpus::prblts[k]/sumPrblts;
				}
				resFile<<endl;
			}
		}
	}

}
#endif