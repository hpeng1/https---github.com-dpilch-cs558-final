
#ifndef BPLUS_TEST_H
#define BPLUS_TEST_H

#include "MyDB_AttType.h"  
#include "MyDB_BufferManager.h"
#include "MyDB_Catalog.h"  
#include "MyDB_Page.h"
#include "MyDB_PageReaderWriter.h"
#include "MyDB_Record.h"
#include "MyDB_Table.h"
#include "MyDB_TableReaderWriter.h"
#include "MyDB_BPlusTreeReaderWriter.h"
#include "MyDB_Schema.h"
#include "QUnit.h"
#include "Sorting.h"
#include "ScanJoin.h"
#include "Aggregate.h"
#include "BPlusSelection.h"
#include "RegularSelection.h"
#include "ScanJoin.h"
#include "SortMergeJoin.h"
#include <iostream>
#include <vector>
#include <utility>

using namespace std;

int main () {

	QUnit::UnitTest qunit(cerr, QUnit::verbose);

	{
		// create a catalog
		MyDB_CatalogPtr myCatalog = make_shared <MyDB_Catalog> ("catFile");

		// now make a schema
		MyDB_SchemaPtr mySchemaL = make_shared <MyDB_Schema> ();
		mySchemaL->appendAtt (make_pair ("l_suppkey", make_shared <MyDB_IntAttType> ()));
		mySchemaL->appendAtt (make_pair ("l_name", make_shared <MyDB_StringAttType> ()));
		mySchemaL->appendAtt (make_pair ("l_address", make_shared <MyDB_StringAttType> ()));
		mySchemaL->appendAtt (make_pair ("l_nationkey", make_shared <MyDB_IntAttType> ()));
		mySchemaL->appendAtt (make_pair ("l_phone", make_shared <MyDB_StringAttType> ()));
		mySchemaL->appendAtt (make_pair ("l_acctbal", make_shared <MyDB_DoubleAttType> ()));
		mySchemaL->appendAtt (make_pair ("l_comment", make_shared <MyDB_StringAttType> ()));
	
		// and a right schema
		MyDB_SchemaPtr mySchemaR = make_shared <MyDB_Schema> ();
		mySchemaR->appendAtt (make_pair ("r_suppkey", make_shared <MyDB_IntAttType> ()));
		mySchemaR->appendAtt (make_pair ("r_name", make_shared <MyDB_StringAttType> ()));
		mySchemaR->appendAtt (make_pair ("r_address", make_shared <MyDB_StringAttType> ()));
		mySchemaR->appendAtt (make_pair ("r_nationkey", make_shared <MyDB_IntAttType> ()));
		mySchemaR->appendAtt (make_pair ("r_phone", make_shared <MyDB_StringAttType> ()));
		mySchemaR->appendAtt (make_pair ("r_acctbal", make_shared <MyDB_DoubleAttType> ()));
		mySchemaR->appendAtt (make_pair ("r_comment", make_shared <MyDB_StringAttType> ()));

		MyDB_SchemaPtr mySchemaOut = make_shared <MyDB_Schema> ();
		mySchemaOut->appendAtt (make_pair ("l_name", make_shared <MyDB_StringAttType> ()));
		mySchemaOut->appendAtt (make_pair ("combined_comment", make_shared <MyDB_StringAttType> ()));

		// use the schema to create a table
		MyDB_TablePtr myTableLeft = make_shared <MyDB_Table> ("supplierLeft", "supplierLeft.bin", mySchemaL);
		MyDB_TablePtr myTableRight = make_shared <MyDB_Table> ("supplierRight", "supplierRight.bin", mySchemaR);
		MyDB_TablePtr myTableOut = make_shared <MyDB_Table> ("supplierOut", "supplierOut.bin", mySchemaOut);

		// get the tables
		MyDB_BufferManagerPtr myMgr = make_shared <MyDB_BufferManager> (131072, 128, "tempFile");
		MyDB_TableReaderWriterPtr supplierTableL = make_shared <MyDB_TableReaderWriter> (myTableLeft, myMgr);
		MyDB_TableReaderWriterPtr supplierTableR = make_shared <MyDB_TableReaderWriter> (myTableRight, myMgr);
		MyDB_TableReaderWriterPtr supplierTableOut = make_shared <MyDB_TableReaderWriter> (myTableOut, myMgr);

		// load up from a text file
		cout << "loading left\n";
		supplierTableL->loadFromTextFile ("supplier.tbl");
		cout << "loading right\n";
		supplierTableR->loadFromTextFile ("supplierBig.tbl");

		// This basically runs:
		//
		// SELECT supplierLeft.l_name, supplierLeft.l_comment + " " + supplierRight.r_comment
		// FROM supplierLeft, supplierRight
		// WHERE (supplierLeft.l_nationkey = 4 OR
		//        supplierLeft.l_nationkey = 3) AND
		//       (supplierRight.r_nationkey = 3) AND
		//       (supplierLeft.l_suppkey = supplierRight.r_suppkey) AND
		//       (supplierLeft.l_name = supplierRight.r_name)
		// 
		// It does this by hashing the smaller table (supplierLeft) on 
		// supplierLeft.l_suppkey and supplierLeft.l_name.  It then scans
		// supplierRight, probing the hash table for matches

		vector <pair <string, string>> hashAtts;
		hashAtts.push_back (make_pair (string ("[l_suppkey]"), string ("[r_suppkey]")));
		hashAtts.push_back (make_pair (string ("[l_name]"), string ("[r_name]")));

		vector <string> projections;
		projections.push_back ("[l_name]");
		projections.push_back ("+ (+ ([l_comment], string[ ]), [r_comment])");

		ScanJoin myOp (supplierTableL, supplierTableR, supplierTableOut, 
			"&& ( == ([l_suppkey], [r_suppkey]), == ([l_name], [r_name]))", projections, hashAtts,
			"|| ( == ([l_nationkey], int[3]), == ([l_nationkey], int[4]))",
			"== ([r_nationkey], int[3])");
			
		cout << "running join\n";
		myOp.run ();

                MyDB_RecordPtr temp = supplierTableOut->getEmptyRecord ();
                MyDB_RecordIteratorAltPtr myIter = supplierTableOut->getIteratorAlt ();

                while (myIter->advance ()) {
                        myIter->getCurrent (temp);
			cout << temp << "\n";
                }
	}
}

#endif
