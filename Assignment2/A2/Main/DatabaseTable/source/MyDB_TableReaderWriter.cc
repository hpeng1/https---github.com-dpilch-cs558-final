
#ifndef TABLE_RW_C
#define TABLE_RW_C

#include <fstream>
#include "MyDB_PageReaderWriter.h"
#include "MyDB_TableReaderWriter.h"

using namespace std;

MyDB_TableReaderWriter :: MyDB_TableReaderWriter (MyDB_TablePtr forMe, MyDB_BufferManagerPtr myBuffer) 
{
	local_TablePtr = forMe;
	local_BufferManagerPtr = myBuffer;
	pageSize = myBuffer->getPageSize();
	totalNumberOfPages = 0;
}

MyDB_PageReaderWriter &MyDB_TableReaderWriter :: operator [] (size_t i) {
	MyDB_PageHandle handle = local_BufferManagerPtr->getPage (local_TablePtr, i);
	cout<<"want to access "<<i<<" page"<<endl;
	local_pageReadWritePtr  = make_shared<MyDB_PageReaderWriter>(handle, local_BufferManagerPtr->getPageSize(), i);
	return *local_pageReadWritePtr;
}

MyDB_RecordPtr MyDB_TableReaderWriter :: getEmptyRecord () 
{
	MyDB_RecordPtr newRecordPtr = make_shared <MyDB_Record> (local_TablePtr->getSchema());
	return newRecordPtr;
}

MyDB_PageReaderWriter &MyDB_TableReaderWriter :: last () 
{

	return (*this)[totalNumberOfPages];
}


void MyDB_TableReaderWriter :: append (MyDB_RecordPtr appendMe) 
{
	MyDB_PageReaderWriter lastPage = last();
	if (lastPage.append(appendMe)==false)
	{
		totalNumberOfPages++;
		MyDB_PageHandle temp_handle = local_BufferManagerPtr->getPage(local_TablePtr, totalNumberOfPages);
		local_pageReadWritePtr = make_shared<MyDB_PageReaderWriter>(temp_handle,pageSize);
		local_pageReadWritePtr->append(appendMe);
	}
	local_TablePtr->setLastPage(totalNumberOfPages);
}

void MyDB_TableReaderWriter :: loadFromTextFile (string fromMe) 
{
/*
	char* str = new char[pageSize];
	ifstream openfile(fromMe.c_str());
	MyDB_RecordPtr recordPtr = this->getEmptyRecord();
	while(openfile.getline(str,pageSize))
	{
		recordPtr->fromString(str);
		this->append(recordPtr);
	}
*/
	char* str = new char[pageSize];
	ifstream openfile(fromMe.c_str());
	MyDB_RecordPtr recordPtr = this->getEmptyRecord();
	MyDB_PageHandle temp_handle = local_BufferManagerPtr->getPage(local_TablePtr, totalNumberOfPages);
	local_pageReadWritePtr = make_shared<MyDB_PageReaderWriter>(temp_handle, pageSize);
	while(openfile.getline(str,pageSize))
	{
		recordPtr->fromString(str);
		if(local_pageReadWritePtr->append(recordPtr)==false)
		{
			totalNumberOfPages++;
			temp_handle = local_BufferManagerPtr->getPage(local_TablePtr, totalNumberOfPages);
			local_pageReadWritePtr = make_shared<MyDB_PageReaderWriter>(temp_handle, pageSize);
			local_pageReadWritePtr->append(recordPtr);
		}
	}
	local_TablePtr->setLastPage(totalNumberOfPages);
}

MyDB_RecordIteratorPtr MyDB_TableReaderWriter :: getIterator (MyDB_RecordPtr iterateIntoMe) 
{
	MyDB_tableRecordIteratorPtr table_iteratorPtr = make_shared <MyDB_tableRecordIterator> (local_BufferManagerPtr, local_TablePtr , iterateIntoMe,  totalNumberOfPages , pageSize);
	return (MyDB_RecordIteratorPtr)table_iteratorPtr;
}

void MyDB_TableReaderWriter :: writeIntoTextFile (string toMe) 
{
	MyDB_RecordPtr recordPtr = this->getEmptyRecord();
	MyDB_RecordIteratorPtr iterator = getIterator(recordPtr);
	char* str = new char[pageSize];
	FILE* op=fopen(toMe.c_str(),"w");
	while (iterator->hasNext()==true)
	{
		iterator->getNext();
		recordPtr->toBinary(str);
		fprintf(op,str,recordPtr->getBinarySize());
		fprintf(op,"\n");
	}
	fclose(op);
}

MyDB_TableReaderWriter :: ~MyDB_TableReaderWriter()
{
	local_TablePtr = nullptr;
	local_BufferManagerPtr = nullptr;
	local_RecordPtr = nullptr;
	local_pageReadWritePtr = nullptr;
}


#endif

