#include <cstdint>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>

using namespace std;
class cache_simulator
{
	uint16_t *tag;
	uint8_t *lru;
	uint8_t *mesi_bits;

	int *operation;
	int *addr;
	int size;

	cache_simulator(int init_size)
	{
		size = init_size;
		tag = new uint16_t[size];
	        lru = new uint8_t[size];
		mesi_bits = new uint8_t[size];	
	}

	void read_trace(uint8_t *operation, string *addr, string PATH);// seperate instruction and operation
  	void reset_cache();
	void print_valid_cache();	



};


int main(int argc, char* argv[])
{
   if(argc == 1)
   {
	cout<<"Please provide trace path";
   }
   else
   {
     string path = argv[1];
     ifstream my_file(path);
     if(my_file.is_open())
     {
	 string line;
	 while(getline(my_file, line))
              {
                  istringstream iss(line);
                  string s;
                  int string_count = 0;
                  while (getline(iss, s, ' '))
                     {
                      if(string_count == 0)
                         {
                            cout<<"Command : "<<s<<"\n";
                            string_count = 1;
                         }
                      else
                         {
                            cout<<"Address : "<<s<<"\n";
                         }
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
