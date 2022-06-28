#include <stdio.h>
#include <stack>
#include <vector>
#include <iostream>
#include <fstream>
#include <string>
#include <malloc.h>
using namespace std;
//token结构
struct token {
    int code; //token的类别, code为1则是符号，为2则是标识符或者常数
    char value; //token的值
};
typedef struct token tokens;
vector<tokens> tokenBuffer; //用于存储token的缓冲区
stack<tokens> tokenBuffers;
vector<char> token_value; //用于存储词法分析的的标识符和常数结果

//产生式结构
struct formula {
    int id; //产生式编号
    char left; //产生式左部
    char right[32]; //产生式右部
    int r_length; //产生式右部长度
};
typedef struct formula formulas;
formulas formulaBuffer[11]; //用于存储产生式的缓冲区

//四元式结构
struct quaternion {
    char symbol; //操作符
    char num1; //第一个操作数
    char num2; //第二个操作数
    char result; //结果变量
};
typedef struct quaternion quaternions;
vector<quaternions> quaternionBuffer; //用于存储四元式的缓冲区
int quaternionCount = 0; //四元式个数

//分析表中每一项的结构
struct analysisTable {
    char currentState; //分析栈的栈顶符号
    char currentToken; //当前字符
    int quaternionNum; //对应的产生式编号
};
typedef struct analysisTable analysisTables;
vector<analysisTables> analysisTableBuffer; //LL1分析表的缓冲区
stack<char> analysisStack; //分析栈
stack<char> sematicStack; //语义栈

//初始化LL1分析表
void initialAnalysisTableBuffer() {
    for(int i = 0; i < token_value.size(); i++){
        analysisTables* temp1 = new analysisTable;
        temp1->quaternionNum = 1;
        temp1->currentState = 'E';
        temp1->currentToken = token_value[i];
        analysisTableBuffer.push_back(*temp1);
    }

    analysisTables* temp2 = new analysisTable;
    temp2->quaternionNum = 1;
    temp2->currentState = 'E';
    temp2->currentToken = '(';
    analysisTableBuffer.push_back(*temp2);

    analysisTables* temp3 = new analysisTable;
    temp3->quaternionNum = 2;
    temp3->currentState = 'L';
    temp3->currentToken = '+';
    analysisTableBuffer.push_back(*temp3);

    analysisTables* temp4 = new analysisTable;
    temp4->quaternionNum = 3;
    temp4->currentState = 'L';
    temp4->currentToken = '-';
    analysisTableBuffer.push_back(*temp4);

    analysisTables* temp5 = new analysisTable;
    temp5->quaternionNum = 4;
    temp5->currentState = 'L';
    temp5->currentToken = ')';
    analysisTableBuffer.push_back(*temp5);

    analysisTables* temp6 = new analysisTable;
    temp6->quaternionNum = 4;
    temp6->currentState = 'L';
    temp6->currentToken = '#';
    analysisTableBuffer.push_back(*temp6);

    for(int i = 0; i < token_value.size(); i++){
        analysisTables* temp7 = new analysisTable;
        temp7->quaternionNum = 5;
        temp7->currentState = 'T';
        temp7->currentToken = token_value[i];
        analysisTableBuffer.push_back(*temp7);
    } 

    analysisTables* temp8 = new analysisTable;
    temp8->quaternionNum = 5;
    temp8->currentState = 'T';
    temp8->currentToken = '(';
    analysisTableBuffer.push_back(*temp8);

    analysisTables* temp9 = new analysisTable;
    temp9->quaternionNum = 8;
    temp9->currentState = 'M';
    temp9->currentToken = '+';
    analysisTableBuffer.push_back(*temp9);

    analysisTables* temp10 = new analysisTable;
    temp10->quaternionNum = 8;
    temp10->currentState = 'M';
    temp10->currentToken = '-';
    analysisTableBuffer.push_back(*temp10);

    analysisTables* temp11 = new analysisTable;
    temp11->quaternionNum = 6;
    temp11->currentState = 'M';
    temp11->currentToken = '*';
    analysisTableBuffer.push_back(*temp11);

    analysisTables* temp12 = new analysisTable;
    temp12->quaternionNum = 7;
    temp12->currentState = 'M';
    temp12->currentToken = '/';
    analysisTableBuffer.push_back(*temp12);

    analysisTables* temp13 = new analysisTable;
    temp13->quaternionNum = 8;
    temp13->currentState = 'M';
    temp13->currentToken = ')';
    analysisTableBuffer.push_back(*temp13);

    analysisTables* temp14 = new analysisTable;
    temp14->quaternionNum = 8;
    temp14->currentState = 'M';
    temp14->currentToken = '#';
    analysisTableBuffer.push_back(*temp14);

    for(int i = 0; i < token_value.size(); i++){
        analysisTables* temp15 = new analysisTable;
        temp15->quaternionNum = 9;
        temp15->currentState = 'F';
        temp15->currentToken = token_value[i];
        analysisTableBuffer.push_back(*temp15);
    }

    analysisTables* temp16 = new analysisTable;
    temp16->quaternionNum = 10;
    temp16->currentState = 'F';
    temp16->currentToken = '(';
    analysisTableBuffer.push_back(*temp16);    
}

