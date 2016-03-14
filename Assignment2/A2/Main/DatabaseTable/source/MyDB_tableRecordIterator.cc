#include "MyDB_tableRecordIterator.h"

MyDB_tableRecordIterator :: MyDB_tableRecordIterator(MyDB_BufferManagerPtr in_bufManager, MyDB_TablePtr in_table,  MyDB_RecordPtr in_recordPtr,  int in_numPage, int in_pageSize)
{
	table_recordPtr = in_recordPtr;
    local_table = in_table;
    local_bufferMgr = in_bufManager;
	MyDB_PageHandle temp_handle = local_bufferMgr->getPage(local_table, 0);
    local_curPageRecordIterator = make_shared<MyDB_pageRecordIterator>(table_recordPtr, temp_handle, pageSize);
	currPage = 0;
	totalPage = in_numPage;
	pageSize = in_pageSize;
}

void MyDB_tableRecordIterator :: getNext ()
{
	if (local_curPageRecordIterator->hasNext() == false) 
	{
        if (currPage == local_table->lastPage())
		{
			return;
		}
		else
		{
			currPage++;
            local_curPageRecordIterator = make_shared<MyDB_pageRecordIterator>(table_recordPtr, local_bufferMgr->getPage(local_table, currPage), pageSize);
		}
	}
    if (local_curPageRecordIterator->hasNext())
	{
        local_curPageRecordIterator->getNext();
	}
}
bool MyDB_tableRecordIterator :: hasNext ()
{
	if (local_curPageRecordIterator->hasNext() == false) 
	{
        if (currPage == local_table->lastPage())
		{
            return false;
		}
		else
		{
			currPage++;
            local_curPageRecordIterator = make_shared<MyDB_pageRecordIterator>(table_recordPtr, local_bufferMgr->getPage(local_table, currPage), pageSize);
		}     
    }
    return true;
}

MyDB_tableRecordIterator::~MyDB_tableRecordIterator()
{
    table_recordPtr = nullptr;
    local_table = nullptr;
    local_bufferMgr = nullptr;
    local_curPageRecordIterator = nullptr;
}
