#include "_Declaration.h"
#include "_Parameter.h"
#include "_Model.h"
#include "_Corpus.h"
#include "_Initer.h"
#include "_GibbsSampler.h"
#include "_IncreLearn.h"
#include "_Test.h"

using namespace std;

int main(int argc ,char** argv)
{
	int mode = 0;

	// \/ 1.read Parameters  [8/19/2015 by ShanWu]
	cout<<"start to read Parameters"<<endl;
	if (argc >= 2)
		Parameter::readParameters (argv[1]);
	else 
		Parameter::readParameters ();
	cout<<"finish reading Parameters"<<endl;



	// \/ 2.init the parameters of Model and Corpus
	cout<<"start to init Parameters"<<endl;
	Initer::InitParameters (mode);
	cout<<"finish initializing Parameters"<<endl;

	// \/ 3.read the Corpus data
	cout<<"start to read Corpus"<<endl;
	Initer::readCorpus (mode);
	cout<<"finish reading Corpus"<<endl;

/*
	// \/ 5.GibbsSample and Output the Model
	cout<<"start to GibbsSample"<<endl;
	GibbsSampler::sample();
	cout<<"finishing GibbsSampling"<<endl;
*/ //  [8/8/2015 by ShanWu]

	// \/ 4.incremental learning.
	cout<<"start to increLearn"<<endl;
	//when finish reading labeled data, sample the Indicators.
	IncreLearn::learn();
	cout<<"finishing increLearning"<<endl;

	// \/ 5.test the nists
	cout<<"start to test"<<endl;
	Test::readTest();
	Test::test();
	cout<<"finishing testing"<<endl;

	// \/ 6.Delete all
	cout<<"start to delete"<<endl;
	Model::deleteEntities ();
	Model::deleteMentions ();
	if (IncreLearn::selectMode == 4)
		Model::deleteCxtWords ();
	cout<<"finishing deleting"<<endl;

	return 0;
}