#include "../include/cSecurity.h"

// char* fGetUniversalID()  // generating the universal id which contains macID and hdID and date(date of creation)
// {
//     system("/bin/udevadm info --query=property --name=sda | grep 'ID_SERIAL_SHORT'|cut -d'=' -f2 > data"); //it will gives mac ID
//     system("ifconfig | awk '$0 ~ /HWaddr/ { print $5 }' > data2");//it will give the hardware ID
//     string hdId, MACAddr;
//     ifstream infile;
//     
//     infile.open ("data");
//     getline(infile,hdId); // copied to respective name hdId
//     infile.close();
//     
//     infile.open ("data2");
//     getline(infile,MACAddr); //copied to respective name MACAddr
//     infile.close();
//     
//     string uID = hdId + MACAddr; //created a string uID
//     
// //     cout<<"HD Id : "<<hdId<<endl<<"MAC Address : "<<MACAddr<<endl;
//     
//     system("rm -rf data data2");  //removed those unnecessary data
//     
//     time_t now = time(NULL);
//     struct tm *t = localtime(&now);
//     string PresentTime;
//     char date[9];
//     strftime( date, sizeof(date), "%x", t );
//     string date_val = date; //created a string and stored the present date
//     
//     uID = uID + "$_$" + date_val;//added the present date to the generated uID
//     
//     
//     char *name_file = new char[uID.length() + 1];//it is genearating the universalID with out encrypting.
//     strcpy(name_file, uID.c_str());
//     cout <<"universal ID"<<name_file<<endl;
//     
//     return name_file;
// }

char* fGetUniversalID()  // generating the universal id which contains macID and hdID and date(date of creation)
{ 
    string linuxVersion; 
    system("lsb_release -r | cut -d \":\" -f2 > osVersion");
    ifstream infile;
    infile.open ("osVersion");
    getline(infile, linuxVersion);
    infile.close();
    
    string os = "18.04";
    string os1 = "20.04";
    
    linuxVersion.erase(remove(linuxVersion.begin(), linuxVersion.end(), '\t'), linuxVersion.end());
    
    string mountLoc;
    system("findmnt -t ext4 | grep '^/' | awk '{ print $2 }' > mountLocation");
    infile.open ("mountLocation");
    getline(infile, mountLoc);
    infile.close();
    
    mountLoc.erase(0,5);
    
    string execCommand;
    
    if(linuxVersion == os)
        execCommand = "/sbin/udevadm info --query=property --name=" + mountLoc +  "| grep 'ID_SERIAL_SHORT'|cut -d'=' -f2 > data";
    else if(linuxVersion == os1)	
        execCommand = "/bin/udevadm info --query=property --name=" + mountLoc +  "| grep 'ID_SERIAL_SHORT'|cut -d'=' -f2 > data";
    
    system(execCommand.c_str()); //it will gives mac ID
    
    
    system("ifconfig | awk '$0 ~ /HWaddr/ { print $5 }' > data2");//it will give the hardware ID
    
    string hdId, MACAddr;
    
    infile.open ("data");
    getline(infile,hdId); // copied to respective name hdId
    infile.close();
    
    infile.open ("data2");
    getline(infile,MACAddr); //copied to respective name MACAddr
    infile.close();
    
    string uID = hdId + MACAddr; //created a string uID
    
    system("rm -rf osVersion mountLocation data data2");  //removed those unnecessary data
    
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    string PresentTime;
    char date[9];
    strftime( date, sizeof(date), "%x", t );
    string date_val = date; //created a string and stored the present date
    
    uID = uID + "$_$" + date_val;//added the present date to the generated uID
    
    
    char *name_file = new char[uID.length() + 1];//it is genearating the universalID with out encrypting.
    strcpy(name_file, uID.c_str());
    //cout <<"universal ID"<<name_file<<endl;
    
    return name_file;
}


bool fVerifyPassword(char *uID, char *pass)
{
   vector<int> Key;
   Key.resize(128);
    Key=encodeAES(uID);
   
    char *val = pass;
    vector<int> KeyValue;
    string val1 = val;
    vector<int> pos;
    pos.resize(16);
    pos[0]=0;
    for(int i=1;i<16;i++)
    {
      pos[i] = val1.find('-',pos[i-1]+1);
    }
    
    KeyValue.push_back(atoi(val1.substr(0,pos[1]).c_str()));
    for(int i=1;i<pos.size()-1;i++)
    {
          KeyValue.push_back(atoi(val1.substr(pos[i]+1, (pos[i+1]-pos[i]-1)).c_str()));
    }
    KeyValue.push_back(atoi(val1.substr(pos[15]+1,val1.length()).c_str()));
    
    bool isSucess= true;
    
    for(int i=0;i<16;i++)
    {
      if(Key[i] != KeyValue[i])
	  isSucess = false;
    }
    
    return isSucess;
}

//encrypt data
void encrypt (char e[] ) 
{
for( int i=0; e[i] != '\0'; ++i ) e[i] += 4;
} // encrypt

//decrypt data
void decrypt( char * ePtr ) {
for( ; * ePtr != '\0'; ++ ePtr ) (* ePtr) -= 4;
}
