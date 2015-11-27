#ifndef GIBBS_SAMPLER_H
#define GIBBS_SAMPLER_H

#include "_Declaration.h"
#include "_Parameter.h"
#include "_Model.h"
#include "_Corpus.h"
#include "_Util.h"
#include "cmath"
/////
//this file includes:
//    the whole GibbsSampler namespace
//    and the sample part of Corpus
/////
// \/ tips:When addEntityWord, change the cnt_bg/cnt_df manually
namespace GibbsSampler
{		
// 	int output_interval;	 //the interval to output model parameters
// 	int burnt_out_times;	 //the interval to burn out
// 	int total_times;	 //the total interval in burn-in period
	bool is_output;	    //whether output during gibbs sampling

	// \/ some evaluation Indexes
// 	int sampleMissCnt = 0;
	int sampleChangeCnt = 0;
	int sampleAllCnt = 0;

	int binary_search(double *p,double r,int start,int end)
	{
		int length = end - start;
		if (length < 10)
		{
			for (int k=start;k!=end;++k)
			{
				if (p[k] >= r) 
					return k;
			}
			return -1;
		}
		else
		{
			int mid = (start + end)/2;
			if (p[mid] < r)
				return binary_search(p,r,mid+1,end);
			else
				return binary_search(p,r,start,mid+1);
		}
	}

	void trickBack( Corpus::PseudoDoc & pd, bool labeled) 
	{
		static unsigned int j=0;
		if (!labeled)
		{
			--Model::getEntity (pd.defaultEntity).pseudoDocCnt;
			assert(Model::mentions[pd.windowStr[pd.menIndex]].entityOptions_Cnt.find (pd.defaultEntity)!=Model::mentions[pd.windowStr[pd.menIndex]].entityOptions_Cnt.end ());
			--Model::mentions[pd.windowStr[pd.menIndex]].entityOptions_Cnt[pd.defaultEntity];
		}
		for (j=0;j<pd.windowStr.size ();++j)
		{
			// \/ here skip the menIndex
			if (j==pd.menIndex)  
				continue;

			// \/ track back for indicator
			if (pd.windowTi[j])
			{
				--Model::cnt_df;
				Model::getEntity (pd.defaultEntity).addWordCnt (pd.windowStr[j],-1);
			}
			else
			{
				--Model::cnt_bg;
				Model::getEntity (BACKGROUND).addWordCnt (pd.windowStr[j],-1);
			}
		}
	}

	inline void trickBack( Corpus::PseudoDoc & pd ) 
	{
		trickBack(pd,pd.labeled);
	}


	// \/ actually this is unused.
	void sampleOnce() //the labeled and unlabeled corpus will be processed together
	{
		static unsigned int j=0;
		Corpus::PseudoDocs::size_type i = 0;
		sampleChangeCnt = 0;
		sampleAllCnt = 0;

		for (i=0;i<Corpus::pseudoDocs.size ();++i)
		{
			Corpus::PseudoDoc &pd=Corpus::pseudoDocs[i];
			assert(pd.windowStr.size () == pd.windowTi.size ());

			trickBack(pd);

			if (!pd.labeled)
			{
				// \/ reset(sample) the default Entity
				pd.SampleEntity();
			}

			for (j=0;j<pd.size ();++j)
			{
				// \/ reset(sample) the indicator
				if (j==pd.menIndex)
				{
					// \/ doing this or pass depend on whether trick back the menIndex
// 					Model::getEntity (defaultEntity).allWordCnt(windowStr[i]);
// 					++Model::cnt_df;
					continue;
				}
				pd.SampleIndicator (j);
			}
		}
		cout<<"Changing rate:"<<sampleChangeCnt<<'/'<<sampleAllCnt<<" = "<<1.0*sampleChangeCnt/sampleAllCnt<<endl;
	}

	void sampleOnce(const vector< int> & indexes) //the labeled and unlabeled corpus will be processed together
	{
		static unsigned int j=0;
		sampleChangeCnt = 0;
		sampleAllCnt = 0;

		for (vector<int>::size_type t=0;t<indexes.size ();++t)
		{
			Corpus::PseudoDoc &pd= Corpus::pseudoDocs[indexes[t]];
			assert(pd.windowStr.size () == pd.windowTi.size ());

			trickBack(pd);

			if (!pd.labeled)
			{
				// \/ reset(sample) the default Entity
				pd.SampleEntity();
			}

			for (j=0;j<pd.size ();++j)
			{
				// \/ reset(sample) the indicator
				if (j==pd.menIndex)
				{
					continue;
				}
				pd.SampleIndicator (j);
			}
		}
		cout<<"Changing rate:"<<sampleChangeCnt<<'/'<<sampleAllCnt<<" = "<<1.0*sampleChangeCnt/sampleAllCnt<<endl;
	}

