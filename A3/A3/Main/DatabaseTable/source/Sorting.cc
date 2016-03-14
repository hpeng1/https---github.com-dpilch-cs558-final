
#ifndef SORT_C
#define SORT_C

#include <queue>
#include "MyDB_PageReaderWriter.h"
#include "MyDB_TableRecIterator.h"
#include "MyDB_TableRecIteratorAlt.h"
#include "MyDB_TableReaderWriter.h"
#include "IteratorComparator.h"
#include "Sorting.h"

using namespace std;

void mergeIntoFile (MyDB_TableReaderWriter &sortIntoMe, vector <MyDB_RecordIteratorAltPtr> &mergeUs, 
	function <bool ()> comparator, MyDB_RecordPtr lhs, MyDB_RecordPtr rhs) {

	// create the comparator and the priority queue
	IteratorComparator temp (comparator, lhs, rhs);
	priority_queue <MyDB_RecordIteratorAltPtr, vector <MyDB_RecordIteratorAltPtr>, IteratorComparator> pq (temp);

	// load up the set
	for (MyDB_RecordIteratorAltPtr m : mergeUs) {
		if (m->advance ()) {
			pq.push (m);
		}
	}

	// and write everyone out
	int counter = 0;
	while (pq.size () != 0) {

		// write the dude to the output
		auto myIter = pq.top ();
		myIter->getCurrent (lhs);
		sortIntoMe.append (lhs);
		counter++;

		// remove from the q
		pq.pop ();

		// re-insert
		if (myIter->advance ()) {
			pq.push (myIter);
		}
	}
}

void appendRecord (MyDB_PageReaderWriter &curPage, vector <MyDB_PageReaderWriter> &returnVal, 
	MyDB_RecordPtr appendMe, MyDB_BufferManagerPtr parent) {

	// try to append to the current page
	if (!curPage.append (appendMe)) {

		// if we cannot, then add a new one to the output vector
		returnVal.push_back (curPage);
		MyDB_PageReaderWriter temp (*parent);
		temp.append (appendMe);
		curPage = temp;
	}
}

vector <MyDB_PageReaderWriter> mergeIntoList (MyDB_BufferManagerPtr parent, MyDB_RecordIteratorAltPtr leftIter, 
	MyDB_RecordIteratorAltPtr rightIter, function <bool ()> comparator, MyDB_RecordPtr lhs, MyDB_RecordPtr rhs) {
	
	vector <MyDB_PageReaderWriter> returnVal;
	MyDB_PageReaderWriter curPage (*parent);
	bool lhsLoaded = false, rhsLoaded = false;

	// if one of the runs is empty, get outta here
	if (!leftIter->advance ()) {
		while (rightIter->advance ()) {
			rightIter->getCurrent (rhs);
			appendRecord (curPage, returnVal, rhs, parent);
		}
	} else if (!rightIter->advance ()) {
		while (leftIter->advance ()) {
			leftIter->getCurrent (lhs);
			appendRecord (curPage, returnVal, lhs, parent);
		}
	} else {
		while (true) {
	
			// get the two records

			// here's a bit of an optimization... if one of the records is loaded, don't re-load
			if (!lhsLoaded) {
				leftIter->getCurrent (lhs);
				lhsLoaded = true;
			}

			if (!rhsLoaded) {
				rightIter->getCurrent (rhs);		
				rhsLoaded = true;
			}
	
			// see if the lhs is less
			if (comparator ()) {
				appendRecord (curPage, returnVal, lhs, parent);
				lhsLoaded = false;

				// deal with the case where we have to append all of the right records to the output
				if (!leftIter->advance ()) {
					appendRecord (curPage, returnVal, rhs, parent);
					while (rightIter->advance ()) {
						rightIter->getCurrent (rhs);
						appendRecord (curPage, returnVal, rhs, parent);
					}
					break;
				}
			} else {
				appendRecord (curPage, returnVal, rhs, parent);
				rhsLoaded = false;

				// deal with the ase where we have to append all of the right records to the output
				if (!rightIter->advance ()) {
					appendRecord (curPage, returnVal, lhs, parent);
					while (leftIter->advance ()) {
						leftIter->getCurrent (lhs);
						appendRecord (curPage, returnVal, lhs, parent);
					}
					break;
				}
			}
		}
	}
	
	// remember the current page
	returnVal.push_back (curPage);
	
	// outta here!
	return returnVal;
}
	
void sort (int page_num, MyDB_TableReaderWriter &table, MyDB_TableReaderWriter &sorted_table, 
	function <bool ()>comp, MyDB_RecordPtr lhs, MyDB_RecordPtr rhs) {

MyDB_BufferManagerPtr manager = table.getBufferMgr();
int runs;
int run_size = page_num;
int rest_page_num = table.getNumPages();
vector<MyDB_PageReaderWriter >::iterator it;
vector <MyDB_PageReaderWriter> pageOutput;

vector <MyDB_RecordIteratorAltPtr> phase_2;
cout<<"total "<<table.getNumPages ()<<" pages to be sorted"<<endl;

if( rest_page_num % page_num == 0)
{
	runs = rest_page_num/page_num;
	cout<<"total number of runs: "<<runs<<endl;
}	
else
{
	runs = (rest_page_num/page_num) + 1;
	cout<<"total number of runs: "<<runs<<endl;
}	

phase_2.clear();
// phase 1: sort each run, runs=6, page_num = 64, last_page_num = 24
for(int i=0; i < runs; i++)
{
	vector <MyDB_PageReaderWriter> sorted_pages;
	cout<<"rest_page_num: "<<rest_page_num<<"  run_size: "<<run_size<<endl;
	
	if(rest_page_num > page_num)
		run_size = page_num;
	else
		run_size = rest_page_num;
	rest_page_num = rest_page_num - run_size;

	// clear the vector and put the first sorted page in it
	sorted_pages.push_back( *table[i*page_num].sort(comp, lhs, rhs) );
	int j;
	for(j=1; j < run_size; j++)
	{
		// sort each page
		MyDB_PageReaderWriterPtr sorted = table[ i*page_num + j].sort(comp, lhs, rhs);
		// merge each page with the vector
		vector <MyDB_PageReaderWriter> temp;
		temp.push_back(*sorted);
		sorted_pages = mergeIntoList (manager, getIteratorAlt(sorted_pages), getIteratorAlt(temp), comp, lhs, rhs);
		
		//cout<<"sorted pages size: "<<sorted_pages.size()<<endl;
	}
	cout<<"sorted pages size: "<<sorted_pages.size()<<endl;
	cout<<"pages "<<i*page_num<<" to "<<i*page_num + j-1<<" are sorted\n"<<endl;
	
	// push the sorted vector's record iterator into phase_2 vector
	phase_2.push_back(getIteratorAlt(sorted_pages));
	//table.writeIntoTextFile();
}	
	cout<<"phase 2 vector size: "<<phase_2.size()<<endl;

	// phase 2: merge all the vectors into the file
	mergeIntoFile (sorted_table, phase_2, comp, lhs, rhs);

	return;
}

#endif
