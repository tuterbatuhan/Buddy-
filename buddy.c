#include <stdlib.h>
#include <stdio.h>
#//include <math.h>

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
void freeIndice(char * freeptr );
char* findRemoveIndice (unsigned long place,unsigned long chunkStart, unsigned long chunkEnd ,char* memoryPointer);


int power(int x){
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
		
	for(i = 0;i<(power(n)-1); i++){
		charptr[i] = 'f';
		//printf("%d:  %c\n",i, charptr[i]);
	}
	//End of tree creation	

	//Begining of allocating the size for tree in order to be used entire process.
		
	//There will be 2^(n-1) nodes at leaf in the worst case, thus there will be 2^n-1 nodes in tree total in the worst case
	
	int x = power(n)-1;
	
	if(x<256)
		x=256;
	printf("\nsize of tree is: %d bytes", x);
	balloc(x);//As the size of a char is only 1 byte, this portion won't be able to allocated anymore or freed until the process finishes.	
	
	//Ending of allocating the size for tree in order to be used entire process.
	
	
	printf("binit called\n");

	return (0);		// if success
}

void* balloc(int objectsize)
{
	//Start from the left. Find or create a suitable place. If node is a parent check childs.
	//Calculate the memory location
	//If found place starts after (beginningPointer+chunkSize) return null
	//If found place starts before (beginningPointer+chunkSize) and finishes after (beginningPointer+chunkSize) return null
	//If found place is OK allocate. Return pointer to the begining of the allocated space
	
	int internalMaxPowerNum = 0;
	
	while(power(internalMaxPowerNum)<objectsize)
		internalMaxPowerNum++;//Calculate the n where 2^(n-1) is smaller than objectsize and 2^n is larger than objectsize
	
	if(power(internalMaxPowerNum)>=chunkSize){
		printf("Error 1");
		return 0;
	}
	else{
		//int currentCheckSize = power(externalMaxPowerNum);//Set the initial size of the node that is the head node
		if(availabilityArray[internalMaxPowerNum-8]>0){
			printf("called finder");
			char * result = recursiveBallocSearcherFinder(externalMaxPowerNum, internalMaxPowerNum, beginningPointer, beginningPointer);
			if(result<beginningPointer+chunkSize&&result+objectsize<=beginningPointer+chunkSize)
				return result;
			else
				return NULL;
			//return (void *)recursiveBallocSearcherFinder(externalMaxPowerNum, internalMaxPowerNum, beginningPointer, beginningPointer);
		}else{
			printf("called forced");
			char * result = recursiveBallocSearcherForced(externalMaxPowerNum, internalMaxPowerNum, beginningPointer, beginningPointer);
			if(result<beginningPointer+chunkSize&&result+objectsize<=beginningPointer+chunkSize)
				return result;
			else
				return NULL;
			//return (void *)recursiveBallocSearcherForced(externalMaxPowerNum, internalMaxPowerNum, beginningPointer, beginningPointer);
		}
	}
	
	printf("balloc called\n");

	return (NULL);		// if not success
}

char* recursiveBallocSearcherForced(int size, int desiredSize, char *memoryPointer, char *returnMemoryPointer){
	
	if(memoryPointer[0] == 'a'){
		return 0;
	}
	else if(memoryPointer[0] == 'p'){
		if(desiredSize == size){
			return 0;
		}else if(desiredSize < size){
			int blabla = power(size-1);
			char *leftChild = recursiveBallocSearcherForced(size-1, desiredSize, ((memoryPointer-beginningPointer)*2+1)+beginningPointer, returnMemoryPointer);
			if(leftChild != 0){//Go to left child
				return leftChild;
			}
			char *rightChild = recursiveBallocSearcherForced(size-1, desiredSize, ((memoryPointer-beginningPointer)*2+2)+beginningPointer, returnMemoryPointer+blabla);
			if(rightChild != 0){
				return rightChild;
			}
		}
	}else if(memoryPointer[0] == 'f'){
		if(desiredSize < size){
			memoryPointer[0] = 'p';
			availabilityArray[size-9]++;
			availabilityArray[size-8]--;
			if(desiredSize==size-1){
				(((memoryPointer-beginningPointer)*2+1)+beginningPointer)[0] = 'a';
				return returnMemoryPointer;
			}
			else{
				return recursiveBallocSearcherForced(size-1, desiredSize, ((memoryPointer-beginningPointer)*2+1)+beginningPointer, returnMemoryPointer);
			}
		}
	}
	return 0;
}

