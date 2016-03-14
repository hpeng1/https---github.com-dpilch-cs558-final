
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

using namespace std;

MyDB_PageHandle MyDB_BufferManager :: getPage () {
    // create an unnamed handle
    // the id of an unnamed handle is -1, later when the slot is found, the id will become an another number
     MyDB_PageHandle new_handle = make_shared <MyDB_PageHandleBase> (-1, page_size, temp_file);
	
     if(pool_size < page_num)
     { 
        // if the pool is not full
        pages_pool[pool_size] = new_handle;
        new_handle->set_clean();
        cout<<"page "<<pages_pool[pool_size]->page_id<<" has been inserted to the pool"<<endl;
        pool_size++;
        new_handle->set_unpin();
        traverse_pages_pool();
     }
     else
     {
        // if the pool is full
        int victim_id = find_victim();
        cout<<"should evict: "<<victim_id<<endl;
        int evict_id = victim_id;
            // need to evict this page
            cout<<"evict the page "<<pages_pool[evict_id]->page_id<<" and write it back to "<<pages_pool[evict_id]->show_location()<<endl;
            if(pages_pool[evict_id]->see_dirty_status() == 1)
                write_back_page((char *)pages_pool[evict_id]->getBytes(), pages_pool[evict_id]->page_id, page_size, pages_pool[evict_id]->show_location());
            else
                cout<<"No need to write back, the page is clean"<<endl;
            pages_pool[evict_id] = new_handle;
            new_handle->set_clean();
            new_handle->set_unpin();
            traverse_pages_pool();

     }
    return new_handle;		
}

MyDB_PageHandle MyDB_BufferManager :: getPage (MyDB_TablePtr table_ptr, long index) {
    
    MyDB_PageHandle result = fetch_from_memory(index, table_ptr->getStorageLoc());
    MyDB_PageHandle newpage ;
	
    if( result == nullptr)
    {
        // the page is not in the memory, we need to push this page into the pool
        cout<<"Page "<<index<<" is not in the memory."<<endl;
        
        // if pool is not full yet, just create a new page in the pool
        if( pool_size < page_num)
        {
			// create a new handle
		    MyDB_PageHandle new_handle = make_shared <MyDB_PageHandleBase> (index, page_size, table_ptr->getStorageLoc());

            //push it into the pool
            pages_pool[pool_size] = new_handle;

			newpage = new_handle;
            newpage->set_clean();
            cout<<"page "<<pages_pool[pool_size]->page_id<<" has been inserted to the pool"<<endl;
            pool_size++;

            // pin down this page
            newpage->set_unpin();
            
            traverse_pages_pool();
			return (MyDB_PageHandle)newpage;
        }
        else
        {
            int victim_id = find_victim();
            cout<<"should evict: "<<victim_id<<endl;
            // pool is already full, need to evict a unpinned page
            // find the first unpinned page
            int evict_id = victim_id;
            cout<<"evict the page "<<pages_pool[evict_id]->page_id<<" and write it back to "<<pages_pool[evict_id]->show_location()<<endl;

            // find the location of the file(disk) and write the content back
            if(pages_pool[evict_id]->see_dirty_status() == 1)
                write_back_page((char *)pages_pool[evict_id]->getBytes(), pages_pool[evict_id]->page_id, page_size, pages_pool[evict_id]->show_location());
            else
                cout<<"No need to write back, the page is clean"<<endl;
            // use table_ptr
            
            // replace the evicted page handle with a new page handle
			MyDB_PageHandle new_handle = make_shared <MyDB_PageHandleBase> (index, page_size, table_ptr->getStorageLoc());
            pages_pool[evict_id] = new_handle;
			newpage = new_handle;
            newpage->set_clean();
            // pin down this page
            newpage->set_unpin();
            
            traverse_pages_pool();
			return (MyDB_PageHandle)newpage;
        }
    }
    else
    {
        // the page is in the pool's memory, ok to return the handle
        // pin down this page
        cout<<"Page "<<result->page_id<<" is already in the memory."<<endl;
        result->set_count(result->see_count()+1);
		return (MyDB_PageHandle)result;
    }
    
    
}


MyDB_PageHandle MyDB_BufferManager :: getPinnedPage (MyDB_TablePtr table_ptr, long index) {
    
    // see if the requested page is in the memory
    MyDB_PageHandle result = fetch_from_memory(index, table_ptr->getStorageLoc());
    MyDB_PageHandle newpage;

    if( result == nullptr)
    {
		// the page is not in the memory, we need to push this page into the pool 
        cout<<"Page "<<index<<" is not in the memory."<<endl;  

        // if pool is not full yet, just create a new page in the pool		
        if( pool_size < page_num)
        {		
            MyDB_PageHandle new_handle = make_shared <MyDB_PageHandleBase> (index, page_size, table_ptr->getStorageLoc());
			pages_pool[pool_size] = new_handle;
			newpage = new_handle;
            newpage->set_clean();
            cout<<"page "<<pages_pool[pool_size]->page_id<<" has been inserted to the pool"<<endl;
            pool_size++;
            // pin down this page
            newpage->set_pin();
            
            traverse_pages_pool();
			return (MyDB_PageHandle)newpage;
        }
        else
        {
            int victim_id = find_victim();
            cout<<"should evict: "<<victim_id<<endl;
            // pool is already full, need to evict a unpinned page
            // find the first unpinned page
            int evict_id = victim_id;
            cout<<"evict the page "<<pages_pool[evict_id]->page_id<<" and write it back to "<<pages_pool[evict_id]->show_location()<<endl;

            // find the location of the file(disk) and write the content back
            if(pages_pool[evict_id]->see_dirty_status() == 1)
                write_back_page((char *)pages_pool[evict_id]->getBytes(), pages_pool[evict_id]->page_id, page_size, pages_pool[evict_id]->show_location());
            else
                cout<<"No need to write back, the page is clean"<<endl;
            // use table_ptr
            
            // replace the evicted page with a new page
			MyDB_PageHandle new_handle = make_shared <MyDB_PageHandleBase> (index, page_size, table_ptr->getStorageLoc());
            pages_pool[evict_id] = new_handle;
          //  newpage = &pages_pool[evict_id];
			newpage = new_handle;
            newpage->set_clean();
            // pin down this page
            newpage->set_pin();
            
            traverse_pages_pool();
			return (MyDB_PageHandle)newpage;
        }
    }
    else
    {
        // the page is in the memory, ok to return the handle
        // pin down this page
        result->set_pin();
        cout<<"Page "<<result->page_id<<" is already in the memory."<<endl;
        result->set_count(result->see_count()+1);
		return (MyDB_PageHandle)result;
    }
    
    
}

