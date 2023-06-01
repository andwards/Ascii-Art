/**
* This program is an add on to the Original Text Art program. This adds new menu options by using recursion.
*/
#include <iostream>
#include <windows.h>
#include <conio.h>
#include "Definitions.h"
using namespace std;

// This structure is used to hold a row/column combination in a single struct in our program.
Point::Point(DrawPoint p)
{
	row = (int)round(p.row);
	col = (int)round(p.col);
}

// https://math.stackexchange.com/questions/39390/determining-end-coordinates-of-line-with-the-specified-length-and-angle
DrawPoint findEndPoint(DrawPoint start, int len, int angle)
{
	DrawPoint end;
	end.col = start.col + len * cos(degree2radian(angle));
	end.row = start.row + len * sin(degree2radian(angle));
	return end;
}


// Use this to draw characters into the canvas, with the option of performing animation
void drawHelper(char canvas[][MAXCOLS], Point p, char ch, bool animate)
{
	// Pause time between steps (in milliseconds)
	const int TIME = 50;

	// Make sure point is within bounds
	if (p.row >= 0 && p.row < MAXROWS && p.col >= 0 && p.col < MAXCOLS)
	{
		// Draw character into the canvas
		canvas[p.row][p.col] = ch;

		// If animation is enabled, draw to screen at same time
		if (animate)
		{
			gotoxy(p.row, p.col);
			printf("%c", ch);
			Sleep(TIME);
		}
	}
}


// Fills gaps in a row caused by mismatch between match calculations and screen coordinates
// (i.e. the resolution of our 'canvas' isn't very good)
void drawLineFillRow(char canvas[][MAXCOLS], int col, int startRow, int endRow, char ch, bool animate)
{
	// determine if we're counting up or down
	if (startRow <= endRow)
		for (int r = startRow; r <= endRow; r++)
		{
			Point point(r, col);
			drawHelper(canvas, point, ch, animate);
		}
	else
		for (int r = startRow; r >= endRow; r--)
		{
			Point point(r, col);
			drawHelper(canvas, point, ch, animate);
		}
}


// Draw a single line from start point to end point
void drawLine(char canvas[][MAXCOLS], DrawPoint start, DrawPoint end, bool animate)
{
	char ch;

	Point scrStart(start);
	Point scrEnd(end);

	// vertical line
	if (scrStart.col == scrEnd.col)
	{
		ch = '|';

		drawLineFillRow(canvas, scrStart.col, scrStart.row, scrEnd.row, ch, animate);
	}
	// non-vertical line
	else
	{
		int row = -1, prevRow;

		// determine the slope of the line
		double slope = (start.row - end.row) / (start.col - end.col);

		// choose appropriate characters based on 'steepness' and direction of slope
		if (slope > 1.8)  ch = '|';
		else if (slope > 0.08)  ch = '`';
		else if (slope > -0.08)  ch = '-';
		else if (slope > -1.8) ch = '\'';
		else ch = '|'; //bar

		// determine if columns are counting up or down
		if (scrStart.col <= scrEnd.col)
		{
			// for each column from start to end, calculate row values
			for (int col = scrStart.col; col <= scrEnd.col; col++)
			{
				prevRow = row;
				row = (int)round(slope * (col - start.col) + start.row);

				// draw from previous row to current row (to fill in row gaps)
				if (prevRow > -1)
				{
					drawLineFillRow(canvas, col, prevRow, row, ch, animate);
				}
			}
		}
		else
		{
			// for each column from start to end, calculate row values
			for (int col = scrStart.col; col >= scrEnd.col; col--)
			{
				prevRow = row;
				row = (int)round(slope * (col - start.col) + start.row);

				// draw from previous row to current row (to fill in row gaps)
				if (prevRow > -1)
				{
					drawLineFillRow(canvas, col, prevRow, row, ch, animate);
				}
			}
		}
	}
}


// Draws a single box around a center point 
void drawBox(char canvas[][MAXCOLS], Point center, int height, bool animate)
{
	int sizeHalf = height / 2;
	int ratio = (int)round(MAXCOLS / (double)MAXROWS * sizeHalf);

	// Calculate where the four corners of the box should be
	DrawPoint points[4];
	points[0] = DrawPoint(center.row - sizeHalf, center.col - ratio);
	points[1] = DrawPoint(center.row - sizeHalf, center.col + ratio);
	points[2] = DrawPoint(center.row + sizeHalf, center.col + ratio);
	points[3] = DrawPoint(center.row + sizeHalf, center.col - ratio);

	// Draw the four lines of the box
	for (int x = 0; x < 3; x++)
	{
		drawLine(canvas, points[x], points[x + 1], animate);
	}
	drawLine(canvas, points[3], points[0], animate);

	// Replace the corners with a better looking character
	for (int x = 0; x < 4; x++)
	{
		drawHelper(canvas, points[x], '+', animate);
	}
}


