/*
 * indexer.c
 *      Author: Brennan
 */


/*
 ============================================================================
 Name        : indexer.c
 Author      : Brennan Ashton
 Version     : 0.2
 Description : Code Sample. Print an index of words in files provided
 ============================================================================
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define BUFLEN 100  /*This is something to start with, it will allocate more
					 *if needed
					 */

#define WORDDEL " \r\n\t~!@#$%^&*()_+`-={}|[]\\:\";'<>?,./" /*Delimiters*/

/*This is the base struct for linked lists*/
typedef struct LinkedList {
	void *data;
	struct LinkedList *nextelm;
} linkedlist;

/*It is a pain to refer to the elements always as data and nextelm
 *It kills the readability of the code, however we can get away with
 *creating additional types in the same format, much like a subclass
 */

/* A linked list type for holding words and the associated files */
typedef struct WordStr {
	char *word;
	struct LinkedList *files;
} word_str;

/* A linked list type for holding files and the associated lines */
typedef struct FilesStr {
	char *filename;
	struct LinkedList *lines;
}file_str;

/* Prototypes */
linkedlist* list_start(void *data);
linkedlist * list_add(linkedlist *oldnode, void *data);
linkedlist* merge(linkedlist *head_one, linkedlist *head_two,
		int (*comp)(linkedlist*,linkedlist*));
int linecmp(linkedlist *list1, linkedlist *list2);
int filenmcmp(linkedlist *list1, linkedlist *list2);
int wordcmp(linkedlist *list1, linkedlist *list2);
linkedlist* mergesort(linkedlist *head,int (*comp)(linkedlist*,linkedlist*));
void filemergepost(linkedlist *currlist);
void wordmergepost(linkedlist *currlist);
linkedlist* mergelist(linkedlist *wlist,int (*comp)(linkedlist*,linkedlist*),
		void (*posttask)(linkedlist*));

/**
 * linkedlist* list_start(void *data) - Used to generate a linked list
 * @data:	Pointer to data payload for first element.
 *
 * @return: Returns the created linked list element
 */
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

/**
 * linkedlist * list_add(linkedlist *oldnode, void *data)
 * 			Used to add a node to an existing linked list
 * 			New node is placed at top of the list
 *
 * @oldnode:	Pointer to current top node of linked list.
 * @data:		Pointer to payload data of new node.
 *
 * @return: Returns the new top node
 */
linkedlist * list_add(linkedlist *oldnode, void *data)
{
	linkedlist *node;
	node = (linkedlist *)malloc(sizeof(linkedlist));
	if(node == NULL)
	{
		printf("Could not allocate mem for new linked list node!\n");
		exit(-1);
	}
	node->data=(void*)data;
	node->nextelm = oldnode;
	return node;
}

/**
 * linkedlist* merge(linkedlist *head_one, linkedlist *head_two,
 * 				     int (*comp)(linkedlist*,linkedlist*))
 *
 * 			Used to merge two linked lists
 *
 * @headone:	Pointer to first linked list head.
 * @headtwo:	Pointer to second linked list head.
 * @comp:		This function allows for custom comparison
 * 				functions to be used that take in linked lists
 * 				and return <1 if headone is less than head two
 * 				and return >1 if headone is greater than head two
 *
 * @return: Returns the new merged top node
 */
