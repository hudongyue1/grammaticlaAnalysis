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
public:
	enum SymbolType {
		T, NT // terminal or not-terminal
	};
private:
	SymbolType type;
	int id; 
public:
	Symbol() {
		this->type = SymbolType::T;
		this->id = 2;
	}
	Symbol(SymbolType type, int id) {
		this->type = type;
		this->id = id;
	}

	SymbolType getType() const {
		return this->type;
	}

	int getId() const {
		return this->id;
	}

	bool operator < (const Symbol& otherSymbol) const{
		return this->id < otherSymbol.id;
	}

	bool operator == (const Symbol& otherSymbol) const {
		return this->id == otherSymbol.id;
	}

	// 输出Symbol
	friend ostream& operator << (ostream& os, const Symbol& word) {
		os << word.getId();
		/*if (word.getType() == Symbol::SymbolType::T) cout << "终结符)";
		else cout << "非终结符";*/
		return os;
	}
};

// 定义三个特殊符号
const Symbol BEGIN(Symbol::SymbolType::NT, 0); // S'
const Symbol END(Symbol::SymbolType::T, 1); // $
const Symbol EPSILON(Symbol::SymbolType::T, 2); // ε


// 项目(产生式)
class Item {
private:
	Symbol leftSymbol;
	vector<Symbol> rightSymbol;
	Symbol foreSymbol; // 向前看符号
	int dotPosition; // 圆点位置(0 -- rightSymbol.size())
public:
	Item() {}
	Item(Symbol leftSymbol, vector<Symbol> rightSymbol, Symbol foreSymbol = EPSILON, int dotPosition = 0) {
		this->leftSymbol = leftSymbol;
		this->rightSymbol = rightSymbol;
		this->foreSymbol = foreSymbol;
		this->dotPosition = dotPosition;
	}

	bool operator == (const Item& otherItem) const{
		if (this->leftSymbol == otherItem.getLeftSymbol() &&
			this->foreSymbol == otherItem.getForeSymbol() &&
			this->dotPosition == otherItem.getDotPosition()) {
			int len = rightSymbol.size();
			const vector<Symbol>& temp = otherItem.getRightSymbol();
			if (len != temp.size()) return false;
			else {
				for (int i = 0; i < len; ++i) {
					if (this->rightSymbol[i] == temp[i]) continue;
					else return false;
				}
			}
			return true;
		}else return false;
	}

	// 当dot在最后表示规约项目
	bool isReductionItem() const {
		return dotPosition == rightSymbol.size();
	}

	// 返回dot后面的第一个符号
	Symbol symbolAfterDot() const {
		if (isReductionItem()) return END;
		else return rightSymbol[dotPosition];
	}

	// 返回右侧第一个符号
	Symbol firstSymbolOfRight() const {
		return rightSymbol[0];
	}

	// 返回右侧index后的符号串
	vector<Symbol> allSymbolsAfterIndex(int index) const {
		vector<Symbol> temp;
		int len = rightSymbol.size();
		for (int i = index+1; i < len; ++i)
			temp.push_back(rightSymbol[i]);
		// 如果index后的符号串为ε
		if (temp.size() == 0) temp.push_back(EPSILON);
		return temp;
	}

	friend ostream& operator << (ostream& os, Item& item) {
		cout << endl;
		cout << '[' << item.getLeftSymbol() << " --> ";
		int len = item.getRightSymbol().size();
		for (int i = 0; i < len; ++i) {
			if (i == item.getDotPosition()) cout << "·";
			cout << item.getRightSymbol()[i] << " ";
		}
		cout << "," << item.getForeSymbol() << ']';
		return os;
	}

	Symbol getLeftSymbol() const { return leftSymbol; }
	vector<Symbol>& getRightSymbol() { return rightSymbol; }
	const vector<Symbol>& getRightSymbol() const { return rightSymbol; }
	Symbol getForeSymbol() const { return foreSymbol;  }
	int getDotPosition() const { return dotPosition;  }
};

// 判断符号是否为终结符
bool isTerminal(const Symbol theWord) {
	return theWord.getType() == Symbol::SymbolType::T;
}

// 项目集合
class ItemSet {
private:
	int id;
	int size;
	vector<Item> items;
public:
	ItemSet(int id) { 
		this->id = id; 
		size = 0;
		items = vector<Item>();
	}

