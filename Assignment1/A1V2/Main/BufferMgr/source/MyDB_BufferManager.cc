
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


MyDB_PageHandle MyDB_BufferManager :: getPage () {
    MyDB_pagePtr newpage ;
    if( pool_size < page_num)
        {   
            // there is still pace in the pool
            MyDB_pagePtr new_pagePtr = make_shared <MyDB_page> (-1, page_size, temp_file,1);
            pages_pool[pool_size] = new_pagePtr;
            newpage = new_pagePtr;
            newpage->set_clean();
            cout<<"page "<<pages_pool[pool_size]->getPageId()<<" has been inserted to the pool"<<endl;
            newpage->put_into_buffer() ;
            pool_size++;
            
            // create a new handle
            new_pagePtr->set_count(new_pagePtr->see_count()+1);
            MyDB_PageHandle new_handle = make_shared <MyDB_PageHandleBase>(new_pagePtr, this);

            traverse_pages_pool();
            return new_handle;
        }
        else
        {
            // pool is already full, need to evict a unpinned page
            // find the first unpinned page
            int evict_id = find_victim();    
            if(evict_id==-1)
            {
                return nullptr;
            }
            cout<<"evict the page "<<pages_pool[evict_id]->getPageId()<<" and write it back to disk"<<endl;
            pages_pool[evict_id]->remove_from_buffer ();

            if(pages_pool[evict_id]->see_dirty_status() == 1)
                write_back_page((char *)pages_pool[evict_id]->getPageContent(), pages_pool[evict_id]->getPageId(), page_size, pages_pool[evict_id]->show_location());
            else
                cout<<"No need to write back, the page is clean"<<endl;
            // find the location of the file(disk) and write the content back
            // use table_ptr
            
            // replace the evicted page with a new page
            MyDB_pagePtr new_pagePtr = make_shared <MyDB_page> (-1, page_size, temp_file,1);
            pages_pool[evict_id] = new_pagePtr;
            newpage = new_pagePtr;
            newpage->set_clean();
            newpage->put_into_buffer() ;

            // create a new handle of that page
            MyDB_PageHandle new_handle = make_shared <MyDB_PageHandleBase>(new_pagePtr, this);
            new_pagePtr->set_count(new_pagePtr->see_count()+1);
            traverse_pages_pool();
            
            return new_handle;
        }

	return nullptr;		
}

MyDB_PageHandle MyDB_BufferManager :: getPage (MyDB_TablePtr table_ptr, long index) {
    
	MyDB_pagePtr result = fetch_from_memory(index,table_ptr->getStorageLoc());
    MyDB_pagePtr newpage ;
	
    if( result == nullptr)
    {
        // the page is not in the memory, we need to push this page into the pool
        cout<<"Page "<<index<<" is not in the memory."<<endl;
        
        // if pool is not full yet, just create a new page in the pool
		
        if( pool_size < page_num)
        {	
			MyDB_pagePtr new_pagePtr = make_shared <MyDB_page> (index, page_size, table_ptr->getStorageLoc(),0);
            pages_pool[pool_size] = new_pagePtr;
			newpage = new_pagePtr;
            newpage->set_clean();
            newpage->put_into_buffer() ;
			cout<<"page "<<pages_pool[pool_size]->getPageId()<<" has been inserted to the pool"<<endl;
            pool_size++;
            
            // create a new handle
            new_pagePtr->set_count(new_pagePtr->see_count()+1);
			MyDB_PageHandle new_handle = make_shared <MyDB_PageHandleBase>(new_pagePtr, this);

			traverse_pages_pool();
            return new_handle;
        }
        else
        {
            // pool is already full, need to evict a unpinned page
            // find the first unpinned page
			int evict_id = find_victim();    
			if(evict_id==-1)
			{
				return nullptr;
			}
			cout<<"evict the page "<<pages_pool[evict_id]->getPageId()<<" and write it back to disk"<<endl;
            pages_pool[evict_id]->remove_from_buffer ();

			if(pages_pool[evict_id]->see_dirty_status() == 1)
				write_back_page((char *)pages_pool[evict_id]->getPageContent(), pages_pool[evict_id]->getPageId(), page_size, pages_pool[evict_id]->show_location());
            else
                cout<<"No need to write back, the page is clean"<<endl;
            // find the location of the file(disk) and write the content back
            // use table_ptr
            
            // replace the evicted page with a new page
			MyDB_pagePtr new_pagePtr = make_shared <MyDB_page> (index, page_size, table_ptr->getStorageLoc(),0);
            pages_pool[evict_id] = new_pagePtr;
			newpage = new_pagePtr;
            newpage->set_clean();
            newpage->put_into_buffer() ;

            // create a new handle of that page
            MyDB_PageHandle new_handle = make_shared <MyDB_PageHandleBase>(new_pagePtr, this);
            new_pagePtr->set_count(new_pagePtr->see_count()+1);
            traverse_pages_pool();
			
			return new_handle;
        }
    }
    else
    {
        // the page is in the memory, ok to return the handle
        // pin down this page
        cout<<"Page "<<result->getPageId()<<" is in the memory."<<endl;
		MyDB_PageHandle new_handle = make_shared <MyDB_PageHandleBase>(result);
		result->set_count(result->see_count()+1);
		return new_handle;
    }
}


