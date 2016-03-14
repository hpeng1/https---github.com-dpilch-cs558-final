
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
	// if the page is still in the buffer, just return the content of the buffer
	if(page_ptr->is_in_buffer() == 1)
	{
		cout<<"page is still in the buffer, so get the page content"<<endl;
		manager->time++;
		page_ptr->timestamp = manager->time;
		return (char*) manager->get_content( page_ptr->see_buffer_index() );
	}	
	else
	{
		cout<<"content is not in the buffer, will load it from the file"<<endl;

		if(manager->see_buffer_size() >= manager->see_page_num())
		{
			cout<<"buffer is full, evict a victim"<<endl;
			// pages buffer is full, need to evict
			int victim_index = manager->find_victim();
			if(victim_index == -1)	// no space in buffer
			{
				cout<<"Program terminated! All pages are pinned, no free memory!"<<endl;
				exit(1);
			}	
			MyDB_pagePtr victim_page = manager->get_page(victim_index);
			victim_page->timestamp = 0;

			// need to write back when the victim is dirty
			if(victim_page->see_dirty_status() == 1)
				manager->write_back_page(manager->get_content( victim_index), victim_page->getPageId(), victim_page->getPageSize(), victim_page->show_location());

			// read the content from file to buffer
			cout<<"Read result from the file: ";
			read_from_file(page_ptr, manager->get_content( victim_index) );

			// put the page in the pages buffer
			manager->put_page_in_buffer(page_ptr, victim_index);
			page_ptr->change_buffer_index( victim_index );
			page_ptr->put_into_buffer();
			page_ptr->set_clean();
			manager->time++;
			page_ptr->timestamp = manager->time;

			return manager->get_content( victim_index );
		}
		else
		{
			// pages buffer is not full, append the page to pages_buffer
			cout<<"pages buffer not full yet"<<endl;
			page_ptr->change_buffer_index( manager->see_buffer_size() );
			page_ptr->put_into_buffer();
			page_ptr->set_clean();
			manager->time++;
			page_ptr->timestamp = manager->time;
			cout<<"buffer size: "<<manager->see_buffer_size()<<endl;
			manager->put_page_in_buffer( page_ptr, manager->see_buffer_size() );
			//manager->traverse_pages_buffer();
			// load the content from the file to buffer

			cout<<"Read result from the file: ";
			//manager->traverse_pages_buffer();
			read_from_file(page_ptr, manager->get_content( manager->see_buffer_size() ) );
			manager->buffer_size_increase();

			return manager->get_content( page_ptr->see_buffer_index() );
		}


	}
}

void MyDB_PageHandleBase :: wroteBytes () {
    // user call this function to mark the page dirty
	cout<<"mark page dirty"<<endl;
	page_ptr->set_dirty();
    return;
}

MyDB_PageHandleBase :: ~MyDB_PageHandleBase () {
	cout<<"***********Panic! handle to page "<<page_ptr->getPageId()<<" is lost*********"<<endl;

	// when an handle is lost, decrease the count of the page by 1
	page_ptr->set_count(page_ptr->see_count() - 1);

	// if the count of a page is decreased to 0, should unpin
	if(page_ptr->see_count()<=0)
	{
		cout<<"page's reference count is 0, should unpin"<<endl;
		page_ptr->set_unpin();


		// if the page is anouymous, should also free the page in the pool, no write back
		if(page_ptr->see_anonymous()== 1 && page_ptr->is_in_buffer()==1)
		{
			cout<<"anonymous page will be cleaned from the pool"<<endl;
			//cout<<"testing forwarding, page size: "<<manager->page_size<<endl;
			if(page_ptr->see_dirty_status()==1)
			{
				manager->write_back_page(manager->get_content(page_ptr->see_buffer_index()), page_ptr->getPageId(), page_ptr->getPageSize(), page_ptr->show_location());
			}
			manager->clean_up(page_ptr);
			cout<<"also recycle the bytes in the tempfile"<<endl;
			cout<<"***********Panic! handle to page "<<page_ptr->getPageId()<<" is lost*********"<<endl;
			//manager->traverse_pages_buffer();
			manager->available_Id.push_back(page_ptr->getPageId());

		}
	}

}

MyDB_PageHandleBase :: MyDB_PageHandleBase (MyDB_pagePtr pagePtr, MyDB_BufferManager* mgr) {
	page_ptr=pagePtr;
	manager = mgr;
	//manager = pool_manager;
} 

void MyDB_PageHandleBase :: show_content(){
	if(page_ptr->is_in_buffer() == 1)
	{
		// only show content if it in the buffer
		cout<<"content of the page: ";
		cout<<manager->get_content( page_ptr->see_buffer_index() )<<endl;
	}
	return;
}

void MyDB_PageHandleBase :: set_unpin(){
	page_ptr->set_unpin();
}
/*
int MyDB_PageHandleBase :: see_manager_pool_size(){
	manager->see_pool_size();
}

*/
void MyDB_PageHandleBase :: read_from_file(MyDB_pagePtr page_ptr, char* target){
	// read from the file into the target
	FILE* fp = fopen(page_ptr->show_location().c_str(),"rb+"); 
	if(fp == nullptr)
	{
		cout<<"File not found, program will exit!"<<endl;
		exit(1);
	}
	int position = page_ptr->getPageId();
	int page_size = page_ptr->getPageSize();

	fseek(fp, position*page_size, SEEK_SET);  
    fread (target,1,page_size,fp);
	cout<<target<<endl;
	return;
}


#endif

