/* Filename:  table.h

   Programmer:  Br. David Carlson

   Date:  November 10, 1997

   Modified:  May 23, 1999 and June 27, 2000

   This header file sets up TableBaseClass as an abstract base class for
   tables that are stored in a file.
*/

#ifndef _TABLE_H_
#define _TABLE_H_
#include "itemtype.h"


class TableBaseClass
   {
   public:
      virtual bool Empty(void) = 0;
      virtual bool Insert(const ItemType & Item) = 0;
      virtual bool Retrieve(long long SearchKey, ItemType & Item) = 0;
   protected:
      fstream DataFile;   // the file stream for the table data
      long NumItems;      // number of records in the table
      char OpenMode;      // r or w (read or write) mode for the table
   };

#endif
