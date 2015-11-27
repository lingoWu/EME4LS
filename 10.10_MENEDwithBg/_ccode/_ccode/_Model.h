#ifndef MODEL_H
#define MODEL_H

#include "_Declaration.h"
#include "_Parameter.h"
#include <memory.h>
#include <set>
#include <map>
#include <vector>
#include <ctime>
#include <string>

using namespace std;
/////
//this file mainly contains all Model parameters  [4/17/2015 by WuShan]
//    and the definition of Entity and Entities  [5/19/2015 by WuShan]
//    and the definition of Mention and Mentions  [5/19/2015 by WuShan]
//    and the Input & Output of Model  [!!unfinished!!]
/////
namespace Model
{
	// #START the model parameters
	int window_sz;
	float gamma_df,gamma_bg ; //hyper_parameters in the distribution of t[di]
	float beta,beta_df,beta_bg; //hp.. in the distr.. of label to context words
	float lambda,lambda_df; //hp.. in the distr.. of label to mentions
	float alpha,alpha_df; //hp.. in the dis.. of entities
	double cnt_df ,cnt_bg; //#training parameters
	float eta;
	bool null_one;
	bool hasDefault;
	bool noUnlabel;
	bool menCxt;
	void getHp()//using in init.
	{
		Parameter::getParameter ("gamma_df",gamma_df);
		Parameter::getParameter ("gamma_bg",gamma_bg);
		Parameter::getParameter ("lambda",lambda);
		Parameter::getParameter ("lambda_df",lambda_df);
		Parameter::getParameter ("beta",beta);
		Parameter::getParameter ("beta_df",beta_df);
		Parameter::getParameter ("beta_bg",beta_bg);
		Parameter::getParameter ("alpha",alpha);
		Parameter::getParameter ("alpha_df",alpha_df);
		Parameter::getParameter ("eta",eta);
	}
	// ###END the model parameters

	// #START the data structure of Entity
	// \/ Entity:the abstract base class
	class Entity
	{
	public:
		int allWordCnt; //#training parameters
		int pseudoDocCnt;
		//this save the num of all words (Including the repeated words)
		//i.e. the occurrence number of this entity
		Entity():allWordCnt(0),pseudoDocCnt(0){}
		virtual void addWordCnt(int m,int i = 1) = 0;
		virtual int getWordCnt(int w) = 0;
		virtual int getSize() = 0;
		virtual void outputLines(ostream & out) = 0; //Just for Print the model.[7/2/2015 by WuShan]
	};

	// \/ ArrayEntity:use Array to save the number of Entity2Word
	class ArrayEntity : public Entity
	{
	protected:
		int * wordid_cnt; //#training parameters
		int sz;
		//int sz;
		// !!! sz = mentionNum + 2
		// !!!! sz = contextNum + 2
	public:
		void Create()
		{
			sz = 0;
			wordid_cnt = new int[contextNum + 2];
			memset (wordid_cnt,0,(contextNum + 2)*sizeof(int));
		}
		void Delete()
		{
			delete []wordid_cnt;
		}
		void addWordCnt (int c,int i) override
		{
// 			if (wordid_cnt[c+2] == 0)
// 			{
// 				++sz;
// 				wordid_cnt[c+2] = i;
// 			}
// 			else
// 			{
// 				wordid_cnt[c+2] += i;
// 				if (wordid_cnt[c+2] == 0)
// 				{
// 					--sz;
// 				}
// 			}
			wordid_cnt[c+2] += i; //
			allWordCnt+=i;
		}
		int getWordCnt (int w) override
		{
			return wordid_cnt[w+2];
		}
		int getSize () override //|||?
		{
			return sz;
		}
		void outputLines(ostream & out) override
		{
			int cnt;
			for (int i=-2;i<(int)(contextNum);++i)
			{
				cnt = wordid_cnt[i+2];
				if (cnt>0)
				{
					out<<i<<'\t'<<cnt<<endl;
				}
			}
		}

	};

	// \/ MapEntity:use Map to save the number of Entity2Word
	class MapEntity : public Entity
	{
	protected:
		map<int,int> wordid_cnt; //#training parameters
	public:
		void addWordCnt (int m,int i) override
		{
/*
			if (m == WORDOUT )
			{
				cout<<"WORDOUT"<<endl;
				return ;
			}
*/
			if (wordid_cnt.find (m)==wordid_cnt.end())
			{
				wordid_cnt[m]=i;
			}
			else
			{
				wordid_cnt[m]+=i;
			}
			allWordCnt+=i;
		}
		int getWordCnt (int w) override
		{
			map<int,int>::iterator it = wordid_cnt.find (w);
			if (it==wordid_cnt.end ())
			{
				return 0;
			}
			else return it->second;
		}
		int getSize () override
		{
			return wordid_cnt.size ();
		}
		void outputLines(ostream & out) override
		{
			int cnt;
			for (map<int,int>::iterator it=wordid_cnt.begin ();it!=wordid_cnt.end ();++it)
			{
				cnt = it->second;
				if (cnt>0)
				{
					out<<it->first<<'\t'<<cnt<<endl;
				}
			}
		}
	};

