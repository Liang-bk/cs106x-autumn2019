/**
 * File: ssmodel.cpp
 * ------------------
 * This file will contain the implementation of the SSModel class (once you
 * have finished it!
 */
 
#include "ssmodel.h"
#include "parser.h"
using namespace std;

SSModel::SSModel(int nRows, int nCols, SSView *view) {
    _numRows = nRows;
    _numCols = nCols;
    _view = view;
    context = make_shared<EvaluationContext>();
    dependency = Vector<Vector<location>>(_numCols * _numRows, Vector<location>());
    edgeG = Vector<Vector<location>>(_numCols * _numRows, Vector<location>());
    //
    cellExp.clear();
    for (int i = 0; i < _numCols * _numRows; ++i) cellExp.push_back(nullptr);
}

SSModel::~SSModel() {}

int SSModel::indexOfCell(const location& cell) const {
    // "A1" -> 0, "B1" -> 1 ...
    int col = cell.col - 'A';
    int row = cell.row - 1;
    return row * _numCols + col;
}

bool SSModel::checkAdd(const Vector<location> &others, const location &cell) const {
    // a little complicated, we have to copy the Vector edgeG and try to add some edges.
    Vector<Vector<location>> edgeGCopy = edgeG;
    for (const location &other : others) {
        int otherIndex = indexOfCell(other);
        edgeGCopy[otherIndex].add(cell);
    }
    bool hasCycle = false;
    Vector<int> vis(_numCols * _numRows, 0);
    vis[indexOfCell(cell)] = 1;
    Queue<location> q;
    q.enqueue(cell);
    // start bfs, if it find cell twice, then it must have a cycle
    while (!q.isEmpty()) {
        location top = q.peek();
        q.dequeue();
        int topIndex = indexOfCell(top);
        if (hasCycle) break;
        for (const location &other : edgeGCopy[topIndex]) {
            int otherIndex = indexOfCell(other);
            if (vis[otherIndex] == 0) {
                vis[otherIndex] = 1;
                q.enqueue(other);
            }
            if (other == cell) {
                hasCycle = true;
                break;
            }
        }
    }
    // if graph has cycle, return false, else return true, means it's no problem to add edge
    return !hasCycle;
}

void SSModel::addDependency(const Vector<location> &others, const location &cell) {
    int cellIndex = indexOfCell(cell);
    for (const location &other : others) {
        int otherIndex = indexOfCell(other);
        edgeG[otherIndex].add(cell);
        dependency[cellIndex].add(other);
    }
}

void SSModel::removeDependency(const location& cell) {
    int cellIndex = indexOfCell(cell);
    for (const location &other : dependency[cellIndex]) {
        int otherIndex = indexOfCell(other);
        auto it = std::find(edgeG[otherIndex].begin(), edgeG[otherIndex].end(), cell);
        if (it != edgeG[otherIndex].end()) {
            edgeG[otherIndex].remove(it - edgeG[otherIndex].begin());
        }
    }
    dependency[cellIndex].clear();
}

void SSModel::updateOtherCell(const location& cell) {
    Map<location, int> degree;
    // bfs first
    Queue<location>q;
    q.enqueue(cell);
    while (!q.isEmpty()) {
        location top = q.peek();
        q.dequeue();
        int topIndex = indexOfCell(top);
        for (const location &other : edgeG[topIndex]) {
            degree[other] += 1;
            q.enqueue(other);
        }
    }
    // we got degree which is needed for toposort
    q.enqueue(cell);
    while (!q.isEmpty()) {
        location top = q.peek();
        q.dequeue();
        int topIndex = indexOfCell(top);
        for (const location & other : edgeG[topIndex]) {
            degree[other] -= 1;
            if (degree[other] == 0) {
                int otherIndex = indexOfCell(other);
                // we have to re-eval the expression of later cells
                double value = cellExp[otherIndex]->eval(*context);
                // reset cell value
                context->setValue(locationToString(other), value);
                this->_view->displayCell(locationToString(other), doubleToString(value));
                // update next one
                q.enqueue(other);
            }
        }
    }
}


bool SSModel::nameIsValid(const string& cellname) const {
    // cellname[0]: A-Z cellname[1-...] when it turns to numbers, it shouldn't bigger than _numRows
    // it could use stringtolocation but I forget so i might modify it later
    int rowDigits = 0, x = _numRows;
    while (x > 0) {
        x /= 10;
        rowDigits += 1;
    }
    //
    if (cellname.size() > rowDigits + 1 || cellname.size() < 2) return false;

    string lowername = toLowerCase(cellname);
    if (lowername[0] < 'a' || lowername[0] > 'a' + _numCols - 1) return false;

    if (lowername[1] <= '0' || lowername[1] > '9') return false;

    x = lowername[1] - '0';
    for (size_t i = 2; i < lowername.size(); ++i) {
        if (lowername[i] < '0' || lowername[i] > '9') return false;
        x = x * 10 + (lowername[i] - '0');
    }
    if (x > _numRows) return false;

    return true;
}

