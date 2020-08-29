//vector<size_t> docid_count_v;
    //docid_count_v.resize(50000, 0);
    //docid_count_v.reserve(50000);
    //vector<pair<size_t, size_t>> search_results;
    //search_results.resize(50000, { 0, 0 });
    //vector<pair<size_t, size_t>> search_results(index.GetIndexSize());
    //vector<pair<size_t, size_t>> docid_count_v(50000, { 0, 0 });
    //auto finish = steady_clock::now();
    //auto start = steady_clock::now();
    //auto dur_split = std::chrono::duration_cast<std::chrono::milliseconds>(finish - finish).count();
    //auto dur_lookup = std::chrono::duration_cast<std::chrono::milliseconds>(finish - finish).count();
    //auto dur_lookup_map = std::chrono::duration_cast<std::chrono::milliseconds>(finish - finish).count();
    //auto dur_lookup_vector = std::chrono::duration_cast<std::chrono::milliseconds>(finish - finish).count();
    //auto dur_sort = std::chrono::duration_cast<std::chrono::milliseconds>(finish - finish).count();
    //auto dur_out = std::chrono::duration_cast<std::chrono::milliseconds>(finish - finish).count();
//start = steady_clock::now();
//finish = steady_clock::now();
        //dur_split += std::chrono::duration_cast<std::chrono::milliseconds>(finish - start).count();

        //start = steady_clock::now();

        //map<size_t, size_t> docid_count;
        //for (const auto& word : words) {
        //    for (const size_t docid : index.Lookup(word)) {
        //        docid_count[docid]++;
        //    }
        //}
        //vector<pair<size_t, size_t>> search_results(
        //    docid_count.begin(), docid_count.end()
        //);

        //finish = steady_clock::now();
        //dur_lookup_map += std::chrono::duration_cast<std::chrono::milliseconds>(finish - start).count();

        //start = steady_clock::now();
 //vector<pair<size_t, size_t>> search_results_v;
        //for (size_t j = 0; j < docid_count.size(); ++j) {
        //    if (docid_count[j] != 0) {
        //        search_results_v.push_back({ j, docid_count[j] });
        //    }
        //}
        //finish = steady_clock::now();
        //dur_lookup_vector += std::chrono::duration_cast<std::chrono::milliseconds>(finish - start).count();

        //finish = steady_clock::now();
        //dur_lookup += std::chrono::duration_cast<std::chrono::milliseconds>(finish - start_lookup).count();

        //start = steady_clock::now();
        //finish = steady_clock::now();
        //dur_sort += std::chrono::duration_cast<std::chrono::milliseconds>(finish - start).count();

        //start = steady_clock::now();
        //finish = steady_clock::now();
        //dur_out += std::chrono::duration_cast<std::chrono::milliseconds>(finish - start).count();
    //std::cout << "SplitIntoWords: " << dur_split << endl;
    //std::cout << "index.Lookup_map: " << dur_lookup_map << endl;
    //std::cout << "index.Lookup_vector: " << dur_lookup_vector << endl;
    //std::cout << "index.Lookup: " << dur_lookup << endl;
    //std::cout << "sort: " << dur_sort << endl;
    //std::cout << "out: " << dur_out << endl;