	// \/ just for every mention 's null trans
	class NullEntity : public MapEntity
	{
	public:
		int men;
		void outputLines(ostream & out) override
		{
			assert (wordid_cnt[men] == pseudoDocCnt) ;
			MapEntity::outputLines (out);
		}
	};
	// ###END the data structure of Entity

	// #START Entities
	unsigned int entityNum = 0;
	unsigned int contextNum = 0;
	MapEntity * MapEntities;
	ArrayEntity bgEntity;
	NullEntity	* nullEntities;//[7/1/2015 by WuShan]
		//  [8/22/2015 by WuShan] //ArrayEntities[ARRSZ];
	MapEntity dfEntity;
	void createEntities()
	{
		bgEntity.Create ();
		nullEntities = new NullEntity[mentionNum];
		for (unsigned i=0;i<mentionNum;++i)
		{
			nullEntities[i].men = i;
		}
		//nullEntities.Create ();//[7/1/2015 by WuShan] //  [8/22/2015 by WuShan]
// 		for (int i=0;i<ARRSZ;++i)
// 		{
// 			ArrayEntities[i].Create ();
// 		}
		MapEntities = new MapEntity[entityNum];
		MapEntity Me;
		for (unsigned int i=0;i<entityNum;++i)
		{
			MapEntities[i] = Me;
		}
// 		entMenAllCnt = new int[entityNum+1]; //contain default entity  [9/29/2015 byWuShan]
// 		memset (entMenAllCnt,0,(entityNum+1)*sizeof(int)); //  [9/29/2015 byWuShan]
	}
	void deleteEntities()
	{
		bgEntity.Delete ();
// 		for (int i=0;i<ARRSZ;++i)
// 		{
// 			ArrayEntities[i].Delete ();
// 		}
		delete []nullEntities;
		delete []MapEntities;
	}
	Entity & getEntity(int e)
	{
		if (e == BACKGROUND)
		{
			return bgEntity;
		}
		else if (e <= NOTRANS) //[7/1/2015 by WuShan]
		{
			return nullEntities[NOTRANS - e];
		}
		else if (e == entityNum && hasDefault)
		{
			return dfEntity;
		}
		else
			return MapEntities[e];
	}

	// ###END Entities

	// #BEGIN some other
	// 	float gamma_df,gamma_bg ; //hyper_parameters in the distribution of t[di]
	// 	float beta,beta_df,beta_bg; //hp.. in the dis.. of label to words
	// 	float alpha,alpha_df; //hp.. in the dis.. of entities
	inline float getAlpha(int e)
	{
		if (e == entityNum && hasDefault)
		{
			return alpha_df;
		}
		else
			return alpha;
	}
	inline float getBeta(int e)
	{
		if (e == BACKGROUND)
		{
			return beta_bg;
		}
		else if (e == entityNum && hasDefault)
		{
			return beta_df;
		}
		else
			return beta;
	}

	int * cxtWords;
	inline void createCxtWords()
	{
		cxtWords = new int[contextNum+2];
		memset(cxtWords,0,sizeof(int)*(contextNum+2));
	}
	inline int getCxtCnt(int c)
	{
		return cxtWords[c+2];
	}
	inline void addCxtCnt(int c,int i = 1)
	{
		if (!(c>=-2&&c<(int)(contextNum))) cout<<':'<<c<<':'<<endl;
		assert(c>=-2&&c<(int)(contextNum));
		cxtWords[c+2]+=i;
	}
	inline void deleteCxtWords()
	{
		delete []cxtWords;
	}
	// ###END some other

	// #START the data structure of Mention
	class Mention
	{
	public:
		map<int,int> entityOptions_Cnt;
		int * entityOptions;
// 		int * entityCnts;
		Mention() { entityOptions = NULL; }
		int getOptionsSize()
		{
			return entityOptions_Cnt.size ();
		}
		void arrange()
		{
			if (entityOptions) delete[]entityOptions;
// 			if (entityCnts) delete[]entityCnts;

			entityOptions = new int[entityOptions_Cnt.size ()];
			assert(entityOptions_Cnt.size ());
// 			entityCnts = new int[entityOptions_Cnt.size ()];
			int i=0;
// 			for (map<int,int>::iterator it = entityOptions_Cnt.begin ();it!=entityOptions_Cnt.end ();++it)
// 			{
// 				entityOptions[i++] = *it;
// 			}
			for (map<int,int>::iterator it = entityOptions_Cnt.begin ();it!=entityOptions_Cnt.end ();++it)
			{
				entityOptions[i] = it->first;
// 				entityCnts[i] = it->second;
				++i;
			}
			return ;
		}
	};
	// ###END the data structure of Mention