void produce_tokenfile(){
    fstream fs("tokens.txt", ios::out);
    if(!fs.is_open()) {
        cout << "error when produce tokens!" << endl;
    }
    string str1,str;
    while(cin>>str1)
    {
        //读入代码（字符串形式）
        str = str+' '+str1;
    }
    //开始处理读入的代码
    int length_str = str.length();
    for(int i=0;i<length_str;i++)
    {
        //当遇到空格或换行时，跳过继续执行
        if(str[i]==' ' || str[i]=='\n' || str[i]=='\t') continue;
        //识别常数
        else if(isdigit(str[i]))
        {
            char digit = str[i];
            token_value.push_back(digit);
            fs<<"2 "<<digit<<endl;
        }
        else if(isalpha(str[i]))
        {
            char lett = str[i];
            token_value.push_back(lett);
            fs<<"2 "<<lett<<endl;
        }
        else
        {
            switch(str[i])
            {
            case '+':
                fs<<"1 +"<<endl;
                break;
            case '-':
                fs<<"1 -"<<endl;
                break;
            case '*':
                fs<<"1 *"<<endl;
                break;
            case '/':
                fs<<"1 /"<<endl;
                break;
            case '(':
                fs<<"1 ("<<endl;
                break;
            case ')':
                fs<<"1 )"<<endl;
                break;
            //错误处理
            default:
                fs<<"error"<<endl;
                break;
            }
        }
    }
    fs<<"1 #"<<endl;
    cout<<"Succeed in producing the tokens."<<endl;
}

//初始化token缓冲区
void initialTokenBuffer() {
    ifstream fin("tokens.txt");
    char temp[30][5];
    int i = 0;
    while (!fin.eof()) {
        fin.getline(temp[i], 4);
        i++;
    }
    fin.close();
    do {
        i--;
        tokens *token_temp = new tokens();
        token_temp->code = atoi(&temp[i][0]);
        token_temp->value = temp[i][2];
        tokenBuffer.push_back(*token_temp);
        tokenBuffers.push(*token_temp);
    } while (i != 0);
}

//初始化产生式缓冲区
void initialFormulaBuffer() {
    formulas *fTemp1 = new formula;
    formulas *fTemp2 = new formula;
    formulas *fTemp3 = new formula;
    formulas *fTemp4 = new formula;
    formulas *fTemp5 = new formula;
    formulas *fTemp6 = new formula;
    formulas *fTemp7 = new formula;
    formulas *fTemp8 = new formula;
    formulas *fTemp9 = new formula;
    formulas *fTemp10 = new formula;
    fTemp1->id = 1;
    fTemp1->left = 'E';
    fTemp1->r_length = 2;
    fTemp1->right[0] = 'T';
    fTemp1->right[1] = 'L';
    fTemp1->right[2] = '\0';

    fTemp2->id = 2;
    fTemp2->left = 'L';
    fTemp2->r_length = 4;
    fTemp2->right[0] = '+';
    fTemp2->right[1] = 'T';
    fTemp2->right[2] = 'Y';
    fTemp2->right[3] = 'L';
    fTemp2->right[4] = '\0';

    fTemp3->id = 3;
    fTemp3->left = 'L';
    fTemp3->r_length = 4;
    fTemp3->right[0] = '-';
    fTemp3->right[1] = 'T';
    fTemp3->right[2] = 'U';
    fTemp3->right[3] = 'L';
    fTemp3->right[4] = '\0';

    fTemp4->id = 4;
    fTemp4->left = 'L';
    fTemp4->r_length = 0;
    //fTemp->right[0] = "N";

    fTemp5->id = 5;
    fTemp5->left = 'T';
    fTemp5->r_length = 2;
    fTemp5->right[0] = 'F';
    fTemp5->right[1] = 'M';

    fTemp6->id = 6;
    fTemp6->left = 'M';
    fTemp6->r_length = 4;
    fTemp6->right[0] = '*';
    fTemp6->right[1] = 'F';
    fTemp6->right[2] = 'I';
    fTemp6->right[3] = 'M';

    fTemp7->id = 7;
    fTemp7->left = 'M';
    fTemp7->r_length = 4;
    fTemp7->right[0] = '/';
    fTemp7->right[1] = 'F';
    fTemp7->right[2] = 'O';
    fTemp7->right[3] = 'M';

    fTemp8->id = 8;
    fTemp8->left = 'M';
    fTemp8->r_length = 0;

    fTemp9->id = 9;
    fTemp9->left = 'F';
    fTemp9->r_length = 2;
    fTemp9->right[0] = 'i';
    fTemp9->right[1] = 'P';

    fTemp10->id = 10;
    fTemp10->left = 'F';
    fTemp10->r_length = 3;
    fTemp10->right[0] = '(';
    fTemp10->right[1] = 'E';
    fTemp10->right[2] = ')';

    formulaBuffer[0] = *fTemp1;
    formulaBuffer[1] = *fTemp2;
    formulaBuffer[2] = *fTemp3;
    formulaBuffer[3] = *fTemp4;
    formulaBuffer[4] = *fTemp5;
    formulaBuffer[5] = *fTemp6;
    formulaBuffer[6] = *fTemp7;
    formulaBuffer[7] = *fTemp8;
    formulaBuffer[8] = *fTemp9;
    formulaBuffer[9] = *fTemp10;
}

