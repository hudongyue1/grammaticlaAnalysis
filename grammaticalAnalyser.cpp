#define _CRT_SECURE_NO_WARNINGS
#include<iostream>
#include<string>
#include<vector>
#include<algorithm>
#include<map>
#include<set>
#include<fstream>
#include<iomanip>

using namespace std;


class Symbol { // 符号：终结符、非终结符
private:
    enum SymbolType {
        T, NT // terminal or not-terminal
    };
    SymbolType type;
    int id; // 记号 --> terminal
public:
    Symbol(SymbolType type, int id) {
        this.type = type;
        this.id = id; 
    }

    bool operator < (const Symbol& otherSymbol) {
        return this.id < otherSymbol.id;
    }

    bool operator == (const Symbol& otherSymbol) {
        return this.id == otherSymbol.id;
    }
};

// 定义两个特殊符号
const Symbol END(Symbol::SymbolType::T, 0); // $
const Symbol EPSILON(Symbol::SymbolType::T, 1); // ε


// first(), 当前符号串可能的第一个终结符
vector<Symbol> first(vector<Symbol>) {

}


// 项目(产生式)
class item { 
private:
    int id;
    Symbol leftSymbol;
    vector<Symbol> rightSymbol;
    Symbol foreSymbol; // 向前看符号
    int dotPosition; // 圆点位置(0 -- rightSymbol.size())
public:
    item(int id, Symbol leftSymbol, vector<Symbol> rightSymbol, Symbol foreSymbol, int dotPosition) {
        this.id = id;
        this.leftSymbol = leftSymbol;
        this.rightSymbol = rightSymbol;
        this.foreSymbol = foreSymbol;
        this.dotPosition = dotPosition;
    }

    // 当dot在最后表示规约项目
    bool isReductionItem() { 
        return dotPosition == rightSymbol.size();
    }

    // 返回dot后面的第一个符号
    Symbol symbolAfterDot() { 
        return rightSymbol[dotPosition];
    }
};

// 项目集合
class itemSet { 
private:
    int id;
    int size;
    vector<item> items;
public:
    itemSet() {}


    // 往项目集合中加入新项目
    void joinSet(item theItem) { 
        items.push_back(theItem);
    }    

};

// 求项目集合的闭包
itemSet closure(itemSet theItemSet) { 

}

// 项目集族
class itemSetGroup { 
private:
    vector<itemSet> itemSets;
    vector<Symbol> allSymbols;

public:

};

// go(I, X) = closure(J)
itemSet go(itemSet theItemSet, Symbol theSymbol) { 

}


void main() {



}