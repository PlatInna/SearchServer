/* SearchServer.cpp : https://www.coursera.org/ C++ Development Fundamentals: Red Belt, Week 6. Capstone Project
Programming Assignment: Search Engine
*/

/* SearchServer.cpp : https://www.coursera.org/ Основы разработки на C++: красный пояс, Неделя 6. Финальная задача
Задание по программированию: Поисковая система
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
//#include <chrono>
//#include <cctype>

using namespace std;
//using namespace std::chrono;

//vector<future<void>> futures;

//vector<string> SplitIntoWords(const string& line) {
//    istringstream words_input(line);
//    return { istream_iterator<string>(words_input), istream_iterator<string>() };
//}

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


//void SearchServer::UpdateIndexSinglTread(istream& document_input) {
void UpdateIndexSinglTread(istream& document_input, Synchronized<InvertedIndex>& index) {
    InvertedIndex new_index;
    for (string current_document; getline(document_input, current_document); ) {
        new_index.Add(move(current_document));
    }
    swap(index.GetAccess().ref_to_value, new_index);
    //index.GetAccess().ref_to_value.swap(new_index);
    //index.GetAccess().ref_to_value = move(new_index);
}

SearchServer::SearchServer(istream& document_input) {
    //UpdateDocumentBase(document_input);
    UpdateIndexSinglTread(document_input, index);
}

void SearchServer::UpdateDocumentBase(istream& document_input) {
    //vector<future<void>> futures;

    futures.push_back(async( UpdateIndexSinglTread, 
        ref(document_input), 
        ref(index) ));

    //futures.push_back(async(&SearchServer::UpdateIndexSinglTread, this, ref(document_input)));
    //UpdateIndexSinglTread(document_input);
    //pdateIndexSinglTread(document_input, index);

    //InvertedIndex new_index;
    //for (string current_document; getline(document_input, current_document); ) {
    //    new_index.Add(move(current_document));
    //}
    //swap(index.GetAccess().ref_to_value, new_index);
    //index.GetAccess().ref_to_value = move(new_index);

    //index = move(new_index); // последовательно без обертки Synchronized<InvertedIndex>;
}

//void SearchServer::AddQueriesSinglTread(istream& query_input, ostream& search_results_output) {
void AddQueriesSinglTread(istream& query_input, ostream& search_results_output, Synchronized<InvertedIndex>& index) {

    for (string current_query; getline(query_input, current_query); ) {
        //const auto words = SplitIntoWords(current_query);
        auto words = SplitIntoWordsView(move(current_query));
        vector<pair<size_t, size_t>> search_results(index.GetAccess().ref_to_value.GetDocsSize()); 
        //for (const auto& word : words) {
        for (auto& word : words) {
            for (const auto& docid : index.GetAccess().ref_to_value.Lookup(word)) {
                search_results[docid.first].first = docid.first;
                search_results[docid.first].second += docid.second;
            }
        }

        //int top_search = 5;
        //if (search_results.size() < 5) { top_search = search_results.size(); }
        partial_sort(
            //begin(search_results), begin(search_results) + top_search,
            begin(search_results), 
            begin(search_results) + min<size_t>(5, search_results.size()),
            end(search_results),
            [](pair<size_t, size_t> lhs, pair<size_t, size_t> rhs) {
                //int64_t lhs_docid = lhs.first;
                //auto lhs_hit_count = lhs.second;
                //int64_t rhs_docid = rhs.first;
                //auto rhs_hit_count = rhs.second;
                //return make_pair(lhs_hit_count, -lhs_docid) > make_pair(rhs_hit_count, -rhs_docid);
                //return make_pair(lhs.second, -(lhs.first)) > make_pair(rhs.second, -(rhs.first));
                return make_pair(lhs.second, rhs.first) > make_pair(rhs.second, lhs.first);
            }
        );
        
        /*
        vector<size_t> docid_count(index.GetAccess().ref_to_value.GetDocsSize());
        vector<size_t> search_results(index.GetAccess().ref_to_value.GetDocsSize());
        iota(search_results.begin(), search_results.end(), 0);

        for (const auto& word : SplitIntoWordsView(current_query)) {
            for (const auto& docid : index.GetAccess().ref_to_value.Lookup(word)) {
                docid_count[docid.first] += docid.second;
            }
        }

        partial_sort(
            search_results.begin(),
            search_results.begin() + min<size_t>(5, index.GetAccess().ref_to_value.GetDocsSize()),
            search_results.end(),
            [&docid_count](int lhs, int rhs) {
                return make_pair(docid_count[lhs], -lhs) > make_pair(docid_count[rhs], -rhs);
            }
        );
        */

        search_results_output << current_query << ':';
        for (auto [docid, hitcount] : Head(search_results, 5)) {
            if (hitcount > 0) {
                search_results_output << " {"
                    << "docid: " << docid << ", "
                    << "hitcount: " << hitcount << '}';
            }
        }

        //for (auto& docid : Head(search_results, 5)) {
        //    if (docid_count[docid] > 0) {
        //        search_results_output << " {"
        //            << "docid: " << docid << ", "
        //            << "hitcount: " << docid_count[docid] << '}';
        //    }
        //}

        //search_results_output << endl;
        search_results_output << "\n";
        //search_results.clear();
    }
}

void SearchServer::AddQueriesStream(istream& query_input, ostream& search_results_output) {
    //vector<future<void>> futures;
    if (index.GetAccess().ref_to_value.GetDocsSize() > 0) {
        futures.push_back(async(AddQueriesSinglTread, 
            ref(query_input), 
            ref(search_results_output), 
            ref(index)));
        //futures.push_back(async([this, &query_input, &search_results_output] { AddQueriesSinglTread(query_input, search_results_output); }));
        //AddQueriesSinglTread(query_input, search_results_output);
        //AddQueriesSinglTread(query_input, search_results_output, index);
        //this_thread::sleep_for(1ms);
    }
}

//void InvertedIndex::Add(const string& document) {
//void InvertedIndex::Add(string & document) {
void InvertedIndex::Add(string document) {
    //docs.push_back(document);
    docs.push_back(move(document));
    const size_t docid = docs.size() - 1;
    unordered_map<string_view, size_t> temp;
    //map<string, size_t> temp;
    //for (const auto& word : SplitIntoWords(document)) {
    //for ( const auto& word : SplitIntoWordsView(document)) {
    for (auto& word : SplitIntoWordsView(docs.back())) {
        //++temp[word];
        ++temp[move(word)];
    }
    for (const auto& [key, value] : temp) {
        index[key].push_back({ docid,value });
    }
}

//vector<pair<size_t, size_t>> InvertedIndex::Lookup(const string_view& word) const {
const vector<pair<size_t, size_t>>& InvertedIndex::Lookup(const string_view& word) const {
    if (auto it = index.find(word); it != index.end()) {
        return it->second;
    }
    else {
        //return {};
        return index.at("___INITIALIZATION___");
    }
}
