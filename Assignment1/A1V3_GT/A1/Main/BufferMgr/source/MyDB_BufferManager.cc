
#ifndef BUFFER_MGR_C
#define BUFFER_MGR_C

#include "MyDB_BufferManager.h"
#include <string>
#include <iostream>
#include <unistd.h>
#include <stdio.h>
#include <fstream>
#include <stdlib.h>
#include <vector>
#include "MyDB_page.h"

       #include <sys/types.h>
       #include <sys/stat.h>
       #include <fcntl.h>
using namespace std;

MyDB_PageHandle MyDB_BufferManager :: getPinnedPage () {
    // create a new page pointer
    MyDB_pagePtr new_pagePtr = make_shared <MyDB_page> (page_size, temp_file,available_Id.back());
	available_Id.pop_back();
	anolymousId++;	
	available_Id.push_back(anolymousId);

    new_pagePtr->set_clean();
    new_pagePtr->set_pin();
    new_pagePtr->remove_from_buffer();
    new_pagePtr->change_buffer_index(-1);
    // a new pinned page is created, but not in the pool yet

    // create a new handle for that page, increase the page's count by 1
    MyDB_PageHandle new_handle = make_shared <MyDB_PageHandleBase>(new_pagePtr, this);
    new_pagePtr->set_count(new_pagePtr->see_count()+1);

    // return the new handle
    return new_handle;      
}

MyDB_PageHandle MyDB_BufferManager :: getPage () {
    // create a new page pointer
    MyDB_pagePtr new_pagePtr = make_shared <MyDB_page> (page_size, temp_file,available_Id.back());
	available_Id.pop_back();
	anolymousId++;	
	available_Id.push_back(anolymousId);

    new_pagePtr->set_clean();
    new_pagePtr->set_unpin();
    new_pagePtr->remove_from_buffer();
    new_pagePtr->change_buffer_index(-1);
    // a new page is created, but not in the pool yet

    // create a new handle for that page, increase the page's count by 1
    MyDB_PageHandle new_handle = make_shared <MyDB_PageHandleBase>(new_pagePtr, this);
    new_pagePtr->set_count(new_pagePtr->see_count()+1);

    // return the new handle
    return new_handle; 
}

MyDB_PageHandle MyDB_BufferManager :: getPage (MyDB_TablePtr table_ptr, long id) {
    
	MyDB_pagePtr result = fetch_from_buffer(id,table_ptr->getStorageLoc());
    if( result == nullptr)
    {
        // the page is not in the buffer, but it might be in the pool
        MyDB_pagePtr second_result = fetch_from_pool(id,table_ptr->getStorageLoc());
        if (second_result == nullptr)
        {
            cout<<"Page "<<id<<" is not in the buffer nor the pool."<<endl;

            // create a new page
            MyDB_pagePtr new_pagePtr = make_shared <MyDB_page> (id, page_size, table_ptr->getStorageLoc(),0);
            new_pagePtr->set_clean();
            new_pagePtr->set_unpin();
            new_pagePtr->remove_from_buffer();
            new_pagePtr->change_buffer_index(-1);

            // insert the new page into the pool
            pages_pool.push_back(new_pagePtr);

            // create a new handle for that page, increase the page's count by 1, return it
            MyDB_PageHandle new_handle = make_shared <MyDB_PageHandleBase>(new_pagePtr, this);
            new_pagePtr->set_count(new_pagePtr->see_count()+1);
            return new_handle;
        }
        else
        {
            cout<<"Page "<<id<<" is not in the buffer, but in the pool:)"<<endl;

            // create a handle of that page in the pool, increase the page's count by 1, return it
            MyDB_PageHandle new_handle = make_shared <MyDB_PageHandleBase>(second_result, this);
            second_result->set_count(second_result->see_count()+1);
            return new_handle;
        }
        
    }
    else
    {
        // the page is in the buffer, just create a new handle, increase the page's count by 1
        cout<<"Page "<<result->getPageId()<<" is in the pages buffer."<<endl;
		MyDB_PageHandle new_handle = make_shared <MyDB_PageHandleBase>(result, this);
		result->set_count(result->see_count()+1);
		return new_handle;
    }
}


