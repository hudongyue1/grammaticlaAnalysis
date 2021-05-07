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

// 字符总数目
int charCount = 0;

// 空格数目
int spaceCount = 0;

// 行数记录
int rowCount = 1;

// 错误数目记录
int errorCount = 0;

// 状态
int state = 0;

// 当前读取字符
char thisChar;

// 回退标志
bool isRetract = false;

// token
string token = "";

// 保留字表
vector<pair<string, string>> reservedTable;

// 符号表
set<string> symbolTable;

// 单词数目记录表
map<string, int> wordsCount;

// 错误行号记录
set<int> errorRowCount;

// 将字符连接在token后
void cat(const char& ch) {
	token += ch;
}

// 读取一个字符，同时更新字符总数、行数、空格数目
char getAChar() {
	char ch = getchar();
	if (ch != '\n' && ch != '\t' && ch != ' ' && ch != EOF) ++charCount;
	if (ch == '\n') ++rowCount;
	if (ch == ' ') ++spaceCount;
	return ch;
}

// 设置回退
void retract() {
	isRetract = true;
	return;
}

// 判断是否为字母或者下划线
bool isLetter(const char& ch) {
	if (('a' <= ch && ch <= 'z') || ('A' <= ch && ch <= 'Z'))
		return true;
	else return false;
}

// 判断是否为数字
bool isDigit(const char& ch) {
	if ('0' <= ch && ch <= '9')
		return true;
	else return false;
}

// 检查是否是保留字
string isReserved(const string& token) {
	for (auto str : reservedTable) {
		if (str.first == token)
			return str.second;
	}
	return "";
}

// 错误处理，记录出错行数，并直接跳过该字符
void error() {
	++errorCount;
	errorRowCount.insert(rowCount);
	return;
}

// 输出结果到文件
ofstream fOut("result.txt", ios::out);

// 输出分析 
void pushResult(const string& str, string attribute = "——") {
	fOut << "( " << rowCount << " )       " << setw(15) << left << str << "   " << attribute << endl;
}


