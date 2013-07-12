/*
 * indexer.c
 *
 *  Created on: Jul 12, 2013
 *      Author: Brennan
 */


/*
 ============================================================================
 Name        : indexer.c
 Author      : Brennan Ashton
 Version     : 0.1
 Description : Code Sample. Print an index of words in files provided
 ============================================================================
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define BUFLEN 100  //This is something to start with, it will allocate more if needed
#define WORDDEL " "  //Delimiter

//We use linked lists to sort and store data
typedef struct LinkedList {
	void *data;
	struct LinkedList *nextelm;
} linkedlist;

//Data in a linkedlist for a word
typedef struct WordStr {
	char *word;
	struct LinkedList *files;
} word_str;

//Data in a linkedlist for a file
//typedef struct FilesStr {
//	char *filename;
//	struct LinkedList *lines;
//}file_str;

//Prototypes
linkedlist* list_start(void *data);
linkedlist * list_add(linkedlist *oldnode, void *data);
linkedlist* merge(linkedlist *head_one, linkedlist *head_two, int (*comp)(linkedlist*,linkedlist*));
int linecmp(linkedlist *list1, linkedlist *list2);
int filenmcmp(linkedlist *list1, linkedlist *list2);
int wordcmp(linkedlist *list1, linkedlist *list2);
linkedlist* mergesort(linkedlist *head,int (*comp)(linkedlist*,linkedlist*));
void filemergepost(linkedlist *currlist);
void wordmergepost(linkedlist *currlist);
linkedlist* mergelist(linkedlist *wlist,int (*comp)(linkedlist*,linkedlist*), void (*posttask)(linkedlist*));

//Used to create a generic linkedlist
linkedlist* list_start(void *data)
{
	linkedlist *node;
	node = (linkedlist *)malloc(sizeof(linkedlist));
	if(node == NULL)
	{
		printf("Could not allocate mem!");
		exit(-1);
	}
	node->data = (void*)data;
	node->nextelm = NULL;
	return node;
}
//Used to add items to a generic linkedlist
linkedlist * list_add(linkedlist *oldnode, void *data)
{
	linkedlist *node;
	node = (linkedlist *)malloc(sizeof(linkedlist));
	if(node == NULL)
	{
		printf("Could not allocate mem!");
		exit(-1);
	}
	node->data=(void*)data;
	node->nextelm = oldnode;
	return node;
}
//Perform the merge operation for merge sort, takes in the two linked list as well as a comparison function
linkedlist* merge(linkedlist *head_one, linkedlist *head_two, int (*comp)(linkedlist*,linkedlist*))
{
	linkedlist *head_three;

	if(head_one == NULL)
		return head_two;

	if(head_two == NULL)
		return head_one;

//if(strcmp(head_one->word, head_two->word)<0)
	if(((*comp)(head_one, head_two))<0)
	{
		head_three = head_one;
		head_three->nextelm = merge(head_one->nextelm, head_two,(comp));
	} else {
		head_three = head_two;
		head_three->nextelm = merge(head_two->nextelm, head_one,(comp));
	}
	return head_three;
}
//comparison function used to compare line numbers can be passed to merge functions
int linecmp(linkedlist *list1, linkedlist *list2)
{
	return((int)list1->data-(int)list2->data);
}
//comparison function used to compare filenames can be passed to merge functions
int filenmcmp(linkedlist *list1, linkedlist *list2)
{
	return(strcmp(((linkedlist *)list1->data)->data,((linkedlist *)list2->data)->data));
}
//comparison function used to compare words can be passed to merge functions
int wordcmp(linkedlist *list1, linkedlist *list2)
{
	return(strcmp(((word_str *)list1->data)->word,((word_str *)list2->data)->word));
}
//perform a mergesort on a linked list, pass the comparison function to be used
linkedlist* mergesort(linkedlist *head,int (*comp)(linkedlist*,linkedlist*))
{
	linkedlist *head_one;
	linkedlist *head_two;
	if((head == NULL)||(head->nextelm == NULL))
		return head;

	head_one = head;
	head_two = head->nextelm;
	while((head_two != NULL) && (head_two->nextelm != NULL))
	{
		head = head->nextelm;
		head_two = head->nextelm->nextelm;
	}
	head_two = head->nextelm;
	head->nextelm = NULL;

	return merge(mergesort(head_one,(comp)), mergesort(head_two,(comp)),(comp));
}
//When merging file listings we needs to perform some clean up on the line lists
void filemergepost(linkedlist *currlist)
{
	//sort the line numbers
	((linkedlist*)currlist->data)->nextelm = mergesort(((linkedlist*)currlist->data)->nextelm,linecmp);
}
//when merging word listings we need to perform some clean up on the file lists
void wordmergepost(linkedlist *currlist)
{
	//sort the files after a merge
	((word_str*)currlist->data)->files = mergesort(((word_str*)currlist->data)->files,filenmcmp);
	//merge any files that are the same
	((word_str*)currlist->data)->files = mergelist(((word_str*)currlist->data)->files,filenmcmp,filemergepost);
}
//Generic function used to merge dup listings in a sorted linked list,  pass the function for finding dups
linkedlist* mergelist(linkedlist *wlist,int (*comp)(linkedlist*,linkedlist*), void (*posttask)(linkedlist*))
{
	linkedlist *currlist = wlist;
	while(currlist !=NULL){
		if(currlist->nextelm == NULL)
			break;
		//combine the elements when the comp function says they are the same
		while(((*comp)(currlist, currlist->nextelm))==0)
		{
			linkedlist *newfilelist;
			newfilelist = ((word_str*)currlist->nextelm->data)->files;
			newfilelist->nextelm = ((word_str*)currlist->data)->files;
			((word_str*)currlist->data)->files=newfilelist;
			currlist->nextelm = currlist->nextelm->nextelm;
			if(currlist->nextelm == NULL)
				break;
		}
		//run any cleanup tasks that are needed after a group compression
		((*posttask)(currlist));
		//move on to the next element
		currlist = currlist->nextelm;
	}
	return wlist;
}
//Format the final index nicely
void display(linkedlist *wlist)
{
	//iterate through list displaying index
	while(wlist)
	{
		printf("%s\n",((word_str*)wlist->data)->word);
		while(((word_str*)wlist->data)->files)
		{
			printf("\t%s",((linkedlist*)((word_str*)wlist->data)->files->data)->data);
			while(((linkedlist*)((word_str*)wlist->data)->files->data)->nextelm)
			{
				printf(" %d",(int)((linkedlist*)((word_str*)wlist->data)->files->data)->nextelm->data);
				((linkedlist*)((word_str*)wlist->data)->files->data)->nextelm = ((linkedlist*)((word_str*)wlist->data)->files->data)->nextelm->nextelm;
			}
			printf("\n");
			((word_str*)wlist->data)->files = ((word_str*)wlist->data)->files->nextelm;
		}
		//move to next word
		wlist = wlist->nextelm;
	}
}

//Main function pass command line arg for each of the files to parse
int main ( int argc, char *argv[] ) {
	int i;

	if(argc <= 1)
	{
		printf("Sorry no files were given!");
		return(-1);
	}

	linkedlist *wlist = NULL;
	FILE *fp;
	char *linebuf;
	unsigned int buflen;
	char *word;
	int linenum;
	//iterate through the files
	for(i=1;i<argc;i++)
	{
		linenum = 0;
		fp = fopen(argv[i],"r");
		if(fp == NULL)
		{
			printf("Bad filename detected\n");
			return(-1);
		}
		//reading each line
		while(!feof(fp))
		{
			linenum++;
			//allocate some mem for the line read
			buflen = BUFLEN;
			linebuf = (char*)malloc(sizeof(char)*buflen);
			if(linebuf == NULL)
			{
				printf("Could not allocate mem!");
				return(-1);
			}
			fgets(linebuf,sizeof(char)*BUFLEN,fp);
			//was if there was not enough buffer length tack on some more and read more of the file
			while(1)
			{
				if(!(feof(fp)||(linebuf[strlen(linebuf)-1]=='\n')))
				{
					buflen += BUFLEN-1;
					linebuf = (char*)realloc(linebuf,sizeof(char)*buflen);
					if(linebuf == NULL)
					{
						printf("Could not allocate mem!");
						return(-1);
					}
				}else{
					break;
				}
				if(fgets(&linebuf[buflen-BUFLEN],sizeof(char)*BUFLEN,fp)==NULL)
				{
					//we reached eof and no data was read make sure that word is cleared nice
					word[0]='\n';
				}
			}
			//break up the line into words
			word = strtok(linebuf, WORDDEL);
			while(word != NULL &&!feof(fp))
			{
				//see if the line was blank, skip it
				if(linebuf[0]=='\n')
					break;
				//we dont want the 'word' and 'word\n' to be different
				if(word[strlen(word)-1]=='\n')
				{
					word[strlen(word)-1] = '\0';
				}
				//record information about the file
				linkedlist *filestr;
				filestr->data=(void*)malloc(sizeof(char)*strlen(argv[i]));
				if(filestr->data == NULL)
				{
					printf("Could not allocate mem!");
					return(-1);
				}
				strcpy(filestr->data,argv[i]);
				int *linenump;
				linenump = (int*)malloc(sizeof(int));
				if(linenump == NULL)
				{
					printf("Could not allocate mem!");
					return(-1);
				}
				*linenump = linenum;
				filestr->nextelm = list_start((void*)linenump);
				//record information about the word
				word_str *wordstr;
				wordstr = (word_str*)malloc(sizeof(word_str));
				if(wordstr == NULL)
				{
					printf("Could not allocate mem!");
					return(-1);
				}
				wordstr->word = (char*)malloc(sizeof(char)*strlen(word));
				if(wordstr->word == NULL)
				{
					printf("Could not allocate mem!");
					return(-1);
				}
				strcpy(wordstr->word,word);
				//create the linked list of files for the word
				wordstr->files = list_start(filestr);
				//Add the word to the linkedlist, determine if the list is new
				if(wlist==NULL)
					wlist = list_start(wordstr);
				else
					wlist = list_add(wlist,wordstr);
				//clear out the word
				word = strtok(NULL, WORDDEL);
			}
			//free all of the memmory that was allocated for the line
			free(linebuf);
		}
		//close the file for the next file
		fclose(fp);
	}
	//sort all of the words
	wlist = mergesort(wlist,wordcmp);
	//combine the file linkedlists for dup words
	wlist = mergelist(wlist,wordcmp,wordmergepost);
	//Desplay formated index
	display(wlist);
	return (0);
}

