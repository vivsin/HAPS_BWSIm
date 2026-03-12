#ifndef _EVENTS_H_
#define  _EVENTS_H_
#include<iostream>
#include<deque>
#include<queue>
#include<vector>
#include<algorithm>
#include<cstring>
#include<stdint.h>
#include<functional>
#include<math.h>
#include<ostream>

enum EventID{

  TRANSMIT=0,GENERATEDATA,POSTSCHEDULE,MPIEXCHANGE,SCHEDULE,UPDATE,PRESCHEDULE,PREUPDATE,END_SIMULATION,LINKINIT,PRINTOUTPUT,PROCESS,RECEIVE,CQIREPORT
};


typedef struct events{
  unsigned long int time_stamp;
  int priority;
  int secondLevelPriority;
  //std::function<bool()> dependancy();
  EventID event_id;
  std::function<void()> object;
  events(){}
  events(unsigned long int ts,int pri,EventID id,std::function<void()> obj,int slPriority=0){
    events();
    time_stamp=ts;
    priority=pri;
    event_id=id;
    object=obj;
    secondLevelPriority=slPriority;
  }
  bool operator < (const events& evn) const {
    return((time_stamp > evn.time_stamp) || ( (time_stamp == evn.time_stamp) && (priority < evn.priority)) || ( (time_stamp == evn.time_stamp) && (priority == evn.priority) && (secondLevelPriority < evn.secondLevelPriority)));
//     return(((int)(time_stamp/1e-6) > (int)(evn.time_stamp/1e-6)) || ( ((int)(time_stamp/1e-6) == (int)(evn.time_stamp/1e-6)) && (priority < evn.priority)));
  }
}event;
inline std::ostream & operator<<(std::ostream & a , event  ev){
  
  a<<" EV  ts:"<<ev.time_stamp<<" pr:"<<ev.priority<< "id:"<<ev.event_id<<std::endl;
return a; 
}



// extern std::vector<int> event_time; 
//extern std::vector<event> sim_events;
//extern std::deque<event> temp_events;
// void sch_event(int next_event_time); 
void add_event(long unsigned int executionTime, int pri, EventID id, std::function< void() > obj, int slPriority=0); 
void add_event(event ev);
event getnext_event();
// void add_init_event(int tu, EventID id, int objid);
void update_fel();
// void remove_event(EventID id, std::function<void()> objid);
// uint64_t getnext_event_time(); 
unsigned long int getnext_simtime();
void set_simtime(unsigned long int a);
int getnext_priority();
void set_priority(int p);
EventID getnext_eventid();
std::function<void()> getnext_object();
void erase_current_event();
// void run_curr_event();

void initializeEvents();

#endif


