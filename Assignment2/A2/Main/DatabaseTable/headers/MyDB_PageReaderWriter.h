
#ifndef PAGE_RW_H
#define PAGE_RW_H

#include <string.h>
#include "MyDB_PageType.h"
//#include "MyDB_TableReaderWriter.h"
#include "MyDB_pageRecordIterator.h"
#include "page_info.h"

class MyDB_PageReaderWriter;
typedef shared_ptr <MyDB_PageReaderWriter> MyDB_PageReaderWriterPtr;

class MyDB_PageReaderWriter {

public:

	// ANY OTHER METHODS YOU WANT HERE
	MyDB_PageReaderWriter( MyDB_PageHandle in_pageHandle, int in_pageSize);
	MyDB_PageReaderWriter( MyDB_PageHandle in_pageHandle, int in_pageSize, int i);
	~MyDB_PageReaderWriter();
	// empties out the contents of this page, so that it has no records in it
	// the type of the page is set to MyDB_PageType :: RegularPage
	void clear ();	

	// return an itrator over this page... each time returnVal->next () is
	// called, the resulting record will be placed into the record pointed to
	// by iterateIntoMe
	MyDB_RecordIteratorPtr getIterator (MyDB_RecordPtr iterateIntoMe);

	// appends a record to this page... return false is the append fails because
	// there is not enough space on the page; otherwise, return true
	bool append (MyDB_RecordPtr appendMe);

	// gets the type of this page... this is just a value from an ennumeration
	// that is stored within the page
	MyDB_PageType getType ();

	// sets the type of the page
	void setType (MyDB_PageType toMe);
	
private:

	// ANYTHING ELSE YOU WANT HERE
	int pageSize;
	MyDB_PageHandle pageHand;

};

#endif
