#include <cstdint>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <cstring>
using namespace std;


/* Bus Operation types */
#define READ 1 /* Bus Read */
#define WRITE 2 /* Bus Write */
#define INVALIDATE 3 /* Bus Invalidate */
#define RWIM 4 /* Bus Read With Intent to Modify */
/* Snoop Result types */
#define NOHIT 0 /* No hit */
#define HIT 1 /* Hit */
#define HITM 2 /* Hit to modified line */
/* L2 to L1 message types */
#define GETLINE 1 /* Request data for modified line in L1 */
#define SENDLINE 2 /* Send requested cache line to L1 */
#define INVALIDATELINE 3 /* Invalidate a line in L1 */
#define EVICTLINE 4 /* Evict a line from L1 */
// this is when L2's replacement policy causes eviction of a line that
// may be present in L1. It could be done by a combination of GETLINE
// (if the line is potentially modified in L1) and INVALIDATELINE.

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/* Used to represent states in MESI protocol */
#define invalid 0 
#define exclusive 1  
#define shared 2 
#define modified 3 
//////////////////////////////////////////////////////////////////////////////////////////////////////////
#define address_length 32 /// addres length
#define sets 32768 // number of  sets 
#define ways 8// number of ways 
///////////////////////////////////////////////////////////////////////////////////

class cache_simulator
{
	
	struct {
		short int PLRU; ///////////// LSB being the start of the root ////////// {d,e,f,g,b,c,a}////////////////// 
		int line [ways]; //////////// Tag array//////// {Messi bits[1:0],tagbits} ////////////// 
	} cache[sets];


	public:

	void update_state(unsigned int state, unsigned int set, unsigned int way);	
	int check_state(unsigned int state, unsigned int set, unsigned int way);

//


    //utilities
    unsigned int HexToDec(string inAddr);

    int get_addr(unsigned int set,unsigned int way);
    int bitExtracted(unsigned int addr, int k, int p);            ///////////// used to select tag or index or byte offset from addr////////////https://www.geeksforgeeks.org/extract-k-bits-given-position-number/
    void updatePLRU(unsigned int set,unsigned int way);
    int getLRU(unsigned int set);
    int check_for_empty_way(unsigned int set);
    void write_cache( unsigned int addr);
///// Mark faust's functions 
	int GetSnoopResult(unsigned int Address);
    void BusOperation(int BusOp, unsigned int Address, int *SnoopResult);
    void PutSnoopResult(unsigned int Address, int SnoopResult);
    void MessageToCache(int Message, unsigned int Address) ;     
};



int cache_simulator::check_for_empty_way(unsigned int set)
 {
   int way_temp;
   for (int i=0; i<8 ; i++)
   {
   if(bitExtracted(cache[set].line[i], 2,12)==0)
   way_temp=i;
   else
   way_temp=8;  
   }
   return way_temp;
 }

void cache_simulator::updatePLRU(unsigned int set,unsigned int way) {
	switch (way)
{
case 0: cache[set].PLRU= (cache[set].PLRU | 0x45); break;
case 1: cache[set].PLRU= (cache[set].PLRU & 0xbf)| 0x05;break;
case 2: cache[set].PLRU= (cache[set].PLRU & 0xfb)| 0x21;break;
case 3: cache[set].PLRU= (cache[set].PLRU & 0xdb)| 0x01;break;
case 4: cache[set].PLRU= (cache[set].PLRU & 0xfe)| 0x12;break;
case 5: cache[set].PLRU= (cache[set].PLRU & 0xee)| 0x02;break;
case 6: cache[set].PLRU= (cache[set].PLRU & 0x0c)| 0x08;break;
case 7: cache[set].PLRU= (cache[set].PLRU & 0x04);break;	
	}	
} 


