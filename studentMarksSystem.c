#include<stdlib.h>
#include<conio.h>
#include<stdio.h>
#pragma warning(disable:4001)
//makes a table of students and marks
struct marks{
	int mark;
	struct marks*nextStudMarks;
	struct marks*nextSubMarks;
};

struct student{
	char name[25];
	int key;
	struct student *nextStudent;
	struct marks *marksList;
};

void error(char *str){
	//prints error
	printf("%s",str);
	_getch();
	return;
}

void takeWord( FILE *fp,char* word,char delimiter){
	//take a word from file and saves in word
	int iter;
	char ch;
	word[0]='\0';	//inisializing
	do{
		ch=(char)fgetc(fp);
		for(iter=0;iter<25 && (ch>='a' && ch<='z') || (ch>='A' && ch<='Z') || ch==' ';iter++){	//checks valid charecter and enough space in array
			word[iter] = ch;
			ch=(char)fgetc(fp);
		}
	}while(word[0]=='\0' && ch == '\n');	//ignores empty lines
	if(iter==25){
		error("word too long");				//array size exeded
		return;
	}
	if(ch!=delimiter){
		error("invalid word ");				//checks if it ended as expected
		return;
	}
	word[iter]='\0';
}

int keyVal(char *word){
	//calculates a key value which will be easy to search the word later
	char ch=word[0];
	int key=0;
	for(int iter=0;ch!='\0' && (ch>='a' && ch<='z') || (ch>='A' && ch<='Z');iter++){
		if(ch>='A' && ch<='Z')			//converts everything into lowercase 
			ch=ch + (int)'a' - (int)'A';
		key = (key*10) + (int)ch - (int)'a';
		ch=word[iter];
	}
	return key;
}

int takeNum(char*ch,FILE *fp){
	//take a number from file and returns it
	long int ret=0;
	if(*ch==' ')		//checks if alreary ch is read
		*ch=(char)fgetc(fp);
	while(*ch>='0' && *ch<='9'){
		ret=(ret * 10) + (*ch - '0');
		*ch=(char)fgetc(fp);
	}
	if(*ch!='\n' && *ch!=',' && *ch!=EOF){
		error("invalid number");
		return 0;
	}
	return ret;
}

void takeNewStud(FILE *fp,char *ch,struct student *studHead){
	//takes name and marks of new student
	struct marks *list=studHead->marksList;
	struct marks *tempList;
	struct student *newStud=(struct student *) malloc(sizeof(struct student ));
	//takes name of student and assignes a key value for it
	takeWord(fp,newStud->name,'\n');
	newStud->key=keyVal(newStud->name);
	newStud->nextStudent=studHead->nextStudent;
	studHead->nextStudent=newStud;
	//takes marks of student
	newStud->marksList=(struct marks*)malloc(sizeof(struct marks));
	tempList=newStud->marksList;
	while(list!=NULL && tempList->nextStudMarks!=NULL){
		//for eash subject
		*ch=(char)fgetc(fp);
		if((*ch>='a' && *ch<='z') || (*ch>='A' && *ch<='Z')){
			ungetc(*ch,fp);
			takeWord(fp,newStud->name,'\n');
			newStud->key=keyVal(newStud->name);
			*ch=' ';
		}
		tempList->mark = takeNum(ch,fp);
		tempList->nextSubMarks=(struct marks*)malloc(sizeof(struct marks*));
		tempList->nextStudMarks=list->nextStudMarks;
		list->nextStudMarks=tempList;
		list=list->nextSubMarks;
		tempList=tempList->nextSubMarks;
	}
	tempList->nextSubMarks=NULL;
}

void displayAll(struct student *studHead,char subjects[20][25],int count){
	//displays all marks of every student
	struct marks *tempList;
	int iter;
	while(studHead->nextStudent!=NULL){
		studHead=studHead->nextStudent;
		printf_s("%s\n",studHead->name);
		tempList=studHead->marksList;
		for(iter=0;iter<count && tempList !=NULL;iter++){
			//prints subject name, marks
			printf_s("\t%s :%d \n",subjects[iter],tempList->mark);
			tempList=tempList->nextSubMarks;
		}
	}
}

void displayStudMarks(char *name,struct student *studHead,char subjects[20][25],int count){
	//displays a students marks
	struct marks *tempList;
	int iter,key=keyVal(name);
	while(studHead->nextStudent!=NULL){
		studHead=studHead->nextStudent;
		if(studHead->key==key){		//searchs the name using keys
			printf_s("%s\n",studHead->name);
			tempList=studHead->marksList;
			for(iter=0;iter<count && tempList !=NULL;iter++){
				//prints all marks
				printf_s("\t%s :%d \n",subjects[iter],tempList->mark);
				tempList=tempList->nextSubMarks;
			}
			return;
		}
	}
	if(studHead->nextStudent==NULL)
		printf_s("student name not found\n");
}

void displayStudSubMarks(char *name,char*subject,struct student *studHead,int *keys,int count){
	//displays student's perticular subject marks
	struct marks *tempList;
	int iter,keyName=keyVal(name),keySubject=keyVal(subject);
	while(studHead->nextStudent!=NULL){
		studHead=studHead->nextStudent;
		if(studHead->key==keyName){				//searchs the name using keys
			printf_s("%s got :",studHead->name);
			tempList=studHead->marksList;
			for(iter=0;iter<count && tempList !=NULL;iter++){
				if(keySubject==keys[iter]){		//searchs the subject using keys
					printf_s("%d marks in %s\n",tempList->mark,subject);
					break;
				}
				tempList=tempList->nextSubMarks;
			}
			if(iter==count)
				printf_s("subject name not found\n");
			return;
		}
	}
	if(studHead->nextStudent==NULL)
		printf_s("student name not found\n");
}