	// #START mentions
	unsigned int mentionNum = 0;
	Mention * mentions;
	void createMentions()
	{
		mentions = new Mention[mentionNum];
		Mention m;
		if (hasDefault)
		{
			m.entityOptions_Cnt[entityNum] = 0;
		}
		for (unsigned int i=0;i<mentionNum;++i)
		{
			mentions[i]=m;
		}
	}
	void arrangeMentions()
	{
		for (unsigned int i=0;i<mentionNum;++i)
		{
			mentions[i].arrange ();
		}
	}
// 	void mentionAccessDebug()
// 	{
// 		ofstream arrangeMentionsDebug("arrangeMentionsDebug.debug");
// 		for (unsigned int i=0;i<mentionNum;++i)
// 		{
// 			arrangeMentionsDebug<<mentions[i].getOptionsSize ()
// 				<<'\t'<<mentions[i].entityOptions[0]
// 				<<'\t'<<mentions[i].entityOptions_Cnt[entityNum]<<endl;
// 		}
// 		arrangeMentionsDebug.close ();
// 	}
	void deleteMentions()
	{
		for (unsigned int i=0;i<mentionNum;++i)
		{
			delete [](mentions[i].entityOptions);
		}
		delete []mentions;
	}
	inline Mention & getMention(int m)
	{
		assert(m>=0&&m<int(mentionNum));
		return mentions[m];
	}
	// ###END mentions

	// #START model Input & Output

	/////
	//  lastly modified in [7/9/2015 WuShan]
	//	if change the output, must meantime change the input func
	//  @note: need to update
	/////
	void outputModel(string fname)
	{
		time_t t = time(0);
		char NowTimeStr[64];
		strftime( NowTimeStr, sizeof(NowTimeStr), "_%Y-%m-%d_%H.%M.%S",localtime(&t) );
		fname = fname + NowTimeStr + ".xml";

		ofstream fout(fname.c_str (),ios::out);
		if (fout.fail ())
		{
			std::cerr<<"outputModel : can't open the file"<<endl;
			return ;
		}

		fout<<"<Model>"<<endl;
		fout<<"<Corpus remark=' "<<Corpus::corpus_remark<<" ' time=' "<<Corpus::corpus_time<<" '/>"<<endl;
		fout<<"<Time Now=' "<<NowTimeStr<<" '/>"<<endl;

		// \/ outPut all model setting
		fout<<"<modelSetting>"<<endl;
		fout<<"window_sz "<<window_sz<<endl;
		fout<<"null_one "<<null_one<<endl;
		fout<<"|W|: contextNum"<<endl;
		fout<<"hasDefault "<<hasDefault<<endl;
		fout<<"</modelSetting>"<<endl;

		// \/ outPut all hyperParameters
		fout<<"<hyperParameters>"<<endl;
		fout<<"alpha  "<<alpha<<endl;
		fout<<"alpha_df  "<<alpha_df<<endl;
		fout<<"lambda  "<<Model::lambda<<endl;
		fout<<"lambda_df  "<<Model::lambda_df<<endl;
		fout<<"beta  "<<beta<<endl;
		fout<<"beta_df  "<<beta_df<<endl;
		fout<<"beta_bg  "<<beta_bg<<endl;
		fout<<"gamma_df  "<<gamma_df<<endl;
		fout<<"gamma_bg  "<<gamma_bg<<endl;
		fout<<"eta  "<<eta<<endl;
		fout<<"</hyperParameters>"<<endl;

		// \/ outPut all possible Entity of Mention
		fout<<"<MentionOptionsEntities>"<<endl;
		for (int i=0;i<(int)(mentionNum);++i)
		{
			fout<<"<Mention No.=' "<<i<<" '>"<<endl;
			for (int j=0;j<mentions[i].getOptionsSize ();++j)
			{
				fout<<mentions[i].entityOptions[j]<<endl;
			}
			fout<<"</Mention>"<<endl;
		}
		fout<<"</MentionOptionsEntities>"<<endl;

		// \/ outPut all entities
		fout<<"<Entity No.=' BACKGROUND ' >"<<endl;
		getEntity (BACKGROUND).outputLines (fout);
		fout<<"</Entity>"<<endl;
		for (unsigned int j=0;j<entityNum;++j)
		{
			fout<<"<Entity No.=' "<<j<<" '>"<<endl;
			getEntity (j).outputLines (fout);
			fout<<"</Entity>"<<endl;
		}

		fout<<"</Model>"<<endl;
		return ;
	}
}

#endif
