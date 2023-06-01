/*
* This program allows display and editing of text art (also called ASCII art).
*/


#include <iostream>
#include <fstream>
#include <cctype>
#include <windows.h>
#include <conio.h>
#include "Definitions.h"
using namespace std;

// Event handler to process Ctrl-C or other shutdown events
static BOOL WINAPI Handler(DWORD cntrlEvent);

int main()
{
	Node* current = newCanvas();
	List undoList, redoList, clipList;

	// Initializes
	char input[BUFFERSIZE], choice = {};
	bool fileResult;
	char newChr, oldChr;
	int newCol, newRow;
	bool animate = false;
	bool file = true;
	char fileName[FILENAMESIZE - 15];
	char filePath[FILENAMESIZE];
	int counter = 1;

	
	char menu[] = "<E>dit / <M>ove / <R>eplace / <D>raw / <C>lear / <L>oad / <S>ave / <Q>uit: ";
	int menuLength = (int)strlen(menu) + 20;


	if (!SetConsoleCtrlHandler(Handler, TRUE)) {
		cout << "Error setting server exit handler\n";
		return 1;
	}


	system("cls");

	do {

		// Display content into canvas array
		displayCanvas(current->item);

		cout << "<A>nimate: " << (animate ? 'Y' : 'N');
		cout << " / <U>ndo: " << undoList.count;
		if (redoList.count > 0) {
			cout << " / Red<O>: " << redoList.count;
		}
		cout << " / Cl<I>p: " << clipList.count;
		if (clipList.count >= 2) {
			cout << " / <P>lay";
		}

		// Menu to redisplay everytime an option is chosen
		printf("\n%s", menu);
		cin.getline(input, BUFFERSIZE);
		choice = toupper(input[0]);
		clearLine(MAXROWS + 2, menuLength);
		clearLine(MAXROWS + 1, menuLength);

		switch (choice) {
	
		// Changes the animate indicator of the menu
		case 'A':
			animate = !animate;
			break;

		// Calls the restore function to undo an action made on the canvas.
		case 'U':
			restore(undoList, redoList, current);
			break;

		// Calls restore function to redo an action made on the canvas
		case 'O':
			restore(redoList, undoList, current);
			break;

		// Calls the addNode function to add the current node to the clipList
		case 'I':
			addNode(clipList, newCanvas(current));
			break;
			
		// Calls the play function to animate a list of clips in clipList
		case 'P':
			play(clipList);
			break;

		// Allows user to edit the canvas
		case 'E':
			addUndoState(undoList, redoList, current);
			cout << "Press <ESC> to stop editing: ";
			editCanvas(current->item);
			break;

		// Asks for a number of column and row to move. Calls moveCanvas to move exisiting array
		case 'M':
			addUndoState(undoList, redoList, current);
			cout << "Enter column units to move: ";
			cin >> newCol;
			cout << "Enter row units to move: ";
			cin >> newRow;
			cin.ignore();
			moveCanvas(current->item, newRow, newCol);
			break;

		// Asks what character you want to replace and with what and calls on replace function to switch them
		case 'R':
			addUndoState(undoList, redoList, current);
			cout << "Enter character to replace: ";
			cin.getline(input, BUFFERSIZE);
			oldChr = input[0];
			cout << "Enter character to replace with: ";
			cin.getline(input, BUFFERSIZE);
			newChr = input[0];
			replace(current->item, oldChr, newChr);
			break;

		// Displays menuTwo with available options
		case 'D':
			menuTwo(current, undoList, redoList, clipList, animate);
			break;

		// Initializes canvas with the 2d array located in the current node.
		case 'C':
			addUndoState(undoList, redoList, current);
			initCanvas(current->item);
			break;

		// Lets user load a file to the canvas or load a list of clips to create an animation.
		case 'L':
			cout << "<C>anvas or <A>nimation ? ";
			cin >> choice;
			cin.ignore();
			clearLine(MAXROWS + 1, menuLength);

			if (toupper(choice) == 'C') {
				addUndoState(undoList, redoList, current);
				cout << "Enter the filename (don't enter 'txt'): ";

				cin.getline(fileName, FILENAMESIZE - 15);

				snprintf(filePath, FILENAMESIZE, "SavedFiles\\%s.txt", fileName);

				fileResult = loadCanvas(current->item, filePath);

				if (!fileResult) {
					cerr << "ERROR: file cannot be read" << endl;
					system("pause");
					break;
				}
			}
			
			if (toupper(choice) == 'A') {
				cout << "Enter the filename (don't enter 'txt'): ";
				cin.getline(fileName, FILENAMESIZE - 15);
				snprintf(filePath, FILENAMESIZE, "SavedFiles\\%s", fileName);

				fileResult = loadClips(clipList, filePath);

				if (!fileResult) {
					cerr << "ERROR: file cannot be read" << endl;
					system("pause");
					break;
				}
				else {
					cout << "Clips loaded! ";
					system("pause");
				}
			}
			break;

		// Lets user save a file of the canvas or save a list of clips
		case 'S':
			cout << "<C>anvas or <A>nimation ? ";
			cin >> choice;
			cin.ignore();
			clearLine(MAXROWS + 1, menuLength);

			if (toupper(choice) == 'C') {
				cout << "Enter the filename (don't enter 'txt'): ";
				cin.getline(fileName, FILENAMESIZE - 15);

				snprintf(filePath, FILENAMESIZE, "SavedFiles\\%s.txt", fileName);
				fileResult = saveCanvas(current->item, filePath);

				if (!fileResult) {
					cerr << "ERROR: file cannot be written" << endl;
					system("pause");
					break;
				}
				else {
					cout << "Canvas saved! ";
					system("pause");
				}
			}

			if (toupper(choice) == 'A') {
				cout << "Enter the filename (don't enter 'txt'): ";
				cin.getline(fileName, FILENAMESIZE - 15);

				snprintf(filePath, FILENAMESIZE, "SavedFiles\\%s", fileName);
				fileResult = saveClips(clipList, filePath);

				if (!fileResult) {
					cerr << "ERROR: file cannot be written" << endl;
					system("pause");
					break;
				}
				else {
					cout << "Clips saved! ";
					system("pause");
				}
			}
			break;
		}

		// Clears the lines under the canvas to clear the menu and other text
		clearLine(MAXROWS + 1, menuLength);
		clearLine(MAXROWS + 2, menuLength);
		clearLine(MAXROWS + 3, menuLength);

	// If Q is selected, quit the program
	} while (choice != 'Q');
	
	// Clears the linked lists
	delete current;
	deleteList(undoList);
	deleteList(redoList);
	deleteList(clipList);


	return 0;
}


