/* Filename:  btread.cpp

   Programmer:  Br. David Carlson

   Date:  November 4, 1997

   Modified:  October 22, 1999 and June 27, 2000

   This is a program to read a B-tree-based table (in btree.dat file) - as
   produced by the btmake program.  This program prompts the user to
   enter a word and then displays the associated definition, if a
   match was found.  The user can do repeated lookups of this sort, until
   a . is entered to quit.

   In order to compile this program you will need to have the following
   files listed in the project:
   btree.cpp   itemtype.h   btread.cpp   btree.h     table.h


#include "btree.h"
*/

/* Given:  Nothing.
   Task:   To read in one word from the keyboard (or . to signal desire
           to quit).
   Return: Word   In char array form, the word read in, capitalized and
                  padded on the right with blanks so that it contains 12
                  characters.
           In the function name, return true if a word was read in, false
           if a . was read in.


bool ReadKeyboard(KeyFieldType Word)
   {
   int k, ch;
   bool start;

   cin >> Word;
   cin.get();   // get the newline

   if (Word[0] == '.')
      return false;

   start = true;

   for (k = 0; k < KeyFieldMax; k++)
      {
      ch = Word[k];
      if (ch == NULL)
         start = false;
      if (start)
         Word[k] = toupper(ch);   // capitalize
      else
         Word[k] = ' ';   // pad with blanks
      }

   Word[KeyFieldMax] = NULL;
   return true;
   }
*/
/*
int main(void)
   {
   ItemType Item;
   //KeyFieldType SearchKey;
   BTTableClass BTTable('r', "btree.dat");

   if (BTTable.Empty())
      Error("Table is empty");

   //cout << "Enter the word to be looked up (or . to quit): ";


   if (BTTable.Retrieve(1234567L, Item))
            cout << " Definition:   " << Item.DataField << endl;
   else
            cout << " Not found" << endl;
   return 0;
   }
*/
