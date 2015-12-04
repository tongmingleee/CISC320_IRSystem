
#ifndef IRSystem_HH
#define IRSystem_HH

#include <stdio.h>
#include <iostream>
#include <vector>
#include <string>
#include <string.h>
#include <set>
#include <unordered_set>
#include <unordered_map>
#include <algorithm>
#include <ctype.h>
#include <math.h>

using namespace std;

#define VNAME(x) #x //macro that convert variable name to string
#define WHITESPACE " "
#define TRUE 1
#define FALSE 0
//-------------Stemming---------------------------------------------------
static char *b;       /* buffer for word to be stemmed */
static int k, k0, j;   /* j is a general offset into the string */

/* cons(i) is TRUE <=> b[i] is a consonant. */

int cons(int i)
{ switch (b[i])
  {
  case 'a': case 'e': case 'i': case 'o': case 'u': return FALSE;
  case 'y': return (i == k0) ? TRUE : !cons(i - 1);
  default: return TRUE;
  }
}

int m()
{ int n = 0;
  int i = k0;
  while (TRUE)
  { if (i > j) return n;
    if (! cons(i)) break; i++;
  }
  i++;
  while (TRUE)
  { while (TRUE)
    { if (i > j) return n;
      if (cons(i)) break;
      i++;
    }
    i++;
    n++;
    while (TRUE)
    { if (i > j) return n;
      if (! cons(i)) break;
      i++;
    }
    i++;
  }
}

int vowelinstem()
{ int i; for (i = k0; i <= j; i++) if (! cons(i)) return TRUE;
  return FALSE;
}

int doublec(int j)
{ if (j < k0 + 1) return FALSE;
  if (b[j] != b[j - 1]) return FALSE;
  return cons(j);
}


int cvc(int i)
{ if (i < k0 + 2 || !cons(i) || cons(i - 1) || !cons(i - 2)) return FALSE;
  { int ch = b[i];
    if (ch == 'w' || ch == 'x' || ch == 'y') return FALSE;
  }
  return TRUE;
}

int ends(char * s)
{ int length = s[0];
  if (s[length] != b[k]) return FALSE; /* tiny speed-up */
  if (length > k - k0 + 1) return FALSE;
  if (memcmp(b + k - length + 1, s + 1, length) != 0) return FALSE;
  j = k - length;
  return TRUE;
}

void setto(char * s)
{ int length = s[0];
  memmove(b + j + 1, s + 1, length);
  k = j + length;
}

void r(char * s) { if (m() > 0) setto(s); }

void step1ab()
{ if (b[k] == 's')
  { if (ends("\04" "sses")) k -= 2; else if (ends("\03" "ies")) setto("\01" "i"); else if (b[k - 1] != 's') k--;
  }
  if (ends("\03" "eed")) { if (m() > 0) k--; } else if ((ends("\02" "ed") || ends("\03" "ing")) && vowelinstem())
  { k = j;
    if (ends("\02" "at")) setto("\03" "ate"); else if (ends("\02" "bl")) setto("\03" "ble"); else if (ends("\02" "iz")) setto("\03" "ize"); else if (doublec(k))
    { k--;
      { int ch = b[k];
        if (ch == 'l' || ch == 's' || ch == 'z') k++;
      }
    }
    else if (m() == 1 && cvc(k)) setto("\01" "e");
  }
}

void step1c() { if (ends("\01" "y") && vowelinstem()) b[k] = 'i'; }