MyDB_PageHandle MyDB_BufferManager :: getPinnedPage () {
    // create an unnamed handle
    // the id of an unnamed handle is -1, later when the slot is found, the id will become an another number
     MyDB_PageHandle new_handle = make_shared <MyDB_PageHandleBase> (-1, page_size, temp_file);
    
     if(pool_size < page_num)
     { 
        // if the pool is not full
        pages_pool[pool_size] = new_handle;
        new_handle->set_clean();
        cout<<"page "<<pages_pool[pool_size]->page_id<<" has been inserted to the pool"<<endl;
        pool_size++;
        new_handle->set_pin();
        traverse_pages_pool();
     }
     else
     {
        int victim_id = find_victim();
        cout<<"should evict: "<<victim_id<<endl;
        // if the pool is full
        int evict_id = victim_id;
            // need to evict this page
            cout<<"evict the page "<<pages_pool[evict_id]->page_id<<" and write it back to "<<pages_pool[evict_id]->show_location()<<endl;
            if(pages_pool[evict_id]->see_dirty_status() == 1)
                write_back_page((char *)pages_pool[evict_id]->getBytes(), pages_pool[evict_id]->page_id, page_size, pages_pool[evict_id]->show_location());
            else
                cout<<"No need write back, the page is clean"<<endl;
            pages_pool[evict_id] = new_handle;
            new_handle->set_clean();
            new_handle->set_pin();
            traverse_pages_pool();

     }
    return new_handle;  
	
}

void MyDB_BufferManager :: unpin (MyDB_PageHandle unpinMe) {
    // just call the public function of that page handle
    unpinMe->set_unpin();
    return;
}

MyDB_BufferManager :: MyDB_BufferManager (size_t pageSize, size_t numPages, string tempFile) {
    // set the page size, page number and the extra file name
    page_size = pageSize;
    page_num = numPages;
    temp_file = tempFile;
    pool_size = 0;
    
    // create the temporary file
    ofstream myfile;
    myfile.open (temp_file);
    myfile << "This is where temporary page goes";
    myfile.close();
    
    // create an array of page handles,
    pages_pool = new MyDB_PageHandle[page_num];

    cout<<page_num<<" page handles are created. Each page size: "<<page_size<<endl;
    cout<< "buffer pool manager is up."<<endl;
}

MyDB_PageHandle MyDB_BufferManager :: fetch_from_memory(long index, string location) {
    
    for(int i =0; i<pool_size; i++)
    {
        // page id should be the same, and location should be identical
        if( pages_pool[i]->page_id==index && location.compare(pages_pool[i]->show_location())==0 )
        {
            cout<<"page found!"<<endl;
            return pages_pool[i];
            break;
        }
    }
    cout<<"page "<< index<< " not found"<<endl;
    return nullptr;
}

int MyDB_BufferManager :: find_victim(){
    // LRU algorithm
    cout<<"The LRU kicks in!"<<endl;
    int victim_id = -1;
    int min_count = 100;
    for(int i = 0; i<pool_size; i++)
    {
        if(pages_pool[i]->see_pin_status()==0 && pages_pool[i]->see_count()<= min_count)
        {
            min_count = pages_pool[i]->see_count();
            victim_id = i;
        }
    }
    if(victim_id == -1)
        cout<<"Warning, all pages are pinned, no free memory!"<<endl;
    return victim_id;
}

void MyDB_BufferManager :: traverse_pages_pool(){
    for(int i=0; i<pool_size; i++)
    {
        cout<<"Page id: "<<pages_pool[i]->page_id<<", pinned: "<<pages_pool[i]->see_pin_status()<<", dirty: "<<pages_pool[i]->see_dirty_status()<<", location: "<<pages_pool[i]->show_location()<<", count: "<<pages_pool[i]->see_count()<<endl;
    }
    return;
}


void MyDB_BufferManager :: write_back_page(char* content, int position, int page_size, string loc){

    cout<<"write back page content to the location: "<<loc<<endl;
    if(loc.compare(temp_file) == 0)
        cout<<"write back to the temp file"<<endl;
    else
        cout<<"write back to the specific file"<<endl;

    return;
}

MyDB_BufferManager :: ~MyDB_BufferManager () {

}
	
#endif


