#include<stdio.h>
#include<stdlib.h>
#include<string.h>

typedef struct
{
	char ch;
	int freq;
}cnode;
typedef struct node
{
	char id[20];
	cnode c;
	struct node *left;
	struct node *right;
	int weight;
}tnode;

/*
void strrev(char *s)
{
    int n=strlen(s),i;
    char temp;
    for(i=0;i<n/2;i++)
    {
        temp=s[i];
        s[i]=s[n-1-i];
        s[n-1-i]=temp;
    }
}
*/

tnode *createnode()
{
	tnode *newnode;
	newnode=malloc(sizeof(tnode));

	newnode->left=NULL;
	newnode->right=NULL;

	return newnode;
}

void decToBin(char *bits,char ch1)
{
	strcpy(bits,"00000000");
	int num=ch1,i=7,m;
	if(num<0)
		num+=256;
	while(num!=0)
	{
		m=num%2;
		bits[i--]=m+48;
		num/=2;
	}
}

int binTochar(char *buffer)
{
	int i,sum=0;
	for(i=0;i<8;i++)
	{
		sum=sum*2+((int)buffer[i]-48);
	}
	return sum;
}

void decodeTree(FILE *cf,tnode* root,char *buffer,int *bufferIndex,int *readCount)
{

	char ch,ch1;

	ch=buffer[(*bufferIndex)++];

	if(*bufferIndex==8)
	{
		strcpy(buffer,"");
		*bufferIndex=0;
		fread(&ch1,sizeof(char),1,cf);
		(*readCount)++;
		decToBin(buffer,ch1);
	}
	if(ch=='1')
	{
		char s[9];
		int i;
		for(i=0;i<8;i++)
		{
			s[i]=buffer[(*bufferIndex)++];
			if(*bufferIndex==8)
			{
				strcpy(buffer,"");
				*bufferIndex=0;
				fread(&ch1,sizeof(char),1,cf);
				(*readCount)++;
				decToBin(buffer,ch1);
			}

		}
		ch=binTochar(s);

		root->c.ch=ch;
	}
	else if(ch=='0')
	{
		root->left=createnode();
		root->right=createnode();
		decodeTree(cf,root->left,buffer,bufferIndex,readCount);
		decodeTree(cf,root->right,buffer,bufferIndex,readCount);
	}
}

void freeAllocatedMemory(tnode *root)
{
	if(root!=NULL)
	{
		freeAllocatedMemory(root->left);
		freeAllocatedMemory(root->right);
		free(root);
	}
}

int main()
{
	int i,j;
	char ch,ch1,file[30],outputFile[30];
	
	FILE *compressedFile;
	printf("Enter the file name of compressed file: ");
	scanf("%s",file);
	compressedFile=fopen(file,"rb");
	if(compressedFile==NULL)
	{
		printf("couldn't open compressed file.\n");
		return 1;
	}
	printf("Enter the name you want to give to the decompressed file:");
	scanf("%s",outputFile);
	fseek(compressedFile,0,SEEK_END);
	//Reading the number of characters stored at the end of compressed file
	char num[20]="";
	i=0;
	fseek(compressedFile,-1L,1);
	fscanf(compressedFile,"%c",&ch1);
	while(ch1!='&')
	{
		fseek(compressedFile,-2L,1);
		num[i++]=ch1;
		fscanf(compressedFile,"%c",&ch1);

	}

	strrev(num);
	long long int cfChar=atoi(num);

	fseek(compressedFile,0,SEEK_SET);
	tnode *root,*r1;
	root=createnode();
	char buffer[9]="";
	int bufferIndex=0;
	int readCount=0;
	fread(&ch1,sizeof(char),1,compressedFile);
	readCount++;
	decToBin(buffer,ch1);
	decodeTree(compressedFile,root,buffer,&bufferIndex,&readCount);

	r1=root;

	FILE *origFile=fopen(outputFile,"w");


	for(;bufferIndex<8;bufferIndex++)
	{
		ch=buffer[bufferIndex];
		if(r1->left==NULL)
		{
			fprintf(origFile,"%c",r1->c.ch);
			r1=root;
		}
		if(ch=='0')
			r1=r1->left;
		else
			r1=r1->right;
	}
	char bits[8];

	int readCount1=readCount;
	while(readCount1<=cfChar)
	{
		fread(&ch1, sizeof(char), 1, compressedFile);
		decToBin(bits,ch1);
		for(j=0;j<8;j++)
		{
			ch=bits[j];
			if(r1->left==NULL)
			{
				fprintf(origFile,"%c",r1->c.ch);
				r1=root;
			}
			if(ch=='0')
				r1=r1->left;
			else
				r1=r1->right;

		}
		readCount1++;
	}

	fread(&ch, sizeof(char), 1, compressedFile);
	while(ch!='&')
	{

		if(r1->left==NULL)
		{
			fprintf(origFile,"%c",r1->c.ch);
			r1=root;
		}
		if(ch=='0')
			r1=r1->left;
		else
			r1=r1->right;
		fread(&ch, sizeof(char), 1, compressedFile);
	}
	fprintf(origFile,"%c",r1->c.ch);
	printf("File successfully decompressed as %s\n",outputFile);
	freeAllocatedMemory(root);
	fclose(origFile);
}