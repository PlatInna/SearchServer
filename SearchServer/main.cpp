#include "search_server.h"
#include "parse.h"
#include "test_runner.h"
#include "profile.h"

//#include "..\..\test_runner.h"
//#include "..\..\profile.h"

#include <algorithm>
#include <iterator>
#include <map>
#include <vector>
#include <string>
#include <sstream>
#include <fstream>
#include <random>
#include <thread>
using namespace std;

void TestFunctionality(
  const vector<string>& docs,
  const vector<string>& queries,
  const vector<string>& expected
) {
  istringstream docs_input(Join('\n', docs));
  istringstream queries_input(Join('\n', queries));

  SearchServer srv;
  srv.UpdateDocumentBase(docs_input);
  ostringstream queries_output;
  srv.AddQueriesStream(queries_input, queries_output);

  const string result = queries_output.str();
  const auto lines = SplitBy(Strip(result), '\n');
  ASSERT_EQUAL(lines.size(), expected.size());
  for (size_t i = 0; i < lines.size(); ++i) {
    ASSERT_EQUAL(lines[i], expected[i]);
  }
}

void TestSerpFormat() {
  const vector<string> docs = {
    "london is the capital of great britain",
    "i am travelling down the river"
  };
  const vector<string> queries = {"london", "the"};
  const vector<string> expected = {
    "london: {docid: 0, hitcount: 1}",
    Join(' ', vector{
      "the:",
      "{docid: 0, hitcount: 1}",
      "{docid: 1, hitcount: 1}"
    })
  };

  TestFunctionality(docs, queries, expected);
}

void TestTop5() {
  const vector<string> docs = {
    "milk a",
    "milk b",
    "milk c",
    "milk d",
    "milk e",
    "milk f",
    "milk g",
    "water a",
    "water b",
    "fire and earth"
  };

  const vector<string> queries = {"milk", "water", "rock"};
  const vector<string> expected = {
    Join(' ', vector{
      "milk:",
      "{docid: 0, hitcount: 1}",
      "{docid: 1, hitcount: 1}",
      "{docid: 2, hitcount: 1}",
      "{docid: 3, hitcount: 1}",
      "{docid: 4, hitcount: 1}"
    }),
    Join(' ', vector{
      "water:",
      "{docid: 7, hitcount: 1}",
      "{docid: 8, hitcount: 1}",
    }),
    "rock:",
  };
  TestFunctionality(docs, queries, expected);
}

void TestHitcount() {
  const vector<string> docs = {
    "the river goes through the entire city there is a house near it",
    "the wall",
    "walle",
    "is is is is",
  };
  const vector<string> queries = {"the", "wall", "all", "is", "the is"};
  const vector<string> expected = {
    Join(' ', vector{
      "the:",
      "{docid: 0, hitcount: 2}",
      "{docid: 1, hitcount: 1}",
    }),
    "wall: {docid: 1, hitcount: 1}",
    "all:",
    Join(' ', vector{
      "is:",
      "{docid: 3, hitcount: 4}",
      "{docid: 0, hitcount: 1}",
    }),
    Join(' ', vector{
      "the is:",
      "{docid: 3, hitcount: 4}",
      "{docid: 0, hitcount: 3}",
      "{docid: 1, hitcount: 1}",
    }),
  };
  TestFunctionality(docs, queries, expected);
}

void TestRanking() {
  const vector<string> docs = {
    "london is the capital of great britain",
    "paris is the capital of france",
    "berlin is the capital of germany",
    "rome is the capital of italy",
    "madrid is the capital of spain",
    "lisboa is the capital of portugal",
    "bern is the capital of switzerland",
    "moscow is the capital of russia",
    "kiev is the capital of ukraine",
    "minsk is the capital of belarus",
    "astana is the capital of kazakhstan",
    "beijing is the capital of china",
    "tokyo is the capital of japan",
    "bangkok is the capital of thailand",
    "welcome to moscow the capital of russia the third rome",
    "amsterdam is the capital of netherlands",
    "helsinki is the capital of finland",
    "oslo is the capital of norway",
    "stockgolm is the capital of sweden",
    "riga is the capital of latvia",
    "tallin is the capital of estonia",
    "warsaw is the capital of poland",
  };

  const vector<string> queries = {"moscow is the capital of russia"};
  const vector<string> expected = {
    Join(' ', vector{
      "moscow is the capital of russia:",
      "{docid: 7, hitcount: 6}",
      "{docid: 14, hitcount: 6}",
      "{docid: 0, hitcount: 4}",
      "{docid: 1, hitcount: 4}",
      "{docid: 2, hitcount: 4}",
    })
  };
  TestFunctionality(docs, queries, expected);
}