char* recursiveBallocSearcherFinder(int size, int desiredSize, char *memoryPointer, char *returnMemoryPointer){
	printf("\n\nsize: %d, desiredSize: %d\n, letter: %c\n", size, desiredSize, memoryPointer[0]);
	if(memoryPointer[0] == 'a'){
		return 0;
	}
	else if(memoryPointer[0] == 'p'){
		if(desiredSize == size){
			return 0;
		}else if(desiredSize < size){
			int blabla = power(size-1);
			char *leftChild = recursiveBallocSearcherFinder(size-1, desiredSize, ((memoryPointer-beginningPointer)*2+1)+beginningPointer, returnMemoryPointer);
			if(leftChild != 0){//Go to left child
				return leftChild;
			}
			char *rightChild = recursiveBallocSearcherFinder(size-1, desiredSize, ((memoryPointer-beginningPointer)*2+2)+beginningPointer, returnMemoryPointer+blabla);
			if(rightChild != 0){
				return rightChild;
			}
		}
	}else if(memoryPointer[0] == 'f'){
		if(desiredSize == size){
			memoryPointer[0] = 'a';
			availabilityArray[size-8]--;
			return returnMemoryPointer;
		}
	}
	
	return 0;
}

void bprint(void)
{
	int i;
	//Print the tree in order. User pre-order traverse algorithm. Calculate addresses and print only states of leaves.
	for(i = 0;i<(power(externalMaxPowerNum-7)); i++){
		printf("\n%d:  %c",i, beginningPointer[i]);
	}
	
	
	printf("bprint called\n");
	return;
}

void bfree(void *objectptr)
{
	//Calculates the position
	//If position is at beginningPointer do not free. Tree is there.
	//Else free.
	//If it is a child and sibling is also free, free the child make the parent free.(As it was parent)
	
	if((char *)objectptr >= (char *)beginningPointer && ((char *)objectptr <= ((char * )beginningPointer+chunkSize)))
	{
		unsigned long place = (unsigned long)((char *)objectptr - (char *)beginningPointer);
		printf("\nPlace: %lu",place);
		
		char * freeptr = findRemoveIndice(place, 0,chunkSize,(char *) beginningPointer);
		//printf ("\n%c\n",freeptr[0]);
		freeIndice(freeptr);
		
		printf("\nbfree called");

		return;
	}
	
	else
	{
		printf("bfree bounds are not valid! \n");
		return;
	}	
}

char* findRemoveIndice (unsigned long place,unsigned long chunkStart, unsigned long chunkEnd ,char* memoryPointer)
{
	
	if(memoryPointer[0]=='p')
	{

		unsigned long middle = floor((chunkEnd-chunkStart)/2)+chunkStart;
		
		printf("\nMiddle: %lu",middle);
		if(place<middle)
		{
			printf("\nHi Left!");
			char * leftChild = ((memoryPointer-(char *)beginningPointer)*2+1+(char *)beginningPointer); 
			return findRemoveIndice (place,chunkStart,middle, leftChild);
		}
		if(place>=middle)
		{
			printf("\nHi Right!");
			char * rightChild = ((memoryPointer-(char *) beginningPointer)*2+2+(char *)beginningPointer); 
			return findRemoveIndice (place,middle, chunkEnd, rightChild);
		}
		
	}
	return memoryPointer;
}
void freeIndice(char * freeptr )
{
	if(freeptr[0]=='a')
	{
		freeptr[0]='f';
		if (freeptr == beginningPointer)//We have the situation that all memory is freed
			return;
		char * parent = (((freeptr-(char *)beginningPointer)-1)/2 + (char *)beginningPointer);
		char * leftChild = ((parent-(char *)beginningPointer)*2+1+(char *)beginningPointer); 		
		char * rightChild = ((parent-(char *)beginningPointer)*2+2+(char *)beginningPointer); 
		if(leftChild[0]=='f' && rightChild[0]=='f')//If sibling is free
		{
			parent[0] = 'a';
			freeIndice(parent);
		}
	}
	else
	{
		printf("\nError in freeIndice");	
		return;
	}
}