	ItemSet(int id, vector<Item>& items) {
		this->id = id;
		size = items.size();
		this->items.assign(items.begin(), items.end());
	}

	// 往项目集合中加入新项目
	void joinItem(Item theItem) {
		items.push_back(theItem);
		++size;
	}

	// 网项目集合中载入items
	void loadItems(vector<Item>& items) {
		this->size = items.size();
		this->items.assign(items.begin(), items.end());
	}

	// 重载=
	ItemSet& operator = (ItemSet& otherItemSet) {
		this->id = otherItemSet.getId();
		this->size = otherItemSet.getSize();
		this->items.assign(otherItemSet.getItems().begin(), otherItemSet.getItems().end());
	}

	// 重载==
	bool operator == (const ItemSet& otherItemSet) const {
		if (size != otherItemSet.getId()) return false;
		else {
			for (int i = 0; i < size; ++i) {
				if (items[i] == otherItemSet.getItems()[i]) continue;
				else return false;
			}
		}
		return true;
	}

	friend ostream& operator << (ostream& os, ItemSet& itemSet) {
		cout << "----------------------" << endl;
		cout << "项目集I" << itemSet.getId() << endl;
		cout << "大小：" << itemSet.getSize() << endl;
		vector<Item>& temp = itemSet.getItems();
		for (int i = 0; i < temp.size(); ++i)
			cout << temp[i];
		cout << endl;
		return os;
	}

	int getId() const { return id; }
	int getSize() const { return size; }
	vector<Item>& getItems() { return items; }
	const vector<Item>& getItems() const { return items; }
};

// 定义两个ItemSet, production是产生式，I0是该文法的以ε为活前缀的有效集
ItemSet productions(-1), I0(0);

// 存储符号X的first()集合
map<Symbol, set<Symbol>> wordFirstSet;

// 存储符号X的follow()集合
map<Symbol, set<Symbol>> wordFollowSet;

// first(), 求单个符号的可能的第一个终结符
void first(set<Symbol>& allSymbols, vector<Item>& productions) {
	// 对所有终结符来说，它们的first()集合就是它们自己
	for (auto i = allSymbols.begin(); i != allSymbols.end(); ++i) {
		set<Symbol> temp;
		if (isTerminal(*i)) temp.insert(*i);
		wordFirstSet.insert(pair<Symbol, set<Symbol>>(*i, temp));
	}
	bool change = true;
	while (change) {
		change = false;
		// 遍历每一个产生式
		int len = productions.size();
		for (int i = 0; i < len; ++i) {
			Item theItem = productions[i];
			set<Symbol>& temp = wordFirstSet[theItem.getLeftSymbol()];
			// 产生式的右边第一个符号为终结符
			Symbol firstRight = theItem.firstSymbolOfRight();
			if (isTerminal(firstRight)) {
				if (temp.find(firstRight) == temp.end()) {
					temp.insert(firstRight);
					change = true;
				}
			}
			// 产生式的右边第一个符号为非终结符
			else {
				// 当这个非终结符能够推导出ε时，需要考虑后面一个终结符
				bool next = true;
				int index = 0, lenOfRight = theItem.getRightSymbol().size();
				while (next && index < lenOfRight) {
					next = false;
					set<Symbol>& joinSet = wordFirstSet[theItem.getRightSymbol()[index]];
					//  当前非终结符joinSet包括ε
					if (joinSet.find(EPSILON) != joinSet.end()) {
						next = true;
						++index;
					}
					for (auto j = joinSet.begin(); j != joinSet.end(); ++j) {
						if (!((*j) == EPSILON) && temp.find(*j) == temp.end()) {
							change = true;
							temp.insert(*j);
						}
					}
				}	
				// 此时表示右边产生式的所有符号皆为非终结符且可以推导出ε
				if (index == lenOfRight) {
					temp.insert(EPSILON);
				}
			}
		}
	}
}

