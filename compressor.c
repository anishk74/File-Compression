#include<stdio.h>
#include<stdlib.h>
#include<string.h>

char huffcodes[128][80];

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

typedef struct qnode
{
	tnode *x;
	struct qnode *next;
}qnode;

tnode *createnode()
{
	tnode *newnode;
	newnode=malloc(sizeof(tnode));
	newnode->left=NULL;
	newnode->right=NULL;

	return newnode;
}

void assignHuffcode(tnode *root,char *bin,int j)
{
	if(root!=NULL)
	{
		if(root->left==NULL&&root->right==NULL)
		{
			int i;
			for(i=0;i<=j;i++)
			{
				huffcodes[(int)root->id[0]][i]=bin[i];
			}
			//Remove the comment below to get the Huffman codes assigned to the characters
			//printf("%s-%s\n",root->id,huffcodes[(int)root->id[0]]);

		}
		++j;
		bin[j]='0';
		assignHuffcode(root->left,bin,j);
		bin[j]='1';
		assignHuffcode(root->right,bin,j);
	}
}

void merge(cnode *s,int low,int mid, int high)
{
	int i=low,j=mid+1,k=0;
	cnode t[high-low+1];
	while(i<=mid&&j<=high)
	{
		if(s[i].freq<s[j].freq)
			t[k++]=s[i++];

		else
			t[k++]=s[j++];
	}
	while(i<=mid)
	{
		t[k++]=s[i++];
	}
	while(j<=high)
	{
		t[k++]=s[j++];
	}
	for(i=low;i<=high;i++)
		s[i]=t[i-low];

}
void mergeSort(cnode *s,int low,int high)
{
	if(low<high)
	{
		int mid=(low+high)/2;
		mergeSort(s,low,mid);
		mergeSort(s,mid+1,high);
		merge(s,low,mid,high);
	}
}


qnode *insert(qnode *qn,tnode *tn)
{
	qnode *newnode;
	newnode=malloc(sizeof(qnode));
	newnode->next=qn;
	newnode->x=tn;
	qn=newnode;
	return qn;
}

qnode *delpnode(qnode *qn,tnode *t1)
{
	qnode *delNode;
	if(qn->x==t1)
	{
		delNode=qn;
		qn=qn->next;
		free(delNode);
		return qn;
	}
	qnode *i1;
	i1=qn;
	while(i1->next->x!=t1)
	{
		i1=i1->next;
	}
	delNode=i1->next;
	i1->next=i1->next->next;
	free(delNode);
	return qn;
}

tnode *min(qnode *qn)
{
	qnode *i,*j;
	i=qn;
	j=qn;
	while(j!=NULL)
	{
		if(j->x->weight<i->x->weight)
		i=j;
		j=j->next;
	}
	return i->x;
}

