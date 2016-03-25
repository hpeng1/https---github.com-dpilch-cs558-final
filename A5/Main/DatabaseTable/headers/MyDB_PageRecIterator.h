

/****************************************************
** COPYRIGHT 2016, Chris Jermaine, Rice University **
**                                                 **
** The MyDB Database System, COMP 530              **
** Note that this file contains SOLUTION CODE for  **
** A2.  You should not be looking at this file     **
** unless you have completed A2!                   **
****************************************************/


#ifndef PAGE_REC_ITER_H
#define PAGE_REC_ITER_H

#include "MyDB_PageHandle.h"
#include "MyDB_Record.h"
#include "MyDB_RecordIterator.h"

class MyDB_PageRecIterator : public MyDB_RecordIterator {

public:

	// put the contents of the next record in the file/page into the iterator record
	// this should be called BEFORE the iterator record is first examined
	void getNext () override;

	// return true iff there is another record in the file/page
	bool hasNext () override;

	// destructor and contructor
	MyDB_PageRecIterator (MyDB_PageHandle myPageIn, MyDB_RecordPtr myRecIn); 
	~MyDB_PageRecIterator ();

private:

	int bytesConsumed;
	MyDB_PageHandle myPage;
	MyDB_RecordPtr myRec;
	
};

#endif