void step2() {
  switch (b[k - 1])
  {
  case 'a': if (ends("\07" "ational")) { r("\03" "ate"); break; }
    if (ends("\06" "tional")) { r("\04" "tion"); break; }
    break;
  case 'c': if (ends("\04" "enci")) { r("\04" "ence"); break; }
    if (ends("\04" "anci")) { r("\04" "ance"); break; }
    break;
  case 'e': if (ends("\04" "izer")) { r("\03" "ize"); break; }
    break;
  case 'l': if (ends("\03" "bli")) { r("\03" "ble"); break; } /*-DEPARTURE-*/
    if (ends("\04" "alli")) { r("\02" "al"); break; }
    if (ends("\05" "entli")) { r("\03" "ent"); break; }
    if (ends("\03" "eli")) { r("\01" "e"); break; }
    if (ends("\05" "ousli")) { r("\03" "ous"); break; }
    break;
  case 'o': if (ends("\07" "ization")) { r("\03" "ize"); break; }
    if (ends("\05" "ation")) { r("\03" "ate"); break; }
    if (ends("\04" "ator")) { r("\03" "ate"); break; }
    break;
  case 's': if (ends("\05" "alism")) { r("\02" "al"); break; }
    if (ends("\07" "iveness")) { r("\03" "ive"); break; }
    if (ends("\07" "fulness")) { r("\03" "ful"); break; }
    if (ends("\07" "ousness")) { r("\03" "ous"); break; }
    break;
  case 't': if (ends("\05" "aliti")) { r("\02" "al"); break; }
    if (ends("\05" "iviti")) { r("\03" "ive"); break; }
    if (ends("\06" "biliti")) { r("\03" "ble"); break; }
    break;
  case 'g': if (ends("\04" "logi")) { r("\03" "log"); break; } /*-DEPARTURE-*/

  }
}

void step3() {
  switch (b[k])
  {
  case 'e': if (ends("\05" "icate")) { r("\02" "ic"); break; }
    if (ends("\05" "ative")) { r("\00" ""); break; }
    if (ends("\05" "alize")) { r("\02" "al"); break; }
    break;
  case 'i': if (ends("\05" "iciti")) { r("\02" "ic"); break; }
    break;
  case 'l': if (ends("\04" "ical")) { r("\02" "ic"); break; }
    if (ends("\03" "ful")) { r("\00" ""); break; }
    break;
  case 's': if (ends("\04" "ness")) { r("\00" ""); break; }
    break;
  }
}

/* step4() takes off -ant, -ence etc., in context <c>vcvc<v>. */

void step4()
{ switch (b[k - 1])
  {
  case 'a': if (ends("\02" "al")) break; return;
  case 'c': if (ends("\04" "ance")) break;
    if (ends("\04" "ence")) break; return;
  case 'e': if (ends("\02" "er")) break; return;
  case 'i': if (ends("\02" "ic")) break; return;
  case 'l': if (ends("\04" "able")) break;
    if (ends("\04" "ible")) break; return;
  case 'n': if (ends("\03" "ant")) break;
    if (ends("\05" "ement")) break;
    if (ends("\04" "ment")) break;
    if (ends("\03" "ent")) break; return;
  case 'o': if (ends("\03" "ion") && (b[j] == 's' || b[j] == 't')) break;
    if (ends("\02" "ou")) break; return;
  /* takes care of -ous */
  case 's': if (ends("\03" "ism")) break; return;
  case 't': if (ends("\03" "ate")) break;
    if (ends("\03" "iti")) break; return;
  case 'u': if (ends("\03" "ous")) break; return;
  case 'v': if (ends("\03" "ive")) break; return;
  case 'z': if (ends("\03" "ize")) break; return;
  default: return;
  }
  if (m() > 1) k = j;
}

/* step5() removes a final -e if m() > 1, and changes -ll to -l if
   m() > 1. */

void step5()
{ j = k;
  if (b[k] == 'e')
  { int a = m();
    if (a > 1 || a == 1 && !cvc(k - 1)) k--;
  }
  if (b[k] == 'l' && doublec(k) && m() > 1) k--;
}

int stem(char * p, int i, int j)
{ b = p; k = j; k0 = i; /* copy the parameters into statics */
  if (k <= k0 + 1) return k; /*-DEPARTURE-*/

  step1ab(); step1c(); step2(); step3(); step4(); step5();
  return k;
}

/*--------------------stemmer definition ends here------------------------*/

static char * s;         /* a char * (=string) pointer; passed into b above */

#define INC 50           /* size units in which s is increased */
static int i_max = INC;  /* maximum offset in s */

void increase_s()
{ i_max += INC;
  { char * new_s = (char *) malloc(i_max + 1);
    { int i; for (i = 0; i < i_max; i++) new_s[i] = s[i]; } /* copy across */
    free(s); s = new_s;
  }
}

