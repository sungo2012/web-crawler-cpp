/* Filename:  btree.h

   Programmer:  Br. David Carlson

   Date:  November 2, 1997

   Modified:  October 25, 1999

   This is the header file to go with btree.cpp, an implementation of
   a B-tree-based table class.
*/
#ifndef _BTREE_H_
#define _BTREE_H_
#include "table.h"

// comment off the following line if you want to omit debugging output:



const int MaxKeys = 1260;//108;   // max number of keys in a node

const int MaxKeysPlusOne = MaxKeys + 1;

const int MinKeys = 5;    // min number of keys in a node

const long NilPtr = -1L;   // the L indicates a long int


typedef struct
   {
   int Count;               // Number of keys stored in the current node
   ItemType Key[MaxKeys];   // Warning: indexing starts at 0, not 1
   long Branch[MaxKeysPlusOne];   // Fake pointers to child nodes
   } NodeType;


void Error(char * msg);


class BTTableClass: public TableBaseClass
   {
   public:
      BTTableClass(char Mode, const char * FileName);
      ~BTTableClass(void);
      bool Empty(void);
      bool Insert(const ItemType & Item);
      bool Retrieve(long long SearchKey, ItemType & Item);
      void Dump(void);   // for debugging only - could be removed
      //void Check(void);   // for debugging only
      void makeSure();
      void getPCurrentNode(NodeType&);//传引用
      void setCurrentNode(NodeType);
      void setRoot(long);
      void setNumNodes(long);
      void setNumItems(long);
   private:
      //void CheckSubtree(long Current, long long & Last); // for debugging
      bool SearchNode(const long long Target, int & location) const;
      void AddItem(const ItemType & NewItem, long NewRight,
         NodeType & Node, int Location);
      void Split(const ItemType & CurrentItem, long CurrentRight,
         long CurrentRoot, int Location, ItemType & NewItem,
         long & NewRight);
      bool PushDown(const ItemType & CurrentItem, long CurrentRoot,
         bool & MoveUp, ItemType & NewItem, long & NewRight);
      long Root;       // fake pointer to the root node
      long NumNodes;   // number of nodes in the B-tree
      int NodeSize;    // number of bytes per node
      NodeType CurrentNode;   // storage for current node being worked on
      int bezero;//为了让整个类的大小为20480字节
   };
#endif
