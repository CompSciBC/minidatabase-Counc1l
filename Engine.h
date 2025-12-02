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

        // insert into id index
        idIndex.insert(rec.id, pos);

        // insert into last name index (lowercase)
        string lname = toLower(rec.last);
        vector<int>* vecPtr = lastIndex.find(lname);
        if (vecPtr) {
            vecPtr->push_back(pos);
        } else {
            lastIndex.insert(lname, vector<int>{pos});
        }

        return rec.id;
    }

    // Deletes a record logically (marks as deleted and updates indexes)
    // Returns true if deletion succeeded.
    bool deleteById(int id) {
        // lookup id
        int* posPtr = idIndex.find(id);
        if (posPtr == nullptr) return false;

        int pos = *posPtr;
        if (pos < 0 || pos >= (int)heap.size()) return false;
        if (heap[pos].deleted) return false;

        // mark deleted
        heap[pos].deleted = true;

        // remove id from idIndex
        idIndex.erase(id);

        // remove position from lastIndex vector; if vector becomes empty, erase the key
        string lname = toLower(heap[pos].last);
        vector<int>* vecLast = lastIndex.find(lname);
        if (vecLast) {
            // remove all occurrences of pos (should be at most one)
            vecLast->erase(std::remove(vecLast->begin(), vecLast->end(), pos), vecLast->end());
            if (vecLast->empty()) {
                lastIndex.erase(lname);
            }
            // else leave updated vector in place
        }

        return true;
    }

    // Finds a record by student ID.
    // Returns a pointer to the record, or nullptr if not found.
    // Outputs the number of comparisons made in the search.
    const Record *findById(int id, int &cmpOut) {
        // lookup id
        int* posPtr = idIndex.find(id);
        if (posPtr == nullptr) return false;

        int pos = *posPtr;
        if (pos < 0 || pos >= (int)heap.size()) return false;
        if (heap[pos].deleted) return false;

        // mark deleted
        heap[pos].deleted = true;

        // remove id from idIndex
        idIndex.erase(id);

        // remove position from lastIndex vector; if vector becomes empty, erase the key
        string lname = toLower(heap[pos].last);
        vector<int>* vecLast = lastIndex.find(lname);
        if (vecLast) {
            // remove all occurrences of pos (should be at most one)
            vecLast->erase(std::remove(vecLast->begin(), vecLast->end(), pos), vecLast->end());
            if (vecLast->empty()) {
                lastIndex.erase(lname);
            }
            // else leave updated vector in place
        }

        return true;
    }

    // Returns all records with ID in the range [lo, hi].
    // Also reports the number of key comparisons performed.
    vector<const Record*> rangeById(int lo, int hi, int &cmpOut) {
        // reset metrics, perform find, read comparisons
        idIndex.resetMetrics();
        int* posPtr = idIndex.find(id);
        cmpOut = idIndex.comparisons;

        if (posPtr == nullptr) return nullptr;
        int pos = *posPtr;
        if (pos < 0 || pos >= (int)heap.size()) return nullptr;
        if (heap[pos].deleted) return nullptr;
        return &heap[pos];
    }

    // Returns all records with ID in the range [lo, hi].
    // Also reports the number of key comparisons performed.
    vector<const Record*> rangeById(int lo, int hi, int &cmpOut) {
        vector<const Record*> out;
        idIndex.resetMetrics();

        idIndex.rangeApply(lo, hi,
            [&](const int &k, int &rid) {
                // Each visited node may have contributed to comparisons inside BST
                if (rid >= 0 && rid < (int)heap.size() && !heap[rid].deleted) {
                    out.push_back(&heap[rid]);
                }
            }
        );

        cmpOut = idIndex.comparisons;
        return out;
    }

    // Returns all records whose last name begins with a given prefix.
    // Case-insensitive using lowercase comparison.
    vector<const Record*> prefixByLast(const string &prefix, int &cmpOut) {
        vector<const Record*> out;
        string low = toLower(prefix);
        string high = low;
        high.push_back(char(0xFF)); // upper bound for prefix range

        lastIndex.resetMetrics();

        lastIndex.rangeApply(low, high,
            [&](const string &lname, vector<int> &positions) {
                // ensure key actually starts with prefix (defensive)
                if (lname.rfind(low, 0) != 0) return;

                for (int pos : positions) {
                    if (pos >= 0 && pos < (int)heap.size() && !heap[pos].deleted) {
                        out.push_back(&heap[pos]);
                    }
                }
            }
        );

        cmpOut = lastIndex.comparisons;
        return out;
    }
};

#endif
