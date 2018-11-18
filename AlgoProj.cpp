#include<iostream>
#include<fstream>
#include<string.h>
#include<stdlib.h>
using namespace std;
#define MAX_NUMBER_OF_CHILDREN 10

typedef struct node
{
  int depth;
  int flag; // tells us if the inner value is a list of nodes or string
  char* name;
  char* value;
  struct node* innerNodes[MAX_NUMBER_OF_CHILDREN]; // list of nodes under it
  int no_of_children;
  char json_id[3];
  int init_no;
} Node;

Node* initNode(int depth)
{
  Node* r = (Node*)malloc(sizeof(Node));
  for(int i=0;i<MAX_NUMBER_OF_CHILDREN;i++)
  {
    r->innerNodes[i]=NULL;
  }
  r->no_of_children=0;
  r->init_no=0;
  r->depth = depth;
  r->json_id[1]='\0';
  return r;
}

void mergit(Node* base, Node* head)
{
  base->init_no = base->no_of_children;
  if(head==NULL)
  {
    return;
  }
  for(int i=0; i<base->no_of_children; i++)
  {
    if(strcmp(base->innerNodes[i]->name,head->name)==0)
    {
      if(head->no_of_children==0 || (base->innerNodes[i]->no_of_children==0 && head->no_of_children!=0))
      {
        char* temp = (char*)malloc(sizeof(char)*(strlen(head->name)+3)); //+2 for additional '/' +1 for '\0'
        temp[0] = '/';
        temp[1] = '/';
        strcpy(temp+2,head->name);
        free(head->name);
        head->name = temp; //+3 for additional '/'
        strcpy(head->json_id, "3"); //Set this to 3 as well so thta it doesnt have to go to
        base->innerNodes[i]->json_id[0]='3';
        for(int k=base->no_of_children; k>i; k--)
        {
          base->innerNodes[k]=base->innerNodes[k-1];
        }
        base->innerNodes[i]=head;
        base->no_of_children++;
      }
      else
      {
	      for(int j=0; j<head->no_of_children; j++)
	      {
	        strcpy(base->innerNodes[i]->json_id,"3\0");
	        mergit(base->innerNodes[i],head->innerNodes[j]);
	      }
      }
      return;
    }
  }
  Node* comment = initNode(head->depth);
  comment->name = (char*)malloc(sizeof(char)*(strlen(head->name)+3)); //+3 for additional '/'
  comment->name[0] = '/';
  comment->name[1] = '/';
  strcpy(comment->name+2,head->name);
  comment->name[strlen(head->name)+2]='\0';
  comment->value = (char*)malloc(sizeof(char)*2);
  strcpy(comment->json_id,head->json_id);
  strcpy(comment->value,head->json_id);
  cout << "THE JSON ID IS "<<comment->json_id<<"chararacter is "<< comment->json_id[0]<<"--";
  base->innerNodes[base->no_of_children++]=comment;
  base->innerNodes[base->no_of_children++]=head;
}

void merge(Node* base, Node* head)
{
  for(int j=0; j<head->no_of_children; j++)
  {
    mergit(base,head->innerNodes[j]);
  }
}

char* readfile(int *i, const char fileName[100])
{
  char ch;
	ifstream file;
	file.open(fileName,ios::in);
  char* string = (char*)malloc(100000);
  *i=0;
	while (!file.eof())
	{
		file >> noskipws >> ch;
    if(ch!='\n')
    {
      string[*i] = ch;
      *i += 1;
    }
	}
  string[*i] = '\0';
	file.close();
  return string;
}

Node* addNode(Node* parent, int flag, void* para)
{
  if(flag==1)
  {
    Node* child = (Node*)para;
    parent->innerNodes[parent->no_of_children] = child;
    parent->no_of_children += 1;
    parent->flag=1;
  }
  else
  {
    char* value = (char*)para;
    parent->value = (char*)malloc((strlen(value)+1)*sizeof(char));
    strcpy(parent->value,value);
    parent->flag=0;
  }
}