#define UC(ch) (ch <= 'Z' && ch >= 'A')
#define LC(ch) (ch <= 'z' && ch >= 'a')
#define LETTER(ch) (UC(ch) || LC(ch))
#define FORCELC(ch) (ch-('A'-'a'))

void stemfile(FILE * f)
{ while (TRUE)
  { int ch = getc(f);
    if (ch == EOF) return;
    if (LETTER(ch))
    { int i = 0;
      while (TRUE)
      { if (i == i_max) increase_s();

        if UC(ch) ch = FORCELC(ch);
        /* forces lower case. Remove this line to make the program work
           exactly like the Muscat stemtext command. */

        s[i] = ch; i++;
        ch = getc(f);
        if (!LETTER(ch)) { ungetc(ch, f); break; }
      }
      s[stem(s, 0, i - 1) + 1] = 0;
      /* the pevious line calls the stemmer and uses its result to
         zero-terminate the string in s */
      printf("%s", s);
    }
    else putchar(ch);
  }
}



class IRSystem
{
public:
  IRSystem(){};


  //methods
  bool isStopWord(string s);  //check if string s is stop word
  void initialize();  //initialize IRSystem: build set that have all stop words and index
  string stemming(string);  //function that stem string and return stemmed string
  string toLowerCase(string); //convert string to all lower cast and return
  vector<string> stringClean(string); //clean a sentence, and make it in to vector of words
  void makeFrequencyMatrix();

  vector<int> search(string);
  vector<int> queryResult(vector<int>);
  void printQueryResult(string);//clean query and return documents that contains words in query

  //insert methods
  void insert(vector<string>, int);
  void insertWordList(string);
  void insertMap(string);
  void insertMapSecond(string, int);

  //getter
  vector<string> getStopWordsList() {return stopwordsList;}
  vector<string> getWordList() {return wordList;}
  vector<string> getDocuments() {return documents;}
  vector<string> getDocumentsName() {return documentsName;}
  unordered_set<string> getStopWordsSet() {return stopWordSet;}
  unordered_map<string, unordered_set<int> > getMap() {return map;}
  vector<vector<int> > getFrequency() {return frequencyMatrix;}
  set<string> getWordSet() {return wordSet;}
  vector<vector<string> > getCleanWordMatrix() {return cleanWordMatrix;}
  unordered_map<string, int> getWordIndexLookUpMap() {return wordIndexLookUpMap;}

  // building methods
  void addDocumentName(string s){documentsName.push_back(s);}
  void addDocument(string s){documents.push_back(s);}
  void addStopWord(string s){stopwordsList.push_back(s);}


private:

  unordered_set<string> stopWordSet;  //hashset used to store all stop words

  unordered_map<string, unordered_set<int> > map; //map used for indexing. It store stemmed word and vector of document index pairs
  //vector<unordered_map<string, int> > frequency;  //vector that store frequency. First item in frequency corrensponding to first document in 'documents'
  vector< vector<int> > frequencyMatrix; // declare 2D vector; will be initialized later
  unordered_map<string,int> wordIndexLookUpMap;

  vector<string> wordList; //list of words after stringclean. all valid words no duplicate
  set<string> wordSet;   //set of word from wordList, purpose is remove duplicate;
  vector< vector<string> > cleanWordMatrix; // clean word, separated by documents