linkedlist* merge(linkedlist *head_one, linkedlist *head_two,
				  int (*comp)(linkedlist*,linkedlist*))
{
	linkedlist *head_three;

	/* if any one of the linked lists are empty just return the other */
	if(head_one == NULL)
		return head_two;

	if(head_two == NULL)
		return head_one;

	/*Example for a string comparison
	 *if(strcmp(head_one->word, head_two->word)<0)
	 */
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

/**
 * int linecmp(linkedlist *list1, linkedlist *list2)
 *
 * 			Used as a comparison function for line numbers
 * 			for a linked list merge operation.
 *
 * @list1:	Pointer to first linked list head.
 * @list2:	Pointer to second linked list head.
 * @return:	Return <1 if head one is less than head two
 * 			Return >1 if head one is greater than head two
 *
 */
int linecmp(linkedlist *list1, linkedlist *list2)
{
	return(*((int*)list1->data)-*((int*)list2->data));
}

/**
 * int filenmcmp(linkedlist *list1, linkedlist *list2)
 *
 * 			Used as a comparison function for file names
 * 			for a linked list merge operation.
 *
 * @list1:	Pointer to first linked list head.
 * @list2:	Pointer to second linked list head.
 * @return:	Return <1 if head one is less than head two
 * 			Return >1 if head one is greater than head two
 *			Comparison is executed based on strcmp
 *
 */
int filenmcmp(linkedlist *list1, linkedlist *list2)
{
	return(strcmp(((file_str *)list1->data)->filename,
			((file_str *)list2->data)->filename));
}

/**
 * int wordcmp(linkedlist *list1, linkedlist *list2)
 *
 * 			Used as a comparison function for words
 * 			for a linked list merge operation.
 *
 * @list1:	Pointer to first linked list head.
 * @list2:	Pointer to second linked list head.
 * @return:	Return <1 if head one is less than head two
 * 			Return >1 if head one is greater than head two
 *			Comparison is executed based on strcmp
 *
 * @return: Returns the new top node
 */
int wordcmp(linkedlist *list1, linkedlist *list2)
{
	return(strcmp(((word_str *)list1->data)->word,
			((word_str *)list2->data)->word));
}

/**
 * linkedlist* mergesort(linkedlist *head,int (*comp)(linkedlist*,linkedlist*))
 *
 * 			Merge sort function for linked lists
 *
 * @list1:	Pointer to linked list head.
 *
 * @comp:		This function allows for custom comparison
 * 				functions to be used that take in linked lists
 * 				and return <1 if node 1 is less than head two
 * 				and return >1 if node 2 is greater than head two
 *
 * @return: Returns the new top node of sorted linked list
 */
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

/**
 * filemergepost(linkedlist *currlist)
 *
 * 			Execute post tasks for sort on file names
 * @list1:	Pointer to linked list head.
 *
 *		When merging file listings we needs to perform
 *		some clean up on the line lists
 */
void filemergepost(linkedlist *currlist)
{
	/*sort the line numbers*/
	((file_str*)currlist->data)->lines =
			mergesort(((file_str*)currlist->data)->lines,linecmp);
}

/**
 * wordmergepost(linkedlist *currlist)
 *
 * 			Execute post tasks for sort on file names
 * @list1:	Pointer to linked list head.
 *
 *		When merging word listings we needs to perform
 *		some clean up on the file lists
 */
void wordmergepost(linkedlist *currlist)
{
	/*sort the files after a merge*/
	((word_str*)currlist->data)->files =
			mergesort(((word_str*)currlist->data)->files,filenmcmp);
	/*merge any files that are the same*/
	((word_str*)currlist->data)->files =
			mergelist(((word_str*)currlist->data)->files,filenmcmp,filemergepost);
}

/**
 * linkedlist* mergelist(linkedlist *wlist,int (*comp)(linkedlist*,linkedlist*),
 * 						 void (*posttask)(linkedlist*))
 *
 * 			Merge duplicate listings when a comparison function says
 * 			the two elements are the same.	Allow for a post function
 * 			to be run after each combine.
 *
 * @wlist:	Pointer to linked list head.
 *
 * @comp:		This function allows for custom comparison
 * 				functions to determine if the elements are
 * 				the same. A return of 0 indicates they are
 * 				the same.
 *
 * @posttask:   A pointer to a function that should be run once
 * 				a combine action has been run. The node of the
 * 				last combined list is provided
 */
linkedlist* mergelist(linkedlist *wlist,int (*comp)(linkedlist*,linkedlist*),
		void (*posttask)(linkedlist*))
{
	linkedlist *currlist = wlist;
	while(currlist !=NULL){
		if(currlist->nextelm == NULL)
			break;
		/*combine the elements when the comp function says they are the same*/
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
		/*run any cleanup tasks that are needed after a group compression*/
		((*posttask)(currlist));
		/*move on to the next element*/
		currlist = currlist->nextelm;
	}
	return wlist;
}

/*Format the final index nicely*/
void display(linkedlist *wlist)
{
	/*iterate through list displaying index*/
	while(wlist)
	{
		printf("%s\n",((word_str*)wlist->data)->word);
		while(((word_str*)wlist->data)->files)
		{
			printf("\t%s",
					((file_str*)((word_str*)wlist->data)->files->data)->filename);
			while(((file_str*)((word_str*)wlist->data)->files->data)->lines)
			{
				printf(" %d",
						*((int*)((file_str*)(
								(word_str*)wlist->data)->files->data)->lines->data));
				((file_str*)((word_str*)wlist->data)->files->data)->lines =
						((file_str*)((word_str*)wlist->data)->files->data)->
						lines->nextelm;
			}
			printf("\n");
			((word_str*)wlist->data)->files =
					((word_str*)wlist->data)->files->nextelm;
		}
		/*move to next word*/
		wlist = wlist->nextelm;
	}
}

/*Main function pass command line arg for each of the files to parse*/
int main ( int argc, char *argv[] ) {
	int i;
	linkedlist *wlist = NULL;
	file_str *filestr;
	FILE *fp;
	char *linebuf;
	unsigned int buflen;
	char *word;
	word_str *wordstr;
	int linenum;
	int *linenump;

	if(argc <= 1)
	{
		printf("Sorry no files were given!");
		return(-1);
	}


	/*iterate through the files*/
	for(i=1;i<argc;i++)
	{
		linenum = 0;
		fp = fopen(argv[i],"r");
		if(fp == NULL)
		{
			printf("Bad filename detected\n");
			return(-1);
		}
		/*reading each line*/
		while(!feof(fp))
		{
			linenum++;
			/*allocate some mem for the line read*/
			buflen = BUFLEN;
			linebuf = (char*)malloc(sizeof(char)*buflen);
			if(linebuf == NULL)
			{
				printf("Could not allocate mem to read line from file!\n");
				return(-1);
			}
			fgets(linebuf,sizeof(char)*BUFLEN,fp);
			/*if there was not enough buffer length tack
			 *on some more and read more of the file
			 */

			while(1)
			{
				if(!(feof(fp)||(linebuf[strlen(linebuf)-1]=='\n')))
				{
					buflen += BUFLEN-1;
					linebuf = (char*)realloc(linebuf,sizeof(char)*buflen);
					if(linebuf == NULL)
					{
						/*The line was too long to read
						 *With some refactoring this operation could be split
						 *into multiple parsing.  Issue is insuring a word is not
						 *cut off
						 */
						printf("Could not allocate mem to read line from file!\n");
						return(-1);
					}
				}else{
					break;
				}
				if(fgets(&linebuf[buflen-BUFLEN],sizeof(char)*BUFLEN,fp)==NULL)
				{
					/*we reached eof and no data was read make sure that word
					 *is cleared nice
					 */
					word[0]='\n';
				}
			}
			/*break up the line into words based on delimeter string*/
			word = strtok(linebuf, WORDDEL);
			while(word != NULL &&!feof(fp))
			{
				/*see if the line was blank, skip it*/
				if(linebuf[0]=='\n')
					break;
				/*we dont want the 'word' and 'word\n' to be different*/
				if(word[strlen(word)-1]=='\n')
				{
					word[strlen(word)-1] = '\0';
				}
				/*record information about the file*/

				filestr = (file_str*)malloc(sizeof(file_str));
				if(filestr == NULL)
				{
					printf("Could not allocate mem to hold list for file name!\n");
					return(-1);
				}
				filestr->filename=(char*)malloc(sizeof(char)*strlen(argv[i]));
				if(filestr->filename == NULL)
				{
					printf("Could not allocate mem to store file name!\n");
					return(-1);
				}
				strcpy(filestr->filename,argv[i]);
				linenump = (int*)malloc(sizeof(int));
				*linenump = linenum;
				filestr->lines = list_start((void*)linenump);
				/*record information about the word*/
				wordstr = (word_str*)malloc(sizeof(word_str));
				if(wordstr == NULL)
				{
					printf("Could not allocate mem for word list storage!\n");
					return(-1);
				}
				wordstr->word = (char*)malloc(sizeof(char)*strlen(word));
				if(wordstr->word == NULL)
				{
					printf("Could not allocate mem to store word!\n");
					return(-1);
				}
				strcpy(wordstr->word,word);
				/*create the linked list of files for the word*/
				wordstr->files = list_start(filestr);
				/*Add the word to the linkedlist, determine if the list is new*/
				if(wlist==NULL)
					wlist = list_start(wordstr);
				else
					wlist = list_add(wlist,wordstr);
				/*clear out the word*/
				word = strtok(NULL, WORDDEL);
			}
			/*free all of the memmory that was allocated for the line*/
			free(linebuf);
		}
		/*close the file for the next file*/
		fclose(fp);
	}
	/*sort all of the words*/
	wlist = mergesort(wlist,wordcmp);
	/*combine the file linkedlists for dup words*/
	wlist = mergelist(wlist,wordcmp,wordmergepost);
	/*Desplay formated index*/
	display(wlist);
	return (0);
}