void TestBasicSearch() {
  const vector<string> docs = {
    "we are ready to go",
    "come on everybody shake you hands",
    "i love this game",
    "just like exception safety is not about writing try catch everywhere in your code move semantics are not about typing double ampersand everywhere in your code",
    "daddy daddy daddy dad dad dad",
    "tell me the meaning of being lonely",
    "just keep track of it",
    "how hard could it be",
    "it is going to be legen wait for it dary legendary",
    "we dont need no education"
  };

  const vector<string> queries = {
    "we need some help",
    "it",
    "i love this game",
    "tell me why",
    "dislike",
    "about"
  };

  const vector<string> expected = {
    Join(' ', vector{
      "we need some help:",
      "{docid: 9, hitcount: 2}",
      "{docid: 0, hitcount: 1}"
    }),
    Join(' ', vector{
      "it:",
      "{docid: 8, hitcount: 2}",
      "{docid: 6, hitcount: 1}",
      "{docid: 7, hitcount: 1}",
    }),
    "i love this game: {docid: 2, hitcount: 4}",
    "tell me why: {docid: 5, hitcount: 2}",
    "dislike:",
    "about: {docid: 3, hitcount: 2}",
  };
  TestFunctionality(docs, queries, expected);
}
void TestSearchServer(vector<pair<istream, ostream*>> streams) {
    // IteratorRange Ч шаблон из задачи Paginator
    // random_time() Ч функци€, котора€ возвращает случайный
    // промежуток времени

    LOG_DURATION("Total");
    SearchServer srv(streams.front().first);
    for (auto& [input, output] :
        IteratorRange(begin(streams) + 1, end(streams))) {
        //this_thread::sleep_for(random_time());
        if (!output) {
            srv.UpdateDocumentBase(input);
        }
        else {
            srv.AddQueriesStream(input, *output);
        }
    }
}

int main() {
  TestRunner tr;
  RUN_TEST(tr, TestSerpFormat);
  RUN_TEST(tr, TestTop5);
  RUN_TEST(tr, TestHitcount);
  RUN_TEST(tr, TestRanking);
  RUN_TEST(tr, TestBasicSearch);
  
      //LOG_DURATION("SetDocumentBase");
      std::random_device rd;
      std::mt19937 gen(rd());
      char ch;
      string word;
      string doc;
      string querie;
      vector<string> words;
      vector<string> docs;
      vector<string> queries;
      queries.reserve(500000);
      docs.reserve(50000);
      words.reserve(15000);
      std::uniform_int_distribution<> words_qty_in_doc(1, 1000);
      std::uniform_int_distribution<> words_qty_in_queries(1, 10);
      std::uniform_int_distribution<> chars_qty_in_word(1, 100);
      std::uniform_int_distribution<> word_in_words(0, 14999);
      std::uniform_int_distribution<> symbol(97, 122);
      for (int w = 1; w <= 15000; ++w) {
          int j_cycle = chars_qty_in_word(gen);
          for (int j = 1; j <= j_cycle; ++j) {
              ch = (symbol(gen));
              word += ch;
          }
          words.push_back(word);
          word = "";
      }
      for (int c = 0; c < 200; ++c) { // формируем базу документов 
          int i_cycle = words_qty_in_doc(gen);
          for (int i = 1; i <= i_cycle; ++i) {
              doc += words[word_in_words(gen)];
              if (i != i_cycle) {
                  doc += " ";
              }
          }
          docs.push_back(doc);
          doc = "";
      }
      for (int q = 0; q < 5000; ++q) { // формируем базу запросов
          int i_cycle = words_qty_in_queries(gen);
          for (int i = 1; i <= i_cycle; ++i) {
              querie += words[word_in_words(gen)];
              if (i != i_cycle) {
                  querie += " ";
              }
          }
          queries.push_back(querie);
          querie = "";
      }

      cout << "SetDocumentBase OK" << endl;
  

    istringstream docs_input(Join('\n', docs));
    SearchServer srv;
    {
        LOG_DURATION("UpdateDocumentBase");
        srv.UpdateDocumentBase(docs_input);
    }

    istringstream queries_input(Join('\n', queries));
    ostringstream queries_output;
    {
        LOG_DURATION("AddQueriesStream");
        srv.AddQueriesStream(queries_input, queries_output);
    }
    
  //istringstream docs_input("x x x y y y\nx y z z z\ny y z z\nx x y y y y\nx y z\nx x x x\nx\ny\nz\nx y\nx z\nx y z\n");
  //SearchServer srv1;
  //srv1.UpdateDocumentBase(docs_input);
  //istringstream queries_input("x\ny\nz\nx y\nx z\nx y z\n");
  //srv1.AddQueriesStream(queries_input, cout);
  //*/
    //srv.~SearchServer();
    return 0;

}
