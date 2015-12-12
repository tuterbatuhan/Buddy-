
//Authors:
//Batuhan Tuter
//ID: 21200624
//İlter Kavlak
//ID: 21201961

#include <stdlib.h>
#include <stdio.h>

//Algrotihm explained	
	//'a' for allocated
	//'f' for free
	//'p' for if it is a parent(which means it is divided before and has childrens)
	//Left child of a node is at 2*i+1 where i is (memory address of node - begining pointer)
	//Right child of a node is at 2*i+2 where i is (memory address of node - begining pointer)
	//Parent of a a node is at (i-1)/2 where i is (memory address of node - begining pointer)
//End of algorithm explanation

unsigned long chunkSize; //Size of the chunk
char *beginningPointer; //Points to the begining of the chunk
int externalMaxPowerNum; //holds the n where 2^n is the total size that can be defined by the tree
int availabilityArray[17];

void* balloc(int objectsize);
char* recursiveBallocSearcherForced(int size, int desiredSize, char *memoryPointer, char *returnMemoryPointer);
char* recursiveBallocSearcherFinder(int size, int desiredSize, char *memoryPointer, char *returnMemoryPointer);
void freeIndice(char * freeptr,int index);
void findRemoveIndice (unsigned long place,unsigned long chunkStart, unsigned long chunkEnd ,char* memoryPointer, int index);

int power(int x)
{
	int i = 0;
	int result = 1;
	while(i<x){
		result = result*2;
		i++;
	}
	return result;
}

int binit(void *chunkpointer, int size)
{
	
	if(size<32||size>32768)
		return -1;
	
	chunkSize = size*1024;//chunksize is given in kbytes, convert it into bytes

	int internalMaxPowerNum = 0;
	while(power(internalMaxPowerNum)<chunkSize)
		internalMaxPowerNum++;//Calculate the n where 2^(n-1) is smaller than chunkSize and 2^n is larger than chunkSize
	
	externalMaxPowerNum = internalMaxPowerNum;//Set the n for the total size that can be defined by the tree
	
	int i;
	for(i = 0; i<16; i++)
		availabilityArray[i] = 0;

	availabilityArray[internalMaxPowerNum-8] = 1;//Sets the 2^n to 1 as there is one free node at size 2^n where 2^n>chunkSize and 2^(n-1)<chunkSize
	
	beginningPointer = (char *)chunkpointer;//Set the begining
	
	//Begining of the tree creation
	char *charptr = (char*)chunkpointer;//Create pointer to iterate over memory
	
	int n = internalMaxPowerNum-7;//As in the worst case user allocated 256 bytes per request(2^8 = 256, 2^internalMaxPowerNum/2^8=chunkSize/256)
		
	for(i = 0;i<(power(n)-1); i++)
		charptr[i] = 'f';
	
	//End of tree creation	

	//Begining of allocating the size for tree in order to be used entire process.
		
	//There will be 2^(n-1) nodes at leaf in the worst case, thus there will be 2^n-1 nodes in tree total in the worst case
	
	int x = power(n)-1;
	
	if(x<256)
		x=256;
	balloc(x);//As the size of a char is only 1 byte, this portion won't be able to allocated anymore or freed until the process finishes.	
	
	//Ending of allocating the size for tree in order to be used entire process.
	
	return (0);		// if success
}

void* balloc(int objectsize)
{
	
	if(objectsize<256||objectsize>65536)
		return 0;
	//Start from the left. Find or create a suitable place. If node is a parent check childs.
	//Calculate the memory location
	//If found place starts after (beginningPointer+chunkSize) return null
	//If found place starts before (beginningPointer+chunkSize) and finishes after (beginningPointer+chunkSize) return null
	//If found place is OK allocate. Return pointer to the begining of the allocated space
	
	int internalMaxPowerNum = 0;
	
	while(power(internalMaxPowerNum)<objectsize)
		internalMaxPowerNum++;//Calculate the n where 2^(n-1) is smaller than objectsize and 2^n is larger than objectsize
	
	if(power(internalMaxPowerNum)>=chunkSize)
	{
		printf("Error: requested size is too large!");
		return 0;
	}
	else
	{	
		if(availabilityArray[internalMaxPowerNum-8]>0)
		{
			char * result = recursiveBallocSearcherFinder(externalMaxPowerNum, internalMaxPowerNum, beginningPointer, beginningPointer);
			if(result<beginningPointer+chunkSize&&result+objectsize<=beginningPointer+chunkSize)
				return result;
			else
				return NULL;
		}
		else
		{
			char * result = recursiveBallocSearcherForced(externalMaxPowerNum, internalMaxPowerNum, beginningPointer, beginningPointer);
			if(result<beginningPointer+chunkSize&&result+objectsize<=beginningPointer+chunkSize)
				return result;
			else
				return NULL;
		}
	}
	return (NULL);		// if not success
}

