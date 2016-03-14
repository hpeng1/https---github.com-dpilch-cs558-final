#include "MyDB_pageRecordIterator.h"

MyDB_pageRecordIterator ::MyDB_pageRecordIterator (MyDB_RecordPtr in_recordPtr, MyDB_PageHandle in_pageHandle, int in_pageSize) 
{
	recordPtr = in_recordPtr;
	local_handle = in_pageHandle;
	pageSize = in_pageSize;
	local_currPosition = sizeof(pageInfo);
}

void MyDB_pageRecordIterator :: getNext ()
{
	char * bytes = (char*)local_handle->getBytes();
    recordPtr->fromBinary(bytes + local_currPosition);
    local_currPosition = local_currPosition +recordPtr->getBinarySize();
}
bool MyDB_pageRecordIterator :: hasNext ()
{
	char* bytes = (char*)local_handle->getBytes();
	pageInfo* info = (pageInfo*)(bytes); 

//	cout<<"currPosition: "<<info->currPosition<<endl;

    if (local_currPosition >= info->currPosition)
        return false;
    return true;
}

MyDB_pageRecordIterator :: ~ MyDB_pageRecordIterator(){
    recordPtr = nullptr;
    local_handle = nullptr;
}
