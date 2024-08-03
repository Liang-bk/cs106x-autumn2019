/**
 * File: random-sentence-generator.cpp
 * -----------------------------------
 * Presents a short program capable of reading in
 * context-free grammar files and generating arbitrary
 * sentences from them.
 */

#include <iostream>
#include <fstream>
using namespace std;

#include "console.h"
#include "simpio.h"   // for getLine
#include "strlib.h"   // for toLowerCase, trim
#include "vector.h"
#include "map.h"
static const int Ntimes = 3;
static const string kGrammarsDirectory = "res/grammars/";
static const string kGrammarFileExtension = ".g";
static string getNormalizedFilename(string filename) {
    string normalizedFileName = kGrammarsDirectory + filename;
    if (!endsWith(normalizedFileName, kGrammarFileExtension))
        normalizedFileName += kGrammarFileExtension;
    return normalizedFileName;
}

static bool isValidGrammarFilename(string filename) {
    string normalizedFileName = getNormalizedFilename(filename);
    ifstream infile(normalizedFileName.c_str());
    return !infile.fail();
}

static string getFileName() {
    while (true) {
        string filename = trim(getLine("Name of grammar file? [<return> to quit]: "));
        if (filename.empty() || isValidGrammarFilename(filename)) return filename;
        cout << "Failed to open the grammar file named \"" << filename << "\". Please try again...." << endl;
    }
}
static Vector<string> sentenceSplit(string sentence) {
    // sentence is something like "<exposition> <conflict> <escape> <conclusion>" or "<exposition><escape>"
    Vector<string> split;
    string s;
    for (char c : sentence) {
        if (c == '<') {
            if (!s.empty()) {
                split.add(s);
                s = string("");
            }
            s.push_back(c);
        } else if (c == '>') {
            s.push_back(c);
            split.add(s);
            s = string();
        } else {
            s.push_back(c);
        }
    }
    if (!s.empty()) split.add(s);
    return split;
}
static string expandProduction(string start, Map<string, Vector<string>>& cfg) {
    //if (!cfg.containsKey(start)) return start;          // terminal
    Vector<string> v = sentenceSplit(start);
    Vector<string> vReplace;
    for (string s : v) {
        if (!cfg.containsKey(s)) vReplace.add(s);         // terminal
        else {                                            // non-terminal
            Vector<string> replace = cfg[s];
            int low = 0, high = replace.size() - 1;
            int index = randomInteger(low, high);
            string nextOne = replace[index];
            vReplace.add(expandProduction(nextOne, cfg));
        }
    }
    return stringJoin(vReplace, "");
}
static void generateSentence(int n, string filename) {
    Map<string, Vector<string>> cfg;            // context-free grammars
    filename = getNormalizedFilename(filename);
    ifstream infile;
    infile.open(filename.c_str());
    string line;
    while (getline(infile, line)) {
        if (line.empty()) continue;
        string exp = line;                      // get the expression like <start>
        getline(infile, line);
        int num = stringToInteger(line);
        for (int i = 0; i < num; i++) {
            getline(infile,line);
            cfg[exp].add(line);                 // <start> -> <exposition> <conflict> <escape> <conclusion>
        }
    }
    infile.close();
    //cout << cfg["<start>"].toString() << endl;
    string sentence = "<start>";
    sentence = expandProduction(sentence, cfg);
    cout << n << ".) " << sentence << endl;
}
int main() {
    while (true) {
        string filename = getFileName();
        if (filename.empty()) break;
        cout << "Here's where you read in the \"" << filename << "\" grammar "
             << "and generate three random sentences." << endl;
        for (int i = 1; i <= Ntimes; i++){
            generateSentence(i, filename);
        }
    }
    cout << "Thanks for playing!" << endl;
    return 0;
}