MyDB_PageHandle MyDB_BufferManager :: getPinnedPage (MyDB_TablePtr table_ptr, long index) {
    
    MyDB_pagePtr result = fetch_from_memory(index,table_ptr->getStorageLoc());
    MyDB_pagePtr newpage ;
	
    if( result == nullptr)
    {
        // the page is not in the memory, we need to push this page into the pool
        cout<<"Page "<<index<<" is not in the memory."<<endl;
        // if pool is not full yet, just create a new page in the pool
		
        if( pool_size < page_num)
        {	
			MyDB_pagePtr new_pagePtr = make_shared <MyDB_page> (index, page_size, table_ptr->getStorageLoc(),0);
            pages_pool[pool_size] = new_pagePtr;
			newpage = new_pagePtr;
            newpage->set_clean();
            newpage->set_pin();
            newpage->put_into_buffer() ;
			cout<<"page "<<pages_pool[pool_size]->getPageId()<<" has been inserted to the pool"<<endl;
            pool_size++;
            //  traverse
           
			MyDB_PageHandle new_handle = make_shared <MyDB_PageHandleBase>(new_pagePtr, this);
            new_pagePtr->set_count(new_pagePtr->see_count()+1); 
            traverse_pages_pool();
			return new_handle;
        }
        else
        {
            // pool is already full, need to evict a unpinned page
            // find the first unpinned page
			int evict_id = find_victim();    
			if(evict_id==-1)
			{
				return nullptr;
			}
			cout<<"evict the page "<<pages_pool[evict_id]->getPageId()<<" and write it back to disk"<<endl;
            pages_pool[evict_id]->remove_from_buffer ();

			if(pages_pool[evict_id]->see_dirty_status() == 1)
				write_back_page((char *)pages_pool[evict_id]->getPageContent(), pages_pool[evict_id]->getPageId(), page_size, pages_pool[evict_id]->show_location());
            else
                cout<<"No need to write back, the page is clean"<<endl;
            // find the location of the file(disk) and write the content back
            // use table_ptr
            
            // replace the evicted page with a new page
			MyDB_pagePtr new_pagePtr = make_shared <MyDB_page> (index, page_size, table_ptr->getStorageLoc(),0);
            pages_pool[evict_id] = new_pagePtr;
			newpage = new_pagePtr;
            new_pagePtr->set_clean();
            new_pagePtr->set_pin();
            newpage->put_into_buffer() ;
            
            new_pagePtr->set_count(new_pagePtr->see_count()+1);
			MyDB_PageHandle new_handle = make_shared <MyDB_PageHandleBase>(new_pagePtr, this);
            traverse_pages_pool();
			return new_handle;
        }
    }
    else
    {
        // the page is in the memory, ok to return the handle
        // pin down this page
        cout<<"Page "<<result->getPageId()<<" is in the memory."<<endl;
		MyDB_PageHandle new_handle = make_shared <MyDB_PageHandleBase>(result);
		result->set_count(result->see_count()+1);
        result->set_pin();
		return new_handle;
    }
}