BOOL WINAPI Handler(DWORD cntrlEvent)
{
	cout << "Close event received\n";
	return FALSE;
}


void gotoxy(short row, short col)
{
	COORD pos = { col, row };
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);
}


void clearLine(int lineNum, int numOfChars)
{
	gotoxy(lineNum, 0);

	for (int x = 0; x < numOfChars; x++)
		cout << " ";

	gotoxy(lineNum, 0);
}

void replace(char canvas[][MAXCOLS], char oldCh, char newCh)
{
	int maxChrs = MAXROWS * MAXCOLS;

	for (int i = 0; i < maxChrs; i++) {
		if (canvas[0][i] == oldCh) {
			canvas[0][i] = newCh;
		}
	}
}

void editCanvas(char canvas[][MAXCOLS])
{
	char input = ' ';
	int row = 0, col = 0;

	gotoxy(row, col);
	input = _getch();

	while (input != ESC) {

		// Checks if it is equivalent to one of four arrow keys.
		if (input == SPECIAL) {
			input = _getch();
			if (input == LEFTARROW && col - 1 >= 0) {
				col--;
			}
			else if (input == RIGHTARROW && col + 1 < MAXCOLS) {
				col++;
			}
			else if (input == UPARROW && row - 1 >= 0) {
				row--;
			}
			else if (input == DOWNARROW && row + 1 < MAXROWS) {
				row++;
			}
		}

		// If user inputs F1 - F12 keys.
		else if (input == '\0') {
			input = _getch();
		}

		// Checks if the input is a printable ASCII character.
		else if (input >= 32 && input <= 126) {
			canvas[row][col] = input;
			cout << canvas[row][col];
		}

		// Moves cursor to the current row/column and gets another input from the user.
		gotoxy(row, col);
		input = _getch();
	}
}

