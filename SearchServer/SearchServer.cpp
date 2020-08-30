/* SearchServer.cpp : https://www.coursera.org/ C++ Development Fundamentals: Red Belt, Week 6. Capstone Project
Programming Assignment: Search Engine
*/

#include "search_server.h"
#include "iterator_range.h"

#include <algorithm>
#include <iterator>
#include <functional>
#include <sstream>
#include <utility>
#include <numeric>

#include <iostream>
#include <ostream>
#include <thread>

using namespace std;

vector <string_view > SplitIntoWordsView(string_view str) {
    vector <string_view > result;
    while (true) {
        size_t space = str.find(' ');
        if (space == 0) {
            str.remove_prefix(1);
            if (empty(str)) {
                break;
            }
            continue;
        }
        result.push_back(str.substr(0, space));
        if (space == str.npos) {
            break;
        }
        else {
            str.remove_prefix(space + 1);
        }
    }
    return result;
}

void UpdateIndexSinglTread(istream& document_input, Synchronized<InvertedIndex>& index) {
    InvertedIndex new_index;
    for (string current_document; getline(document_input, current_document); ) {
        new_index.Add(move(current_document));
    }
    swap(index.GetAccess().ref_to_value, new_index);
}

SearchServer::SearchServer(istream& document_input) {
    UpdateIndexSinglTread(document_input, index);
}

void SearchServer::UpdateDocumentBase(istream& document_input) {
    futures.push_back(async( UpdateIndexSinglTread, 
        ref(document_input), 
        ref(index) ));
}

void AddQueriesSinglTread(istream& query_input, ostream& search_results_output, Synchronized<InvertedIndex>& index) {

    for (string current_query; getline(query_input, current_query); ) {
        auto words = SplitIntoWordsView(move(current_query));
        vector<pair<size_t, size_t>> search_results(index.GetAccess().ref_to_value.GetDocsSize()); 
        for (auto& word : words) {
            for (const auto& docid : index.GetAccess().ref_to_value.Lookup(word)) {
                search_results[docid.first].first = docid.first;
                search_results[docid.first].second += docid.second;
            }
        }
        partial_sort(
            begin(search_results), 
            begin(search_results) + min<size_t>(5, search_results.size()),
            end(search_results),
            [](pair<size_t, size_t> lhs, pair<size_t, size_t> rhs) {
                return make_pair(lhs.second, rhs.first) > make_pair(rhs.second, lhs.first);
            }
        );
        
        search_results_output << current_query << ':';
        for (auto [docid, hitcount] : Head(search_results, 5)) {
            if (hitcount > 0) {
                search_results_output << " {"
                    << "docid: " << docid << ", "
                    << "hitcount: " << hitcount << '}';
            }
        }
        search_results_output << "\n";
    }
}

void SearchServer::AddQueriesStream(istream& query_input, ostream& search_results_output) {
    if (index.GetAccess().ref_to_value.GetDocsSize() > 0) {
        futures.push_back(async(AddQueriesSinglTread, 
            ref(query_input), 
            ref(search_results_output), 
            ref(index)));
    }
}

void InvertedIndex::Add(string document) {
    docs.push_back(move(document));
    const size_t docid = docs.size() - 1;
    unordered_map<string_view, size_t> temp;
    for (auto& word : SplitIntoWordsView(docs.back())) {
        //++temp[word];
        ++temp[move(word)];
    }
    for (const auto& [key, value] : temp) {
        index[key].push_back({ docid,value });
    }
}

const vector<pair<size_t, size_t>>& InvertedIndex::Lookup(const string_view& word) const {
    if (auto it = index.find(word); it != index.end()) {
        return it->second;
    }
    else {
        return index.at("___INITIALIZATION___");
    }
}
