#include "../include/Mainfunctions.h"

void BWSIM_Password_function()
{
  char *universalId =fGetUniversalID();
  encrypt(universalId);
  
  ifstream ifile("/etc/bwsim/license/.license");
  if (ifile) {
    string pid;
    getline(ifile,pid);
    decrypt(universalId);
    char *pid1 = new char[pid.length() + 1];
    strcpy(pid1, pid.c_str());
    decrypt(pid1);
    bool IsSucess = fVerifyPassword(universalId, pid1);
    if(!IsSucess)
    {
      system(" rm -r .license");
      cout<<"[both:]Error. Issue with Licence. Contact administrator"<<endl; exit(0);
    }
    else
    {
      int NumDays;
      string val;
      string uId;
      
      getline(ifile,uId);
      getline(ifile, val);
      char *valch =new char [val.length() - 1];
      strcpy(valch, val.c_str());
      NumDays = strtol(valch, NULL,10);
      
      // 		  cout<<"NumDays : "<<NumDays<<endl;
      int dateDecrypt = (NumDays /7 ) - 152;
      // 		  cout<<"dateDecrypt : "<<dateDecrypt<<endl;
      if(dateDecrypt != -1)
      {
	string uIdstr = uId.substr(uId.find("(c(",0)+3, uId.length());
	char *date_file = new char [uIdstr.length() -1];
	strcpy(date_file, uIdstr.c_str());
	decrypt(date_file);
	
	// 		  string date_val = uId.substr(uId.find("(c(",0)+3, uId.length());
	// 		  char *date_file = new char[date_val.length() + 1];
	// 		  strcpy(date_file, date_val.c_str());
	// 		  decrypt(date_file);
	//     char date_file[date_val.length()] ={'\0'};
	// 		  cout<<"Date of Registration : "<<date_file<<endl;
	
	int month = int(date_file[0]-'0') * 10 + int(date_file[1] - '0');
	int day = int(date_file[3]-'0') * 10 + int(date_file[4] - '0');
	int year = 2000+(int(date_file[6]-'0') * 10 + int(date_file[7] - '0'));
	
	time_t now = time(NULL);
	struct tm *t = localtime(&now);
	
	if(t->tm_year<(year-1900))
	{
	  cout<<"[both:]Error(y) Your trail version is expired. Contact administrator to renew your license."<<endl<<endl;
	  exit(0);
	}
	else if(t->tm_year==(year-1900)&& t->tm_mon<(month - 1))
	{
	  cout<<"[both:]Error(m) Your trail version is expired. Contact administrator to renew your license."<<endl<<endl;
	  exit(0);
	}
	else if(t->tm_year==(year-1900)&& t->tm_mon==(month - 1)&&t->tm_mday<day)
	{
	  cout<<"[both:]Error(d). Your trail version is expired. Contact administrator to renew your license."<<endl<<endl;
	  exit(0);
	}
	//     	          cout<<day<<"  "<<month<<"  "<<year<<endl;
	
	//     int year = 2013,  month = 1, day = 27;
	
	struct tm  tm;
	time_t rawtime;
	time ( &rawtime );
	tm = *localtime ( &rawtime );
	tm.tm_year = year - 1900;
	tm.tm_mon = month - 1;
	tm.tm_mday = day;
	mktime(&tm);
	
	tm.tm_mday = tm.tm_mday + dateDecrypt;
	mktime(&tm);
	
	char date[9];
	strftime( date, sizeof(date), "%x", &tm );
	string date_exp = date;
	
	//     		  cout<<"date_exp : "<<date_exp<<endl;
	
	/*
	 *                     year -= 2000;
	 *                     month -= 1;*/
	
	//         	  cout<< tm.tm_mon <<"  "<<t->tm_mon<<endl;
	// 		  cout<< tm.tm_mday <<" "<<t->tm_mday<<endl;
	// 		  cout<< tm.tm_year <<" "<<t->tm_year<<endl;
	if(tm.tm_year > t->tm_year)
	{
	}
	else if(tm.tm_year==t->tm_year)
	{
	  if(tm.tm_mon>t->tm_mon)
	  {
	    
	  }
	  else if(tm.tm_mon==t->tm_mon)
	  {
	    if(tm.tm_mday>=t->tm_mday)
	    {
	      
	    }
	    else
	    {
	      cout<<"[both:]Error(d). Your trail version is expired. Contact administrator to renew your license."<<endl<<endl;
	      exit(0);
	    }
	  }
	  else
	  {
	    cout<<"[both:]Error(m) Your trail version is expired. Contact administrator to renew your license."<<endl<<endl;
	    exit(0);
	  }
	}
	else
	{
	  cout<<"[both:]Error(y) Your trail version is expired. Contact administrator to renew your license."<<endl<<endl;
	  exit(0);
	}
      }
    }
  }
  else
  {
    cout<<"[both:]/*****    WELCOME NEW USER TO BWSIM SIMULATOR   ******/"<<endl<<endl;
    
    cout<<"[both:]Kindly send below ID to Admin and get correct password "<<endl<< universalId<<endl<<endl<<endl;
    
    cout<<"[both:]Enter Password : ";
    string pass;
    cin>>pass;
    
    char *passwd = new char[pass.length() + 1];
    strcpy(passwd, pass.c_str());
    decrypt(universalId);
    bool IsSucess = fVerifyPassword(universalId, passwd);
    
    int strNumDays = pass.rfind('-', pass.size());
    int NumDays = atoi(pass.substr(strNumDays+1,pass.size()).c_str());
    // 	  cout<<NumDays<<endl;;
    
    
    if(IsSucess)
    {
      ofstream file (".license", ios::out | ios::app | ios::binary);
      system("sudo chmod 0400 .license");
      char ch;
      char level[4];
      char thetime[4];
      if(file.is_open())
      {
	encrypt(passwd);
	file<<passwd<<endl;
	encrypt(universalId);
	file<<universalId<<endl;
	decrypt(universalId);
	file<<NumDays<<endl;
	decrypt(passwd);
	file.close();
      }
      else
      {
	cout << "[both:]Licence file cannot be created"; exit(0);
      }
    }
    else
    {
      cout<<"[both:]Please enter correct password"<<endl;
      exit(0);
    }
  }
}