#include <iostream>
#include <string>
#include<stdlib.h>
#include<vector>
using namespace std;
#define plus 0      // +
#define minus 1     // -
#define times 2     // *
#define slash 3     // /
#define lparen 4    // (
#define rparen 5    // )
#define ident 6     // 标识符
#define number 7    // 无符号整数
#define finish 8    // 完成

string Letter(string str)
{    
    string output;
    //识别基本字
    if(str=="begin")
        output ="(beginsym,begin)";
    else if(str=="call")
        output ="(callsym,call)";
    else if(str=="const")
        output ="(constsym,const)";
    else if(str=="do")
       output ="(dosym,do)";
    else if(str=="end")
        output ="(endsym,end)";
    else if(str=="if")
        output ="(ifsym,if)";
    else if(str=="odd")
        output ="(oddsym,odd)";
    else if(str=="procedure")
        output ="(proceduresym,procedure)";
    else if(str=="read")
        output ="(readsym,read)";
    else if(str=="then")
        output ="(thensym,then)";
    else if(str=="while")
        output ="(whilesym,while)";
    else if(str=="var")
        output ="(varsym,var)";
    else if(str=="write")
        output ="(writesym,write)";
    //识别标识符
    else
        output ="(ident," + str + ")";
    return output;
}

vector<string> func_cifa(string str){
    vector<string> res;
    string temp;
    int length_str = str.length();
    for(int i=0;i<length_str;i++)
    {
      //当遇到空格或换行时，跳过继续执行
      if(str[i]==' ' || str[i]=='\n') continue;
      //识别常数
      else if(isdigit(str[i]))
        {
         string digit;//以字符串形式表示这个常数
         while(isdigit(str[i])){
          digit +=str[i];
          i++;}
         i--;
         temp = "(number," + digit + ")";
         res.push_back(temp);
         temp.clear();
         }
        //识别基本字或标识符
      else if(isalpha(str[i]))
        {
          string lett;//以字符串形式表示这个基本字或者标识符
          while(isdigit(str[i])||isalpha(str[i])){
            lett +=str[i];
            i++;}
            i--;
            temp = Letter(lett);
            res.push_back(temp);
            temp.clear();
        }
        //识别运算符
        else
        {
            switch(str[i])
            {
            case '+':
                temp = "(plus,+)";
                res.push_back(temp);
                temp.clear();
                break;
            case '-':
                temp = "(minus,-)";
                res.push_back(temp);
                temp.clear();
                break;
            case '*':
                temp = "(times,*)";
                res.push_back(temp);
                temp.clear();
                break;
            case '/':
                temp ="(slash,/)";
                res.push_back(temp);
                temp.clear();
                break;
            case '=':
                temp ="(eql,=)";
                res.push_back(temp);
                temp.clear();
                break;
            case '<':
                i++;
                if(str[i]=='>')
                {
                    temp ="(neq,<>)";
                    res.push_back(temp);
                    temp.clear();
                }
                else if(str[i]=='=')
                {
                    temp ="(leq,<=)";
                    res.push_back(temp);
                    temp.clear();
                }
                else
                {
                    i--;
                    temp ="(lss,<)";
                    res.push_back(temp);
                    temp.clear();
                }
                break;
            case '>':
                i++;
                if(str[i]=='=')
                {
                    temp ="(geq,>=)";
                    res.push_back(temp);
                    temp.clear();
                }
                else
                {
                    i--;
                    temp ="(gtr,>)";
                    res.push_back(temp);
                    temp.clear();
                }
                break;
            case ':':
                i++;
                if(str[i]=='=')
                {
                    temp ="(becomes,:=)";
                    res.push_back(temp);
                    temp.clear();
                }
                break;
            //识别界符
            case '(':
                temp ="(lparen,()";
                res.push_back(temp);
                temp.clear();
                break;
            case ')':
                temp ="(rparen,))";
                res.push_back(temp);
                temp.clear();
                break;
            case ',':
                temp ="(comma,,)";
                res.push_back(temp);
                temp.clear();
                break;
            case ';':
                temp ="(semicolon,;)";
                res.push_back(temp);
                temp.clear();
                break;
            case '.':
                temp ="(period,.)";
                res.push_back(temp);
                temp.clear();
                break;
            //错误处理
            default:
                temp = "(number,";
                temp.insert(temp.end(), str[i]);
                temp += ")";
                res.push_back(temp);
                temp.clear();
                break;
            }
        }
    }
    return res;
}

//ifstream symbol;//读文件使用
int sym;//当前的输入符号
bool error;//判断分析成功与否
vector<string> res;
void advance(int num);//读入下一单词符号
void expression(int num);//表达式--递归下降子程序
void term(int num);//项--递归下降子程序
void factor(int num);//因子--递归下降子程序

void advance(int num)//读入下一单词符号
{
    static int count=num;//需要设置一个读取结束标志，故以读取次数结束
    if(count>0)
    {
        string temp = res[num-count];
        count--;
        //getline(symbol,temp);//读取一行（读文件使用）
        int word_begin=temp.find(',');//找到单词符号的结尾
        string keyword=temp.substr(1,word_begin-1);//将单词符号截取出来，去掉无用字符
        //cout<<temp<<endl;
        if(keyword=="plus")sym=plus;
        else if(keyword=="minus")sym=minus;
        else if(keyword=="times")sym=times;
        else if(keyword=="slash")sym=slash;
        else if(keyword=="lparen")sym=lparen;
        else if(keyword=="rparen")sym=rparen;
        else if(keyword=="ident")sym=ident;
        else if(keyword=="number")sym=number;
        else sym=finish;//单词错误，分析结束
    }
    else
	{
        //cout<<"分析结束!"<<endl;
        sym=finish;
    }
}

void expression(int num)//表达式--递归下降子程序
{
    if((sym==plus)||(sym==minus))advance(num);//表达式前面的符号可有可无，有时需要读入下一符号
    term(num);
    while((sym==plus)||(sym==minus))//后面一段可以重复多次
    {
        advance(num);
        term(num);
    }
}

void term(int num)//项--递归下降子程序
{
    factor(num);
    while((sym==times)||(sym==slash))//后面一段可以重复多次
    {
        advance(num);
        factor(num);
    }
}

void factor(int num)//因子--递归下降子程序
{
    if(sym==ident)advance(num);//单词为标识符
    else if(sym==number)advance(num);//单词为无符号整数
    else if(sym==lparen)//单词为左括号
    {
        advance(num);
        expression(num);
        if(sym==rparen)advance(num);//右括号匹配
        else
        {
            error=false;//分析报错
            //cout<<"右括号不匹配"<<endl;
        }
    }else
    {
        error=false;//分析报错
        //cout<<"符号不属于因子！"<<endl;
    }
}

int main()
{
    string my_input,str;
    while(cin>>my_input)
    { 
      //读入代码（字符串形式）
      str = str +' '+my_input;
    }
    res = func_cifa(str);
    int num = res.size();
    for(int j =0; j< num; j++){
         cout<<res[j]<<endl;
    }
    error=true;//默认分析成功
    advance(num);
    expression(num);//表达式是起始分析点
    if(error)printf("Yes,it is correct.\n");
    else printf("No,it is wrong.\n");
    //symbol.close();
    return 0;
}
