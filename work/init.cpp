#include <cstdint>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <cstring>
using namespace std;


class cache_simulator
{
	
	struct {
		short int PLRU; ///////////// LSB being the start of the root ////////// {d,e,f,g,b,c,a}////////////////// 
		int line [8]; //////////// Tag array//////// {Messi bits[1:0],tagbits} ////////////// 
	} cache[32768];


	public:

	void update_state(unsigned int state, unsigned int set, unsigned int way);	
	int check_state(unsigned int state, unsigned int set, unsigned int way);
};

void cache_simulator::update_state(unsigned int state, unsigned int set, unsigned int way)
{
      cache[set].line[way] = (cache[set].line[way] & ~(3<<11)) | (state<<11);

}

int cache_simulator::check_state(unsigned int state, unsigned int set, unsigned int way)
{
	return (cache[set].line[way] >> 11);
}

int main(int argc, char* argv[])
{

   cache_simulator cache_sim;
   cache_sim.update_state(3, 10, 2);
   int my_state = cache_sim.check_state(3, 10, 2);
   cout <<my_state;

   if(argc == 1)
   {
	cout<<"Please provide trace path and mode";
   }
   else
   {
     string path = argv[1];
     string mode = argv[2];

 

     cout <<"\n\n\n Execution mode = "<<mode<<"\n\n\n";

     ifstream my_file(path);
     if(my_file.is_open())
     {
	 string line;
	 while(getline(my_file, line))
              {
                  istringstream iss(line);
                  string s;
		  int command;
		  string address;
                  int string_count = 0;
                  while (getline(iss, s, ' '))
                     {
                      if(string_count == 0)
                         {
		            command = stoi(s);
                            string_count = 1;
                         }
                      else
                         {
			   address = s;
                         }
                     }
		      cout<<"Command = "<<command<<" Address = "<<address<<"\n";

		      switch (command) {
			      case 0: break;
			      case 2: break;
			      case 3: break;
			      case 4: break;
			      case 5: break;
			      case 6: break;
			      case 7: break;
			      case 8: break;
			      case 9: break;
			}
               }
     }
     else
    {
   	cout<<"Unable to open any file";
    }

   }
   return 0;
  
}