	void sampleOnce(const vector<int> & indexes,bool labeled) //the labeled and unlabeled corpus will be processed together
	{
		static unsigned int j=0;
		sampleChangeCnt = 0;
		sampleAllCnt = 0;
		if (!labeled)
		{
			for (vector<int>::size_type t=0;t<indexes.size ();++t)
			{
				Corpus::PseudoDoc &pd= Corpus::pseudoDocs[indexes[t]];
// 				assert(pd.windowStr.size () == pd.windowTi.size ());

				trickBack(pd,false);

				// \/ reset(sample) the default Entity
				pd.SampleEntity();

				for (j=0;j<pd.size ();++j)
				{
					// \/ reset(sample) the indicator
					if (j==pd.menIndex)
					{
						continue;
					}
					pd.SampleIndicator (j);
				}
			}
			cout<<"Unlabeled:: Changing rate:"<<sampleChangeCnt<<'/'<<sampleAllCnt<<" = "<<1.0*sampleChangeCnt/sampleAllCnt<<endl;
		}
		else
		{
			for (vector<int>::size_type t=0;t<indexes.size ();++t)
			{
				Corpus::PseudoDoc &pd= Corpus::pseudoDocs[indexes[t]];

// 				assert(pd.windowStr.size () == pd.windowTi.size ());

				trickBack(pd,true);

				for (j=0;j<pd.size ();++j)
				{
					// \/ reset(sample) the indicator
					if (j==pd.menIndex)
					{
						continue;
					}
					pd.SampleIndicator (j);
				}
			}
			cout<<"Labeled:: Changing rate:"<<sampleChangeCnt<<'/'<<sampleAllCnt<<" = "<<1.0*sampleChangeCnt/sampleAllCnt<<" cnt_bg"<<Model::cnt_bg<<" cnt_df"<<Model::cnt_df<<endl;
		}
	}

/*	void sample(int mode = 0)
	{
		// \/ burn out period
		cout <<"start to burn out"<<endl;
		for (int i=0;i<burnt_out_times;++i)
		{
			printTime ();
			cout<<"Sample #"<<i<<endl;
			sampleOnce ();
			cout<<"#"<<i<<" Ending"<<endl;
		}
		cout<<"finish burning out"<<endl;

		// \/ burn in period
		cout <<"start to burn in"<<endl;
		int k=0;
		for (int i=burnt_out_times;i<total_times;++i)
		{
			time_t t = time(0); 
			strftime( tmp, sizeof(tmp), "_%Y-%m-%d_%H:%M:%S",localtime(&t) ); 
			cout<<tmp<<endl;
			cout<<"Sample #"<<i<<endl;
			sampleOnce ();
			++k;
			if (k>=output_interval)
			{
				string fname;
				stringstream ss;
				cout<<"  ..."<<endl;
				ss<<i;
				ss>> fname;
				string fn = Parameter::getParameter ("iter_result_folder")+"\\model_#"+fname;
#ifdef linux
				fn = Parameter::getParameter ("iter_result_folder")+"/model_#"+fname;
#endif // if linux  '\\' change to  '/'
				Model::outputModel (fn);
				k=0;
			}
		}
		cout <<"finish burning in"<<endl;
	}*/

}

namespace Corpus
{
	// \/ PseudoDoc::randomSample
	void PseudoDoc::randomInitEntity()
	{
		int word = windowStr[menIndex];
		//if ( word == WORDOUT) return ;
		Model::Mention & mention = Model::getMention (word);
		double r=0;
		int devider=0;
		static int entitid ;
		for (int i=0;i<mention.getOptionsSize();++i)
		{
			entitid = mention.entityOptions[i];
			devider += Model::getEntity (entitid).getWordCnt (word);
				//Model::getEntityWord(entitid,word);
		}
		r = (double)rand()/RAND_MAX*(devider);

		int k=0;
		for (double sum=0;k<mention.getOptionsSize()&&sum<=r;++k)
		{
			entitid =mention.entityOptions[k];
			sum += Model::getEntity (entitid).getWordCnt (word);
		}
		--k; //then the chosen topic is mention.entityOptions[k];
		if (k<0) k = 0;
		defaultEntity = mention.entityOptions[k];
		return ;
	}

	

