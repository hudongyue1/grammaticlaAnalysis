#define _CRT_SECURE_NO_WARNINGS
#include<iostream>
#include<string>
#include<vector>
#include<algorithm>
#include<map>
#include<set>
#include<queue>
#include<stack>
#include<fstream>
#include<iomanip>

using namespace std;

// 存储翻译表
map<int, string> numToStr;
map<string, int> strToNum;

string symIdToStr(int num) {
	return numToStr[num];
}

int strToSymId(string str) {
	return strToNum[str];
}

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

	Symbol& operator = (const Symbol& otherSymbol) {
		this->type = otherSymbol.getType();
		this->id = otherSymbol.getId();
		return *this;
	}

	bool operator < (const Symbol& otherSymbol) const {
		return this->id < otherSymbol.id;
	}

	bool operator == (const Symbol& otherSymbol) const {
		return this->id == otherSymbol.id;
	}

	// 输出Symbol
	friend ostream& operator << (ostream& os, const Symbol& word) {
		os << symIdToStr(word.getId());
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
	Item(Symbol leftSymbol, vector<Symbol> rightSymbol, Symbol foreSymbol = END, int dotPosition = 0) {
		this->leftSymbol = leftSymbol;
		this->rightSymbol = rightSymbol;
		this->foreSymbol = foreSymbol;
		this->dotPosition = dotPosition;
	}

	bool operator == (const Item& otherItem) const {
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
		}
		else return false;
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
		for (int i = index + 1; i < len; ++i)
			temp.push_back(rightSymbol[i]);
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
		if (item.getDotPosition() == len) cout << "· ";
		cout << "," << item.getForeSymbol() << ']';
		return os;
	}

	Symbol getLeftSymbol() const { return leftSymbol; }
	vector<Symbol>& getRightSymbol() { return rightSymbol; }
	const vector<Symbol>& getRightSymbol() const { return rightSymbol; }
	Symbol getForeSymbol() const { return foreSymbol; }
	int getDotPosition() const { return dotPosition; }
};


// 项目集合
class ItemSet {
private:
	int id;
	vector<Item> items;
public:
	ItemSet() {}
	ItemSet(int id) {
		this->id = id;
		items = vector<Item>();
	}

	ItemSet(int id, vector<Item>& items) {
		this->id = id;
		this->items.assign(items.begin(), items.end());
	}

	ItemSet& operator = (ItemSet& otherItemSet) {
		this->id = otherItemSet.getId();
		this->items.assign(otherItemSet.getItems().begin(), otherItemSet.getItems().end());
		return *this;
	}

	// 修改项目集合id
	void setId(int id) {
		this->id = id;
	}

	// 往项目集合中加入新项目
	void joinItem(Item theItem) {
		items.push_back(theItem);
	}

	// 往项目集合中载入items
	void loadItems(vector<Item>& items) {
		this->items.assign(items.begin(), items.end());
	}

	// 重载=
	ItemSet& operator = (const ItemSet& otherItemSet) {
		this->items.assign(otherItemSet.getItems().begin(), otherItemSet.getItems().end());
		return *this;
	}

	// 重载==
	bool operator == (const ItemSet& otherItemSet) const {
		int size = this->getSize();
		if (size != otherItemSet.getSize()) return false;
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
	int getSize() const { return this->items.size(); }
	vector<Item>& getItems() { return items; }
	const vector<Item>& getItems() const { return items; }
};

// 项目集族(文法)
class ItemSetGroup {
private:
	vector<ItemSet> itemSets;

public:
	ItemSetGroup() {}

	void loadItemSets(vector<ItemSet>& itemSets) {
		this->itemSets.assign(itemSets.begin(), itemSets.end());
	}

	friend ostream& operator << (ostream& os, ItemSetGroup& itemSet) {
		cout << "ItemSetGrop:" << endl;
		int len = itemSet.getItemSetSize();
		vector<ItemSet>& temp = itemSet.getItemSets();
		for (int i = 0; i < len; ++i) {
			cout << temp[i] << endl;
		}
		return os;
	}

