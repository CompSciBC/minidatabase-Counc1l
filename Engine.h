#ifndef ENGINE_H
#define ENGINE_H

#include <iostream>   
#include <vector>     
#include "BST.h"      
#include "Record.h"
//add header files as needed

using namespace std;

// Converts a string to lowercase (used for case-insensitive searches)
static inline string toLower(string s) {
    for (char &c : s) c = (char)tolower((unsigned char)c);
    return s;
}

// ================== Index Engine ==================
// Acts like a small "database engine" that manages records and two BST indexes:
// 1) idIndex: maps student_id → record index (unique key)
// 2) lastIndex: maps lowercase(last_name) → list of record indices (non-unique key)
struct Engine {
    vector<Record> heap;                  // the main data store (simulates a heap file)
    BST<int, int> idIndex;                // index by student ID
    BST<string, vector<int>> lastIndex;   // index by last name (can have duplicates)

    // Inserts a new record and updates both indexes.
    // Returns the record ID (RID) in the heap.
    int insertRecord(const Record &recIn) {
        Record rec = recIn;
        rec.deleted = false;
        heap.push_back(rec);
        int pos = (int)heap.size() - 1;

        // update idIndex to pos
        idIndex.insert(rec.id, pos);

        // update last name index
        string key = toLower(rec.last);
        vector<int> positions;

        if (lastIndex.find(key, positions)) {
            positions.push_back(pos);
            lastIndex.insert(key, positions);
        } else {
            positions = {pos};
            lastIndex.insert(key, positions);
        }

        return rec.id;
    }

    // Deletes a record logically (marks as deleted and updates indexes)
    // Returns true if deletion succeeded.
    bool deleteById(int id) {
        //TODO
    }

    // Finds a record by student ID.
    // Returns a pointer to the record, or nullptr if not found.
    // Outputs the number of comparisons made in the search.
    const Record *findById(int id, int &cmpOut) {
        //TODO    }

    // Returns all records with ID in the range [lo, hi].
    // Also reports the number of key comparisons performed.
    vector<const Record *> rangeById(int lo, int hi, int &cmpOut) {
        //TODO
    }

    // Returns all records whose last name begins with a given prefix.
    // Case-insensitive using lowercase comparison.
    vector<const Record *> prefixByLast(const string &prefix, int &cmpOut) {
        //TODO
    }
};

#endif
