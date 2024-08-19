/**
 * File: boggle.cpp
 * ----------------
 * Implements the game of Boggle.
 */

#include <cctype>
#include <iostream>
#include <ctype.h>
#include "console.h"
#include "gboggle.h"
#include "simpio.h"
#include "strlib.h"
#include "vector.h"
#include "random.h"
#include "grid.h"
#include "map.h"
#include "set.h"
#include "lexicon.h"
using namespace std;

static const string kStandardCubes[16] = {
   "AAEEGN", "ABBJOO", "ACHOPS", "AFFKPS",
   "AOOTTW", "CIMOTU", "DEILRX", "DELRVY",
   "DISTTY", "EEGHNW", "EEINSU", "EHRTVW",
   "EIOSST", "ELRTTY", "HIMNQU", "HLNNRZ"
};

static const string kBigBoggleCubes[25] = {
   "AAAFRS", "AAEEEE", "AAFIRS", "ADENNN", "AEEEEM",
   "AEEGMU", "AEGMNN", "AFIRSY", "BJKQXZ", "CCNSTW",
   "CEIILT", "CEILPT", "CEIPST", "DDLNOR", "DDHNOT",
   "DHHLOR", "DHLNOR", "EIIITT", "EMOTTT", "ENSSSU",
   "FIPRSY", "GORRVW", "HIPRRY", "NOOTUW", "OOOTTU"
};

static const int kMinLength = 4;
static const double kDelayBetweenHighlights = 100;
static const double kDelayAfterAllHighlights = 500;
static const string dictionaryPath = "./res/dictionary.txt";
struct gXY {
    int r;
    int c;
};
/**
 * Function: welcome
 * Usage: welcome();
 * -----------------
 * Print out a cheery welcome message.
 */
static void welcome() {
    cout << "Welcome!  You're about to play an intense game ";
    cout << "of mind-numbing Boggle.  The good news is that ";
    cout << "you might improve your vocabulary a bit.  The ";
    cout << "bad news is that you're probably going to lose ";
    cout << "miserably to this little dictionary-toting hunk ";
    cout << "of silicon.  If only YOU had a gig of RAM..." << endl;
    cout << endl;
}

/**
 * Function: giveInstructions
 * Usage: giveInstructions();
 * --------------------------
 * Print out the instructions for the user.
 */
static void giveInstructions() {
    cout << "The boggle board is a grid onto which I ";
    cout << "I will randomly distribute cubes. These ";
    cout << "6-sided cubes have letters rather than ";
    cout << "numbers on the faces, creating a grid of ";
    cout << "letters on which you try to form words. ";
    cout << "You go first, entering all the words you can ";
    cout << "find that are formed by tracing adjoining ";
    cout << "letters. Two letters adjoin if they are next ";
    cout << "to each other horizontally, vertically, or ";
    cout << "diagonally. A letter can only be used once ";
    cout << "in each word. Words must be at least four ";
    cout << "letters long and can be counted only once. ";
    cout << "You score points based on word length: a ";
    cout << "4-letter word is worth 1 point, 5-letters ";
    cout << "earn 2 points, and so on. After your puny ";
    cout << "brain is exhausted, I, the supercomputer, ";
    cout << "will find all the remaining words and double ";
    cout << "or triple your paltry score." << endl;
    cout << endl;
    cout << "Hit return when you're ready...";
    getLine(); // ignore return value
}

/**
 * Function: getPreferredBoardSize
 * -------------------------------
 * Repeatedly prompts the user until he or she responds with one
 * of the two supported Boggle board dimensions.
 */

static int getPreferredBoardSize() {
    cout << "You can choose standard Boggle (4x4 grid) or Big Boggle (5x5 grid)." << endl;
    return getIntegerBetween("Which dimension would you prefer: 4 or 5?", 4, 5);
}
/**
 * Function: checkStrValid
 * -----------------------
 * to check a string is dimension * dimension long or not
 * and make sure it is consisted by alphanum.
*/
static int checkStrValid(const string& str, int dimension) {
    if (str.length() != dimension * dimension)
        return 1;
    for (int i = 0; i < str.length(); i++) {
        if (!isalpha(str[i]))
            return 2;
    }
    return 0;
}
/**
 * Function: setCubesByUser
 * ------------------------
 * get user input string and show it in the cubes.
*/
static void setCubesByUser(Grid<char>& upside, int dimension) {
    string str;
    str = getLine("Enter a string: ");
    while (true) {
        int check = checkStrValid(str, dimension);
        if (check == 0)
            break;
        if (check == 1)
            cout << "Enter a string that's precisely " << dimension * dimension << " characters long." << endl;
        else if (check == 2)
            cout << "Enter a string with only alphabetic letters." << endl;
        str = getLine("Try again: ");
    }
    str = toUpperCase(str);
    upside.resize(dimension, dimension);
    for (int row = 0; row < dimension; row++) {
        for (int col = 0; col < dimension; col++) {
            upside[row][col] = str[row * dimension + col];
            labelCube(row, col, str[row * dimension + col]);
        }
    }
}
/**
 * Function: shuffleCubes
 * --------------------
 * Shuffle the Cubes whose dimension is 4 or five and record every cubes' upside character
 */