// Menu for the drawing tools
// parameter are used to manipulate a canvas passed by the main menu and to slow down the drawing tools. 
void menuTwo(Node*& current, List& undoList, List& redoList, List& clips, bool& animate)
{
	//menutwo's option for drawing on canvas
	char menuTwo[] = "<F>ill / <L>ine / <B>ox / <N>ested Boxes / <T>ree / <M>ain Menu: ";
	int menuLength = (int)strlen(menuTwo) + 20;
	char input[BUFFERSIZE], choice = {};

	//constructers and variables to be used in the function call within the menu options.
	int height, branchAngle;
	Point start;
	Point end;
	Point center(MAXROWS / 2, MAXCOLS / 2);



	do
	{
		// Display content into canvas array
		//current->item is the canvas. 
		displayCanvas(current->item);


		//new menu items to accomodate for extra features. 
		cout << "<A>nimate: " << (animate ? 'Y' : 'N');
		cout << " / <U>ndo: " << undoList.count;
		if (redoList.count > 0) {
			cout << " / Red<O>: " << redoList.count;
		}
		cout << " / Cl<I>p: " << clips.count;
		if (clips.count >=  2) {
			cout << " / <P>lay";
		}

		// Menu to redisplay everytime an option is chosen
		printf("\n%s", menuTwo);
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
			addNode(clips, newCanvas(current));
			break;

		// Calls the play function to animate a list of clips in clipList
		case 'P':
			play(clips);
			break;

		case 'F':
			// Gets a character to fill in and the location it wants to start filling.
			cout << "Enter character to fill with from current location / <ESC> to cancel" << endl;
			choice = getPoint(start);
			if (choice == ESC) {
				break;
			}
			addUndoState(undoList, redoList, current);

			// Calls on fillRecursive to fill in the area selected
			fillRecursive(current->item, start.row, start.col, current->item[start.row][start.col], choice, animate);
			break;

		case 'L':

			// Gets initial point for a line to be drawn.
			cout << "Type any letter to choose start point / <ESC> to cancel" << endl;
			choice = getPoint(start);
			if (choice == ESC) {
				break;
			}

			// Outputs the letter typed in the correct location on the canvas
			cout << choice;
			clearLine(MAXROWS + 1, menuLength);

			// Gets the end point for the line to be drawn.
			cout << "Type any letter to choose end point / <ESC> to cancel" << endl;
			choice = getPoint(end);
			if (choice == ESC) {
				break;
			}
			addUndoState(undoList, redoList, current);

			// Calls on drawLine to display the line using the start and end points given.
			drawLine(current->item, start, end, animate);
			break;

		case 'B':
			// Gets the size of a box and ignores the extra enter space
			cout << "Enter Size: ";
			cin >> height;
			cin.ignore();
			clearLine(MAXROWS + 1, menuLength);

			// Gets the location where the center of the box is.
			cout << "Type any letter to choose box center, or <C> for screen center / <ESC> to cancel" << endl;
			choice = getPoint(start);
			if (choice == ESC) {
				break;
			}

			// If C is entered, the center of the canvas is used.
			if (toupper(choice) == 'C') {
				start = center;
			}
			addUndoState(undoList, redoList, current);

			// Calls drawBox function to display the box
			drawBox(current->item, start, height, animate);
			break;

		case 'N':

			cout << "Enter Size: ";
			cin >> height;
			cin.ignore();
			clearLine(MAXROWS + 1, menuLength);


			cout << "Type any letter to choose box center, or <C> for screen center / <ESC> to cancel" << endl;

			// Sets the start parameter of the getPoint function equal to the user's chosen position.
			choice = getPoint(start);

			// If user enters escape key, breaks out of the switch statement.
			if (choice == ESC) {
				break;
			}

			// If user inputs C, the center of the Canvas is used.
			if (toupper(choice) == 'C') {
				start = center;
			}
			addUndoState(undoList, redoList, current);


			drawBoxesRecursive(current->item, start, height, animate);
			break;

		case 'T':
			cout << "Enter approximate tree height: ";
			cin >> height;

			// Clearline function keeps all question prompts on the same line right below the Canvas.
			clearLine(MAXROWS + 1, menuLength);

			// Prompts user to enter desired branch angle.
			cout << "Enter branch angle: ";
			cin >> branchAngle;
			clearLine(MAXROWS + 1, menuLength);
			cin.ignore();


			cout << "Type any letter to choose start point, or <C> for bottom center / <ESC> to cancel" << endl;
			choice = getPoint(start);

			// If user enters escape key, breaks out of the switch statement.
			if (choice == ESC) {
				break;
			}

			// If user inputs C, the center of the Canvas is used.
			if (toupper(choice) == 'C') {
				start = center;
				start.row = MAXROWS - 1;
			}
			addUndoState(undoList, redoList, current);

			//calls on tree recursive function.
			treeRecursive(current->item, start, height, 270, branchAngle, animate);
			break;
		}

		// The clearLine statements clears all leftover lines underneath the Canvas.
		clearLine(MAXROWS + 1, menuLength);
		clearLine(MAXROWS + 2, menuLength);
		clearLine(MAXROWS + 3, menuLength);
	} while (choice != 'M');

}