//入语义栈操作
void accept(tokens temp) {
    if (temp.code == 1) {
        cout << temp.value << "匹配成功" << endl;
        return;
    }
    cout << temp.value << "    匹配成功" << endl;
    cout << temp.value << "    进入语义栈" << endl;
    sematicStack.push(temp.value);
}

//产生四元式并添加到四元式缓冲区中
void producequaternion(char temp) {
    quaternions *eTemp = new quaternions;
    eTemp->num2 = sematicStack.top();
    sematicStack.pop();
    eTemp->num1 = sematicStack.top();
    sematicStack.pop();
    eTemp->symbol = temp;
    eTemp->result = (char) ((int) 'A' + quaternionCount);
    sematicStack.push(eTemp->result);
    quaternionBuffer.push_back(*eTemp);
    quaternionCount++;
}
//输出四元式
void printquaternion() {
    for (vector<quaternions>::iterator i = quaternionBuffer.begin(); i != quaternionBuffer.end(); i++) {
        cout << "四元式:" << i->symbol << " " << (char) (i->num1) << " " << (char) (i->num2) << " " << i->result << endl;
    }
}
//输出读取到的token
void printTokens() {
    for (vector<tokens>::iterator i = tokenBuffer.begin(); i != tokenBuffer.end(); i++) {
        cout << "token：" << i->code << " " << i->value << endl;
    }
}

//查找分析表将相应产生式压入分析栈
bool searchAnalysiTable(char cState, tokens cToken) {
    formulas *fTemp = new formula;
    //查分析表，获取对应的产生式编号
    int e_num;
    vector<analysisTables>::iterator i;
    for (i = analysisTableBuffer.begin(); i != analysisTableBuffer.end(); i++) {
        if (i->currentState == cState && i->currentToken == cToken.value) {
            e_num = i->quaternionNum;
            break;
        }
    }
    if (i == analysisTableBuffer.end()) {
        cout << "parse error" << endl;
        return false;
    }
    cout << "产生式编号：" << e_num << endl;
    //查找产生式缓冲区获得对应产生式
    fTemp = &formulaBuffer[e_num - 1];
    cout << analysisStack.top() << "出栈" << endl;
    analysisStack.pop();
    //将产生式右部逆序压栈
    if (e_num == 9)
        fTemp->right[0] = cToken.value;
    int j = fTemp->r_length;
    for (int i = 0; i < fTemp->r_length; i++) {
        cout << fTemp->right[j - 1] << "进入分析栈" << endl;
        analysisStack.push(fTemp->right[j - 1]);
        j--;
    }
    return true;
}

int main(int argc, char *argv[]) {
    produce_tokenfile();
    initialAnalysisTableBuffer();
    initialTokenBuffer();
    initialFormulaBuffer();
    printTokens();
    analysisStack.push('#');
    analysisStack.push('E');
    bool b_temp;
    while (!analysisStack.empty()) {
        tokens currentToken = tokenBuffers.top();
        if (currentToken.value == analysisStack.top()) {
            accept(currentToken);
            tokenBuffers.pop();
            cout << analysisStack.top() << "出栈" << endl;
            analysisStack.pop();
            continue;
        }
        cout << "current state:" << analysisStack.top() << endl;
        cout << "current token:" << currentToken.value << endl;
        switch (analysisStack.top()) {
            case 'Y':
                producequaternion('+');
                analysisStack.pop();
                break;

            case 'U':
                producequaternion('-');
                analysisStack.pop();
                break;

            case 'I':
                producequaternion('*');
                analysisStack.pop();
                break;

            case 'O':
                producequaternion('/');
                analysisStack.pop();
                break;

            case 'P':
                analysisStack.pop();
                cout << "P出栈" << endl;
                break;

            case '#':
                analysisStack.pop();
                break;

            case 'E':
            case 'F':
            case 'L':
            case 'M':
            case 'N':
            case 'T':
                b_temp = searchAnalysiTable(analysisStack.top(), currentToken);
                break;

            default:
                cout << "error" << endl;
                getchar();
        }
        if (!b_temp)
            return 0;
    }
    printquaternion();
    return 0;
}