	int getItemSetSize() { return itemSets.size(); }
	vector<ItemSet>& getItemSets() { return itemSets; }
};

// 存储符号X的first()集合
map<Symbol, set<Symbol>> wordFirstSet;

// 存储符号X的follow()集合
map<Symbol, set<Symbol>> wordFollowSet;

// 所有的符号和产生式
set<Symbol> allSymbols;
vector<Item> productions;

// DFA图
vector<set<pair<Symbol, int>>> DFARecord;

// pair.first表示操作类型0->Wrong 1->S 2->R 3->ACK; 
// second表示转移状态或者产生式序号
vector<map<Symbol, pair<int, int>>> action;

// goto
vector<map<Symbol, int>> goTo;

// 判断符号是否为终结符
bool isTerminal(const Symbol theWord) {
	return theWord.getType() == Symbol::SymbolType::T;
}

// 项目是否在项目集中
bool isInItemSet(vector<Item>& items, Item& theItem) {
	int len = items.size();
	for (int i = 0; i < len; ++i)
		if (items[i] == theItem) return true;
	return false;
}

// 项目集是否在项目集族中
int isInItemSetGroup(vector<ItemSet>& itemSets, ItemSet& theItemSet) {
	int len = itemSets.size();
	for (int i = 0; i < len; ++i) {
		if (itemSets[i] == theItemSet) {
			return itemSets[i].getId();
		}
	}
	return -1;
}

// 根据编号在allSymbols中查找symbol
Symbol searchSymbol(int id) {
	set<Symbol>& temp = allSymbols;
	for (auto i = temp.begin(); i != temp.end(); ++i) {
		if ((*i).getId() == id) return (*i);
	}
	return EPSILON;
}

// first(), 求单个符号的可能的第一个终结符
void first() {
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
			// 空字符直接跳过
			//if (theWord == EPSILON) {
			//	++index;
			//	continue;
			//}
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
void follow() {
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


// 求项目集合的闭包
void closure(ItemSet& theItemSet) {
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
				int lenOfProduction = productions.size();
				for (int j = 0; j < lenOfProduction; ++j) {
					Item theProduction = productions[j];
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
ItemSet go(ItemSet& I, Symbol theSymbol) {
	vector<Item> temp;
	vector<Item> items = I.getItems();
	int num = I.getId() + 1, len = items.size();
	ItemSet J(num, temp);
	// 遍历I中的所有项目
	for (int i = 0; i < len; ++i) {
		Item theItem = items[i];
		// 当一个项目的dot后的符号为非终结符符且为theSymbol时
		Symbol theSymbolAfterDot = theItem.symbolAfterDot();
		if (!theItem.isReductionItem() && theSymbolAfterDot == theSymbol) {
			Item t(theItem.getLeftSymbol(), theItem.getRightSymbol(),
				theItem.getForeSymbol(), theItem.getDotPosition() + 1);
			J.joinItem(t);
		}
	}
	if (J.getSize() > 0)
		closure(J);
	return J;
}

// 构造项目集族的DFA，已经在itemSetGroup中载入了allSymbols、productions以及BEGIN->S
void DFA(ItemSetGroup& itemSetGroup) {
	// 待扩展队列
	queue<ItemSet> waitExpansion;
	// 获取项目集族的各个数据结构
	vector<ItemSet>& itemSets = itemSetGroup.getItemSets();
	ItemSet& I0 = itemSets[0];
	// 对BEGIN->S求closure，得到I0;
	closure(I0);
	cout << "closure后:" << endl;
	cout << I0;
	// 将I0加入待扩展队列
	waitExpansion.push(I0);
	while (!waitExpansion.empty()) {
		ItemSet& t = waitExpansion.front();
		set<pair<Symbol, int>> thisTran;
		for (auto i = allSymbols.begin(); i != allSymbols.end(); ++i) {
			Symbol theWord = (*i);
			ItemSet temp = go(t, theWord);
			if (temp.getSize() > 0) {
				int id = isInItemSetGroup(itemSets, temp);
				// 项目集族中存在该项目集
				if (id >= 0) {}
				// 项目集族中不存在该项目，将该项目集加入
				else {
					id = itemSets.size();
					temp.setId(id);
					itemSets.push_back(temp);
					waitExpansion.push(temp);
				}
				thisTran.insert(pair<Symbol, int>(theWord, id));
			}
		}
		DFARecord.push_back(thisTran);
		waitExpansion.pop();
	}

	// 打印DFA
	cout << "DFA: " << endl;
	for (int i = 0; i < DFARecord.size(); ++i) {
		cout << "有效项目集I" << i << "的转移:" << endl;
		set<pair<Symbol, int>>& temp = DFARecord[i];
		for (auto j = temp.begin(); j != temp.end(); ++j) {
			cout << (*j).first << "---" << (*j).second << endl;
		}
		cout << endl;
	}
}

void buildAnalysisTable(ItemSetGroup& itemSetGroup) {
	// 获取项目集族的各个数据结构
	vector<ItemSet>& itemSets = itemSetGroup.getItemSets();

	int itemSetSize = itemSetGroup.getItemSetSize();

	// 更改action、goto表大小
	action.resize(itemSetSize);
	for (int i = 0; i < itemSetSize; ++i) {
		for (auto j = allSymbols.begin(); j != allSymbols.end(); ++j) {
			if (isTerminal(*j) && !((*j) == EPSILON)) {
				action[i][*j].first = 0;
				action[i][*j].second = -1;
			}
		}
	}

	goTo.resize(itemSetSize);
	for (int i = 0; i < itemSetSize; ++i) {
		for (auto j = allSymbols.begin(); j != allSymbols.end(); ++j) {
			if (!isTerminal(*j) && !((*j) == BEGIN)) {
				goTo[i][*j] = -1;
			}
		}
	}

	// 遍历项目集族中的每一个项目集
	for (int i = 0; i < itemSetSize; ++i) {
		// 遍历每个项目集中的每个项目
		vector<Item>& tempItems = itemSets[i].getItems();
		for (int j = 0; j < tempItems.size(); ++j) {
			Item& tempItem = tempItems[j];
			// 非规约项目
			if (!tempItem.isReductionItem()) {
				Symbol charAfterDot = tempItem.symbolAfterDot();
				set<pair<Symbol, int>>& tempTran = DFARecord[i];
				// charAfterDot是终结符
				if (isTerminal(charAfterDot) && !(charAfterDot == EPSILON)) {
					for (auto k = tempTran.begin(); k != tempTran.end(); ++k) {
						if ((*k).first == charAfterDot) {
							action[i][charAfterDot].first = 1; // 1->S
							action[i][charAfterDot].second = (*k).second;
							break;
						}
					}
				}
				// charAfterDot是非终结符
				else if (!isTerminal(charAfterDot)) {
					for (auto k = tempTran.begin(); k != tempTran.end(); ++k) {
						if ((*k).first == charAfterDot) {
							goTo[i][charAfterDot] = (*k).second;
							break;
						}
					}
				}
			}
			// 规约项目
			else {
				// 接受项目
				if (tempItem.getLeftSymbol() == BEGIN && tempItem.getForeSymbol() == END)
					action[i][END].first = 3; //3->ACK
				// 规约
				else {
					for (int k = 0; k < productions.size(); ++k) {
						if (productions[k].getLeftSymbol() == tempItem.getLeftSymbol() && productions[k].getRightSymbol() == tempItem.getRightSymbol()) {
							action[i][tempItem.getForeSymbol()].first = 2; // 2->R
							action[i][tempItem.getForeSymbol()].second = k;
							break;
						}
					}
				}
			}
		}
	}
}

// split分隔函数
void SplitString(const string& s, vector<string>& v, const string& c)
{
	string::size_type pos1, pos2;
	pos2 = s.find(c);
	pos1 = 0;
	while (string::npos != pos2)
	{
		v.push_back(s.substr(pos1, pos2 - pos1));
		pos1 = pos2 + c.size();
		pos2 = s.find(c, pos1);
	}
	if (pos1 != s.length())
		v.push_back(s.substr(pos1));
}

void initItemSetGroup(ItemSetGroup& itemSetGroup) {
	// 载入
	ifstream fTranslateTable("translateTable1.txt", ios::in);
	string s1, s2, s3;
	Symbol tempSymbol;
	int num;

	// 载入翻译表和符号表
	if (!fTranslateTable.is_open())
		cout << "无法载入翻译表！！" << endl;
	while (!fTranslateTable.eof()) {
		fTranslateTable >> s1 >> s2 >> s3;
		num = atoi(s1.c_str());
		numToStr.insert({ num, s3 });
		strToNum.insert({ s3, num });
		if (s2 == "T") allSymbols.insert(Symbol(Symbol::SymbolType::T, num));
		else allSymbols.insert(Symbol(Symbol::SymbolType::NT, num));
	}

	// 载入文法(产生式)
	ifstream fGrammer("grammer1.txt", ios::in);
	if (!fGrammer.is_open())
		cout << "无法载入文法" << endl;
	// left - right1 right2 right3
	while (getline(fGrammer, s1)) {
		vector<string> tempVec;
		Symbol left;
		vector<Symbol> right;
		Symbol findSymbol;
		SplitString(s1, tempVec, " ");
		for (int i = 0; i < tempVec.size(); ++i) {
			if (tempVec[i] == "->") continue;
			else if (i == 0) {
				findSymbol = searchSymbol(strToSymId(tempVec[i]));
				if (!(findSymbol == EPSILON))
					left = findSymbol;
			}
			else {
				findSymbol = searchSymbol(strToSymId(tempVec[i]));
				right.push_back(findSymbol);
			}
		}
		Item tempItem(left, right);
		productions.push_back(tempItem);
	}

	cout << "allSymbols:" << endl;
	for (auto i = allSymbols.begin(); i != allSymbols.end(); ++i) {
		cout << (*i) << endl;
	}
	cout << endl;

	cout << "productions:" << endl;
	for (int i = 0; i < productions.size(); ++i) {
		cout << productions[i] << endl;
	}
	cout << endl;

	first();

	for (auto i = allSymbols.begin(); i != allSymbols.end(); ++i) {
		set<Symbol> temp = wordFirstSet[*i];
		cout << "符号" << symIdToStr((*i).getId()) << " 的first（）：";
		for (auto j = temp.begin(); j != temp.end(); ++j) {
			cout << symIdToStr((*j).getId()) << ' ';
		}
		cout << endl;
	}

	cout << endl;
	follow();
	for (auto i = allSymbols.begin(); i != allSymbols.end(); ++i) {
		set<Symbol> temp = wordFollowSet[*i];
		cout << "符号" << symIdToStr((*i).getId()) << " 的follow（）：";
		for (auto j = temp.begin(); j != temp.end(); ++j) {
			cout << symIdToStr((*j).getId()) << ' ';
		}
		cout << endl;
	}

	// 构造ItemSetGroup
	Symbol s = searchSymbol(3);
	vector<Symbol> sr; sr.push_back(s);
	Item p1(BEGIN, sr);
	vector<Item> tt;
	tt.push_back(p1);
	ItemSet I0(0, tt);

	vector<ItemSet> itemSets;
	itemSets.push_back(I0);
	itemSetGroup.loadItemSets(itemSets);
}

void printTable(ItemSetGroup& itemSetGroup) {
	cout << "action:\t";
	for (int i = 0; i < action[0].size(); ++i) cout << "\t";
	cout << " goto:" << endl << "\t";
	// 先打印非终结符
	for (auto j = action[0].begin(); j != action[0].end(); ++j) {
		cout << symIdToStr((*j).first.getId()) << "\t";
	}
	cout << "|\t";
	for (auto j = goTo[0].begin(); j != goTo[0].end(); ++j) {
		cout << symIdToStr((*j).first.getId()) << "\t";
	}
	cout << endl;
	for (int i = 0; i < itemSetGroup.getItemSetSize(); ++i) {
		cout << "I" << i << "\t";
		map<Symbol, pair<int, int>>& tempMap1 = action[i];
		for (auto j = tempMap1.begin(); j != tempMap1.end(); ++j) {
			if ((*j).second.first == 1) { // 1->S
				cout << "S" << (*j).second.second << "\t";
			}
			else if ((*j).second.first == 2) { // 2->R
				cout << "R" << (*j).second.second << "\t";
			}
			else if ((*j).second.first == 3) { // 3->ACK
				cout << "ACK" << "\t";
			}
			else { // 0->Wrong
				cout << "\t";
			}
		}
		cout << "|\t";
		map<Symbol, int>& tempMap2 = goTo[i];
		for (auto j = tempMap2.begin(); j != tempMap2.end(); ++j) {
			if ((*j).second == -1) { // 1->S
				cout << "\t";
			}
			else { // 0->Wrong
				cout << (*j).second << "\t";
			}
		}
		cout << endl;
	}
}

void process() {
	stack<int> stateStk;
	stateStk.push(0);
	//stack<Symbol> symbolStk;
	int ip = 0;
	int state;
	Symbol tempSymbol;
	do {
		//vector<map<Symbol, pair<int, int>>> action;
		//vector<map<Symbol, int>> goTo;
		tempSymbol = testStr[ip];
		state = stateStk.top();
		if (action[state][tempSymbol].first == 1) { // 1->S
			cout << tempSymbol;
			stateStk.push(action[state][tempSymbol].second);
			++ip;
		}
		else if (action[state][tempSymbol].first == 2) { // 2->R
			Item& production = productions[action[state][tempSymbol].second];
			cout << production << endl;
			for (int i = 0; i < production.getRightSymbol().size(); ++i) {
				stateStk.pop();
			}
			state = stateStk.top();
			stateStk.push(goTo[state][production.getLeftSymbol()]);
		}
		else if (action[state][tempSymbol].first == 3) { // 3->ACK
			cout << "ACK!\n" << endl;
			return;
		}
		else { // 0->Wrong
			cout << "Error!\n" << endl;
			++ip;
			return;
		}
	} while (1);
}

void main() {
	// 有效项目集族
	ItemSetGroup itemSetGroup;
	initItemSetGroup(itemSetGroup);

	DFA(itemSetGroup);

	cout << itemSetGroup << endl;

	buildAnalysisTable(itemSetGroup);

	printTable(itemSetGroup);

	//vector<map<Symbol, pair<int, int>>> action;
	//vector<map<Symbol, int>> goTo;
	// 打印action和goto表

}