// first(), 当前符号串可能的第一个终结符
set<Symbol> first(vector<Symbol> theStr) {
	int len = theStr.size();
	set<Symbol> temp;
	bool next = true;
	int index = 0;
	while (next && index < len) {
		next = false;
		Symbol theWord = theStr[index];
		// 当前字符为终结符，则将此终结符加入temp
		if (isTerminal(theWord)) {
			temp.insert(theWord);
			return temp;
		}
		// 当前字符不是终结符，则根据此字符的first()集合是否含有ε来判断是否需要看下一个字符
		else {
			set<Symbol>& joinSet = wordFirstSet[theStr[index]];
			auto findEpsilon = joinSet.find(EPSILON);
			if (findEpsilon != joinSet.end()) {
				next = true;
				++index;
			}
			for (auto j = joinSet.begin(); j != joinSet.end(); ++j) {
				if (!((*j) == EPSILON) && temp.find(*j) == temp.end()) {
					temp.insert(*j);
				}
			}
		}
	}
	// 当index == len说明此字符串中的每个字符都为非终结符并且可能为ε
	if (index == len) {
		temp.insert(EPSILON);
	}
	return temp;
}

// follow(), 求单个符号的可能的下一个终结符
void follow(set<Symbol>& allSymbols, vector<Item>& productions) {
	// 初始时，每个非终结符的follow()集合都为空
	for (auto i = allSymbols.begin(); i != allSymbols.end(); ++i) {
		if (!isTerminal(*i))
			wordFollowSet[*i] = set<Symbol>();
	}
	// follow(S') = {ε}
	wordFollowSet[BEGIN].insert(END);

	bool change = true;
	int len = productions.size();
	while (change) {
		change = false;
		for (int i = 0; i < len; ++i) {
			// 每个产生式的右侧的符号串
			vector<Symbol>& theRightSymbol = productions[i].getRightSymbol();
			int lenOfRight = theRightSymbol.size();
			// 遍历产生式右侧的符号串的每个符号，只考虑非终结符
			for (int j = 0; j < lenOfRight; ++j) {
				Symbol theWord = theRightSymbol[j];
				if (isTerminal(theWord)) { continue; }
				else {
					// 在set中查出当前非终结符已有的follow()集合，并计算出此非终结符后面的符号串的first()集合
					set<Symbol>& theWordFollow = wordFollowSet[theWord];
					set<Symbol> joinSet = first((productions[i]).allSymbolsAfterIndex(j));
					// 当joinSet中存在一些theWordFollow集合里面没有的终结符，则将它们加入到theWordFollow集合中去
					for (auto k = joinSet.begin(); k != joinSet.end(); ++k) {
						if (theWordFollow.find(*k) == theWordFollow.end() && !((*k) == EPSILON)) {
							change = true;
							theWordFollow.insert(*k);
						}
					}
					// 当joinSet含有为ε（包括当前非终结符后面的字符串的first()集合含有ε或者该字符串本身就为ε）
					auto findEpsilon = joinSet.find(EPSILON);
					if (findEpsilon != joinSet.end()) {
						set<Symbol>& theLeftfollow = wordFollowSet[productions[i].getLeftSymbol()];
						for (auto k = theLeftfollow.begin(); k != theLeftfollow.end(); ++k) {
							if (theWordFollow.find(*k) == theWordFollow.end()) {
								change = true;
								theWordFollow.insert(*k);
							}
						}
					}
				}
			}
		}
	}
}

// 项目是否在项目集中
bool isInItemSet(vector<Item>& items, Item& theItem) {
	int len = items.size();
	for (int i = 0; i < len; ++i) 
		if (items[i] == theItem) return true;
	return false;
}

// 求项目集合的闭包
void closure(ItemSet& theItemSet,const ItemSet& productions) {
	bool change = true;
	while (change) {
		change = false;
		vector<Item>& allItems = theItemSet.getItems();
		int len = allItems.size();
		// 遍历每个项目
		for (int i = 0; i < len; ++i) {
			Symbol theWordAfterDot = allItems[i].symbolAfterDot();
			// dot后面的是一个非终结符
			if (!isTerminal(theWordAfterDot)) {
				int dotIndex = allItems[i].getDotPosition();
				// 此非终结符后面的字符串，以及它的first()集合
				vector<Symbol> wordsAfter = allItems[i].allSymbolsAfterIndex(dotIndex);
				// 将前看符号加入
				wordsAfter.push_back(allItems[i].getForeSymbol());
				set<Symbol> firstOfWordsAfter = first(wordsAfter);
				// 遍历每个产生式
				int lenOfProduction = productions.getSize();
				for (int j = 0; j < lenOfProduction; ++j) {
					Item theProduction = productions.getItems()[j];
					// 产生式中左部非终结符和项目dot后的非终结符相同
					if (theProduction.getLeftSymbol() == theWordAfterDot) {
						// 对于每一个b∈FIRST（βa）
						for (auto k = firstOfWordsAfter.begin(); k != firstOfWordsAfter.end(); ++k) {
							// 构造一个新的item[B-->·η,b]
							Item temp(theWordAfterDot, theProduction.getRightSymbol(), (*k), 0);
							// 当此时的closure中没有这个item，将这个item加入closure
							if (!isInItemSet(allItems, temp)) {
								change = true;
								theItemSet.joinItem(temp);
							}
						}
					}
				}
			}
		}
	}
}


