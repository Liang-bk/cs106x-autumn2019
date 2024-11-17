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
}

SSModel::~SSModel() {}

bool SSModel::nameIsValid(const string& cellname) const {
    // cellname[0]: A-Z cellname[1-...] 大小不应该超过_numRows
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
    unique_ptr<Expression> exp(parseExp(scanner, *this));
    switch (exp->getType()) {
        // when every assignment work, it might change the dependency of this cells and other cells, so be care of it
        case DOUBLE: {
            DoubleExp *dexp = (DoubleExp*)exp.get();
            double value = dexp->getDoubleValue();
            context->setValue(cellname, value);
            string content = doubleToString(value);
            _view->displayCell(cellname, content);
            cellContent.put(cellname, content);

            // removeDependency();
            // updateOtherCell();
            break;
        }
        case TEXTSTRING: {
            TextStringExp *texp = (TextStringExp*)exp.get();
            string content = texp->getTextStringValue();
            _view->displayCell(cellname, content);
            cellContent.put(cellname, content);

            // removeDependency();
            // updateOtherCell();
            break;
        }
        case IDENTIFIER: {
            // 建图
            IdentifierExp *iexp = (IdentifierExp*)exp.get();
            double value = iexp->eval(*context);
            // 判断无环
            // checkAdd();
            // // 移除边
            // removeDependency();
            // // 添加边
            // addDependency();
            // // 如果有其他cell依赖当前cell,更新
            // updateOtherCell();
            break;
        }
        case COMPOUND: {
            // 建图
            CompoundExp *cexp = (CompoundExp*)exp.get();
            double value = cexp->eval(*context);
            // 判断无环
            // checkAdd();
            // // 移除边
            // removeDependency();
            // // 添加边
            // addDependency();
            // // 如果有其他cell依赖当前cell,更新
            // updateOtherCell();
            break;
        }
        case FORMULA: {

        }
        default: {
            error("Expression error");
            break;
        }
    }


}

void SSModel::printCellInformation(const string& cellname) const {}

void SSModel::writeToStream(ostream& outfile) const {}

void SSModel::readFromStream(istream& infile) {}

void SSModel::clearAllCells() {}
