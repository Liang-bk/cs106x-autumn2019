/**
 * File: word-ladder.cpp
 * ---------------------
 * Implements a program to find word ladders connecting pairs of words.
 */

#include <iostream>
using namespace std;

#include "console.h"
#include "lexicon.h"
#include "strlib.h"
#include "simpio.h"
#include "vector.h"
#include "queue.h"
#include "map.h"

static string getWord(const Lexicon& english, const string& prompt) {
    while (true) {
        string response = trim(toLowerCase(getLine(prompt)));
        if (response.empty() || english.contains(response)) return response;
        cout << "Your response needs to be an English word, so please try again." << endl;
    }
}
static Vector<string> oneStepaheadOfWord(const Lexicon& english, const string& word, Map<string, bool>& visit) {
    Vector<string> ahead;
    string str = word;
    for (int i = 0; i < word.length(); i++) {
        for (char c = 'a'; c <= 'z'; c++) {
            str[i] = c;
            if (english.contains(str) && !visit.containsKey(str)) {
                ahead.add(str);
            }
        }
        str[i] = word[i];
    }
    return ahead;
}
static void generateLadder(const Lexicon& english, const string& start, const string& end) {
    //cout << "Here's where you'll search for a word ladder connecting \"" << start << "\" to \"" << end << "\"." << endl;
    if (start.length() != end.length()) {
        cout << "The two endpoints must contain the same number of characters, or else no word ladder can exist." << endl;
        return;
    }
    Map<string, string> formerWord;
    Map<string, bool> visit;
    Queue<string> q;
    q.enqueue(start);
    visit[start] = true;
    while (!q.isEmpty()) {
        string top = q.peek();
        //cout << top << endl;
        q.dequeue();
        if (top.compare(end) == 0)
            break;
        // 1. get next words
        Vector<string> ahead = oneStepaheadOfWord(english, top, visit);
        for (string nextWord : ahead) {
            formerWord[nextWord] = top;
            visit[nextWord] = true;
            q.enqueue(nextWord);
        }
    }
    if (formerWord.containsKey(end)) {
        Vector<string> wordPath;
        string str = end;
        while (formerWord.containsKey(str)) {
            wordPath.add(str);
            str = formerWord[str];
        }
        wordPath.add(start);
        reverse(wordPath.begin(), wordPath.end());
        cout << "Found ladder: ";
        for (string word : wordPath) cout << word << ' ';
        cout << endl;
    } else {
        cout << "No word ladder between \""<< start << "\" and \"" << end << "\" could be found." << endl;
    }
}

static const string kEnglishLanguageDatafile = "./res/dictionary.txt";
static void playWordLadder() {
    Lexicon english(kEnglishLanguageDatafile);
    while (true) {
        string start = getWord(english, "Please enter the source word [return to quit]: ");
        if (start.empty()) break;
        string end = getWord(english, "Please enter the destination word [return to quit]: ");
        if (end.empty()) break;
        generateLadder(english, start, end);
    }
}

int main() {
    cout << "Welcome to the CS106 word ladder application!" << endl << endl;
    playWordLadder();
    cout << "Thanks for playing!" << endl;
    return 0;
}