int main() {
	// 载入翻译表
	ifstream fReservedTable("reservedTable.txt", ios::in);
	string t1, t2;
	if (!fReservedTable.is_open())
		cout << "无法载入翻译表！！" << endl;
	while (!fReservedTable.eof()) {
		fReservedTable >> t1 >> t2;
		reservedTable.push_back(make_pair(t1, t2));
	}
	fReservedTable.close();

	// 打开测试文件
	if (freopen("test1.txt", "r", stdin) == NULL) {
		cout << "无法打开测试文件！！" << endl;
		return 0;
	}

	fOut << "词法分析结果：\n" << endl;
	fOut << "行数：    记号            属性" << endl;
	do {
		switch (state) {
		case 0: {
			token = "";
			// 获取下一个字符
			if (isRetract) {
				isRetract = false;
			}
			else thisChar = getAChar();
			while (thisChar == ' ' || thisChar == '\t' || thisChar == '\n') {
				thisChar = getAChar();
			}

			// 判断thisChar
			if (isLetter(thisChar) || thisChar == '_') {
				state = 1;
			}
			else if ('1' <= thisChar && thisChar <= '9') {
				state = 2;
			}
			else if (thisChar == '0') {
				state = 8;
			}
			else if (thisChar == '=') {
				state = 12;
			}
			else if (thisChar == '<') {
				state = 13;
			}
			else if (thisChar == '>') {
				state = 14;
			}
			else if (thisChar == '&') {
				state = 15;
			}
			else if (thisChar == '|') {
				state = 16;
			}
			else if (thisChar == '/') {
				state = 17;
			}
			else if (thisChar == '!' || thisChar == ':' || thisChar == '.' || thisChar == ',' || thisChar == ';' || thisChar == '('
				|| thisChar == ')' || thisChar == '[' || thisChar == ']' || thisChar == '{' || thisChar == '}') {
				string temp = "";
				temp += thisChar;
				++wordsCount[temp];
				pushResult(temp);
				state = 0;
			}
			else if (thisChar == '\'') {
				string temp = "";
				thisChar = getAChar();
				temp += thisChar;
				// 判断\n、\t等情况
				if (thisChar == '\\') {
					thisChar = getAChar();
					temp += thisChar;
					thisChar = getAChar();
					if (thisChar == '\'') {
						++wordsCount["word"];
						pushResult("word", temp);
					}
					else {
						error();
						state = 0;
					}
				}
				else {// 判断char型
					thisChar = getAChar();
					if (thisChar == '\'') {
						++wordsCount["word"];
						if (temp == " ")
							pushResult("word", "space");
						else
							pushResult("word", temp);
					}
					else {
						error();
						state = 0;
					}
				}
			}
			else if (thisChar == '\"') {
				string temp = "";
				while (1) {
					thisChar = getAChar();
					if (thisChar == EOF || thisChar == '\n') {
						error();
						state = 0;
						break;
					}
					else if (thisChar == '\"') {
						++wordsCount["str"];
						pushResult("str", temp);
						state = 0;
						break;
					}
					temp += thisChar;
				}
			}
			else if (thisChar == '+' || thisChar == '-' || thisChar == '*') {
				string temp = "";
				temp += thisChar;
				++wordsCount[temp];
				pushResult(isReserved(temp), temp);
				state = 0;
			}
			else {
				state = 18;
			}
			break;
		}
		case 1: {
			cat(thisChar);
			thisChar = getAChar();
			if (isLetter(thisChar) || isDigit(thisChar) || thisChar == '_') state = 1;
			else {
				// 回退一格
				retract();
				// 回到状态0
				state = 0;
				string signal = isReserved(token);
				if (signal != "") {// 保留字
					++wordsCount[token];
					pushResult(signal);
				}
				else {// 非保留字
					++wordsCount["id"];
					auto ptr = symbolTable.find(token);
					if (ptr == symbolTable.end()) {
						symbolTable.insert(token);
					}
					pushResult(isReserved("id"), token);
				}
			}
			break;
		}

		case 2: {
			cat(thisChar);
			thisChar = getAChar();
			if (isDigit(thisChar)) state = 2;
			else if (thisChar == 'E') state = 5;
			else if (thisChar == '.') state = 3;
			else {
				++wordsCount["num"];
				retract();
				state = 0;
				pushResult("num", token);
			}
			break;
		}

		case 3: {
			cat(thisChar);
			thisChar = getAChar();
			if (isDigit(thisChar)) state = 4;
			else {
				error();
				state = 0;
			}
			break;
		}

		case 4: {
			cat(thisChar);
			thisChar = getAChar();
			if (isDigit(thisChar)) state = 4;
			else if (thisChar == 'E') state = 5;
			else {
				++wordsCount["num"];
				retract();
				state = 0;
				pushResult("num", token);
			}
			break;
		}

		case 5: {
			cat(thisChar);
			thisChar = getAChar();
			if (isDigit(thisChar)) state = 7;
			else if (thisChar == '+' || thisChar == '-') state = 6;
			else {
				error();
				state = 0;
			}
			break;
		}

		case 6: {
			cat(thisChar);
			thisChar = getAChar();
			if (isDigit(thisChar)) state = 7;
			else {
				error();
				state = 0;
			}
			break;
		}

		case 7: {
			cat(thisChar);
			thisChar = getAChar();
			if (isDigit(thisChar)) state = 7;
			else {
				++wordsCount["num"];
				retract();
				state = 0;
				pushResult("num", token);
			}
			break;
		}

		case 8: {
			cat(thisChar);
			thisChar = getAChar();
			if (thisChar == '.') state = 3;
			else if ('0' <= thisChar && thisChar <= '7') state = 9;
			else if (thisChar == 'x' || thisChar == 'X') state = 10;
			else if ('8' <= thisChar && thisChar <= '9') { error(); state = 0; }
			else {
				++wordsCount["num"];
				retract();
				state = 0;
				pushResult("num", token);
			}
			break;
		}

		case 9: {
			cat(thisChar);
			thisChar = getAChar();
			if ('0' <= thisChar && thisChar <= '7') state = 9;
			else {
				++wordsCount["num"];
				retract();
				state = 0;
				pushResult("num", token);
			}
			break;
		}

		case 10: {
			cat(thisChar);
			thisChar = getAChar();
			if (('a' <= thisChar && thisChar <= 'f') || ('A' <= thisChar && thisChar <= 'F')
				|| isDigit(thisChar)) state = 11;
			else {
				error();
				state = 0;
			}
			break;
		}

		case 11: {
			cat(thisChar);
			thisChar = getAChar();
			if (('a' <= thisChar && thisChar <= 'f') || ('A' <= thisChar && thisChar <= 'F')
				|| isDigit(thisChar)) state = 11;
			else {
				++wordsCount["num"];
				retract();
				state = 0;
				pushResult("num", token);
			}
			break;
		}

		case 12: {
			cat(thisChar);
			thisChar = getAChar();
			if (thisChar == '=') {
				++wordsCount["=="];
				cat(thisChar);
				pushResult(isReserved("=="), token);
			}
			else {
				++wordsCount["="];
				pushResult(isReserved("="));
				retract();
			}
			state = 0;
			break;
		}

		case 13: {
			cat(thisChar);
			thisChar = getAChar();
			if (thisChar == '=') {
				++wordsCount["<="];
				cat(thisChar);
				pushResult(isReserved("<="), token);
			}
			else if (thisChar == '>') {
				++wordsCount["<>"];
				cat(thisChar);
				pushResult(isReserved("<>"), token);
			}
			else {
				++wordsCount["<"];
				pushResult(isReserved("<"), token);
				retract();
			}
			state = 0;
			break;
		}

		case 14: {
			cat(thisChar);
			thisChar = getAChar();
			if (thisChar == '=') {
				++wordsCount[">="];
				cat(thisChar);
				pushResult(isReserved(">="), token);
			}
			else {
				++wordsCount[">"];
				pushResult(isReserved(">"), token);
				retract();
			}
			state = 0;
			break;
		}

		case 15: {
			cat(thisChar);
			thisChar = getAChar();
			if (thisChar == '&') {
				++wordsCount["&&"];
				cat(thisChar);
				pushResult(isReserved("&&"), token);
			}
			else {
				++wordsCount["&"];
				pushResult(isReserved("&"), token);
				retract();
			}
			state = 0;
			break;
		}

		case 16: {
			cat(thisChar);
			thisChar = getAChar();
			if (thisChar == '|') {
				++wordsCount["||"];
				cat(thisChar);
				pushResult(isReserved("||"), token);
			}
			else {
				++wordsCount["|"];
				pushResult(isReserved("|"), token);
				retract();
			}
			state = 0;
			break;
		}

		case 17: {
			cat(thisChar);
			thisChar = getAChar();
			// 遇见单行注释符(//)
			if (thisChar == '/') {
				while (thisChar != '\n' && thisChar != EOF) {

					thisChar = getAChar();
				}
			}
			else if (thisChar == '*') {// 遇见段注释符(/*)
				while (1) {
					thisChar = getAChar();
					if (thisChar == EOF) {
						error();
						break;
					}
					else if (thisChar == '*') {
						thisChar = getAChar();
						if (thisChar == '/') {
							break;
						}
					}
				}
			}
			else {
				++wordsCount["/"];
				pushResult(isReserved("/"), token);
				retract();
			}
			state = 0;
			break;
		}

		case 18: {
			error();
			state = 0;
			break;
		}

		default: {
			error();
			state = 0;
			break;
		}
		}
	} while (thisChar != EOF);
	fclose(stdin);
	cout << "          词法分析结束！！        " << endl;
	cout << "----------------------------------" << endl;
	// 统计数据输出
	cout << "统计数据输出：" << endl;
	cout << "行数：" << rowCount << endl;
	cout << "字符总数：" << charCount << endl;
	cout << "空格数目：" << spaceCount << endl;

	// 符号表输出
	cout << "----------------------------------" << endl;
	cout << "符号表" << endl;
	for (auto t : symbolTable) {
		cout << " (  " << setw(10) << left << t << "  ) " << endl;
	}
	// 单词数目记录表输出
	cout << "----------------------------------" << endl;
	cout << "单词数目记录(单词：出现次数)!" << endl;
	for (auto t : wordsCount) {
		cout << "  " << setw(20) << left << t.first << t.second << endl;
	}
	// 错误记录输出
	cout << "----------------------------------" << endl;
	cout << "错误记录" << endl;
	cout << "错误总数：" << errorCount << endl;
	cout << "错误行数：";
	for (auto t : errorRowCount) {
		cout << t << " ";
	}
	cout << endl;


	return 0;
}