void SSModel::setCellFromScanner(const string& cellname, TokenScanner& scanner) {
    // guarantee the cellname is valid, tips: scanner.nextToken() will start from "=" which means set a1="..."
    unique_ptr<Expression> exp = nullptr;
    try {
        unique_ptr<Expression> tmp(parseExp(scanner, *this));
        exp = std::move(tmp);
    } catch(ErrorException ex) {
        cout << "Error when parse the scanner: "
             << ex.getMessage() << endl;
        return;
    }
    string cellName = toUpperCase(cellname);
    location cell;
    stringToLocation(cellName, cell);
    switch (exp->getType()) {
        // when every assignment work, it might change the dependency of this cells and other cells,
        // so be care of it
        case DOUBLE: {
            DoubleExp *dexp = (DoubleExp*)exp.get();
            double value = dexp->getDoubleValue();
            context->setValue(cellName, value);
            string content = doubleToString(value);
            _view->displayCell(cellName, content);
            removeDependency(cell);
            updateOtherCell(cell);
            break;
        }
        case TEXTSTRING: {
            TextStringExp *texp = (TextStringExp*)exp.get();
            context->setValue(cellName, 0.0);
            string content = texp->getTextStringValue();
            _view->displayCell(cellName, content);
            cellContent.put(cellName, content);
            removeDependency(cell);
            updateOtherCell(cell);
            break;
        }
        case IDENTIFIER: {
            IdentifierExp *iexp = (IdentifierExp*)exp.get();
            // check if there's a cycle when add edge
            Vector<location> others;
            location other;
            stringToLocation(iexp->getIdentifierName(), other);
            others.add(other);
            if (!checkAdd(others, cell)) {
                error("There will form a cycle if set worked, check it plz");
                return;
            }
            // remove the current dependency of this cell
            removeDependency(cell);
            // add the new dependency of this cell
            addDependency(others, cell);
            // if other cell depends on this cell, update it
            double value = iexp->eval(*context);
            context->setValue(cellName, value);
            _view->displayCell(cellName, doubleToString(value));
            updateOtherCell(cell);
            break;
        }
        case COMPOUND: {
            // build the graph
            CompoundExp *cexp = (CompoundExp*)exp.get();
            Vector<location> others;
            location other;
            // need to bfs cexp->lhs, cexp->rhs, to find non-compound expression,
            // and for each identitier, add it to Vector.
            // I have to say when you writing to many lines of C++ codes, it really makes you confused at some points.
            Queue<const Expression*> q;
            q.enqueue(exp.get());
            while (!q.isEmpty()) {
                const Expression* top = q.peek();
                q.dequeue();
                switch (top->getType()) {
                case DOUBLE: break;
                case TEXTSTRING: break;
                case IDENTIFIER: {
                    const IdentifierExp* expToi = (const IdentifierExp*)top;
                    stringToLocation(expToi->getIdentifierName(), other);
                    others.add(other);
                    break;
                }
                case COMPOUND: {
                    const CompoundExp* expToc = (const CompoundExp*)top;
                    const Expression* lhs = expToc->getLHS(), *rhs = expToc->getRHS();
                    q.enqueue(lhs);
                    q.enqueue(rhs);
                    break;
                }
                case FORMULA: {
                    const FormulaExp* expTof = (const FormulaExp*)top;
                    range cellr = expTof->getRange();
                    for (char col = cellr.startCell.col; col <= cellr.stopCell.col; ++col) {
                        for (int row = cellr.startCell.row; row <= cellr.stopCell.row; ++row) {
                            other.col = col, other.row = row;
                            others.add(other);
                        }
                    }
                    break;
                }
                default: {
                    error("Unknown error when set variable");
                    break;
                }
                }
            }
            if (!checkAdd(others, cell)) {
                error("There will form a cycle if set worked, check it plz");
                return;
            }
            // remove the current dependency of this cell
            removeDependency(cell);
            // add the new dependency of this cell
            addDependency(others, cell);
            // if other cell depends on this cell, update it
            double value = cexp->eval(*context);
            context->setValue(cellName, value);
            _view->displayCell(cellName, doubleToString(value));
            updateOtherCell(cell);
            break;
        }
        case FORMULA: {
            FormulaExp *fexp = (FormulaExp*)exp.get();
            range cellr = fexp->getRange();
            Vector<location> others;
            location other;
            for (char col = cellr.startCell.col; col <= cellr.stopCell.col; ++col) {
                for (int row = cellr.startCell.row; row <= cellr.stopCell.row; ++row) {
                    other.col = col, other.row = row;
                    others.add(other);
                }
            }
            if (!checkAdd(others, cell)) {
                error("There will form a cycle if set worked, check it plz");
                return;
            }
            // remove the current dependency of this cell
            removeDependency(cell);
            // add the new dependency of this cell
            addDependency(others, cell);
            // if other cell depends on this cell, update it
            double value = fexp->eval(*context);
            context->setValue(cellName, value);
            _view->displayCell(cellName, doubleToString(value));
            updateOtherCell(cell);
            break;
        }
        default: {
            error("Expression error");
            break;
        }
    }
    cellExp[indexOfCell(cell)] = std::move(exp);
}

void SSModel::printCellInformation(const string& cellname) const {}

void SSModel::writeToStream(ostream& outfile) const {}

void SSModel::readFromStream(istream& infile) {}

void SSModel::clearAllCells() {}
