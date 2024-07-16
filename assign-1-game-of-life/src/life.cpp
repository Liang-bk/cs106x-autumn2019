/**
 * File: life.cpp
 * --------------
 * Implements the Game of Life.
 */

#include <iostream>  // for cout
#include <fstream>
using namespace std;

#include "console.h" // required of all files that contain the main function
#include "simpio.h"  // for getLine
#include "gevent.h" // for mouse event detection
#include "strlib.h"
#include "gtimer.h"
#include "life-constants.h"  // for kMaxAge
#include "life-graphics.h"   // for class LifeDisplay

/**
 * Function: welcome
 * -----------------
 * Introduces the user to the Game of Life and its rules.
 */
static void welcome() {
    cout << "Welcome to the game of Life, a simulation of the lifecycle of a bacteria colony." << endl;
    cout << "Cells live and die by the following rules:" << endl << endl;
    cout << "\tA cell with 1 or fewer neighbors dies of loneliness" << endl;
    cout << "\tLocations with 2 neighbors remain stable" << endl;
    cout << "\tLocations with 3 neighbors will spontaneously create life" << endl;
    cout << "\tLocations with 4 or more neighbors die of overcrowding" << endl << endl;
    cout << "In the animation, new cells are dark and fade to gray as they age." << endl << endl;
    getLine("Hit [enter] to continue....   ");
}
// initialize the rows and columns randomly
static void initializeGridRandom(Grid<int> &g){
    int height = randomInteger(40,60);
    int width = randomInteger(40,60);
    g.resize(height,width);
    for (int i = 0; i < g.numRows(); i++){
        for (int j = 0; j < g.numCols(); j++){
            g[i][j] = 0;
            if (randomInteger(0,1)){ // 1 means occupied
                int age = randomInteger(1,kMaxAge);
                g[i][j] = age;
            }
        }
    }
}
static void setGrid2Window(Grid<int> &g, LifeDisplay &w){
    for (int i = 0; i < g.numRows(); i++){
        for (int j = 0; j < g.numCols(); j++){
            w.drawCellAt(i,j,g[i][j]);
        }
    }
}
static void initializeGridFile(Grid<int> &g, ifstream &f){
    string line;
    int r = 0, c = 0;
    int numLine;
    while(getline(f, line)){
        if (startsWith(line, "#")) continue;
        if (stringIsInteger(line)){
            if (r == 0) r = stringToInteger(line);
            else if (c == 0) c = stringToInteger(line);
            if (r && c){
                g.resize(r,c);
                numLine = 0;
            }
        } else {
            // the map
            for (int i = 0; i < line.length(); i++){
                if (line[i] == 'X')
                    g[numLine][i] = 1;
            }
            numLine ++;
        }
    }
}
static void getFileAndInitilize(Grid<int> &board){
    cout << "You can start your colony with random cells or read from a prepared file." << endl;
    bool initFlag = false;
    while (!initFlag) {
        string filePath = getLine("Enter name of conlony file (or RETURN to seed randomly):");
        if (filePath.compare("") == 0){
            initializeGridRandom(board);
            initFlag = true;
        } else {
            ifstream inputFile;
            inputFile.open(filePath.c_str());
            // once failed, the program should exit
            if (inputFile.fail()){
                cout << "Unable to open the file named \"" << filePath << "\". Please select another file." << endl;
                continue;
            }
            initializeGridFile(board,inputFile);
            initFlag = true;
        }
    }
}
static void getChoice(int &choice) {
    cout << "You choose how fast to run the simulation." << endl;
    cout << "\t1 = As fast as this chip can go!" << endl;
    cout << "\t2 = Not too fast, this is a school zone." << endl;
    cout << "\t3 = Nice and slow so I can watch everything that happens." << endl;
    cout << "\t4 = Require enter key be pressed before advancing to next generation." << endl;
    while (true) {
        choice = getInteger("Your choice:");
        if (choice > 4 || choice < 1){
            cout << "Please enter a number between 1 and 4!" << endl;
            continue;
        } else {
            break;
        }
    }
}
static bool getNextGeneration(Grid<int> &board){
    Grid<int> next(board.numRows(), board.numCols());
    // it has eight directions
    int dx[] = {
        -1, -1, -1,
        0,      0,
        1,  1,  1
    };
    int dy[] = {
        -1, 0, 1,
        -1,    1,
        -1, 0, 1,
    };
    int R = board.numRows(), C = board.numCols();
    for (int i = 0; i < R; i++){
        for (int j = 0; j < C; j++){
            // compute the derivitive of this point
            int numNeigh = 0;
            for (int k = 0; k < 8; k++){
                int nx = i + dx[k], ny = j + dy[k];
                if (next.inBounds(nx,ny) && board[nx][ny]){
                    numNeigh ++;
                }
            }
            if (numNeigh <= 1 || numNeigh >= 4){
                next[i][j] = 0;
            } else if (numNeigh == 2){
                next[i][j] = board[i][j];
                if (next[i][j] > 0 && next[i][j] + 1 <= kMaxAge)
                    next[i][j] += 1;
            } else if (numNeigh == 3) {
                next[i][j] = board[i][j];
                if (next[i][j] > 0 && next[i][j] + 1 <= kMaxAge) {
                    next[i][j] ++;
                } else if (next[i][j] == 0){
                    next[i][j] = 1;
                }
            }
        }
    }
    bool flag = false;
    if (next.equals(board))
        flag =  true;
    board = next;
    return flag;
}
static void runAnimation(LifeDisplay& display, Grid<int>& board, int choice) {
    if (choice <= 3) {
        int ms = pow(10, choice);
        GTimer timer(ms);
        timer.start();
        while (true) {
            GEvent event = waitForEvent(TIMER_EVENT + MOUSE_EVENT);
            if (event.getEventClass() == TIMER_EVENT) {
                setGrid2Window(board,display);
                display.repaint();
                if (getNextGeneration(board)) {
                    break;
                }
            } else if (event.getEventType() == MOUSE_PRESSED) {
                break;
            }
        }
        timer.stop();
    } else {
        while (true) {
            string cue = getLine("Please return to advance [or type out \"quit\" to end]:");
            if (cue.compare("quit") == 0){
                break;
            }
            setGrid2Window(board, display);
            display.repaint();
            if (getNextGeneration(board)) {
                break;
            }
        }
    }
}
/**
 * Function: main
 * --------------
 * Provides the entry point of the entire program.
 */
int main() {
    LifeDisplay display;
    display.setTitle("Game of Life");
    welcome();
    Grid<int> board;

    while(true){
        getFileAndInitilize(board);
        int choice = 0;
        getChoice(choice);
        display.setDimensions(board.numRows(),board.numCols());
        runAnimation(display, board, choice);
        if (!getYesOrNo("Would you like to run another? ", "Please enter \"yes\" or \"no\".", "no")) {
            break;
        }
    }
    return 0;
}
