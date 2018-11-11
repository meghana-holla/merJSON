#include<iostream>
#include<fstream>
using namespace std;

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

void genjson(char* json, int len, int* index, int depth)
{
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
      genjson(json, len, index, depth+1);
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
      return;
    }
    if(json[*index]==',') break;
    *index+=1;
  }
  *index+=1;

  cout << name << "-" << value << "-" << depth << "\n";
  if(*index<len)
  {
    return genjson(json, len, index, depth);
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
  genjson(fullfile, length, &index, 0);
}
