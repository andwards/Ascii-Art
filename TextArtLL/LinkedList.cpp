//Authors: Joel Montano, Andrew Edwards, Christopher Alan Wright, Katie Warren, and James Nolan Casey


#include <iostream>
#include <Windows.h>
#include "Definitions.h"
using namespace std;

Node* newCanvas()
{
	// Creates a new node and assigns it to Node* current. Then initializes Canvas with 2d array from node.
	Node* current = new Node;
	initCanvas(current->item);
	current->next = NULL;

	return current;
}


Node* newCanvas(Node* oldNode)
{
	// Creates a new node and assigns it to current. Copies the contents of the previous node's 2d array in the list into the current node's array.
	Node* current = new Node;
	copyCanvas(current->item, oldNode->item);
	current->next = NULL;

	return current;
}

//helper functon to get the playrecursive function going. 
//takes clips list as parameter.
void play(List& clips)
{
	// loops as long as the ESCAPE key is not currently being pressed
	while (!(GetKeyState(VK_ESCAPE) & 0x8000))
	{
		// Checks if there are more than 2 clips, if so calls playRecursive.
		if (clips.count >= 2) {
			playRecursive(clips.head, clips.count);

		}
	}
}

//recursive function that will iterate through the clips list.
//takes pointer and number of clips in list.
void playRecursive(Node* head, int count)
{
	// Checks if head is empty
	if (head == NULL) {
		return;
	}
	// If not empty, displays 2d array of item in the head struct. Displays command needed to quit out of the animation.
	playRecursive(head->next, count - 1);

	if ((GetKeyState(VK_ESCAPE) & 0x8000))
	{
		return;
	}
	displayCanvas(head->item);

	printf("Hold <ESC> key to Quit\t Clip: %3d", count);
	// Pause for 100 milliseconds to slow down animation
	Sleep(100);
}


void addUndoState(List& undoList, List& redoList, Node*& current)
{
	// Adds a node to the undoList and deletes the redoList
	addNode(undoList, newCanvas(current));
	deleteList(redoList);
}

//updates undolist and redolist with correct nodes to redo and undo. 
//takes redolist/redolist and current node. 
void restore(List& undoList, List& redoList, Node*& current)
{
	//Checks if the undoList contains any items, and if so adds the current node to the redoList and removes the first node from the undoList.
	if (undoList.count > 0)
	{
		addNode(redoList, current);
		current = removeNode(undoList);
	}
}

void addNode(List& list, Node* nodeToAdd)
{
	// Adds a node to the linked list and increments the count of items in the list
	nodeToAdd->next = list.head;
	list.head = nodeToAdd;
	list.count++;
}

Node* removeNode(List& list)
{
	//Creates a temporary node equal to the head of the list. 
	Node* temp = list.head;
	//Checks if the head is empty, and if it is not assigns the head to the next node in the list. Decrements the list counter.
	if (list.head != NULL)
	{
		list.head = temp->next;
		temp->next = NULL;
		list.count--;
	}

	return temp;
}

// Deletes the entire list. .
void deleteList(List& list)
{
	while (list.head != NULL) {
		Node* temp = list.head->next;
		delete list.head;
		list.head = temp;
	}
	list.count = 0;
}


//loads 2d arrays (canvases) to clips list and concatenates rest of filename.
//takes clips list and filename as parameter.
bool loadClips(List& clips, char filename[])
{
	char filePath[FILENAMESIZE];
	int counter = 1;
	bool fileResult;

	//deletes original list. 
	deleteList(clips);

	//do while loop that will add canvas to a newly formed node and clips list until the fileResult returns false.
	do {
		Node* current = newCanvas();
		snprintf(filePath, FILENAMESIZE, "%s-%d.txt", filename, counter);
		fileResult = loadCanvas(current->item, filePath);
		if (fileResult) {
			addNode(clips, current);
			counter++;
		}
		else {
			delete current; 
		}
	} while (fileResult); 

	if (clips.count == 0) {
		return false;
	}
	else {
		return true;
	}
}

//saves the 2d arrays (canvases) into the clips list and concatenates rest of filename. 
//takes list for clips and filename as parameters.
bool saveClips(List& clips, char filename[])
{
	char filePath[FILENAMESIZE];
	bool fileResult;
	int counter = clips.count;
	//loop that will iterate through the linked list 
	for (Node* head = clips.head; head != NULL; head = head->next)
	{
		snprintf(filePath, FILENAMESIZE, "%s-%d.txt", filename, counter);
		fileResult = saveCanvas(head->item, filePath);
		counter--;
		if (!fileResult) {
			return false;
		}
	}
	return true;
}