  //documents
  string txt01 = " Hello the world ! It is a great day. ";
  string txt02 = " The world is becoming a place full of greater dangers. What a great world! ";
  string txt03 = "The world is a great place. ";
  vector<string> documents = {txt01, txt02, txt03}; //store documents
  vector<string> documentsName = {VNAME(txt01), VNAME(txt02), VNAME(txt03)};  //store name of documents, same order as documents
  //all stopwords
  vector<string> stopwordsList;
 /* vector<string> stopwordsList = {"a", "about",  "above",  "across", "after",  "afterwards", "again",  "against",  "all",  "almost", "alone",  "along",  "already",  "also", "although", "always", "am", "among",  "amongst",  "amoungst", "amount", "an", "and",  "another",  "any",  "anyhow", "anyone", "anything", "anyway", "anywhere", "are",  "around", "as", "at", "back", "be", "became", "because",  "become", "becomes",  "becoming", "been", "before", "beforehand",
                                  "behind", "being",  "below",  "beside", "besides",  "between",  "beyond", "bill", "both", "bottom", "but",  "by", "call", "can",  "cannot", "cant", "co", "computer", "con",  "could",  "couldnt",  "cry",  "de", "describe", "detail", "do", "done", "down", "due",  "during", "each", "eg", "eight",  "either", "eleven", "else", "elsewhere",  "empty", "enough", "etc",  "even", "ever", "every",  "everyone", "everything", "everywhere", "except", "few",  "fifteen",
                                  "fify", "fill", "find", "fire", "first",  "five", "for",  "former", "formerly", "forty",  "found",  "four", "from", "front",  "full", "further",  "get",  "give", "go", "had",  "has",  "hasnt",  "have", "he", "hence",  "her",  "here",
                                  "hereafter",  "hereby", "herein", "hereupon", "hers", "him",  "his",  "how",  "however",  "hundred",  "i",  "ie", "if", "in", "inc",  "indeed", "interest", "into", "is", "it", "its",  "keep", "last", "latter", "latterly", "least",
                                  "less", "ltd",  "made", "many", "may",  "me", "meanwhile",  "might",  "mill", "mine", "more", "moreover", "most", "mostly", "move", "much", "must", "my", "name", "namely", "neither",  "never",  "nevertheless", "next", "nine",
                                  "no", "nobody", "none", "noone",  "nor",  "not",  "nothing",  "now",  "nowhere",  "of", "off",  "often",  "on", "once", "one",  "only", "onto", "or", "other",  "others", "otherwise",  "our",  "ours", "ourselves",  "out",
                                  "over", "own",  "part", "per",  "perhaps",  "please", "put",  "rather", "re", "same", "see",  "seem", "seemed", "seeming",  "seems",  "serious",  "several",  "she",  "should", "show", "side", "since",  "sincere",  "six",
                                  "sixty",  "so", "some", "somehow",  "someone",  "something",  "sometime", "sometimes",  "somewhere",  "still", "such", "system", "take", "ten",  "than", "that", "the",  "their",  "them", "themselves", "then", "thence",
                                  "there",  "thereafter", "thereby",  "therefore",  "therein",  "thereupon",  "these",  "they", "thick", "thin", "third",  "this", "those",  "though", "three",  "through",  "throughout", "thru", "thus", "to",
                                  "together", "too",  "top",  "toward", "towards",  "twelve", "twenty", "two",  "un", "under",  "until", "up", "upon", "us", "very", "via",  "was",  "we", "well", "were", "what", "whatever", "when", "whence",
                                  "whenever", "where",  "whereafter", "whereas",  "whereby",  "wherein",  "whereupon",  "wherever", "whether", "which",  "while",  "whither",  "who",  "whoever",  "whole",  "whom", "whose",  "why",  "will", "with",             "within", "without",  "would",  "yet",  "you",  "your", "yours",  "yourself"
                                 };
*/

//---------Method Implementation--------------------------------

//
};
void IRSystem::initialize() {
  std::vector<string> temp;
  //makeSet
  for (int i = 0; i < stopwordsList.size(); i ++) {
    stopWordSet.insert(stopwordsList[i]);
  }
  //building index
  for (int i = 0; i < documents.size(); i ++) {
    temp = this->stringClean(documents[i]);
    this->insert(temp, i);
  }



  // print wordSet
  // cout << "set"<< endl;
  // for(auto it = wordSet.begin(); it != wordSet.end(); ++it){
  //   cout << *it << " ";
  // }

  // convert from vector to set and back to vector to remove duplicate
  wordList.assign(wordSet.begin(), wordSet.end());
  for (int i = 0; i < wordList.size(); i ++) {
    this->wordIndexLookUpMap[wordList[i]] = i;
  }
  // cout << "map"<< endl;

  // map contain <word, index> pair; for quick word index loop up
  // for (auto &x : wordIndexLookUpMap){
  //   cout << x.first << " " << x.second << endl;
  // }
//cout << row << column;

}
bool IRSystem::isStopWord(string s) {
  std::unordered_set<std::string>::const_iterator i = stopWordSet.find(s);
  if (i != stopWordSet.end()) {
    return true;
  }
  else {
    return false;
  }
}
string IRSystem::stemming(string s) {
  //stemming
  int length, stemIndex;
  char* temp;
  temp = &s[0u];  //convert string to char* for stem()
  length = sizeof(temp) / sizeof(char); //get length of token
  stemIndex = stem(temp, 0, length) ; //get stemIndex for substr
  s = s.substr(0, stemIndex + 1); //
  return s;
}

