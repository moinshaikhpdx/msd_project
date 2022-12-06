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
float cache_reads, cache_writes, cache_hits, cache_misses, cache_hit_ratio; 
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
		int tag_array [ways]; //////////// Tag array//////// {Messi bits[1:0],tagbits} ////////////// 
	} cache[sets];


	public:
		int NormalMode;
//m
	void update_state(unsigned int state, unsigned int set, unsigned int way);	
	int check_state(unsigned int set, unsigned int way);
	void snooping(int operation, int addr);

    //j
        int check_hit(unsigned int set,unsigned int tag);
        void read_cache( unsigned int addr);
        unsigned int HexToDec(string inAddr);
//s
       void updatePLRU(unsigned int set,unsigned int way);
       void write_cache( unsigned int addr);
       int getLRU(unsigned int set); 
         void report();
    
//d
       int get_addr(unsigned int set,unsigned int way);
       int bitExtracted(unsigned int addr, int k, int p);            ///////////// used to select tag or index or byte offset from addr////////////https://www.geeksforgeeks.org/extract-k-bits-given-position-number/
       int check_for_empty_way(unsigned int set);
       void clear_cache();
       void print_cache();

///// Mark faust's functions 
       int GetSnoopResult(unsigned int Address);
       void BusOperation(int BusOp, unsigned int Address, int SnoopResult);
       void PutSnoopResult(unsigned int Address, int SnoopResult);
       void MessageToCache(int Message, unsigned int Address) ;     
};


void cache_simulator::report()
{
printf("----------------------------------------------------------------------------------------------------- \n");
printf("Number of cache reads = %f \n",cache_reads);
printf("Number of cache writes = %f \n",cache_writes);
printf("Number of cache hits  = %f \n",cache_hits);
printf("Number of cache misses = %f \n",cache_misses);
cache_hit_ratio= cache_hits/(cache_hits+cache_misses);
printf("Cache hit ratio  %f \n",cache_hit_ratio);
}

void cache_simulator::print_cache()
{
    printf("Printing valid cache lines \n");
    printf("//////////////////////////////////////////////////////////////////////////// \n");
    printf("MESI               TAG         SET         WAY\n");
    int valid;
    int set_no;
    for(int i=0;i<=32767;i++)
    {
	 valid = 0;
   	 for(int j=0;j<=7;j++)
    	{
    		if(check_state(i,j)!=0)
    		{
			switch(check_state(i, j))
			{
				case invalid:   printf("Invalid  ");break;
				case shared:    printf("shared   ");break;
				case exclusive: printf("exclusive");break;
				case modified:	printf("modified ");break;
			}
   	     		set_no=i;
    	     		printf("          %d        %d        %d\n",cache[i].tag_array[j], set_no, j);
			valid = 1;
        	}
       }
       if(valid)
       	printf("psuedo bits (HEXA) %x \n",cache[i].PLRU);
    }
}

void cache_simulator::clear_cache()
{
    cout<<"Clearing cache\n";
    for(int i=0;i<=32767;i++)
{
    cache[i].PLRU=0;
    for(int j=0;j<=7;j++)
    cache[i].tag_array[j]=0;
}
}

