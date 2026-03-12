#include"../include/events.h"
//std::deque<event> temp_events;

std::priority_queue<event,std::deque<event>> sim_events;
unsigned long int cltime=0.0;
int pri=0;
void printque(std::priority_queue<event> pq)
{
  while(!pq.empty())
  {
    std::cout<<pq.top()<<std::endl;
    pq.pop();
  }
}

void add_event(unsigned long int executionTime, int pri, EventID id, std::function< void() > obj, int slPriority){
  
  event ev(executionTime,pri,id,obj,slPriority);
//   std::cout<<"added event :"<<ev<<std::endl;
  sim_events.push(ev);
  
//   std::cout<<"status of pq"<<std::endl;
//   printque(sim_events);
//   std::cout<<"++++++"<<std::endl;
}

// void update_fel(){
//   std::stable_sort(sim_events.begin(), sim_events.end());
// }


void add_event(event ev)
{
  
  sim_events.push(ev);
}
event getnext_event()
{
//   std::cout<<" pqsize "<<sim_events.size()<<std::endl;
//   std::cout<<"status of pq before execution "<<std::endl;
//   std::cout<<"--------"<<std::endl;
//   printque(sim_events);
  
  
  if(!sim_events.empty()){
    auto ev=sim_events.top();
    sim_events.pop();
    set_simtime(ev.time_stamp);
    set_priority(ev.priority);
    return ev;
  }//need to handle else case
  std::cout<<"Sim Events Empty...Aborting in getnext_event()"<<std::endl;
  abort();
}
unsigned long int getnext_simtime(){
//   if(!sim_events.empty()){
//     auto ev=sim_events.top();
//     
//     return ev.time_stamp;
//     
//   }
//   return 0;
  return cltime;
}
void set_simtime(unsigned long int a){ cltime=a;}
  

EventID getnext_eventid(){
  if(!sim_events.empty()){
    auto ev=sim_events.top();
    
    return ev.event_id;
  }
//   std::cout<<"pq is empty,returning END_SIMULATION"<<std::endl;
  return END_SIMULATION;
}

std::function<void()> getnext_object(){
  if(sim_events.empty()){
    auto ev=sim_events.top();
    return (std::function<void()>)(ev.object);
  }//else is not handled as of now
  std::cout<<"Sim Events Not Empty...Aborting in getnext_event()"<<std::endl;
  abort();
}

void erase_current_event()
{
  sim_events.pop();
//   std::cout<<" pqsize "<<sim_events.size()<<std::endl;
//   std::cout<<"status of pq after execution "<<std::endl;
//   printque(sim_events);
//   std::cout<<"--------"<<std::endl;
}

int getnext_priority(){
  if(!sim_events.empty()){
    auto ev=sim_events.top();
    
    return ev.priority;
    
  }
  return -1.0;
}
void set_priority(int p){ pri=p;}

/*std::vector<event> sim_events;


void add_event(int t,int pri,EventID eventID,std::function<void()> object){
    event ev(t,pri,eventID,object);
    sim_events.push_back(ev);
}

void update_fel(){
  std::stable_sort(sim_events.begin(), sim_events.end());
}

int getnext_simtime(){
  return sim_events[0].time_stamp;
}

int getnext_priority(){
  return sim_events[0].priority;
}

EventID getnext_eventid(){
  return sim_events[0].event_id;
}

std::function<void()> getnext_object(){
  return (std::function<void()>)(sim_events[0].object);
}

void erase_current_event()
{
  sim_events.erase(sim_events.begin());
}*/



