#ifndef TABLEITERATOR_H
#define TABLEITERATOR_H
#include "MyDB_RecordIterator.h"
#include "MyDB_Record.h"
#include "MyDB_pageRecordIterator.h"
#include "MyDB_BufferManager.h"


#include <memory>
using namespace std;

class MyDB_tableRecordIterator;
typedef shared_ptr <MyDB_tableRecordIterator> MyDB_tableRecordIteratorPtr;

class MyDB_tableRecordIterator : public MyDB_RecordIterator
{
public:
	void getNext ();
	bool hasNext ();
	MyDB_tableRecordIterator ();
	MyDB_tableRecordIterator(MyDB_BufferManagerPtr in_bufManager, MyDB_TablePtr in_table,  MyDB_RecordPtr in_recordPtr,  int in_numPage, int in_pageSize);
    ~MyDB_tableRecordIterator ();

private:
	MyDB_RecordPtr table_recordPtr;
	MyDB_TablePtr  local_table;
	MyDB_BufferManagerPtr local_bufferMgr;
	MyDB_pageRecordIteratorPtr local_curPageRecordIterator;
	int currPage;
	int totalPage;
	int pageSize;              
};
#endif