void cache_simulator::snooping(int operation, int addr)
{

        int set_temp =bitExtracted(addr, 15,7);
        int tag_temp =bitExtracted(addr, 11,22);
        int way_temp = check_hit(set_temp,tag_temp);
	int SnoopResult;
	if(way_temp != 8)
	{
		switch(operation)
		{
			case READ :

				 switch(check_state(set_temp, way_temp))
				{
					case invalid:
						break;

					case exclusive:
						PutSnoopResult(addr, HIT);
  						update_state(shared, set_temp, way_temp);
						break;

					case shared:
						PutSnoopResult(addr, HIT);
						break;

					case modified:
						PutSnoopResult(addr, HITM);
                        MessageToCache(GETLINE,get_addr(set_temp,way_temp));
						BusOperation (WRITE, (addr&0xffffffc0), GetSnoopResult(addr)); /// snarfing is done 
						MessageToCache(INVALIDATE,get_addr(set_temp,way_temp));
  						update_state(shared, set_temp, way_temp);
						break;

				}

			       	break;
			
			case WRITE :

				 switch(check_state(set_temp, way_temp))
				{
					case invalid:
						break;

					case exclusive:
						break;

					case shared:
						break;

					case modified:
						break;
				}
	
			       	break;

			case INVALIDATE :
				 switch(check_state(set_temp, way_temp))
				{
					case invalid:
						break;

					case exclusive:
						break;

					case shared:
						MessageToCache(INVALIDATE,get_addr(set_temp,way_temp));
  						update_state(invalid, set_temp, way_temp);
						break;

					case modified:
						break;
				}
		                break;

			case RWIM :

				 switch(check_state(set_temp, way_temp))
				{
					case invalid:
						break;

					case exclusive:
						MessageToCache(INVALIDATE,get_addr(set_temp,way_temp));
  						update_state(invalid, set_temp, way_temp);
						break;

					case shared:
						MessageToCache(INVALIDATE,get_addr(set_temp,way_temp));
  						update_state(invalid, set_temp, way_temp);
						break;
					case modified:
						MessageToCache(GETLINE,get_addr(set_temp,way_temp));
						BusOperation (WRITE, (addr&0xffffffc0), GetSnoopResult(addr)); 
  						MessageToCache(INVALIDATE,get_addr(set_temp,way_temp));
  						update_state(invalid, set_temp, way_temp);
						break;
				}

		       		break;

		}
	}


}
int cache_simulator::check_hit(unsigned int set, unsigned int tag){
  	int way_temp;
	 for(int i = 0; i < 8 ; i++)
	 {
		 if(tag == bitExtracted(cache[set].tag_array[i],11,1) && (check_state(set,i) != invalid))
         {
			 way_temp = i;
             break;
         }
		 else

	         way_temp = 8;
	 }
	 return way_temp;
}

void cache_simulator::write_cache(unsigned int addr)
{
	cache_writes++;
   int way_temp;
   int set_temp =bitExtracted(addr, 15,7);
   int tag_temp =bitExtracted(addr, 11,22);
   int temp_addr;
   int SnoopResult;

   if(check_hit (set_temp,tag_temp)!=8)
   {///////hit occured/////////////
   	cache_hits++;
   	cout<<"cache hit"<<endl;
   	way_temp = check_hit (set_temp,tag_temp);
   	if(check_state(set_temp,way_temp)==shared) //// bus op for hit
   	{ 
    		BusOperation(INVALIDATE,(addr&0xffffffc0),GetSnoopResult(addr));
   	}
   	update_state(modified, set_temp, way_temp);   
   	MessageToCache(SENDLINE,get_addr(set_temp,way_temp));
   	updatePLRU(set_temp,way_temp);
}
else 
{///// miss occured//////
	cache_misses++;
   if(check_for_empty_way(set_temp)!=8)
   {/////empty way found////////////
      cout<<"cache miss and empty way found"<<endl;
      way_temp=check_for_empty_way(set_temp);
      BusOperation(RWIM,(addr&0xfffffff8),GetSnoopResult(addr));//// bus op for miss with an empty way
      cache[set_temp].tag_array[way_temp] = tag_temp;
      update_state (modified, set_temp, way_temp);
      MessageToCache(SENDLINE,get_addr(set_temp,way_temp));
      updatePLRU(set_temp,way_temp);
   }
   else 
   {////// replace another line //////
      cout<<"cache miss and replacing another line"<<endl;
      way_temp=getLRU(set_temp);
      if(check_state(set_temp, way_temp)==modified);
      {
      		temp_addr=get_addr(set_temp, way_temp);
      		MessageToCache(EVICTLINE,temp_addr);
      		BusOperation(WRITE,temp_addr,GetSnoopResult(addr)); //// bus op for replacing a modified line
      }
      BusOperation(RWIM,(addr&0xfffffff8),GetSnoopResult(addr));
      cache[set_temp].tag_array[way_temp] = tag_temp;
      update_state( modified, set_temp, way_temp);
       MessageToCache(SENDLINE,get_addr(set_temp,way_temp));
      updatePLRU(set_temp,way_temp);
   }
}
}