MyDB_PageHandle MyDB_BufferManager :: getPinnedPage () {
    MyDB_pagePtr newpage ;
    if( pool_size < page_num)
        {   
            // there is still pace in the pool
            MyDB_pagePtr new_pagePtr = make_shared <MyDB_page> (-1, page_size, temp_file,1);
            pages_pool[pool_size] = new_pagePtr;
            newpage = new_pagePtr;
            newpage->set_clean();
            newpage->put_into_buffer() ;
            cout<<"page "<<pages_pool[pool_size]->getPageId()<<" has been inserted to the pool"<<endl;
            pool_size++;
            
            // create a new handle
            new_pagePtr->set_count(new_pagePtr->see_count()+1);
            new_pagePtr->set_pin();
            MyDB_PageHandle new_handle = make_shared <MyDB_PageHandleBase>(new_pagePtr, this);

            traverse_pages_pool();
            return new_handle;
        }
        else
        {
            // pool is already full, need to evict a unpinned page
            // find the first unpinned page
            int evict_id = find_victim();    
            if(evict_id==-1)
            {
                return nullptr;
            }
            cout<<"evict the page "<<pages_pool[evict_id]->getPageId()<<" and write it back to disk"<<endl;
            pages_pool[evict_id]->remove_from_buffer ();

            if(pages_pool[evict_id]->see_dirty_status() == 1)
                write_back_page((char *)pages_pool[evict_id]->getPageContent(), pages_pool[evict_id]->getPageId(), page_size, pages_pool[evict_id]->show_location());
            else
                cout<<"No need to write back, the page is clean"<<endl;
            // find the location of the file(disk) and write the content back
            // use table_ptr
            
            // replace the evicted page with a new page
            MyDB_pagePtr new_pagePtr = make_shared <MyDB_page> (-1, page_size, temp_file,1);
            pages_pool[evict_id] = new_pagePtr;
            newpage = new_pagePtr;
            newpage->set_clean();
            newpage->put_into_buffer() ;

            // create a new handle of that page
            MyDB_PageHandle new_handle = make_shared <MyDB_PageHandleBase>(new_pagePtr, this);
            new_pagePtr->set_count(new_pagePtr->see_count()+1);
            new_pagePtr->set_pin();
            traverse_pages_pool();
            
            return new_handle;
        }

	return nullptr;		
}

void MyDB_BufferManager :: unpin (MyDB_PageHandle unpinMe) {
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
    myfile << "Listen, bugs, I am going to crush you and throw you into the wind!";
    myfile.close();
    
    // create an array to page handles,
 //   pages_pool = (MyDB_PageHandle*)malloc(page_num * sizeof(MyDB_PageHandle));
    pages_pool = new MyDB_pagePtr[page_num];
    //vector<MyDB_PageHandleBase> pages_pool(page_num);
    cout<<page_num<<" page handles are created. Each page size: "<<page_size<<endl;
    cout<< "buffer pool manager is up."<<endl;
}

MyDB_pagePtr MyDB_BufferManager :: fetch_from_memory(long index, string page_location) {
    
    for(int i =0; i<pool_size; i++)
    {
		if( (pages_pool[i]->getPageId()) == index && page_location.compare(pages_pool[i]->show_location())==0 )
        {
            cout<<"page found!"<<endl;
            return pages_pool[i];
        }
    }
    cout<<"page "<< index<< " not found"<<endl;
    return nullptr;
}

void MyDB_BufferManager :: traverse_pages_pool(){
    for(int i=0; i<pool_size; i++)
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
    {
        cout<<"Warning, all pages are pinned, no free memory!"<<endl;
        return 0;
    }    
    return victim_id;
}

void MyDB_BufferManager :: write_back_page(char* content, int position, int page_size, string loc){

    cout<<"write back page content to the location: "<<loc<<endl;
    /*
    int File;
    File = open(loc.c_str(), O_FSYNC);
    write (File , "lol", 3);
    close(File);
    */

    /*
    ofstream fout;
    fout.open(loc.c_str(),ios::app);
    fout.seekp ( position*page_size);
    cout<<"position: "<<position*page_size<<endl;

    // write to the stream
    fout <<content; 
    //fout.seekp (0, ios::beg);
    fout.close();
    */

    FILE *fp = fopen(loc.c_str(), "rb+");
    fseek(fp, position*page_size,SEEK_SET);
    fwrite(content, sizeof(char), page_size, fp);
　　 fclose(fp);
    return;


}

int MyDB_BufferManager :: see_pool_size(){
    return pool_size;
}

void MyDB_BufferManager :: clean_up(MyDB_pagePtr target){
    cout<<"clean up the pool"<<endl;
    // first, find the target in the pool
    int pool_index = -1;
    for(int i=0; i< pool_size; i++)
    {
        if(target->getPageId()== pages_pool[i]->getPageId() && target->show_location().compare(pages_pool[i]->show_location())==0)
        {
            pool_index = i;
            break;
        }
    }

    if(pool_index == -1)
    {
        cout<<"not in the pool, no need to clean up"<<endl;
        return;
    }
    else
    {
        cout<< "shift the pool: "<<pool_size-1-pool_index<<" times"<<endl;
        for(int i=pool_index; i<pool_size-1; i++)
        {
            pages_pool[i] = pages_pool[i+1];
        }
        pool_size--;
        return;
    }
}

MyDB_BufferManager :: ~MyDB_BufferManager () {

}
	
#endif