static void shuffleCubes(Vector<string>& vec, Grid<char>& upside, int dimension) {
    if (dimension == 4){
        for (int i = 0; i < 4 * 4; i++) {
            vec.add(kStandardCubes[i]);
        }
    } else {
        for (int i = 0; i < 5 * 5; i++) {
            vec.add(kBigBoggleCubes[i]);
        }
    }
    for (int i = 0; i < vec.size(); i++) {
        int r = randomInteger(i, vec.size() - 1);
        string tmp = vec[i];
        vec[i] = vec[r];
        vec[r] = tmp;
    }
    upside.resize(dimension, dimension);
    for (int row = 0; row < dimension; row++) {
        for (int col = 0; col < dimension; col++) {
            int index = row * dimension + col;
            char c = vec[index][randomInteger(0,5)];
            labelCube(row, col, c);
            upside.set(row, col, c);
        }
    }
}
/**
 * Function: dfs
 * -------------
 * give a coordinate in g, find every eight direction of this point,
 * if all character in path can consist a word, record it in a map(words)
*/
static void dfs(gXY gxy, const Grid<char>& g, Grid<bool>& visit, Vector<gXY>&path,
                Map<string, Vector<gXY>>& words, Lexicon& english) {
    visit[gxy.r][gxy.c] = true;
    string word = "";
    for (int i = 0; i < path.size(); i++) {
        gXY tmp = path[i];
        char c = g[tmp.r][tmp.c];
        word += c;
    }
    word += g[gxy.r][gxy.c];
    path.add(gxy);
    if (!english.containsPrefix(word)) {
        path.remove(path.size() - 1);
        visit[gxy.r][gxy.c] = false;
        return;
    } else if (english.contains(word)) {
        if (!words.containsKey(word) && word.length() >= 4) {
            words.put(word, path);
        }
    }
    int dx[] = {0, 0, 1, -1, 1, 1, -1, -1};
    int dy[] = {1, -1, 0, 0, 1, -1, 1, -1};
    for (int i = 0; i < 8; i++) {
        gXY gNext;
        gNext.r = gxy.r + dx[i];
        gNext.c = gxy.c + dy[i];
        if (gNext.r >= 0 && gNext.r < g.numRows() && gNext.c >= 0 && gNext.c < g.numRows() && !visit[gNext.r][gNext.c]) {
            dfs(gNext, g, visit, path, words, english);
        }
    }
    path.remove(path.size() - 1);
    visit[gxy.r][gxy.c] = false;
}
/**
 * Function: findAllWords
 * ----------------------
 * use dfs to find words recursively, and save all the
 * results and paths in a map.
*/
static Map<string, Vector<gXY>> findAllWords(Grid<char>& upside) {
    int dimension = upside.numRows();
    Map<string, Vector<gXY>> words;
    Lexicon english(dictionaryPath);
    for (int row = 0; row < dimension; row++) {
        for (int col = 0; col < dimension; col++) {
            Grid<bool> visit(dimension, dimension, false);
            Vector<gXY> path;
            gXY gxy = {row, col};
            dfs(gxy, upside, visit, path, words, english);
        }
    }
    return words;
}
/**
 * Function: playBoggle
 * --------------------
 * Manages all details needed for the user to play one
 * or more games of Boggle.
 */
static void playBoggle() {
    int dimension = getPreferredBoardSize();
    drawBoard(dimension, dimension);
    cout << "I'll give you a chance to set up the board to your specification, "
            "which makes it easier to confirm your boggle program is working." << endl;
    //cout << "Do you want to force the board configuration?" << endl;
    //cout << "This is where you'd play the game of Boggle" << endl;
    Vector<string> vec;
    Grid<char> upside;
    if (getYesOrNo("Do you want to force the board configuration?")){
        // 16 or 25 character input
        setCubesByUser(upside, dimension);
    } else {
        shuffleCubes(vec, upside, dimension);
    }
    Map<string, Vector<gXY>> words = findAllWords(upside);
    Set<string> player;
    Set<string> computer;
    string word;
    // player first
    while ((word = getLine("Enter a word: ")) != "") {
        word = toUpperCase(word);
        if (words.containsKey(word)) {
            recordWordForPlayer(word, HUMAN);
            player.add(word);
            Vector<gXY> path = words[word];
            for (int i = 0; i < path.size(); i++) {
                highlightCube(path[i].r, path[i].c, true);
                pause(100);
            }
            for (int i = path.size() - 1; i >= 0; i--) {
                highlightCube(path[i].r, path[i].c, false);
                pause(100);
            }
        } else {
            cout << "Sorry, that isn't even a word." << endl;
        }
    }
    // computer have the remains.
    int scores[2] = {0};
    Vector<string> wordsKey = words.keys();
    for (const string& s : wordsKey) {
        if (!player.contains(s)) {
            computer.add(s);
            recordWordForPlayer(s, COMPUTER);
            scores[1] += s.length();
        } else {
            scores[0] += s.length();
        }
    }
    if (scores[0] < scores[1]) cout << "Shocker! The computer player prevailed!" << endl;
    else if (scores[0] == scores[1]) cout << "Wow! You matched the computer!  "
                "That's pretty much a Boggle victory for the human race!" << endl;
    else cout << "Whoa!!!  You actually beat the computer at its own game.  Excellent Boggle skills, human!" << endl;
}

/**
 * Function: main
 * --------------
 * Serves as the entry point to the entire program.
 */
int main() {
    GWindow gw(kBoggleWindowWidth, kBoggleWindowHeight);
    initGBoggle(gw);
    welcome();
    if (getYesOrNo("Do you need instructions?")) giveInstructions();
    do {
        playBoggle();
    } while (getYesOrNo("Would you like to play again?"));
    cout << "Thank you for playing!" << endl;
    shutdownGBoggle();
    return 0;
}
