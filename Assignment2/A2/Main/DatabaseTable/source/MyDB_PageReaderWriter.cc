
#ifndef PAGE_RW_C
#define PAGE_RW_C

#include "MyDB_PageReaderWriter.h"
#include "MyDB_pageRecordIterator.h"


MyDB_PageReaderWriter :: MyDB_PageReaderWriter( MyDB_PageHandle in_pageHandle, int in_pageSize)
{
	pageSize = in_pageSize;
	pageHand = in_pageHandle;
	pageInfo Info;
	Info.currPosition=sizeof(pageInfo);
	char *bytes = (char*)pageHand->getBytes();
	memcpy(bytes, &Info, sizeof(pageInfo));
}



MyDB_PageReaderWriter :: MyDB_PageReaderWriter( MyDB_PageHandle in_pageHandle, int in_pageSize, int i)
{
	pageSize = in_pageSize;
	pageHand = in_pageHandle;
}

void MyDB_PageReaderWriter :: clear () 
{
	cout<<"clean up page"<<endl;
	pageInfo Info;
	Info.currPosition=sizeof(pageInfo);
	char *bytes = (char*)pageHand->getBytes();
	pageInfo* info2 = (pageInfo*)bytes;
	memcpy(bytes, &Info, sizeof(pageInfo));
	memset(bytes, '\0', info2->currPosition-sizeof(pageInfo));
	pageHand->wroteBytes();
	return ;
}

MyDB_PageType MyDB_PageReaderWriter :: getType () 
{
	return MyDB_PageType :: RegularPage;
}

MyDB_RecordIteratorPtr MyDB_PageReaderWriter :: getIterator (MyDB_RecordPtr iterateIntoMe) 
{

	MyDB_pageRecordIteratorPtr page_iteratorPtr = make_shared <MyDB_pageRecordIterator> (iterateIntoMe, pageHand, pageSize);
	return (MyDB_RecordIteratorPtr)page_iteratorPtr;
}

void MyDB_PageReaderWriter :: setType (MyDB_PageType toMe) 
{

}

bool MyDB_PageReaderWriter :: append (MyDB_RecordPtr appendMe) 
{
	char* bytes = (char*)pageHand->getBytes();
	pageInfo *info = (pageInfo *) (bytes);
	if (appendMe->getBinarySize() + info->currPosition > pageSize)
	{
		return false;
	}
	else
	{
		appendMe->toBinary(bytes + info->currPosition);
		info->currPosition = info->currPosition + appendMe->getBinarySize();
		memcpy(bytes, info, sizeof(pageInfo));
		pageHand->wroteBytes();
		return true;
	}
}

MyDB_PageReaderWriter::~MyDB_PageReaderWriter()
{
	pageHand = nullptr;
}

#endif
