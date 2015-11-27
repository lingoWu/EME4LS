//this file is just for back up
#ifndef MODEL_H
#define MODEL_H
#include "_Parameter.h"
#include <memory.h>
#include <set>
#include <map>
#include <vector>
#include <ctime>

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
	float beta; //hp.. in the dis.. of label to words
	float alpha; //hp.. in the dis.. of entities
	float cnt_df ,cnt_bg; //#training parameters
	void getHp()//using in init.
	{
		Parameter::getParameter ("gamma_df",gamma_df);
		Parameter::getParameter ("gamma_bg",gamma_bg);
		Parameter::getParameter ("beta",beta);
		Parameter::getParameter ("alpha",alpha);
	}
	// ###END the model parameters

	// #START the data structure of Entity
	// \/ Entity:the abstract base class 
	class Entity
	{
	public:
		int allWordCnt; //#training parameters
		//this save the num of all words (Including the repeated words)
		//i.e. the occurrence number of this entity
		Entity():allWordCnt(0){}
		virtual void addWordCnt(int m,int i = 1) = 0;
		virtual int getWordCnt(int w) = 0;
		virtual int getSize() = 0;
	};

	// \/ ArrayEntity:use Array to save the number of Entity2Word
	class ArrayEntity : public Entity
	{
	private:
		int * wordid_cnt; //#training parameters
		//int sz; //sz = mentionNum + 2
	public: 
		void Create()
		{
			wordid_cnt = new int[mentionNum + 2];
			memset (wordid_cnt,0,(mentionNum + 2)*sizeof(int));
		}
		void Delete()
		{
			delete []wordid_cnt;
		}
		void addWordCnt (int m,int i)
		{
			wordid_cnt[m+2] += i;
			allWordCnt+=i;		
		}
		int getWordCnt (int w)
		{
			return wordid_cnt[w+2];
		}
		int getSize ()
		{
			return mentionNum + 2;
		}
	};

	// \/ MapEntity:use Map to save the number of Entity2Word
	class MapEntity : public Entity
	{
	private:
		map<int,int> wordid_cnt; //#training parameters
	public: 
		void addWordCnt (int m,int i)
		{
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
		int getWordCnt (int w)
		{
			map<int,int>::iterator it = wordid_cnt.find (w);
			if (it==wordid_cnt.end ())
			{
				return 0;
			}
			else return it->second;
		}
		int getSize ()
		{
			return wordid_cnt.size ();
		}
	};
	// ###END the data structure of Entity
	
	// #START Entities
	unsigned int entityNum = 0;
	MapEntity * MapEntities;
	ArrayEntity bgEntity,dfEntity;
	void createEntities()
	{
		bgEntity.Create ();
		dfEntity.Create ();
		MapEntities = new MapEntity[entityNum];
		MapEntity Me;
		for (int i=0;i<entityNum;++i)
		{
			MapEntities[i] = Me;
		}
	}
	void deleteEntities()
	{
		bgEntity.Delete ();
		dfEntity.Delete ();
		delete []MapEntities;
	}
	Entity & getEntity(int e)
	{
		if (e == BACKGROUND)
		{
			return bgEntity;
		}
		else if (e == NOTRANS)
		{
			return dfEntity;
		}
		else 
			return MapEntities[e];
	}
	// ###END Entities
	
	// #START the data structure of Mention
	class Mention 
	{
	private:
	public:
		set<int> entityOptionsSet;
		int * entityOptions;
		Mention() { }
		int getOptionsSize()
		{
			return entityOptionsSet.size ();
		}
		void arrange()
		{
			entityOptions = new int[entityOptionsSet.size ()];
			int i=0;
			for (set<int>::iterator it = entityOptionsSet.begin ();it!=entityOptionsSet.end ();++it)
			{
				entityOptions[i++] = *it;
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
		for (int i=0;i<mentionNum;++i)
		{
			Mention m;
			mentions[i]=m;
		}
	}
	void arrangeMentions()
	{
		for (int i=0;i<mentionNum;++i)
		{
			mentions[i].arrange ();
		}
	}
	void deleteMentions()
	{
		for (int i=0;i<mentionNum;++i)
		{
			delete [](mentions[i].entityOptions);
		}
		delete []mentions;
	}
	// ###END mentions
	
	// #START model Input & Output

	/////
	//  lastly modified in [5/5/2015 WuShan]
	//	if change the output, must meantime change the input func
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

		fout<<"this part is unfinished"<<endl;

// 		fout<<"<Model>"<<endl;
// 		fout<<"<Time Now=' "<<NowTimeStr<<" '/>"<<endl;
// 
// 		// \/ outPut all hyperParameters
// 		fout<<"<hyperParameters>"<<endl;
// 		fout<<"alpha  "<<alpha<<endl;
// 		fout<<"beta  "<<beta<<endl;
// 		fout<<"gamma_df  "<<gamma_df<<endl;
// 		fout<<"gamma_bg  "<<gamma_bg<<endl;
// 		fout<<"</hyperParameters>"<<endl;
// 
// 		// \/ outPut all entities
// 		fout<<"<Entity No.= 'BACKGROUD ' >"<<endl;
// 		fout<<entities[BACKGROUND];
// 		fout<<"</Entity>"<<endl;
// 		for (unsigned long i = 0;i<entities.size ();++i)
// 		{
// 			fout<<"<Entity No.=' "<<i<<" '>"<<endl;
// 			fout<<entities[i];
// 			fout<<"</Entity>"<<endl;
// 		}
// 
// 		fout<<"</Model>"<<endl;
// 		return ;
	}
}

#endif