void displayStudMaxMarks(char *name,struct student *studHead,char subjects[20][25],int count){
	//displays student's maximum scored subject
	struct marks *tempList;
	int iter,iter1,key=keyVal(name),max=-1,maxIndex[15]={0};
	while(studHead->nextStudent!=NULL){
		studHead=studHead->nextStudent;
		if(studHead->key==key){		//searchs the name using keys
			printf_s("Maximum marks of %s :\n",studHead->name);
			tempList=studHead->marksList;
			for(iter=0;iter<count && tempList !=NULL;iter++){
				if(tempList->mark > max){		//finds the max 
					iter1=0;
					max=tempList->mark;
					maxIndex[0]=iter;
				}
				else if(tempList->mark == max){	//stores the list of subjects of max
					iter1++;
					maxIndex[iter1]=iter;
				}
				tempList=tempList->nextSubMarks;
			}
			for(iter=0;iter<=iter1;iter++)	
				//and prints the list
				printf_s("\t%s :%d \n",subjects[maxIndex[iter]],max);
			return;
		}
	}
	if(studHead->nextStudent==NULL)
		printf_s("student name not found\n");
}

void displayStudAvgMarks(char *name,struct student *studHead,char subjects[20][25],int count){
	//displays student's average score 
	struct marks *tempList;
	int iter,key=keyVal(name),avg=0;
	while(studHead->nextStudent!=NULL){
		studHead=studHead->nextStudent;
		if(studHead->key==key){		//searchs the name using keys
			printf_s("Average marks of %s :",studHead->name);
			tempList=studHead->marksList;
			for(iter=0;iter<count && tempList !=NULL;iter++){
				avg=avg+tempList->mark;		//calculates average
				tempList=tempList->nextSubMarks;
			}
			printf_s("%g \n",avg*1.0/count);
			return;
		}
	}
	if(studHead->nextStudent==NULL)
		printf_s("student name not found\n");
}

void displaySubMarks(char *name,struct student *studHead,int *keys,int count){
	//displays the subject marks
	struct marks *tempList=studHead->marksList;
	int iter,key=keyVal(name);
	for(iter=0;iter<count;iter++)
		if(key==keys[iter])		break;		//searchs the subject name using keys
	if(iter==count)		 printf("subject not found\n");
	else{
		for(count=0;count<iter;count++)
			tempList=tempList->nextSubMarks;
		while(studHead->nextStudent!=NULL && tempList->nextStudMarks !=NULL){
			//prints name of student and his marks
			studHead=studHead->nextStudent;
			tempList=tempList->nextStudMarks;
			printf_s("\n%s : %d",studHead->name,tempList->mark);
		}
	}
}

int main(){
	FILE *fp;
	char ch=' ', subjects[20][25],name[25]; 
	int count,keys[20],iter;
	struct student *studHead=(struct student *) malloc(sizeof(struct student *));
	struct marks *temp;
	if(fopen_s(&fp,"input.txt","r")!=NULL){
		error("unable to open file");
		return 0;
	}
	//inisialize head of table
	studHead->key=0;
	studHead->nextStudent=NULL;
	studHead->marksList=NULL;
	count=takeNum(&ch,fp);
	if(count-- >0){
		studHead->marksList=(struct marks*)malloc(sizeof(struct marks*));
		temp=studHead->marksList;
		temp->mark=0;
		temp->nextStudMarks=NULL;
	}
	//table headers for all subjects
	for(iter=0;iter<count;iter++){
		takeWord(fp,subjects[iter],',');
		keys[iter]=keyVal(subjects[iter]);
		temp->nextSubMarks=(struct marks*)malloc(sizeof(struct marks*));
		temp=temp->nextSubMarks;
		temp->mark=0;
		temp->nextStudMarks=NULL;
	}
	temp->nextSubMarks=NULL;
	takeWord(fp,subjects[count],'\n');
	keys[count]=keyVal(subjects[count]);
	count++;
	//takes the student details
	ch='\n';
	while(ch=='\n')
		takeNewStud(fp,&ch,studHead);
	while(1){
		printf_s("\nchoose a number \n1.Display all students marks \n2.Display perticular student's marks \n3.Display perticular student's perticular mark's \n4.Maximum marks of a student \n5.Average marks of a student \n6.Marks of perticular subject \n7.exit \n");
		scanf_s("%d",&iter);
		fflush(stdin);
		switch(iter){
			case 1:
				displayAll(studHead,subjects,count);
				break;
			case 2:
				printf_s("name: ");
				gets_s(name);
				displayStudMarks(name,studHead,subjects,count); 
				break;
			case 3:
				char sub[25];
				printf_s("name: ");
				gets_s(name);
				fflush(stdin);
				printf_s("subject: ");
				gets_s(sub);
				displayStudSubMarks(name,sub,studHead,keys,count); 
				break;
			case 4:
				printf_s("name: ");
				gets_s(name);
				fflush(stdin);
				displayStudMaxMarks(name,studHead,subjects,count); 
				break;
			case 5:	
				printf_s("name: ");
				gets_s(name);
				fflush(stdin);
				displayStudAvgMarks(name,studHead,subjects,count); 
				break;
			case 6:
				printf_s("subject: ");
				gets_s(name);
				fflush(stdin);
				displaySubMarks(name,studHead,keys,count); 
				break;
			default:
				return 0;
		}
		_getch();
	}
}