int cache_simulator::getLRU(unsigned int set)
{
	if(!(cache[set].PLRU & 0x01)) ////////////!root a
	{
     	if(!(cache[set].PLRU & 0x04))/////////////! root b 
     	{
      		if(!(cache[set].PLRU & 0x40))/////////////! root d 
      			return(0);
      		else if(cache[set].PLRU & 0x40)/////////////// root d
      			return(1);
      		else 
      		return(8);
	 	}

	   else if(cache[set].PLRU & 0x04)/////////////root b  
   	   {
     		if(!(cache[set].PLRU & 0x20))/////////////! root e
      			return(2);
      		else if (cache[set].PLRU & 0x20)/////////////// root e
      			return(3);
      		else 
      		return(8);
	   }
	   else 
	   return (8);
    }
	 ///////////////////////////half //////////////////////////////////////
	else if (cache[set].PLRU & 0x01)//////////root a 
	{
	 	if(!(cache[set].PLRU & 0x02))/////////////! root c
     	{
      		if(!(cache[set].PLRU & 0x10))/////////////! root f
      			return(4);
      		else if (cache[set].PLRU & 0x10)/////////////// root f
      			return(5);
      		else 
      		return(8);
	 	}
	    else if(cache[set].PLRU & 0x02)/////////////root c 
     	{
     		if(!(cache[set].PLRU & 0x08))/////////////! root g
      			return(6);
      		else if (cache[set].PLRU & 0x08)/////////////// root g
      	        return(7);
             else 
      		 return(8);
    }
    else 
    return(8);
	}
	 else 
	 return(8);
	

}

/* Simulate the reporting of snoop results by other caches */
int cache_simulator::GetSnoopResult(unsigned int Address) {
	/* returns HIT, NOHIT, or HITM */
int SnoopResult =bitExtracted(Address, 2,22);
return (SnoopResult);
}

/*
Used to simulate a bus operation and to capture the snoop results of last level
caches of other processors
*/
void cache_simulator::BusOperation (int BusOp, unsigned int Address, int *SnoopResult) 
{
	*SnoopResult = GetSnoopResult(Address);
//if (mode == "normal")
	printf("BusOp: %d, Address: %h, Snoop Result: %d\n",BusOp,Address, *SnoopResult);
}

/* Report the result of our snooping bus operations performed by other caches */
void cache_simulator::PutSnoopResult(unsigned int Address, int SnoopResult) {
//if (NormalMode)
	printf("SnoopResult: Address %h, SnoopResult: %d\n", Address, SnoopResult);
}

/* Used to simulate communication to our upper level cache */
void cache_simulator::MessageToCache (int Message, unsigned int Address) {
//if (NormalMode)
	printf("L2: %d %h\n", Message, Address);
}

int cache_simulator::bitExtracted (unsigned int addr, int k, int p) 
{
    return (((1 << k) - 1) & (addr >> (p - 1)));
}


int cache_simulator::get_addr(unsigned int set,unsigned int way)
{
   int temp_addr=0;
   int temp_tag=bitExtracted(cache[set].line[way], 11,22);
   temp_addr=set<<6;
   temp_addr= temp_addr|temp_tag<<21;
   return(temp_addr);
}


unsigned int cache_simulator::HexToDec(string inAddr){                                      // Function For Hex To Unsigned Dec Address Converstion
	//cout<<"[INFO]The input Hex Address is :"<<inAddr<<endl;
	 int len;
	 unsigned int dec = 0;
	 int base = 1;
	int i;

	len = inAddr.size();
	i = len-1;
	if(!(len <= 8))
		cout<<"[WARNING]: HexToDec Function -> Address Out Of Range "<<endl;
	else{

	   // cout<<"value of size is :"<<len<<endl;
        if(len < 8) {
            short int diff_len;
            string s = "0";
            diff_len = 8 - len;
            for (int i = 0; i < diff_len; i++) {
                s += inAddr;
            }
            //cout<<"[INFO]: The Corrected Hex Addr is -> "<<inAddr<<endl;
        }
	   do
	   {
	      //i = len;
	        if (inAddr[i] >= '0' && inAddr[i] <= '9') {

                 dec += (int(inAddr[i]) - 48) * base;

                 base = base * 16;

                }
			else if (inAddr[i] >= 'A' && inAddr[i] <= 'F') {
                 //cout<<"BASE is "<<base<<endl;
                 dec += (int(inAddr[i]) - 55) * base;

                 base = base * 16;}
            else if(inAddr[i] >= 'a' && inAddr[i] <= 'f')
            {
               // cout<<"BASE is "<<base<<endl;
                dec += (int(inAddr[i]) - 87) * base;
                base = base*16;
            }
                	i--;
            //cout<<"[DEC]   value     is -->"<<dec<<endl;
	    }while(i>=0);
	}
  return dec;
}
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
		  int int_address;
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
              int_address = cache_sim.HexToDec(address);
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