// Get a single point from screen, with character entered at that point
char getPoint(Point& pt)
{
	char input = ' ';
	int row = 0, col = 0;

	// Move cursor to 0,0 and then get a single character from the keyboard
	gotoxy(row, col);
	input = _getch();

	// checks user input. If input does not equal ESC key, checks various conditionals.
	while (input != ESC) {

		// If the keyboard input equals a ASCII SPECIAL input, the program asks for a second input, 
		// Checks if it is equivalent to one of four arrow keys and increments accordingly.
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

		// Checks if user entered a printable ASCII character.
		else if (input >= 32 && input <= 126) {
			pt.row = row;
			pt.col = col;

			return input;
		}

		// Moves cursor to the current row/column and gets another input from the user.
		gotoxy(row, col);
		input = _getch();
	}

	return input;
}


// Recursively fill a section of the screen
void fillRecursive(char canvas[][MAXCOLS], int row, int col, char oldCh, char newCh, bool animate)
{
	// Base cases that checks whether the canvas has reached the border of the canvas or a spot in the canvas is not equal to the old character.
	if (row < 0 || row >= MAXROWS || col < 0 || col >= MAXCOLS) {
		return;
	}
	if (canvas[row][col] != oldCh) {
		return;
	}
	if (oldCh == newCh) {
		return;
	}

	// Helps with the animation of the function. 
	drawHelper(canvas, Point(row, col), newCh, animate);

	// Recursively calls until base cases are met going north, east, south, and west.
	fillRecursive(canvas, row + 1, col, oldCh, newCh, animate);
	fillRecursive(canvas, row - 1, col, oldCh, newCh, animate);
	fillRecursive(canvas, row, col + 1, oldCh, newCh, animate);
	fillRecursive(canvas, row, col - 1, oldCh, newCh, animate);
}


// Recursively draws a tree
void treeRecursive(char canvas[][MAXCOLS], DrawPoint start, int height, int startAngle, int branchAngle, bool animate)
{
	// Base cases that checks to see whether the tree has reached the border of the canvas or tree height is less than 3. 
	if (start.row >= MAXROWS || start.col >= MAXCOLS || start.row < 0 || start.col < 0) {
		return;
	}
	if (height < 3) {
		return;
	}

	// Finds the next endpoint for part of the tree
	DrawPoint end = findEndPoint(start, height / 3, startAngle);

	// Draws the next portion of the tree
	drawLine(canvas, start, end, animate);

	// Recursive calls that will keep stacking with a new height and starting angle until conditions for base cases are met. 
	treeRecursive(canvas, end, height - 2, startAngle + branchAngle, branchAngle, animate);
	treeRecursive(canvas, end, height - 2, startAngle - branchAngle, branchAngle, animate);
}


// Recursively draw nested boxes
void drawBoxesRecursive(char canvas[][MAXCOLS], Point center, int height, bool animate)
{
	// Base case that checks whether the box height (allocated rows) are less than 2. 
	if (height < 2) {
		return;
	}

	// Draws a box with a point and height into the canvas
	drawBox(canvas, center, height, animate);

	// Recursive call with a height -2, to make the next box smaller, until base case is met.
	drawBoxesRecursive(canvas, center, height - 2, animate);
}