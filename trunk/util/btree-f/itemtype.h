/* Filename:  itemtype.h

   Programmer:  Br. David Carlson

   Date:  May 23, 1999 and June 27, 2000

   This header file sets up ItemType and associated items.
*/
#ifndef _ITEMTYPE_H_
#define _ITEMTYPE_H_
#include <iostream>
#include <fstream>
using namespace std;


const int KeyFieldMax = 12;

const int KFMaxPlus1 = KeyFieldMax + 1;

const int DataFieldMax = 36;

const int DFMaxPlus1 = DataFieldMax + 1;


typedef char KeyFieldType[KFMaxPlus1];

typedef char DataFieldType[DFMaxPlus1];

typedef struct
   {
   long long KeyField;
   int DataField;
   } ItemType;
#endif
