
#ifndef PAGE_HANDLE_C
#define PAGE_HANDLE_C


#include <memory>
#include <stdlib.h>
#include <string>
#include <iostream>
#include <unistd.h>
#include <stdio.h>
#include <fstream>
#include <stdlib.h>
#include <vector>
#include "MyDB_PageHandle.h"

 	   #include <sys/types.h>
       #include <sys/stat.h>
       #include <fcntl.h>
using namespace std;


void * MyDB_PageHandleBase :: getBytes () {
	// if the page is still in the pool, just return the content of the page
	if(page_ptr->is_in_buffer() == 1)
	{
		cout<<"page is still in the pool, so get the page content"<<endl;
		return page_ptr->getPageContent();
	}	
	else
	{
		cout<<"page is not in the pool, will load the content from the file"<<endl;

		FILE* fp = fopen(page_ptr->show_location().c_str(),"rb+"); 
		int position = page_ptr->getPageId();
		int page_size = page_ptr->getPageSize();

		fseek(fp, position*page_size, SEEK_SET);  
        fread (page_ptr->getPageContent(),1,page_size,fp);

		cout<< page_ptr->getPageContent()<<endl;
		return page_ptr->getPageContent();
	}
}

void MyDB_PageHandleBase :: wroteBytes () {
    // if the page is still in the pool, just set the page dirty
    if(page_ptr->is_in_buffer() == 1)
	{
		cout<<"page is still in the pool, so mark it dirty"<<endl;
		page_ptr->set_dirty();
	}	
	else
	{
		cout<<"page is not in the pool, will write those content into the file"<<endl;
		FILE *fp = fopen(page_ptr->show_location().c_str(), "rb+");
		int position = page_ptr->getPageId();
		int page_size = page_ptr->getPageSize();

    	fseek(fp, position*page_size,SEEK_SET);
    	fwrite(page_ptr->getPageContent(), sizeof(char), page_size, fp);
　　 	fclose(fp);
	}
    return;
}

MyDB_PageHandleBase :: ~MyDB_PageHandleBase () {
	cout<<"Panic! handle to page "<<page_ptr->getPageId()<<" is lost"<<endl;

	// when an handle is lost, decrease the count of the page by 1
	page_ptr->set_count(page_ptr->see_count() - 1);

	// if the count of a page is decreased to 0, should unpin
	if(page_ptr->see_count()<=0)
	{
		cout<<"page count is 0, should unpin"<<endl;
		page_ptr->set_unpin();

		// if the page is anouymous, should also free the page in the pool, no write back
		if(page_ptr->see_anonymous()== 1)
		{
			cout<<"anonymous page will be cleaned from the pool"<<endl;
			//cout<<"testing forwarding, page size: "<<manager->page_size<<endl;
			manager->clean_up(page_ptr);
			cout<<"also recycle the bytes in the tempfile"<<endl;

		}
	}

}

MyDB_PageHandleBase :: MyDB_PageHandleBase (MyDB_pagePtr pagePtr, MyDB_BufferManager* mgr) {
	page_ptr=pagePtr;
	manager = mgr;
	cout<<"create handle with manager reference"<<endl;
	//manager = pool_manager;
} 

MyDB_PageHandleBase :: MyDB_PageHandleBase (MyDB_pagePtr pagePtr) {
	page_ptr=pagePtr;
	cout<<"create handle without manager reference"<<endl;
	//manager = pool_manager;
}

void MyDB_PageHandleBase :: show_content(){
	page_ptr->show_content();
}

void MyDB_PageHandleBase :: set_unpin(){
	page_ptr->set_unpin();
}
/*
int MyDB_PageHandleBase :: see_manager_pool_size(){
	manager->see_pool_size();
}

*/


#endif

