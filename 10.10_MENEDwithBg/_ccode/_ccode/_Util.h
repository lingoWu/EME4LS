#ifndef UTIL_H 
#define UTIL_H

#include "ctime"
#include <string>
#include <functional>

inline char getSeparator()
{
#ifdef linux
	return '/';
#else
	return '\\';
#endif 
}

//注意：当字符串为空时，也会返回一个空字符串  
void split(const std::string& s,const std::string& delim,std::vector< std::string >& ret)  
{  
	size_t last = 0;  
	size_t index=s.find_first_of(delim,last);  
	while (index!=std::string::npos)  
	{  
		ret.push_back(s.substr(last,index-last));  
		last=index+1;  
		index=s.find_first_of(delim,last);  
	}  
	if (index-last>0)  
	{  
		ret.push_back(s.substr(last,index-last));  
	}  
} 


char tmp[64]; 
void printTime()
{
	time_t t = time(0); 
	strftime( tmp, sizeof(tmp), "_%Y-%m-%d_%H:%M:%S",localtime(&t) ); 
	std::cout<<tmp<<endl;
}

#endif