void IRSystem::insertWordList(string s) {
  wordList.push_back(s);
}

void IRSystem::insertMap(string s) {
  map[s]; //insert key to map
}

void IRSystem::insertMapSecond(string first, int second ) {
  map[first].insert(second);  //insert key value pair to map
}

string IRSystem::toLowerCase(string s) {
  for (int i = 0; i < s.size(); i++) {
    s[i] = tolower(s[i]);
  }
  return s;
}

void IRSystem::insert(vector<string> words, int documentNumber) {
  //I can do frequency here.
  std::vector<string> temp;
  for (auto word : words) {
    //this->insertWordList(word);
    temp.push_back(word);
    wordSet.insert(word);
    this->insertMapSecond(word, documentNumber);  //build map for indexing
  }
  cleanWordMatrix.push_back(temp);
  //temp.clear();
}

vector<string> IRSystem::stringClean(string sentence) {
  /*Steps for document handling
    - replace punctuation by space
    - extract words; get last word
    - stemming
    - powerfunction build index;
  */
  std::vector<string> words;
  int pos = 0;
  string token;
  string temp;
  for (int j = 0; j < sentence.size(); j ++) {
    if (ispunct(sentence[j])) {
      sentence[j] = ' ';   //replace punctuation by space
    }
  }
  //cout << "stemming test:" << endl;
  //extract words based on space
  pos = sentence.find(WHITESPACE);  //find position of space
  while (pos != string::npos) { //while not end of string
    token = sentence.substr(0, pos); //extract word into token
    if (token.find_first_not_of(' ') != string::npos) { //if is white space

      token = this->toLowerCase(token); //to lower case
      if (!this->isStopWord(token)) { //check if in stop word

        //cout << token << " ";
        token = this->stemming(token);  //stemming
        //cout << token << endl;

        words.push_back(token);
      }
    }
    sentence.erase(0, pos + 1);
    pos = sentence.find(WHITESPACE);
  }//while end

  //handle last word
  token = sentence.substr(0, sentence.size());
  token = this->toLowerCase(token);
  if ((token.find_first_not_of(' ') != string::npos) && (!this->isStopWord(token))) { // not white space and not stop word
    token = this->stemming(token);
    words.push_back(token);
  }
  return words;
}

std::vector<int> IRSystem::search(string input) {
  int size;
  int index = 0;
  std::vector<string> afterClean;
  std::vector<int> result;
  unordered_map<string, unordered_set<int> >::const_iterator got;

  afterClean = this->stringClean(input);
  for (int i = 0; i < this->documents.size(); i ++) {
    result.push_back(0);
  }
  for (auto word : afterClean) {
    got = map.find(word);
    if (got != map.end()) {
      for (auto i : got->second) {
        result[i] += 1;
      }
    }
  }
  return result;
}

vector<int> IRSystem:: queryResult(vector<int> v) {
  std::vector<int> result;
  int haha = 9; //get top 10 documents that contains most words from query
  int tempmax = 0;
  int index = 0;

  while (haha >= 0) { //get top 10 index base on how many times words apprea in documents
    //find max
    for (int i = 0; i < v.size(); i++) {
      if (v[i] > tempmax) {
        tempmax = v[i];
        index = i;
        v[i] = 0;
      }
    }
    if (tempmax > 0) {
      result.push_back(index);
    }
    tempmax = 0;
    haha --;
  }

  return result;
}

void IRSystem::printQueryResult(string s) {
  vector<int> v = this->queryResult(this->search(s));
  if (v.size() > 0) {
    cout << "we found following document(s) for you:" << endl;
    for (int c : v) {
      cout << this->getDocumentsName()[c] << ", ";
    }
    cout << endl;
  }
  else {
    cout << "No document found" << endl;
  }
}