// go(I, X) = closure(J)
ItemSet go(ItemSet I, Symbol theSymbol) {
	vector<Item> temp;
	vector<Item> items = I.getItems();
	int num = I.getId() + 1, len = items.size();
	ItemSet J(num, temp);
	// 遍历I中的所有项目
	for (int i = 0; i < len; ++i) {
		Item theItem = items[i];
		// 当一个项目的dot后的符号为非终结符符且为theSymbol时
		Symbol theSymbolAfterDot = theItem.symbolAfterDot();
		if (theSymbolAfterDot == theSymbol) {
			Item t(theItem.getLeftSymbol(), theItem.getRightSymbol(),
				 theItem.getForeSymbol(), theItem.getDotPosition() + 1);
			J.joinItem(t);
		}
	}
	if (J.getSize() > 0)
		closure(J, productions);
	return J;
}


// 项目集族(文法)
class ItemSetGroup {
private:
	vector<ItemSet> itemSets;
	set<Symbol> allSymbols;
	vector<Item> production;

public:

};



void main() {
	Symbol S(Symbol::SymbolType::NT, 3), E(Symbol::SymbolType::NT, 4), a(Symbol::SymbolType::T, 5),
		b(Symbol::SymbolType::T, 6), e(Symbol::SymbolType::T, 7), i(Symbol::SymbolType::T, 8),
		t(Symbol::SymbolType::T, 9), SS(Symbol::SymbolType::NT, 10);

	set<Symbol> allSymbols;
	allSymbols.insert(SS); allSymbols.insert(S); allSymbols.insert(E); allSymbols.insert(a); allSymbols.insert(b); allSymbols.insert(e); allSymbols.insert(i); allSymbols.insert(t); allSymbols.insert(BEGIN);
	allSymbols.insert(END); allSymbols.insert(EPSILON);

	vector<Symbol> t1, t2, t3, t4, t5, t6;
	t1.push_back(S); 
	t2.push_back(i); t2.push_back(E); t2.push_back(t); t2.push_back(S); t2.push_back(SS); 
	t3.push_back(E); t3.push_back(a);
	t4.push_back(e); t4.push_back(S);
	t5.push_back(EPSILON);
	t6.push_back(b);
	Item p1(BEGIN, t1), p2(S, t2), p3(S, t3), p4(SS, t4), p5(SS, t5), p6(E, t6);

	vector<Item> test;
	test.push_back(p1); test.push_back(p2); test.push_back(p3); test.push_back(p4); test.push_back(p5); test.push_back(p6);

	first(allSymbols, test);
	for (auto i = allSymbols.begin(); i != allSymbols.end(); ++i) {
		set<Symbol> temp = wordFirstSet[*i];
		cout << "符号" << (*i).getId() << " 的first（）：";
		for (auto j = temp.begin(); j != temp.end(); ++j) {
			cout << (*j).getId() << ' ';
		}
		cout << endl;
	}

	cout << endl;
	follow(allSymbols, test);
	for (auto i = allSymbols.begin(); i != allSymbols.end(); ++i) {
		set<Symbol> temp = wordFollowSet[*i];
		cout << "符号" << (*i).getId() << " 的follow（）：";
		for (auto j = temp.begin(); j != temp.end(); ++j) {
			cout << (*j).getId() << ' ';
		}
		cout << endl;
	}
	vector<Item> tt;
	tt.push_back(p1);

	ItemSet theSet(0, tt);
	productions.loadItems(test);
	closure(theSet, productions);
	cout << theSet;

	ItemSet test2 = go(theSet, i);
	cout << test2;
}