MyDB_PageHandle MyDB_BufferManager :: getPinnedPage (MyDB_TablePtr table_ptr, long id) {
    MyDB_pagePtr result = fetch_from_buffer(id,table_ptr->getStorageLoc());
    if( result == nullptr)
    {
        // the page is not in the buffer, but it might be in the pool
        MyDB_pagePtr second_result = fetch_from_pool(id,table_ptr->getStorageLoc());
        if (second_result == nullptr)
        {
            cout<<"Page "<<id<<" is not in the buffer nor the pool."<<endl;

            // create a new page
            MyDB_pagePtr new_pagePtr = make_shared <MyDB_page> (id, page_size, table_ptr->getStorageLoc(),0);
            new_pagePtr->set_clean();
            new_pagePtr->set_pin();
            new_pagePtr->remove_from_buffer();
            new_pagePtr->change_buffer_index(-1);

            // insert the new page into the pool
            pages_pool.push_back(new_pagePtr);

            // create a new handle for that page, increase the page's count by 1, return it
            MyDB_PageHandle new_handle = make_shared <MyDB_PageHandleBase>(new_pagePtr, this);
            new_pagePtr->set_count(new_pagePtr->see_count()+1);
            return new_handle;
        }
        else
        {
            cout<<"Page "<<id<<" is not in the buffer, but in the pool:)"<<endl;

            // create a handle of that page in the pool, increase the page's count by 1, return it
            MyDB_PageHandle new_handle = make_shared <MyDB_PageHandleBase>(second_result, this);
            second_result->set_pin();
            second_result->set_count(second_result->see_count()+1);
            return new_handle;
        }
        
    }
    else
    {
        // the page is in the buffer, just create a new handle, increase the page's count by 1
        cout<<"Page "<<result->getPageId()<<" is in the pages buffer."<<endl;
        result->set_pin();
        MyDB_PageHandle new_handle = make_shared <MyDB_PageHandleBase>(result, this);
        result->set_count(result->see_count()+1);
        return new_handle;
    }
   
}


void MyDB_BufferManager :: unpin (MyDB_PageHandle unpinMe) {
    unpinMe->set_unpin();
    return;
}

MyDB_BufferManager :: MyDB_BufferManager (size_t pageSize, size_t numPages, string tempFile) {
    // set the page size, page number, pool size and the extra file name
    page_size = pageSize;
    page_num = numPages;
    temp_file = tempFile;
    buffer_size = 0;
    time = 0;
    
	//initial  
	anolymousId=0;
	available_Id.push_back(0);

    // create the temporary file
    /*
    ofstream myfile;
    myfile.open (temp_file);
    myfile << "Bugs, I am going to crush you and throw you into the wind!";
    myfile.close();
    */

    fopen(tempFile.c_str(),"rb+");
    
    // create an array for page pointers
    pages_buffer = new MyDB_pagePtr[page_num];

    // create an array of char* as buffer
    // later on, getBytes() will return buffer[i]
    buffer = (char**)malloc(page_num*sizeof(char*));
    for(int i = 0; i < page_num; i++)
        buffer[i] = (char*)malloc(page_size*sizeof(char));
       //buffer[i] = new char[page_size];

    cout<<page_num<<" page handles are created. Each page size: "<<page_size<<endl;
    cout<< "buffer pool manager is up."<<endl;
    cout<<""<<endl;
}

MyDB_pagePtr MyDB_BufferManager :: fetch_from_buffer(long search_id, string page_location) {
    // see if the page is in the pool, will return null if not
    // if is in the pool, will return that page pointer
    for(int i =0; i<buffer_size; i++)
    {
		if( (pages_buffer[i]->getPageId()) == search_id && page_location.compare(pages_buffer[i]->show_location())==0 )
        {
            cout<<"page found!"<<endl;
            return pages_buffer[i];
        }
    }
    cout<<"page "<< search_id<< " not found"<<endl;
    return nullptr;
}

MyDB_pagePtr MyDB_BufferManager :: fetch_from_pool(long search_id, string page_location) {
    // see if the page is in the pool, will return null if not
    // if is in the pool, will return that page pointer
    for(int i =0; i<pages_pool.size(); i++)
    {
        if( (pages_pool[i]->getPageId()) == search_id && page_location.compare(pages_pool[i]->show_location())==0 )
        {
            cout<<"page found!"<<endl;
            return pages_pool[i];
        }
    }
    cout<<"page "<< search_id<< " not found"<<endl;
    return nullptr;
}

