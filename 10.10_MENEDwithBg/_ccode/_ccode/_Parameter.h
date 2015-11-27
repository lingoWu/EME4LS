#ifndef PARAMETER_H
#define PARAMETER_H

#include "_Declaration.h"

namespace Parameter
{
	bool readen = false;
	map<string, string> parameters;

	void readParameters( const string& config_file= "config.ini")
	{
		if (readen) return;
		string line, title_str,value_str;
		ifstream is(config_file.c_str());
		if(!is) { 
			cerr << "Parameter Error, fail to read config file from \"" << config_file << "\"." << endl;
			exit(-1);
		}

		while (getline(is, line)) 
		{
			if (line == "")  continue; 
			istringstream iss(line.c_str());
			iss >> title_str >> value_str;
			parameters[title_str.substr(1,title_str.size()-2)] = value_str;
			title_str = "";// modified on 2011/05/17
			value_str = "";
		}

		is.close();
		readen = true;
	}

	void setParameter(const string &key,const string &value)
	{
		if (!readen)
		{
			readParameters ();
		}
		parameters[key] = value;
	}

	string getParameter(const string &key)
	{
		if (!readen)
		{
			readParameters ();
		}
		
		if (parameters.find(key) != parameters.end())
			return parameters[key];
		return "";
	}
	
	template<class T>  //add [4/17/2015 by WuShan]
	void getParameter (const string &key,T& val)
	{
		istringstream iss(parameters[key].c_str ());
		iss>>val;
		return ;
	}
}
#endif
