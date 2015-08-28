#include "../include/IniReader.h"
#include <iostream>
#include <algorithm>
#include <fstream>
#include <string.h>
#include <stdlib.h>
#include <cstring>
#include <string>
#include <stdlib.h>
#include <cstdlib>
// // routine to expand env var

char* expand_environment_variables( char* c ) {
  
  std::string s,s1;
  s.append(c);

  if( s.find( "${" ) == std::string::npos ) return c;
  
  std::string pre  = s.substr( 0, s.find( "${" ) );
  std::string post = s.substr( s.find( "${" ) + 2 );
  
  if( post.find( '}' ) == std::string::npos ) return c;
  
  std::string variable = post.substr( 0, post.find( '}' ) );
  std::string value    = "";
  
  post = post.substr( post.find( '}' ) + 1 );
  
  if( getenv( variable.c_str() ) != NULL ) value = std::string( getenv( variable.c_str() ) );
  
  //s1.append(expand_environment_variables( pre + value + post ));
  s1.append(pre+value+post);
  char * cstr = new char [s1.length()+1];
  std::strcpy (cstr, s1.c_str());  
  return  cstr;

}


char* IniReadString(char* szFileName, char* szKey, char* sDefault)
{

  std::string line="";  
  std::string myline="";
  std::string str_szKey( szKey );
  


  std::ifstream myfile(szFileName); 
  //int ck = 0;
  if (myfile.is_open())
    {
      while ( myfile.good() )
	{
	  getline (myfile,line);
	  std::size_t founds = line.find("##");
	  if((int)founds!=-1){
	    continue;
	  }
	  //std::cout<<line<< std::endl;
	  std::size_t found = line.find(szKey);
	  if((int)found!=-1){	    
	    myline=line.substr(str_szKey.length(),line.length());
	    myline.erase(remove(myline.begin(),myline.end(),' '),myline.end());
	    myline.erase(remove(myline.begin(),myline.end(),'='),myline.end());
	    myline.erase(remove(myline.begin(),myline.end(),':'),myline.end());
	    myline.erase(remove(myline.begin(),myline.end(),'"'),myline.end());
	    
	    //std::cout << myline << std::endl;
	  }
	}
      myfile.close();
    }

  
  else std::cout << "Unable to open file:" << szFileName << " to search " << szKey <<std::endl; 
  char *Sout;
  if(myline.length()==0){
    Sout = sDefault;
  }
  else{
    Sout = new char[myline.length() + 1];
    //std::strcpy(Sout,myline.c_str());
    strcpy(Sout,myline.c_str());
  }
  return  expand_environment_variables(Sout);
}

int IniReadInt(char* szFileName, char* szKey, int iDefault)
{

  std::string line="";  
  std::string myline="";
  std::string str_szKey( szKey );
  std::ifstream myfile(szFileName); 
  if (myfile.is_open())
    {
      while ( myfile.good() )
	{
	  getline (myfile,line);
	  std::size_t founds = line.find("##");
	  if((int)founds!=-1){
	    continue;
	  }
	  //std::cout<<line<< std::endl;
	  std::size_t found = line.find(szKey);
	  if((int)found!=-1){	    
	    myline=line.substr(str_szKey.length(),line.length());
	    myline.erase(remove(myline.begin(),myline.end(),' '),myline.end());
	    myline.erase(remove(myline.begin(),myline.end(),'='),myline.end());
	    myline.erase(remove(myline.begin(),myline.end(),':'),myline.end());
	    //std::cout << myline << std::endl;
	  }
	}
      myfile.close();
    }

  else std::cout << "Unable to open file:" << szFileName << " to search " << szKey <<std::endl;   
  int Iout;
  if(myline.length()==0){
    Iout = iDefault;
  }
  else{
  Iout = atoi(myline.c_str());
  }
  return Iout;
}


double IniReadDouble(char* szFileName, char* szKey, double dDefault)
{

  std::string line="";  
  std::string myline="";
  std::string str_szKey( szKey );
  std::ifstream myfile(szFileName); 
  if (myfile.is_open())
    {
      while ( myfile.good() )
	{
	  getline (myfile,line);
	  std::size_t founds = line.find("##");
	  if((int)founds!=-1){
	    continue;
	  }
	  //std::cout<<line<< std::endl;
	  std::size_t found = line.find(szKey);
	  if((int)found!=-1){	    
	    myline=line.substr(str_szKey.length(),line.length());
	    myline.erase(remove(myline.begin(),myline.end(),' '),myline.end());
	    myline.erase(remove(myline.begin(),myline.end(),'='),myline.end());
	    myline.erase(remove(myline.begin(),myline.end(),':'),myline.end());
	    //std::cout << myline << std::endl;
	  }
	}
      myfile.close();
    }

  else std::cout << "Unable to open file:" << szFileName << " to search " << szKey <<std::endl;   
  double Dout;
  if(myline.length()==0){
  
    Dout = dDefault;
  }
  else{
    Dout = (double)atof(myline.c_str());
  }
  //std::cout<<"\t\treslu = "<<Dout<<std::endl;
  return Dout;
}


bool string2bool (const std::string & v)
{
    return !v.empty () &&
        (strcasecmp (v.c_str (), "true") == 0 ||
         atoi (v.c_str ()) != 0);
}

bool IniReadBool(char* szFileName, char* szKey, bool bDefault)
{

  std::string line="";  
  std::string myline="";
  std::string str_szKey( szKey );
  std::ifstream myfile(szFileName); 
  if (myfile.is_open())
    {
      while ( myfile.good() )
	{
	  getline (myfile,line);
	  std::size_t founds = line.find("##");
	  if((int)founds!=-1){
	    continue;
	  }
	  //std::cout<<line<< std::endl;
	  std::size_t found = line.find(szKey);
	  if((int)found!=-1){	    
	    myline=line.substr(str_szKey.length(),line.length());
	    myline.erase(remove(myline.begin(),myline.end(),' '),myline.end());
	    myline.erase(remove(myline.begin(),myline.end(),'='),myline.end());
	    myline.erase(remove(myline.begin(),myline.end(),':'),myline.end());
	    //std::cout << myline << std::endl;
	  }
	}
      myfile.close();
    }

  else std::cout << "Unable to open file:" << szFileName << " to search " << szKey <<std::endl;   
  bool Bout;
 if(myline.length()==0){
    Bout = bDefault;
  }
  else{
    Bout =string2bool(myline);
  }
  return Bout;
}