void IRSystem::makeFrequencyMatrix() {
  // get map
  int row = cleanWordMatrix.size();
  int column = wordSet.size();
  cout << endl;


  // cout << "size" << endl;
  for (int i = 0; i < cleanWordMatrix.size(); i ++) {
    for (int j = 0; j < cleanWordMatrix[i].size(); j ++) {
      // cout << cleanWordMatrix[i][j] << " ";
    }
    // cout << endl;
  }
  double matrix[row][column]; // all elements initialized to 0.
  
  // cout << "before" << endl;
  for (int i = 0; i < row; i++) {
    for (int j = 0; j < column; j++) {
      matrix[i][j] = 0;
      // cout << matrix[i][j] << " ";
    }
    // cout << endl;
  }
  /*
  cleanWordMatrix
  */
  int index;
  unordered_map<string, int>::const_iterator got;

  // number of time each word appears in each documents
  for (int i = 0; i < cleanWordMatrix.size(); i ++) {
    for (int j = 0; j < cleanWordMatrix[i].size(); j ++) {
      got = wordIndexLookUpMap.find(cleanWordMatrix[i][j]);
      if (got != wordIndexLookUpMap.end()) {
        index = got->second;
        matrix[i][index] += 1;
      }
    }
    //cout << endl;
  }

  /* Term frequency */
  vector<int> size; // store size of each document
  // cout << "size" << endl;
  for (int i = 0; i < cleanWordMatrix.size(); i ++) {
    size.push_back(cleanWordMatrix[i].size());
    // cout << cleanWordMatrix[i].size() << " ";
  }
  cout << endl;
  // Term Frequency Matrix
  for (int i = 0; i < row; i ++) {
    for (int j = 0; j < column; j ++) {
      matrix[i][j] /= size[i];
    }
  }


  // print matrix
  for (int i = 0; i < row; i++) {
    for (int j = 0; j < column; j++) {
      // cout << matrix[i][j] << " ";
    }
  }

  /*  Inverse Document Frequency */
  /*
  Steps:
  1. calculate total number of documents in the collection, N
  2. calculate number of documents in which the term t appears, nt
  3. calculate log(N/nt)
  */

  //1. calculate total number of documents in the collection, N
  int totalNumberOfDocuments = documents.size();

  unordered_map<string, double> IDF;
  unordered_map<string, unordered_set<int> >::const_iterator hah; //for map

  // 2. calculate number of documents in which the term t appears, nt
  for (auto c : wordSet) {
    hah = map.find(c);
    if (hah != map.end()) {
      // 4.
      IDF[c] = log(totalNumberOfDocuments / (hah->second.size())); //calculate log(N/nt)
    }
  }

  // print IDF
  // cout << "before" << endl;
  // for (auto &x : IDF) {
  //   cout << x.first << " " << x.second << endl;
  // }

  // for (auto &x : IDF) {
  //   x.second = totalNumberOfDocuments / x.second;
  //   x.second = log(x.second);
  // }
  // cout << "after" << endl;
  // for (auto &x : IDF) {
  //   cout << x.first << " " << x.second << endl;
  // }

  /* Tf*IDF*/
  /*
  for each word in wordList, find it's index,
  matrix[index][column] *= IDF
  */
  cout << "wordlist" << endl;
  for ( auto  c : wordList) {
    cout << c;
  } cout << endl;

  cout << "FINAL MATRIX" << endl;
  for (int i = 0; i < row; i ++) {
    for (int j = 0; j < column; j ++) {
      cout << matrix[i][j] << " ";
    }
    cout << endl;
  }
  int rowNumber;  //index of word in matrix
  double idfvalue;

  unordered_map<string, double>::const_iterator IDFIt;
  for (int i = 0; i < column; i ++) {
    IDFIt = IDF.find(wordList[i]);
    if (IDFIt != IDF.end()) {
      idfvalue = IDFIt->second;
      for (int j = 0; j < row; j++) {
        matrix[j][i] *= idfvalue;  // TF * IDF
      }
    }
  }

  cout << "-----------------" << endl;
  for (int i = 0; i < row; i ++) {
    for (int j = 0; j < column; j ++) {
      cout << matrix[i][j] << " ";
    }
    cout << endl;
  }

  /*compute Euclidian distance*/
  std::vector<double> distance;

}

#endif

