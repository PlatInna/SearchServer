"Red Belt" Capstone Project Instructions

It would be strange teaching you on Yandex course and do not ask to develop your own search engine.
In the final project of the "Red Belt" we will do just that :)
In this task, you need to work with the SearchServer class, which allows you to search in the document database:

class SearchServer {
public:
  SearchServer() = default;
  explicit SearchServer(istream& document_input);

  void UpdateDocumentBase(istream& document_input);
  void AddQueriesStream(istream& query_input, ostream& search_results_output);
};

Let's consider its interface.
Constructor
The constructor for the SearchServer class accepts an input stream containing a document database. 
Wherein:
  one document is one line of the input stream;
  documents consist of words separated by one or more spaces;
  words are composed of lowercase Latin letters. For example, the code below loads a database of three documents into an object of the SearchServer class:

string docs;
docs += "london is the capital of great britain\n"; 
    // the document contains 7 words
docs += "i am travelling down the river\n";
    // the document contains 6 words
docs += "  here     we    go             ";
    // the document contains 3 words
istringstream document_input(docs);
SearchServer srv(document_input);

Method AddQueriesStream(istream& query_input, ostream& search_results_output)
The method AddQueriesStream does the actual search. It accepts an input stream for search queries and an output stream for writing search results. 
Wherein:
  one request is one line in the query_input stream
  each search query consists of words separated by one or more spaces
  just like in documents, words in queries consist of lowercase Latin letters
The result of processing a search query is a set of up to five most relevant documents. In real search engines, relevance metric is pretty complex.
As part of our task, we will use the total number of occurrences of all query words in the document as a relevance metric.
For example, let's say we have a search database of three documents: "london is the capital of great britain", "moscow is the capital of the russian federation", 
"paris is the capital of france" and the search query "the best capital". Then the relevance metric for our documents will be as follows:
  london is the capital of great britain - 2 (the word "the" appears in the document 1 time, the word "best" - never, the word "capital" - 1 time)
  moscow is the capital of the russian federation — 3 (the word "the" appears in the document 2 times, the word "best" - never, the word "capital" - 1 time)
  paris is the capital of france — 2 ("the" — 1, "best" — 0, "capital" — 1)
As a result, it turns out that the document "moscow is the capital of the russian federation" appears to be the most relevant to the query "the best capital".
For each search query, the AddQueriesStream method should output one line to the search_results_output stream in the format [query text]: 
{docid: <value>, hitcount: <value>} {docid: <value>, hitcount: <value>} ..., where docid is the document identifier (see below), and hitcount is the relevance 
metric value for this document (that is, the total number of occurrences of all query words in this document).
Two important notes:
  There is no need to add documents with zero hitcount to search results
  when calculating hitcount, only whole words need to be taken into account, that is, the word "there" is not an occurrence of the word "the"
Method UpdateDocumentBase(istream& document_input)
The method UpdateDocumentBase replaces the current document base with the new one contained in the document_input stream.
In this case, the document from the first line of this stream will have an identifier (docid) 0, a document from the second line - identifier 1, etc.
Similarly, the constructor of the SearchServer class should assign IDs to documents. For example the code

const string doc1 = "london is the capital of great britain";
const string doc2 = "moscow is the capital of the russian federation";
istringstream doc_input1(doc1 + '\n' + doc2);
SearchServer srv(doc_input1);

const string query = "the capital";
istringstream query_input1(query);
srv.AddQueriesStream(query_input1, cout);

istringstream doc_input2(doc2 + '\n' + doc1);
srv.UpdateDocumentBase(doc_input2);
istringstream query_input2(query);
srv.AddQueriesStream(query_input2, cout);

should output
  
the capital: {docid: 1, hitcount: 3} {docid: 0, hitcount: 2}
the capital: {docid: 0, hitcount: 3} {docid: 1, hitcount: 2}

The first part:
This task has two parts. In the first part, you are given the correct implementation of the SearchServer class, which is not fast enough. You need to find and 
eliminate bottlenecks in its implementation. You need to submit a cpp file or an archive of several source files to the testing system, containing your accelerated 
implementation.
Implementation testing will be done for performance and integrity.
When testing for performance, your implementation will be tested with the following function (an object of the SearchServer class will be created once, and the 
AddQueriesStream method will be called on it once):

void TestSearchServer(istream& document_input, istream& query_input,
                      ostream& search_results_output) {
  SearchServer srv(document_input);
  srv.AddQueriesStream(query_input, search_results_output);
}

Wherein:
  document_input contains no more than 50,000 documents
  each document contains no more than 1000 words
  the total number of different words in all documents does not exceed 15,000
  the maximum length of one word is 100 characters, words consist of lowercase Latin letters and are separated by one or more spaces
  query_input contains no more than 500,000 queries, each query contains from 1 to 10 words.

Unlike performance testing, when testing for the integrity and correctness of the original server functionality, it is allowed to call UpdateDocumentBase, 
including from an already created and initialized object of the SearchServer class.
See the unit tests in the solution template for a more detailed explanation of how the SearchServer class should work.
course_project_first_part.zip
Second part of the task
In this part, we will simulate the operation of a web server that simultaneously processes several streams of search queries, as well as updates to the document base. 
Your implementation will be tested like this:

void TestSearchServer(vector<pair<istream, ostream*>> streams) {
  // IteratorRange — template from Paginator task
  // random_time() — a function that returns a random amount of time
  LOG_DURATION("Total");
  SearchServer srv(streams.front().first);
  for (auto& [input, output] :
       IteratorRange(begin(streams) + 1, end(streams))) {
    this_thread::sleep_for(random_time());
    if (!output) {
      srv.UpdateDocumentBase(input);
    } else {
      srv.AddQueriesStream(input, *output);
    }
  }
}

As you can see, using the LOG_DURATION macro, we measure the total running time of the testing function. Your implementation should be faster than simply 
sequentially applying the UpdateDocumentBase and AddQueriesStream methods.
In practice, such a web server processes search queries most of the time. The document base is updated not very often with a certain frequency, for example, 
once a day. Updating the database can take a significant amount of time, so the server should not stop processing requests while it is running. Your solution 
to the second part of the problem should take this into account.
  The method AddQueriesStream must be prepared to execute in parallel with the method UpdateDocumentBase, and vice versa.
  The AddQueriesStream method is not required to process all queries with the version of the document database that was current when it was run.
  That is, if the document database has been updated while the AddQueriesStream method is running, it can use the new version of the database to process the remaining requests.