void cache_simulator::read_cache(unsigned int addr){

  cache_reads++;
	int tag_temp = bitExtracted(addr,11,22);
	int set_temp = bitExtracted(addr,15,7);
	int way_temp = check_hit(set_temp,tag_temp);
	int current_state = check_state(set_temp,way_temp);

    	int LRU_addr = getLRU(set_temp);
    	int Snoop_result;
    	int empty_way = check_for_empty_way(set_temp);
	if(way_temp == 8)
	{// miss occured
			cache_misses++;
	     if(empty_way == 8)
		 { ///replace line
             cout<<"[INFO] CACHE MISS AND REPLACING A LINE "<<endl;

			 if(check_state(set_temp,LRU_addr) == modified)
             {
				 MessageToCache(EVICTLINE,get_addr(set_temp,LRU_addr));
				 BusOperation(WRITE,get_addr(set_temp,LRU_addr),GetSnoopResult(addr));
				 BusOperation(READ,(addr&0xfffffff8),GetSnoopResult(addr));
                 if(GetSnoopResult(addr) == NOHIT)
                 {
                     cache[set_temp].tag_array[LRU_addr] = tag_temp;
                     update_state(exclusive,set_temp,LRU_addr);
                 }
                 else if((GetSnoopResult(addr) == HIT) || (GetSnoopResult(addr) == HITM))
                 {
                     cache[set_temp].tag_array[LRU_addr] = tag_temp;
                     update_state(shared,set_temp,LRU_addr);
                 }
				 MessageToCache(SENDLINE,get_addr(set_temp,LRU_addr));
			 }
			 else
			 {
			    BusOperation(READ,(addr&0xfffffff8),GetSnoopResult(addr));
                if(GetSnoopResult(addr) == NOHIT)
                {
                    cache[set_temp].tag_array[LRU_addr] = tag_temp;
                    update_state(exclusive,set_temp,LRU_addr);
                }
                else if((GetSnoopResult(addr) == HIT) || (GetSnoopResult(addr) == HITM))
                {
                    cache[set_temp].tag_array[LRU_addr] = tag_temp;
                    update_state(shared,set_temp,LRU_addr);
                }
			    MessageToCache(SENDLINE,get_addr(set_temp,LRU_addr));

			 }
         }
         else{
             cout<<"[INFO] CACHE MISS AND FOUND A EMPTY LINE "<<endl;
             if(GetSnoopResult(addr) == NOHIT)
             {
                 BusOperation(READ,(addr&0xfffffff8),GetSnoopResult(addr));
                 cache[set_temp].tag_array[empty_way] = tag_temp;
                 update_state(exclusive,set_temp,empty_way);
                 updatePLRU(set_temp,empty_way);
                 MessageToCache(SENDLINE,get_addr(set_temp,empty_way));
             }
             else if((GetSnoopResult(addr) == HIT) || (GetSnoopResult(addr) == HITM))
             {
                 BusOperation(READ,(addr&0xfffffff8),GetSnoopResult(addr));
                 cache[set_temp].tag_array[empty_way] = tag_temp;
                 update_state(shared,set_temp,empty_way);
                 updatePLRU(set_temp,empty_way);
                 MessageToCache(SENDLINE,get_addr(set_temp,empty_way));
             }
         }
	}
	else{////way hit
		cache_hits++;
		cout<<"[INFO] CACHE HIT "<<endl;
		update_state(current_state,set_temp,way_temp);
		updatePLRU(set_temp,way_temp);
		MessageToCache(SENDLINE,addr);
	}
}


