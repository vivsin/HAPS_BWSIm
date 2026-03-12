

#include "../include/itppWrap.h"

//The following variables are not related to parse : is related to MPI support   : initialized here to be available globally . extern declaration in parse.h
int currentTaskid=0; //! Used for MPI 
int nTasks=1;

string exec(string cmd) {
  FILE* pipe = popen(cmd.c_str(), "r");
  if (!pipe) return "ERROR";
		     int length = cmd.length()+1;
  char* buffer = new char [length];
  string result = "";
  while(!feof(pipe)) {
    if(fgets(buffer, length, pipe) != NULL)
      result += buffer;
  }
  pclose(pipe);
  delete [] buffer;
  return result;
}

void randomizeSeed()
{
    RNG_randomize();
}


void setSeed(unsigned int seed)
{
    static bool isSet = false;
    if(!isSet)
    {
        currentSimulationSeed = seed;isSet = true;
    }
    RNG_randomize();
    if(seed)
        RNG_reset(seed);
}

unsigned int getSeed()
{
    return currentSimulationSeed;
}

/*! \brief Following function returns true if parseValue correspoding to parseKey is blank(or ';')
 * \Warning In inputFile, maximum one (parse-key, parse-value) pair is expected in a line.
 * \author Anver<anver@cewit.org.in> 	*/
bool isParseValueBlank(string inputFileName, string stringToParse) {
  
  std::ostringstream convert;
  convert << currentTaskid;
  std::string inputFileNameTemp = inputFileName+".temp"+convert.str();
  //! Delete all comments/blank-lines, and spaces @ beginning/end/around '=' ..
  ostringstream shellScript;
  shellScript<<"perl -pe 'BEGIN{undef $/;} s/\\n?[[:blank:]]*\\/\\*\\X*?\\*\\/[[:blank:]]*//smg, s/(\\n)*[[:blank:]]*\\/\\/[[:print:]]*//smg, s/^[[:blank:]]*\\n//smg, s/[[:blank:]]*=[[:blank:]]*/=/sg, s/[[:blank:]]*\\;[[:blank:]]*/\\;/sg, s/[[:blank:]]*\\n[[:blank:]]*/\\n/smg' "<<inputFileName<<" > "<<inputFileNameTemp<<";";
  shellScript<<"grep -P "<<stringToParse<<" "<<inputFileNameTemp<<" |sed -e 's/"<<stringToParse<<"\\s*=\\s*//'; ";
  ostringstream shellScriptOutput;
  shellScriptOutput<<exec(shellScript.str())<<endl;
  cout<<exec(string("rm -f ")+inputFileNameTemp);
  if( shellScriptOutput.str()=="\n\n" || shellScriptOutput.str()==";\n\n")
    return true;
  else
    return false;
}


cvec conv(cvec &a, cvec &b)
{
  int outLength = a.length()+b.length()-1;  
  return(xcorr(a,reverse(b)).right(outLength));
}

vec conv(vec &a, vec &b)
{
    int outLength = a.length()+b.length()-1;
    return(xcorr(a,reverse(b)).right(outLength));
}

