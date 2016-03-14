#ifndef PAGEITERATOR_H
#define PAGEITERATOR_H
#include "MyDB_PageHandle.h"
#include "MyDB_RecordIterator.h"
#include "MyDB_Record.h"
#include "page_info.h"

#include <memory>
using namespace std;

class MyDB_pageRecordIterator;
typedef shared_ptr <MyDB_pageRecordIterator> MyDB_pageRecordIteratorPtr;

class MyDB_pageRecordIterator : public MyDB_RecordIterator
{
public:
	void getNext ();
	bool hasNext ();
	MyDB_pageRecordIterator (MyDB_RecordPtr in_recordPtr, MyDB_PageHandle in_pageHandle, int in_pageSize);
	virtual ~MyDB_pageRecordIterator ();
private:
	int pageSize;
	int local_currPosition;
	MyDB_RecordPtr recordPtr; 
	MyDB_PageHandle local_handle;
};
#endif

