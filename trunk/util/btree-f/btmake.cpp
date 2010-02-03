/* Filename:  btmake.cpp

 Programmer:  Br. David Carlson

 Date:  November 2, 1997

 Modified:  May 24, 1999 and June 27, 2000

 This is a test program to create a table (B-tree-based) in a file.  It
 reads dictionary data from the source text file btree.txt.  This file
 contains on each line a word (in upper case, up to 12 characters) and
 starting in column 13 the definition (up to 36 characters).  The
 B-tree-based table will be stored in the file btree.dat.

 To compile this program, you will need to have the following files
 listed in the project:
 btmake.cpp   btree.cpp   itemtype.h   btree.h     table.h

 Use the associated btread program to look up data stored in this table.


#include "btree.h"
#include <stdlib.h>

const int LineMax = KeyFieldMax + DFMaxPlus1;

typedef char StringType[LineMax];
*/
/* Given:  InStream     An input file stream open for reading.
 StringMax    The maximum number of characters that can be put
 into String, including the NULL that marks the end
 of the string.
 Task:   To read up to StringMax - 1 characters from InStream, storing
 them in String, but stopping if a newline is read or end of
 file or an error condition is reached.
 Return: String       The string just read, with a NULL appended to mark
 the end of the string.  If a newline was read, it
 is not stored in String.
 The number of characters read into String (not counting the NULL)
 is returned in the function name.

int MyGetLine(istream & InStream, char * String, int StringMax) {
	char Ch;
	int Count, Last;

	Count = 0;
	Last = StringMax - 1;
	Ch = InStream.get();

	while ((Ch != '\n') && (!InStream.fail())) {
		if (Count < Last)
			String[Count++] = Ch;
		Ch = InStream.get();
	}

	String[Count] = NULL;
	return Count;
}
 */
/* Given:  InputFile   A file stream already opened for input on a text file.
 Task:   To read in a Word and its Definition from one line of this file.
 Return: Word        In char array form, the word read in.
 Definition  In char array form, the definition read in.

void ReadLine(fstream & InputFile, KeyFieldType Word, DataFieldType Definition) {
	char Line[LineMax];
	int k, ch;

	MyGetLine(InputFile, Line, LineMax); // will read the newline char

	for (k = 0; k < KeyFieldMax; k++)
		Word[k] = Line[k];
	Word[KeyFieldMax] = NULL;

	for (k = 0; k < DataFieldMax; k++) {
		ch = Line[KeyFieldMax + k];
		if (ch == '\n')
			break;
		Definition[k] = ch;
	}
	Definition[k] = NULL;
}
*/
/* Given:  InputFile   A file stream already opened for input.
 Task:   To read the data from InputFile and load it into the Table.
 Return: Table       The B-tree table containing the data.
 *//*
void Load(fstream & InputFile, BTTableClass & Table) {
	ItemType Item;

	InputFile.read((char*) (&Item.KeyField), sizeof(long long));
	InputFile.read((char*) &Item.DataField, sizeof(int));
	Table.Insert(Item);

#ifdef DEBUG
	Table.Check();
#endif

}

int main(void) {
	fstream Source;

	BTTableClass BTTable('w', "btree.dat");

	//b
	ItemType Item;
	long long k = 1234567L;
	int v = 1;
	Item.KeyField = k;
	Item.DataField = v;
	BTTable.Insert(Item);
	BTTable.Insert(Item);
	//e



	return 0;
}
*/