char* recursiveBallocSearcherForced(int size, int desiredSize, char *memoryPointer, char *returnMemoryPointer){//Finds a suitable place for allocation by dividing
	
	if(memoryPointer[0] == 'a')//If the node is allocated return 0
		return 0;
	else if(memoryPointer[0] == 'p')//If the node is a parent node
	{
		if(desiredSize == size)//If it is not available and has a size of requested size return
			return 0;
		
		else if(desiredSize < size)//If desired size is less than current size
		{
			int blabla = power(size-1);
			char *leftChild = recursiveBallocSearcherForced(size-1, desiredSize, ((memoryPointer-beginningPointer)*2+1)+beginningPointer, returnMemoryPointer);//Continue to the left child
			if(leftChild != 0)//Go to left child
				return leftChild;
			
			char *rightChild = recursiveBallocSearcherForced(size-1, desiredSize, ((memoryPointer-beginningPointer)*2+2)+beginningPointer, returnMemoryPointer+blabla);//Continue to the right child
			if(rightChild != 0)
				return rightChild;
			
		}
	}
	else if(memoryPointer[0] == 'f')//If it is a free node
	{
		if(desiredSize < size)//If desired size is less than current size
		{
			memoryPointer[0] = 'p';//Divide the node
			availabilityArray[size-9]++;//Rearrange availability array
			availabilityArray[size-9]++;
			availabilityArray[size-8]--;
			if(desiredSize==size-1)//If the left child's size is desired size
			{
				(((memoryPointer-beginningPointer)*2+1)+beginningPointer)[0] = 'a';//Allocate left child
				availabilityArray[size-9]--;//Rearrange availability array
				return returnMemoryPointer;//Return allocated address
			}
			else
			{
				return recursiveBallocSearcherForced(size-1, desiredSize, ((memoryPointer-beginningPointer)*2+1)+beginningPointer, returnMemoryPointer);//Search to deeper
			}
		}
	}
	return 0;
}

char* recursiveBallocSearcherFinder(int size, int desiredSize, char *memoryPointer, char *returnMemoryPointer)
{//Finds a sutiable place for desired size

	if(memoryPointer[0] == 'a')//If node is an allocated node return
		return 0;
	
	else if(memoryPointer[0] == 'p')//If node is a parent node
	{
		if(desiredSize == size)//If desired size is the current size return
			return 0;
		else if(desiredSize < size)//If the desired size is less than current size
		{
			int blabla = power(size-1);
			char *leftChild = recursiveBallocSearcherFinder(size-1, desiredSize, ((memoryPointer-beginningPointer)*2+1)+beginningPointer, returnMemoryPointer);//Search through left child
			if(leftChild != 0)//Go to left child
				return leftChild;
			
			char *rightChild = recursiveBallocSearcherFinder(size-1, desiredSize, ((memoryPointer-beginningPointer)*2+2)+beginningPointer, returnMemoryPointer+blabla);//Search through right child
			if(rightChild != 0)
				return rightChild;
		}
	}
	else if(memoryPointer[0] == 'f')//If the node is a free node
	{
		if(desiredSize == size)//If the current size is desired size
		{
			memoryPointer[0] = 'a';//Set node allocated
			availabilityArray[size-8]--;//Rearrange availability array
			return returnMemoryPointer;//Return allocation address
		}
	}
	return 0;
}

