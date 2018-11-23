#include<iostream>
#include<fstream>
#include<string.h>
#include<stdlib.h>
using namespace std;
#define MAX_NUMBER_OF_CHILDREN 100

typedef struct node
{
  int depth;
  int flag; // tells us if the inner value is a list of nodes or string
  char* name;
  char* value;
  struct node* innerNodes[MAX_NUMBER_OF_CHILDREN]; // list of nodes under it
  int no_of_children;
  char json_id[2];
} Node;

Node* initNode(int depth)
{
  Node* r = (Node*)malloc(sizeof(Node));
  for(int i=0;i<MAX_NUMBER_OF_CHILDREN;i++)
  {
    r->innerNodes[i]=NULL;
  }
  r->no_of_children=0;
  r->depth = depth;
  r->json_id[1]='\0';
  return r;
}

void mergit(Node* base, Node* head)
{
  if(head==NULL)
  {
    return;
  }
  for(int i=0; i<base->no_of_children; i++)
  {
    if(strcmp(base->innerNodes[i]->name,head->name)==0)
    {
      if(head->no_of_children==0 || base->innerNodes[i]->no_of_children == 0)
      {
        char* temp = (char*)malloc(sizeof(char)*(strlen(head->name)+3)); //+3 for additional '/'
        temp[0] = '/';
        temp[1] = '/';
        strcpy(temp+2,head->name);
        free(head->name);
        head->name = temp;
        for(int k=base->no_of_children; k>i+1; k--)
        {
          base->innerNodes[k]=base->innerNodes[k-1];
        }
        base->innerNodes[i+1]=head;
        base->no_of_children++;
      }
      else
      {
        base->innerNodes[i]->json_id[0]='3';
        Node* comment = initNode(head->depth);
        comment->name = (char*)malloc(sizeof(char)*(strlen(head->name)+3)); //+3 for additional '/'
        comment->name[0] = '/';
        comment->name[1] = '/';
        strcpy(comment->name+2,head->name);
        comment->name[strlen(head->name)+2]='\0';
        comment->value = (char*)malloc(sizeof(char)*2);
        strcpy(comment->json_id,"3\0");
        comment->value[0]='$';
        comment->value[1]='\0';
        for(int k=base->no_of_children; k>i; k--)
        {
          base->innerNodes[k]=base->innerNodes[k-1];
        }
        base->innerNodes[i]=comment;
        base->no_of_children++;
        i+=1;
        for(int j=0; j<head->no_of_children; j++)
        {
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
  comment->value[0]='#';
  comment->value[1]='\0';
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

void setnodemode(node* root, char mode, int* check, char** nodemode, int depth, int i)
{
  if(*check==0)
  {
    if(root->name[0]=='/' && root->name[1]=='/')
    {
      if(root->no_of_children==0 && root->value[0]=='#')
      {
        *check=1;
        nodemode[depth][i]='2';
      }
      else if(root->no_of_children==0 && root->value[0]=='$')
      {
        *check=1;
        nodemode[depth][i]='3';
      }
      else
      {
        nodemode[depth][i]='2';
      }
    }
    else
    {
      nodemode[depth][i]='1';
    }
  }
  else
  {
    *check=0;
    nodemode[depth][i]=nodemode[depth][i-1];
  }
}

void print_tree(node* root, char mode, char** nodemode, int depth, int index, FILE* file) //flag is for checking for comments type
{
  int count=0;
  if(root==NULL)
  {
    return;
  }
  int check=0;
  nodemode[depth+1] = (char*)malloc(root->no_of_children*sizeof(char));
  nodemode[depth+1][0]='0';
  for(int i=0; i<root->no_of_children; i++)
  {
    setnodemode(root->innerNodes[i],mode,&check,nodemode,depth+1,i);
  }
  /*cout << depth+1;
  for(int i=0; i<root->no_of_children; i++)
  {
    cout << nodemode[depth+1][i] << " ";
  }
  cout << '\n';*/
  if(nodemode[depth][index]==mode || mode=='3' || nodemode[depth][index]=='3')
  {
    fputs("\"",file);
    cout << root->name;
    fputs(root->name,file);
    fputs("\"",file);
    fputs(" : ",file);//<<endl;
    if(root->no_of_children>0)
    {
      fputs("\n",file);
      for(int d=0; d<root->depth+1; d++)
      fputs("    ",file);
      fputs("{\n",file);
      for(int d=0; d<root->depth+1; d++)
      fputs("    ",file);
    }
    else
    {
      fputs("\"",file);
      fputs(root->value,file);
      fputs("\"",file);
    }
    for(int i=0;i<root->no_of_children;i++)
    {
      print_tree(root->innerNodes[i],mode,nodemode,depth+1,i,file);
      if(nodemode[depth+1][i]==mode || mode=='3' || nodemode[depth+1][i]=='3')
      {
        int flag=0;
        for(int j=i+1;j<root->no_of_children;j++)
        {
          if(nodemode[depth+1][j]==mode || mode=='3' || nodemode[depth+1][j]=='3')
          {
            flag=1;
            break;
          }
        }
        count++;
        if(flag && count<root->no_of_children)
        {
            fputs(",",file);
            fputs("\n",file);
            for(int d=0; d<root->depth+1; d++)
            fputs("    ",file);
        }
      }
    }

    if(root->no_of_children>0)
    {
      fputs("\n",file);
      for(int d=0; d<root->depth+1; d++)
      fputs("    ",file);
      fputs("}",file);
    }
  }
}

void print_tree_t(node* root, char mode)
{
  int count=0;
  if(root==NULL)
  {
    return;
  }

  if(root->json_id[0]==mode || mode=='3' || root->json_id[0]=='3')
  {
    cout << "\"" <<root->name <<"\"" << " : ";//<<endl;
    if(root->no_of_children>0)
    {
      cout<< "\n" << string((root->depth+1)*5,' ');
      cout<<"{";
    }
    else cout<< "\"" << root->value<<"\"";
    for(int i=0;i<root->no_of_children;i++)
    {
      print_tree_t(root->innerNodes[i],mode);
      if(root->innerNodes[i]->json_id[0]==mode || mode=='3' || root->innerNodes[i]->json_id[0]=='3')
      {
        int flag=0;
        for(int j=i+1;j<root->no_of_children;j++)
        {
          if(root->innerNodes[j]->json_id[0]==mode || mode=='3' || root->innerNodes[j]->json_id[0]=='3')
          {
            flag=1;
            break;
          }
        }
        count++;
        if(flag && count<root->no_of_children)
        {
            cout<< ","<< "\n" << string((root->depth+1)*5,' ');
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
  char* fullfile1 = readfile(&length1,"json_files/base.json");
  cout << fullfile1 << "\n"<< length1 << "\n";
  char* fullfile2 = readfile(&length2,"json_files/head.json");
  cout << fullfile2 << "\n"<< length2 << "\n";

  char name[100];
  char value[100];
  int index1=0;
  int index2=0;

  FILE* file1;
  file1 = fopen("json_results/result_base.json","w");
  FILE* file2;
  file2 = fopen("json_results/result_head.json","w");
  FILE* file3;
  file3 = fopen("json_results/result_merged.json","w");

  Node* base = initNode(-1);
  base->name = (char*)malloc(10);
  strcpy(base->name,"~");
  strcpy(base->json_id,"3");

  Node* head = initNode(-1);
  head->name = (char*)malloc(10);
  strcpy(head->name,"~");
  strcpy(head->json_id,"3");

  genjson(fullfile1, length1, &index1, 0, base,"1");
  genjson(fullfile2, length2, &index2, 0, head,"2");

  merge(base,head);
  cout << endl << endl << endl << endl;

  char* nodemode[100];
  int check=0;
  nodemode[0] = (char*)malloc(sizeof(char));
  nodemode[0][0]='3';

  cout << '\n';

  cout << "String1 ";
  print_tree(base,'1',nodemode,0,0,file1);

  cout << "\n\nCorrect1 ";
  print_tree_t(base,'1');

  cout << "\n\nString2 ";
  print_tree(base,'2',nodemode,0,0,file2);

  cout << "\n\nCorrect2 ";
  print_tree_t(base,'2');

  cout << "\n\nString3 ";
  print_tree(base,'3',nodemode,0,0,file3);

  cout << "\n\nCorrect3 ";
  print_tree_t(base,'3');

  int depth=1;

}
