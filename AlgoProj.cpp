#include<iostream>
#include<fstream>
#include<string.h>
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
} Node;

char* readfile(int *i)
{
  char ch;
	const char *fileName="people.json";
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

Node* initNode(int depth)
{
  Node* r = (Node*)malloc(sizeof(Node));
  for(int i=0;i<MAX_NUMBER_OF_CHILDREN;i++)
  {
    r->innerNodes[i]=NULL;
  }
  r->no_of_children=0;
  r->depth = depth;
  return r;
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

void genjson(char* json, int len, int* index, int depth, Node* parent)
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
      genjson(json, len, index, depth+1, child);
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
    return genjson(json, len, index, depth, parent);
  }
}

/*

typedef struct node
{
  int depth;
  int flag; // tells us if the inner value is a list of nodes or string
  char* name;
  char* value;
  struct node* innerNodes[MAX_NUMBER_OF_CHILDREN]; // list of nodes under it
  int no_of_children;
} Node;

*/

void print_tree(node* root)
{
  if(root==NULL)
  {
    return;
  }
  if(root->depth>-1) cout << string(root->depth*2,'\t') << root->name << ":" << root->value <<endl;
  for(int i=0;i<root->no_of_children;i++)
  {
    print_tree(root->innerNodes[i]);
  }
}

int main()
{
  int length;
  char* fullfile = readfile(&length);
  cout << fullfile << "\n"<< length << "\n";

  char name[100];
  char value[100];
  int index=0;
  Node* root = initNode(-1);
  root->name = (char*)malloc(100);
  strcpy(root->name,"~");
  genjson(fullfile, length, &index, 0, root);
  cout << endl << endl << endl << endl;
  print_tree(root);
}