int cache_simulator::check_for_empty_way(unsigned int set)
 {
   int way_temp;
   for (int i=0; i<=7 ; i++)
   {
   	if(bitExtracted(cache[set].tag_array[i], 2,12)==0)
   	{
   		way_temp=i;
   		break;
   	}
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
	int SnoopResult =bitExtracted(Address,2,1);
	if(SnoopResult==0)
    		return HIT;
	else if(SnoopResult==1)
    		return HITM;
	else 
    		return NOHIT;
}



/*
Used to simulate a bus operation and to capture the snoop results of last level
caches of other processors
*/
void cache_simulator::BusOperation (int BusOp, unsigned int Address, int SnoopResult) 
{

    string snoopresult_string,busop_string;

    if(SnoopResult==2)
        snoopresult_string="HITM";
    else if(SnoopResult==1)
        snoopresult_string="HIT";
    else
        snoopresult_string="NOHIT";

     if(BusOp==1)
        busop_string="READ";
    else  if(BusOp==2)
        busop_string="WRITE";
    else  if(BusOp==3)
        busop_string="INVALIDATE";
    else  if(BusOp==4)
        busop_string="RWIM";
cout<<snoopresult_string;
if (NormalMode)
     printf("BusOp: %s, Address: %x, Snoop Result: %s \n",busop_string.c_str(),Address,snoopresult_string.c_str());

}

/* Report the result of our snooping bus operations performed by other caches */
void cache_simulator::PutSnoopResult(unsigned int Address, int SnoopResult) {

  string snoopresult_string;

    if(GetSnoopResult(Address)==2)
        snoopresult_string="HITM";
    else if(GetSnoopResult(Address)==1)
        snoopresult_string="HIT";
    else 
        snoopresult_string="NOHIT";

if (NormalMode)
	printf("SnoopResult: Address %x, SnoopResult: %s\n", Address, snoopresult_string.c_str());
}

/* Used to simulate communication to our upper level cache */
void cache_simulator::MessageToCache (int Message, unsigned int Address) {
	string l2tol1_message;

	if(Message==1)
		l2tol1_message="GETLINE";
	else if(Message==2)
		l2tol1_message="SENDLINE";
	else if(Message==3)
		l2tol1_message="INVALIDATELINE";
	else if (Message==4)
		l2tol1_message="EVICTLINE";


if (NormalMode)
	printf("L2: %s %x \n ", l2tol1_message.c_str(), Address);
}

int cache_simulator::bitExtracted (unsigned int addr, int k, int p) 
{
    return (((1 << k) - 1) & (addr >> (p - 1)));
}


int cache_simulator::get_addr(unsigned int set,unsigned int way)
{
   int temp_addr=0;
   int temp_tag=bitExtracted(cache[set].tag_array[way], 11,1);
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
      cache[set].tag_array[way] = (cache[set].tag_array[way] & ~(3<<11)) | (state<<11);
}


int cache_simulator::check_state(unsigned int set, unsigned int way)
{
	return (cache[set].tag_array[way] >> 11);
}
/////////////////////////////////////////////main///////////////////////////////////////////////
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
	 cache_sim.clear_cache();
	 while(getline(my_file, line))
              {
                  istringstream iss(line);
                  string s;
		  int command;
		  string address;
		  unsigned int int_address;
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
              if(mode=="normal")
              cache_sim.NormalMode=1;
              else if(mode=="silent")
              cache_sim.NormalMode=0;
              if(cache_sim.NormalMode==1)
              {
              cout<<"//////////////////////////////////////////////////////////////////////////////////////"<<endl;
		      cout<<"Command = "<<command<<" Address = "<<address<<"\n";
		  }
              int_address = cache_sim.HexToDec(address);
              
		      switch (command) {
			      case 0: cache_sim.read_cache(int_address); break;
	              case 1: cache_sim.write_cache(int_address); break;
			      case 2: cache_sim.read_cache(int_address); break;
			      case 3: cache_sim.snooping(INVALIDATE, int_address); break;
			      case 4: cache_sim.snooping(READ, int_address); break;
			      case 5: cache_sim.snooping(WRITE, int_address); break;
			      case 6: cache_sim.snooping(RWIM, int_address); break;
			      case 8: cache_sim.clear_cache(); break;
			      case 9: cache_sim.print_cache();break;
			}
               }
     }
     else
    {
   	cout<<"Unable to open any file";
    }

   }
   cache_sim.report();
   return 0;
  
}