void MyDB_BufferManager :: traverse_pages_buffer(){
    // display all information of pages
    // buffer is a page_ptr array
    for(int i=0; i<buffer_size; i++)
    {
        cout<<"Page id: "<<pages_buffer[i]->getPageId()<<", pinned: "<<pages_buffer[i]->see_pin_status()<<", dirty: "<<pages_buffer[i]->see_dirty_status()<<", location: "<<pages_buffer[i]->show_location()<<", count: "<<pages_buffer[i]->see_count()<<", anonymous: "<<pages_buffer[i]->see_anonymous()<<", in buffer: "<<pages_buffer[i]->is_in_buffer();
        cout<<", buffer index: "<<pages_buffer[i]->see_buffer_index();
        cout<<", content: "<<buffer[i];
        cout<<", timestamp: "<<pages_buffer[i]->timestamp<<endl;
    }
    return;
}

void MyDB_BufferManager :: traverse_pages_pool(){
    // display all information of pages in the pool
    // pool is a vector
    for(int i=0; i<pages_pool.size(); i++)
    {
		cout<<"Page id: "<<pages_pool[i]->getPageId()<<", pinned: "<<pages_pool[i]->see_pin_status()<<", dirty: "<<pages_pool[i]->see_dirty_status()<<", location: "<<pages_pool[i]->show_location()<<", count: "<<pages_pool[i]->see_count()<<", anonymous: "<<pages_pool[i]->see_anonymous()<<", in buffer: "<<pages_pool[i]->is_in_buffer()<<endl;
    }
    return;
}


int MyDB_BufferManager :: find_victim()
{
    // LRU algorithm
    cout<<"The LRU kicks in!"<<endl;
    int victim_id = -1;
    int min_count = 10000;
    for(int i = 0; i<buffer_size; i++)
    {
        if(pages_buffer[i]->see_pin_status()==0 && pages_buffer[i]->timestamp<= min_count)
        {
            min_count = pages_buffer[i]->timestamp;
            victim_id = i;
        }
    }
    if(victim_id == -1)
    {
        cout<<"Warning, all pages are pinned, no free memory!"<<endl;
        return -1;
    }    
    return victim_id;
}

void MyDB_BufferManager :: write_back_page(char* content, int position, int page_size, string loc){

    cout<<"write back page content to the location: "<<loc<<endl;
    cout<<""<<endl;
    FILE *fp = fopen(loc.c_str(), "rb+");
    if(fp == nullptr)
        exit(1);
    fseek(fp, position*page_size,SEEK_SET);
    fwrite(content, sizeof(char), page_size, fp);
	fclose(fp);
    return;
}

int MyDB_BufferManager :: see_buffer_size(){
    return buffer_size;
}

int MyDB_BufferManager :: see_page_num(){
    return page_num;
}

void MyDB_BufferManager :: clean_up(MyDB_pagePtr target){
    cout<<"clean up the buffer and pages buffer"<<endl;
    // first, find the target in the pool
    int pool_index;
    pool_index = target->see_buffer_index();
    cout<< "the clean up index is: "<<pool_index<<endl;

        cout<< "shift the pages buffer and buffer: "<<buffer_size-1-pool_index<<" times"<<endl;
        for(int i=pool_index; i<buffer_size-1; i++)
        {
            pages_buffer[i] = pages_buffer[i+1];
            pages_buffer[i]->change_buffer_index(pages_buffer[i]->see_buffer_index()-1);
            //buffer[i] = buffer[i+1];
            for(int j=0; j<page_size; j++)
            {
                buffer[i][j] = buffer[i+1][j];
            }
            //strcpy(buffer[i],buffer[i+1]);
        }
        buffer_size--;
        cout<<"new buffer size: "<<buffer_size<<endl;
        return;
}

char* MyDB_BufferManager :: get_content(int index){
    return buffer[index];
}

void MyDB_BufferManager :: put_page_in_buffer(MyDB_pagePtr page_ptr, int index)
{
    cout<<"put page: "<<page_ptr->getPageId ()<<" into index "<<index<<endl;
    pages_buffer[index] = page_ptr;
    return;
}

void MyDB_BufferManager :: buffer_size_increase() {
    buffer_size++;
    return;
}

void MyDB_BufferManager :: buffer_size_decrease() {
    buffer_size--;
    return;
}

MyDB_pagePtr MyDB_BufferManager :: get_page(int index) {
    return pages_buffer[index];
}

MyDB_BufferManager :: ~MyDB_BufferManager () {
    cout<<"Manager destructed, write all dirty pages back"<<endl;
    for(int i = 0; i< buffer_size; i++)
    {
        // write back all dirty pages
        if(pages_buffer[i]->see_dirty_status()== 1 )
        {
            write_back_page(get_content(i),pages_buffer[i]->getPageId(),pages_buffer[i]->getPageSize(),pages_buffer[i]->show_location());
        }
    }
}
	
#endif