void genjson(char* json, int len, int* index, int depth, Node* parent,char json_id[2])
{
  Node* child = initNode(depth);

  char name[100];
  char value[100];
  name[0]='\0';
  value[0]='\0';

  int inrec=0;
  while(*index<len)
  {
    if(json[*index]=='\"')
    {
      *index -= 1;
      break;
    }
    if(json[*index]=='{')
    {
      break;
    }
    if(json[*index]==',')
    {
      break;
    }
    *index+=1;
  }
  *index+=1;

  while(*index<len)
  {
    if(json[*index]=='\"')
    {break;}
    *index+=1;
  }
  *index+=1;

  int t=0;
  while(*index<len)
  {
    if(json[*index]=='\"') break;
    name[t++] = json[*index];
    *index+=1;
  }
  name[t] = '\0';
  child->name = (char*)malloc(sizeof(name));
  strcpy(child->name,name);
  strcpy(child->json_id,json_id);
  addNode(parent,1,(void*)child);
  *index+=1;

  while(*index<len)
  {
    if(json[*index]==':') break;
    *index+=1;
  }
  *index+=1;

  while(*index<len)
  {
    if(json[*index]=='{')
    {
      *index+=1;
      genjson(json, len, index, depth+1, child,json_id);
      *index-=1;
      inrec=1;
      break;
    }
    if(json[*index]=='\"') break;
    *index+=1;
  }
  *index+=1;

  t=0;
  if(inrec==0)
  {
    while(*index<len)
    {
      if(json[*index]=='\"') break;
      value[t++] = json[*index];
      *index+=1;
    }
    value[t] = '\0';
    *index+=1;
  }

  while(*index<len)
  {
    if(json[*index]=='}')
    {
      *index += 1;
      cout << name << "-" << value << "-" << depth << "\n";
      addNode(child,0,(void*)value);
      return;
    }
    if(json[*index]==',') break;
    *index+=1;
  }
  *index+=1;

  cout << name << "-" << value << "-" << depth << "\n";
  addNode(child,0,(void*)value);
  if(*index<len)
  {
    return genjson(json, len, index, depth, parent,json_id);
  }
}

void print_tree(node* root)
{
  int count=0;
  if(root==NULL)
  {
    return;
  }
  if(root->depth>-1) cout << "\"" <<root->name <<"\"" << " : ";//<<endl;
  if(root->no_of_children>0) cout<<"{";
  else cout<< "\"" << root->value<<"\"";
  for(int i=0;i<root->no_of_children;i++)
  {
    print_tree(root->innerNodes[i]);
    count++;
    if(count<root->no_of_children) cout<< ","<< endl;//fix this
    else cout<<"}";
  }
}

void print_split_tree(node* root,int mode)
{
  int count=0;
  if(root==NULL)
  {
    return;
  }
  if(!(root->name[0]=='/' && root->name[1]=='/'))
  {
  	if(root->depth>-1) cout << "\"" <<root->name <<"\"" << " : ";//<<endl;
    if(root->no_of_children>0) cout<<"{";
    else cout<< "\"" << root->value<<"\"";
  }
  // cout<<"No of children = "<<root->no_of_children<<endl;
  int i=0;
  if(mode==0)
  {
		for(int i=0;i<root->no_of_children;i++)
		{
			cout<<root->init_no<<" ";
			if(root->innerNodes[i]->name[0]!='/' && root->innerNodes[i]->name[1]!='/')
			{
		  		print_split_tree(root->innerNodes[i],0);
		  		count++;
		  		if(count<root->init_no-1) cout<<",";
			}
		  	else if(root->innerNodes[i]->name[0]=='/' && root->innerNodes[i]->name[1]=='/')
		  	{
		  		if(root->innerNodes[i]->json_id[0]=='3' && i+1<root->no_of_children)
		  		{
		  			//?
		  		}
		  		else if(root->innerNodes[i]->json_id[0]=='2' && i+1<root->no_of_children)
		  		{
		  			i+=1;
		  		}
		  	}

		}
		if(root->no_of_children>0)
		cout<<"}";
	}
}

int main()
{
  int length1;
  int length2;
  char* fullfile1 = readfile(&length1,"./merJSON/json_files/base.json");
  cout << fullfile1 << "\n"<< length1 << "\n";
  char* fullfile2 = readfile(&length2,"./merJSON/json_files/head.json");
  cout << fullfile2 << "\n"<< length2 << "\n";

  char name[100];
  char value[100];
  int index1=0;
  int index2=0;

  Node* base = initNode(-1);
  base->name = (char*)malloc(10);
  strcpy(base->name,"~");

  Node* head = initNode(-1);
  head->name = (char*)malloc(10);
  strcpy(head->name,"~");

  genjson(fullfile1, length1, &index1, 0, base,"1");
  genjson(fullfile2, length2, &index2, 0, head,"2");

  merge(base,head);
  cout << endl << endl << endl << endl;
  print_tree(base);
  cout<<endl<<endl; 
  print_split_tree(base,0);
}