//converting buffer bits to equivalent characters
int binTochar(char *buffer)
{
	int i,sum=0;
	for(i=0;i<8;i++)
	{
		sum=sum*2+((int)buffer[i]-48);
	}
	return sum;
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

void treeOfCf(FILE *cf,tnode *root,char *buffer1,int *buffer1Index,int *cfChar)
{

	if(root==NULL)
		return;
	if(root->left==NULL)
	{
		buffer1[(*buffer1Index)++]='1';
		if(*buffer1Index==8)
		{
			fprintf(cf,"%c",binTochar(buffer1));

			(*cfChar)++;
			strcpy(buffer1,"");
			*buffer1Index=0;
		}
		char s[9];
		decToBin(s,root->c.ch);


		int i;
		for(i=0;i<8;i++)
		{
			buffer1[(*buffer1Index)++]=s[i];

			if(*buffer1Index==8)
			{
				fprintf(cf,"%c",binTochar(buffer1));

				(*cfChar)++;
				strcpy(buffer1,"");
				*buffer1Index=0;
			}
		}

	}
	else
	{
		buffer1[(*buffer1Index)++]='0';

		if(*buffer1Index==8)
		{
			fprintf(cf,"%c",binTochar(buffer1));


			(*cfChar)++;
			strcpy(buffer1,"");
			*buffer1Index=0;
		}
		treeOfCf(cf,root->left,buffer1,buffer1Index,cfChar);
		treeOfCf(cf,root->right,buffer1,buffer1Index,cfChar);
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
	int n=128,m=0,i;
	char ch,file[30],outputFileC[30];
	cnode s[n];
	for(i=0;i<n;i++)
	{
		s[i].ch=(char)i;
		s[i].freq=0;
	}
	FILE *fp,*compressedFile;
	printf("Enter the file name of the text file to be compressed:");
	scanf("%s",file);
	fp=fopen(file,"r");
	if(fp==NULL)
	{
		printf("Couldn't open input file\n");
		return 1;
	}
	//counting frequencies
	printf("Enter the file name you want to assign to the compressed file:");
	scanf("%s",outputFileC);
	while(fread(&ch,sizeof(char),1,fp))
	{
		(s[(int)ch].freq)++;
		m++;
	}

	mergeSort(s,0,n-1);

	int j=0;
	while(s[j].freq==0)
	j++;
	qnode *qu;
	qu=NULL;
	tnode *temp;
	for(i=j;i<n;i++)
	{
		temp=malloc(sizeof(tnode));
		temp->id[0]=s[i].ch;
		temp->c=s[i];
		temp->left=NULL;
		temp->right=NULL;
		temp->weight=s[i].freq;
		qu=insert(qu,temp);
	}
	qnode *q1;
	q1=qu;
	i=0;
	while(q1!=NULL)
	{
		i++;
		q1=q1->next;
	}
	tnode *root;
	tnode *m1;
	tnode *m2;
	i=0;
	while(i!=n-j-1)
	{
		m1=min(qu);
		qu=delpnode(qu,m1);
		m2=min(qu);
		qu=delpnode(qu,m2);
		tnode *parent;
		parent=malloc(sizeof(tnode));
		parent->weight = m1->weight+m2->weight;
		strcpy(parent->id,"mid-node");
		if(m1->weight<m2->weight)
		{
			parent->left=m1;
			parent->right=m2;
		}
		else
		{
			parent->left=m2;
			parent->right=m1;
		}
		qu=insert(qu,parent);
		i++;
	}

	root=qu->x;


	//char ch1,ch2;




	char bin[100];
	strcpy(bin,"0");
	assignHuffcode(root,bin,-1);
	compressedFile=fopen(outputFileC,"wb");
	char buffer1[9]="";
	int buffer1Index=0;
	int charCount=0;
	treeOfCf(compressedFile,root,buffer1,&buffer1Index,&charCount);
	fseek(fp,0,SEEK_SET);

	//writing the file in compressed file after storing the tree

	char readedCh;
	char code[80];
	int codeIndex=0;
	fread(&readedCh,sizeof(char),1,fp);
	strcpy(code,huffcodes[(int)readedCh]);

	int codeLen=strlen(code);

	for(;buffer1Index<8;)
	{
		buffer1[buffer1Index++]=code[codeIndex++];
		if(codeIndex==codeLen)
		{
			codeIndex=0;
			fread(&readedCh,sizeof(char),1,fp);
			strcpy(code,huffcodes[(int)readedCh]);
			codeLen=strlen(code);
		}
	}
	fprintf(compressedFile,"%c",binTochar(buffer1));
	char buffer[9]="";
	int bufferIndex=0;
	int flag=0;

	int buf;

	while(1)
	{
		for(bufferIndex=0;bufferIndex<8;)
		{
			if(codeIndex==codeLen)
			{
				if(fread(&readedCh,sizeof(char),1,fp)==0)
				{
					flag=1;
					for(i=0;i<bufferIndex;i++)
						fprintf(compressedFile,"%c",buffer[i]);
					break;
				}
				strcpy(code,huffcodes[(int)readedCh]);
				codeLen=strlen(code);
				codeIndex=0;
			}
			buffer[bufferIndex++]=code[codeIndex++];

		}
		if(flag==1)
			break;

		buf=binTochar(buffer);
		fprintf(compressedFile,"%c",(char)buf);

		strcpy(buffer,"");
		charCount++;
	}
	fprintf(compressedFile,"&%d",charCount);
	fclose(fp);
	fclose(compressedFile);

	freeAllocatedMemory(root);
	free(qu);
	printf("File successfully compressed as %s",outputFileC);
	return 0;
}