void moveCanvas(char canvas[][MAXCOLS], int rowValue, int colValue)
{
	char newArray[MAXROWS][MAXCOLS];

	initCanvas(newArray);
	
	for (int i = 0; i < MAXROWS; ++i) {
		for (int j = 0; j < MAXCOLS; ++j) {
			int newRow = i + rowValue;
			int newCol = j + colValue;
			
			if (newRow < MAXROWS && newCol < MAXCOLS && newRow >= 0 && newCol >= 0) {
				newArray[newRow][newCol] = canvas[i][j];
			}
		}
	}
	
	copyCanvas(canvas, newArray);
}

void initCanvas(char canvas[][MAXCOLS])
{
	int maxChrs = MAXROWS * MAXCOLS;

	for (int x = 0; x < maxChrs; x++) {
		canvas[0][x] = ' ';
	}
}

void displayCanvas(char canvas[][MAXCOLS])
{
	char buffer[MAXROWS + 1][MAXCOLS + 2];

	for (int i = 0; i < MAXROWS; ++i) {
		memcpy(buffer[i], canvas[i], MAXCOLS);
		memcpy(&buffer[i][MAXCOLS], "|\n", 2);
	}

	for (int j = 0; j < MAXCOLS + 1; ++j) {
		buffer[MAXROWS][j] = '-';
	}

	buffer[MAXROWS][MAXCOLS + 1] = '\n';

	gotoxy(0, 0);
	cout.write(&buffer[0][0], (MAXROWS + 1) * (MAXCOLS + 2));
}

void copyCanvas(char to[][MAXCOLS], char from[][MAXCOLS])
{
	for (int i = 0; i < MAXROWS; i++) {
		for (int j = 0; j < MAXCOLS; j++) {
			to[i][j] = from[i][j];
		}
	}
}

bool saveCanvas(char canvas[][MAXCOLS], char filename[])
{
	ofstream outFile;
	outFile.open(filename);

	if (outFile.fail()) {

		return false;
	}

	for (int x = 0; x < MAXROWS; x++) {
		for (int y = 0; y < MAXCOLS; y++) {
			outFile << canvas[x][y];
		}
		outFile << '\n';
	}

	
	outFile.close();
	return true;
}

bool loadCanvas(char canvas[][MAXCOLS], char filename[])
{
	ifstream inFile;

	inFile.open(filename);
	if (!inFile) {
		return false;
	}

	initCanvas(canvas);

	char c;
	c = inFile.get();
	int colCounter = 0;
	int rowCounter = 0;

	while (!inFile.eof() && rowCounter < MAXROWS) {

		// If a \n is read, move to a new row and restart the column 
		if (c == '\n') {

			colCounter = 0;
			rowCounter++;
		}

		// If there are more columns than max, restart column counter and increase row
		else if (colCounter == MAXCOLS) {
			colCounter = 0;
			rowCounter++;

			// Will continue to read until \n is found to indicate new row
			while (c != '\n' && !inFile.eof()) {
				c = inFile.get();
			}
		}
		
		// Copys c into the canvas and increases the column counter
		else {
			canvas[rowCounter][colCounter] = c;
			colCounter++;
		}

		c = inFile.get();

	}
	inFile.close();

	return true;
}