	unsigned int ent_sz = 5;
	vector<double> prblts;
// 	bool debugg = false;
	/////
	// get the Probability of each entities
	// and save them in prblts[0:ent_sz-1]
	// the detail of this func can see Doc 
	// lastly checked  [8/7/2015 by ShanWu]
	/////
	void PseudoDoc::getEntsPrblts(/*bool context*/)
	{
		static double  comPart;
		static int entitid;
		static float beta_ent;
		Model::Mention & mention = Model::getMention (windowStr[menIndex]);
		ent_sz = mention.getOptionsSize ();
		if (ent_sz > prblts.size ())
		{
			prblts.resize (3*ent_sz);
		}
		comPart = (Model::gamma_bg + Model::cnt_bg ) / (Model::gamma_df + Model::cnt_df)
										/ ( Model::beta_bg + Model::cnt_bg);
		unsigned int i=0;
		for (map<int,int>::iterator it = mention.entityOptions_Cnt.begin ();it!=mention.entityOptions_Cnt.end ();++it,++i)
		{
			entitid = it->first;
			prblts[i] = Model::alpha + Model::getEntity (entitid).pseudoDocCnt;
			if (entitid == Model::entityNum)	beta_ent = Model::beta_df / Model::contextNum;
			else	beta_ent = Model::beta / Model::contextNum;
			for (unsigned int j=0;j<windowStr.size ();++j)
			{
				if (j==menIndex)
				{
					if (Model::menCxt)
					{
						if (entitid == Model::entityNum)
						{
							prblts[i]*=	
								( Model::lambda_df / Model::mentionNum  + it->second )
								/( Model::lambda_df + Model::getEntity (entitid).pseudoDocCnt);
						}
						else
						{
							prblts[i]*=	
								( Model::lambda / Model::mentionNum  + it->second )
								/( Model::lambda + Model::getEntity (entitid).pseudoDocCnt);
						}
					}
					else
					{
						prblts[i]*=	
							( beta_ent + Model::getEntity(entitid).getWordCnt(windowStr[j]) )
							/(beta_ent * Model::contextNum + Model::getEntity (entitid).allWordCnt );
					}
				}
				else
				{
					prblts[i]*=
						comPart *
						( Model::beta_bg / Model::contextNum + Model::getEntity(BACKGROUND).getWordCnt(windowStr[j]) ) 
						+ 
						( beta_ent + Model::getEntity(entitid).getWordCnt(windowStr[j]) )
						/(beta_ent * Model::contextNum + Model::getEntity (entitid).allWordCnt );
						

				}
			}
		}
		return ;
	}

	void PseudoDoc::SampleEntity(bool init) //whether it's the first to get its entity
	{
		/*
		//this segment will be hard to read, because the simplification
		static int ents_sz=5;
		static double * ntors_PZ = new double[5]; //numerators(FenZi) of P(Z) {theta -> topic}
		static double * Terms_PWs = new double[5]; //terms of P(ws) {topic -> all words in pseudoc [through inditators ]}
		int ntors_sz;
		double dtorTotol_P = 0; //denominator(FenMu) of the whole formula.
		static double tpt = 0;
		tpt = Model::gamma_bg + Model::gamma_df + Model::cnt_bg + Model::cnt_df;

		assert (windowStr[menIndex] >= 0);

		//Model::Mention & mention = Model::getMention (windowStr[menIndex]);
		//ntors_sz = mention.getOptionsSize ();
		if (ents_sz<ntors_sz)
		{
			ents_sz = 2*ntors_sz;
			delete []ntors_PZ;
			delete []Terms_PWs;
			ntors_PZ = new double[ents_sz];
			Terms_PWs = new double[ents_sz];
		}

		for (int i=0;i<ntors_sz;++i)
		{
			int entitid = mention.entityOptions[i];

			ntors_PZ[i]=Model::alpha + Model::getEntity (entitid).allWordCnt;

			Terms_PWs[i] = 1;
			for (unsigned int j=0;j<windowStr.size ();++j)
			{
				static double pt[2]={0,0};
				//0 -- background , 1 -- default
				if (j==menIndex)
				{
					//pt[0] = 0;
					//pt[1] = 1;
					Terms_PWs[i] *= ( Model::beta + Model::getEntity(entitid).getWordCnt(windowStr[j]))/(Model::beta * Model::getEntity (entitid).getSize () +  Model::getEntity (entitid).allWordCnt);
				}
				else
				{
					pt[0]=(Model::gamma_bg + Model::cnt_bg)/tpt ;
					pt[1]=(Model::gamma_df + Model::cnt_df)/tpt ;
					pt[0]*=(Model::beta + Model::getEntity (BACKGROUND).getWordCnt (windowStr[j]))/(Model::beta * Model::getEntity (BACKGROUND).getSize () + Model::getEntity (BACKGROUND).allWordCnt);
					pt[1]*=(Model::beta + Model::getEntity (entitid).getWordCnt (windowStr[j]))/(Model::beta * Model::getEntity (entitid).getSize () + Model::getEntity (entitid).allWordCnt);

					Terms_PWs[i] *=pt[0]+pt[1];
				}
			}
			dtorTotol_P+=ntors_PZ[i]*Terms_PWs[i];
		}*/ //  [8/7/2015 by ShanWu]
		

		/*
		static double r=0;
		r = (double)rand()/RAND_MAX*(dtorTotol_P);

		int k=0;
		for (double sum=0;k<ntors_sz&&sum<=r;++k)
		{
			sum+=ntors_PZ[k]*Terms_PWs[k];
		}*/ //  [8/7/2015 by ShanWu]

		getEntsPrblts ();

		Model::Mention & mention = Model::getMention (windowStr[menIndex]);
		static double sumPrblts = 0;
		sumPrblts = 0;
		for (unsigned int i=0;i<ent_sz;++i)
		{
			sumPrblts += prblts[i];
		}

		static double r=0;
		r = (double)rand()/RAND_MAX*(sumPrblts);

		static int k=0;
		k = 0; 
		for (double sum=0;k<(int)(ent_sz)&&sum<=r;++k)
		{
			sum+=prblts[k];
		}

		--k; //then the chosen topic is mention.entityOptions[k]
		if (k<0) k = 0;
		// /\ this part can be improved with binary search

		if (init)
		{
			defaultEntity =  mention.entityOptions[k];
			++GibbsSampler::sampleAllCnt;
		}
		else
		{
			if (defaultEntity !=  mention.entityOptions[k])
			{
				defaultEntity =  mention.entityOptions[k];
				GibbsSampler::sampleChangeCnt += windowStr.size ();
			}
			++GibbsSampler::sampleAllCnt;
		}
		++Model::getEntity (defaultEntity).pseudoDocCnt;
		++Model::mentions[windowStr[menIndex]].entityOptions_Cnt[defaultEntity];
		return ;

	}