void bprintRec(char * memoryPointer, int n, char *addressPointer){
	if(memoryPointer[0]!='a'&&memoryPointer[0]!='f'&& n>=8){
		bprintRec((((memoryPointer-beginningPointer)*2+1)+beginningPointer), n-1, addressPointer);
		bprintRec((((memoryPointer-beginningPointer)*2+2)+beginningPointer), n-1, addressPointer+power(n-1));
	}else{
		printf("\n-State: %c, Size: 2^%d, Address %lx-", memoryPointer[0], n, addressPointer);
	}
}

void bprint(void)//Prints leaves of the tree
{
	//Print the tree in order. User pre-order traverse algorithm. Calculate addresses and print only states of leaves.
	printf("\n----Current Memory----\nFrom Left To Right Order\n**a stands for allocated, f stands for free**\n");
	bprintRec(beginningPointer, externalMaxPowerNum, beginningPointer);
}

void bfree(void *objectptr)
{
	//Calculates the position
	//If position is at beginningPointer do not free. Tree is there.
	//Else free.
	//If it is a child and sibling is also free, free the child make the parent free.(As it was parent)
	
	if((char *)objectptr > (char *)beginningPointer && ((char *)objectptr <= (char * )beginningPointer+chunkSize))
	{
		unsigned long place = (unsigned long)((char *)objectptr - (char *)beginningPointer);
		findRemoveIndice(place, 0,power(externalMaxPowerNum),(char *) beginningPointer,externalMaxPowerNum);
		return;
	}
	else
	{
		printf("\nbfree bounds are not valid!");
		return;
	}	
}

void findRemoveIndice (unsigned long place,unsigned long chunkStart, unsigned long chunkEnd ,char* memoryPointer, int index)
{
	if(memoryPointer[0]=='p')
	{
		unsigned long middle = ((chunkEnd-chunkStart)/2)+chunkStart;
		if(place<middle)
		{
			char * leftChild = ((memoryPointer-(char *)beginningPointer)*2+1+(char *)beginningPointer); 
			findRemoveIndice (place,chunkStart,middle, leftChild,index-1);
		}
		if(place>=middle)
		{
			char * rightChild = ((memoryPointer-(char *) beginningPointer)*2+2+(char *)beginningPointer); 
			findRemoveIndice (place,middle, chunkEnd, rightChild,index-1);
		}	
	}
	else
		freeIndice(memoryPointer,index);
}
void freeIndice(char * freeptr,int index)
{
	if(freeptr[0]=='a')
	{
		freeptr[0]='f';
		availabilityArray[index-8]++;//Increase the availability number
		if (freeptr == beginningPointer)//We have the situation that all memory is freed
			return;
		char * parent = (((freeptr-(char *)beginningPointer)-1)/2 + (char *)beginningPointer);
		char * leftChild = ((parent-(char *)beginningPointer)*2+1+(char *)beginningPointer); 		
		char * rightChild = ((parent-(char *)beginningPointer)*2+2+(char *)beginningPointer); 
		if(leftChild[0]=='f' && rightChild[0]=='f')//If sibling is free
		{
			parent[0] = 'f';
			availabilityArray[index-7]++;//Increase the availability of parent
			availabilityArray[index-8]=availabilityArray[index-8]-2;//Decrease the availability of both childs
			freeIndice(parent,++index);
		}
	}
	else if(freeptr[0]=='f')
	{
		if (freeptr == beginningPointer)//We have the situation that all memory is freed
			return;
		char * parent = (((freeptr-(char *)beginningPointer)-1)/2 + (char *)beginningPointer);
		char * leftChild = ((parent-(char *)beginningPointer)*2+1+(char *)beginningPointer); 		
		char * rightChild = ((parent-(char *)beginningPointer)*2+2+(char *)beginningPointer); 
		if(leftChild[0]=='f' && rightChild[0]=='f')//If sibling is free
		{
			parent[0] = 'f';
			availabilityArray[index-7]++;//Increase the availability of parent
			availabilityArray[index-8]=availabilityArray[index-8]-2;//Decrease the availability of both childs
			freeIndice(parent,++index);
		}
	}
	else
	{
		printf("\nError in freeIndice");	
		return;
	}
}
