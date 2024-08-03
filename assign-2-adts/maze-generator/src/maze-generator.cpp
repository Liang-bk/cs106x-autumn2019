/**
 * File: maze-generator.cpp
 * ------------------------
 * Presents an adaptation of Kruskal's algorithm to generate mazes.
 */

#include <iostream>
using namespace std;

#include "console.h"
#include "simpio.h"
#include "maze-graphics.h"
#include "maze-types.h"
#include "random.h"

static int getMazeDimension(string prompt,
                            int minDimension = 7, int maxDimension = 50) {
    while (true) {
        int response = getInteger(prompt);
        if (response == 0) return response;
        if (response >= minDimension && response <= maxDimension) return response;
        cout << "Please enter a number between "
             << minDimension << " and "
             << maxDimension << ", inclusive." << endl;
    }
}
static Vector<wall> initializeMaze(MazeGeneratorView& maze, int dimension) {
    Vector<cell> cellArray;
    for (int i = 0; i < dimension; i++) {
        for (int j = 0; j < dimension; j++) {
            cell c = {i ,j};
            cellArray.add(c);
        }
    }
    Vector<wall> wallArray;
    for (int i = 0; i < dimension; i++) {
        for (int j = 0; j < dimension - 1; j++) {
            cell c1 = cellArray[i * dimension + j];
            cell c2 = cellArray[i * dimension + j + 1];
            wall w1 = {c1 ,c2};
            wallArray.add(w1);
            cell c3 = cellArray[j * dimension + i];
            cell c4 = cellArray[(j + 1) * dimension + i];
            wall w2 = {c3, c4};
            wallArray.add(w2);
        }
    }
    maze.addAllWalls(wallArray);
    return wallArray;
}
static int find(Vector<int>& chambers, int index) {
    if (chambers[index] == index) return index;
    chambers[index] = find(chambers, chambers[index]);
    return chambers[index];
}
static wall randomRemoveWall(int dimension, Vector<wall>& wallArray, Set<wall>& hasRemoved, Vector<int>& chambers) {
    while (true) {
        int index = randomInteger(0, wallArray.size() - 1);
        // the wall has been removed
        if (hasRemoved.contains(wallArray[index])) continue;
        wall w = wallArray[index];
        cell c1 = w.one, c2 = w.two;
        int chamber1 = c1.row * dimension + c1.col + 1;
        int chamber2 = c2.row * dimension + c2.col + 1;
        // two chambers are already in a same section
        if (find(chambers, chamber1) == find(chambers, chamber2)) continue;
        // merge them
        chambers[find(chambers, chamber2)] = find(chambers, chamber1);
        return w;
    }
}
int main() {
    while (true) {
        int dimension = getMazeDimension("What should the dimension of your maze be [0 to exit]? ");
        if (dimension == 0) break;
        Vector<int> chambers(dimension * dimension + 1,0);
        for (int i = 1; i < chambers.size(); i++) chambers[i] = i;
        cout << "This is where I'd animate the construction of a maze of dimension " << dimension << "." << endl;
        MazeGeneratorView mazeWindow;
        mazeWindow.setDimension(dimension);
        Vector<wall> wallArray = initializeMaze(mazeWindow, dimension);
        Set<wall> hasRemoved;
        mazeWindow.repaint();
        for (int i = 0; i < dimension * dimension - 1; i++) {
            wall w = randomRemoveWall(dimension, wallArray, hasRemoved, chambers);
            hasRemoved.add(w);
            mazeWindow.removeWall(w);
            mazeWindow.repaint();
        }
        waitForClick();
    }

    return 0;
}