	double PseudoDoc::getIndicatorPrblt(int i)
	{
		static double psblt[2]={0,0};
		static float beta_ent;
		//0 -- background , 1 -- default
		if (defaultEntity == Model::entityNum) beta_ent = Model::beta_df;
		else beta_ent = Model::beta;
		//needn't track back the indicator
		psblt[0]=(Model::gamma_bg+Model::cnt_bg)*(Model::beta_bg/Model::contextNum+Model::getEntity (BACKGROUND).getWordCnt (windowStr[i]))/(Model::beta_bg+Model::cnt_bg);
		psblt[1]=(Model::gamma_df+Model::cnt_df)*(beta_ent/Model::contextNum+Model::getEntity (defaultEntity).getWordCnt (windowStr[i]))/(beta_ent+Model::getEntity (defaultEntity).allWordCnt);
		return psblt[0]/(psblt[0]+psblt[1]);
	}

	void PseudoDoc::SampleIndicator(int i)//whether it's labeled can be judged by bool labeled
		//and whether labeled or not doesn't matter 
	{
		static int r = 0;
		static double p = 0;
		r = rand();
		p = getIndicatorPrblt(i);
		if (r < RAND_MAX *p)
		{
			if (windowTi[i])
			{
				windowTi[i]=false;
				++GibbsSampler::sampleChangeCnt;
			}
			++GibbsSampler::sampleAllCnt;
			Model::getEntity (BACKGROUND).addWordCnt (windowStr[i]);
			++Model::cnt_bg;
		}
		else
		{
			if (!windowTi[i])
			{
				windowTi[i]=true;
				++GibbsSampler::sampleChangeCnt;
			}
			++GibbsSampler::sampleAllCnt;
			Model::getEntity (defaultEntity).addWordCnt(windowStr[i]);
			++Model::cnt_df;
		}
	}

	void PseudoDoc::InitIndicator(int i)// whether labeled or not doesn't matter 
	{
		static int r = 0;
		static double p = 0;
		r = rand();
		p = Model::gamma_bg / (Model::gamma_bg + Model::gamma_df);
		//p = getIndicatorPrblt(i);

		if (r < RAND_MAX *p)
		{
			windowTi.push_back(false);
			Model::getEntity (BACKGROUND).addWordCnt(windowStr[i]);
			++Model::cnt_bg;
		}
		else
		{
			windowTi.push_back(true);
			Model::getEntity (defaultEntity).addWordCnt(windowStr[i]);
			++Model::cnt_df;
		}

		if (IncreLearn::selectMode==4)
		{
			Model::addCxtCnt (windowStr[i]);
		}

	}

}
#endif
