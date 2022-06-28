#include<iostream>
#include<string>
#include<stdlib.h>
#include<vector>
#include<cstring>
using namespace std;

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

//构造结构体---二元组
struct Two_tuples
{
  string type;//词法分析的类型
  string value;//词法分析的值
};

string input;//全局变量定义输入
Two_tuples result[200];//存放词法分析二元组
int k = 0;//输入二元组时访问下标
bool ans = true;//存放判断结果
int fh[101],zt[101];//符号栈，状态栈

bool target_judge(string input , string str) //判断是否和目标串匹配
{
  //首先匹配长度，看长度是否一致
  if(input.length()!=str.length()) return false;
  //长度一致，逐个匹配字符
  else
  {
    for(unsigned int i=0;i<str.length();i++)
    {
      if(input[i]!=str[i]) return false;
    }
    return true;
  }
} 

void input_cf(vector<string> res) //输入词法分析的二元组
{
  int num = res.size();
  string str;
  for(int j = 0; j<num; j++)
  {
    str = res[j];
    //定义指针访问
    const char *d = "(),";
    char *p;
    //临时存放字符串，便于分割处理
    char buf[101];
    strcpy(buf,str.c_str()); //字符串转成数组
    //开始处理每个二元组
    p = strtok(buf,d);
    int i = 0;
    //把输入的二元组存储到结构体中
    while(p)
    {
      if(i%2==0) result[k].type = p;
      else result[k].value = p;
      i++;
      p = strtok(NULL,d);
    }
    k++;
  }
  result[k].type = "#";
}
 

//在LR分析表中判断是哪个目标串（0--8）
int chart(string str)
{
  if(target_judge("plus",str)) return 0;
  else if(target_judge("minus",str)) return 1;
  else if(target_judge("times",str)) return 2;
  else if(target_judge("slash",str)) return 3;
  else if(target_judge("lparen",str)) return 4;
  else if(target_judge("rparen",str)) return 5;
  else if(target_judge("ident",str)) return 6;
  else if(target_judge("number",str)) return 7;
  else if(target_judge("#",str)) return 8;
  else return -1;
} 

//ETF归约（9--11）
int gy_1(int num)
{
  //E---表达式
  if(num - 100 == 1) return 9;
  else if(num - 100 == 2) return 9;
  else if(num - 100 == 3) return 9;
  //T---项
  else if(num - 100 == 4) return 10;
  else if(num - 100 == 5) return 10;
  else if(num - 100 == 6) return 10;
  //F---因子
  else if(num - 100 == 7) return 11;
  else if(num - 100 == 8) return 11;
  else if(num - 100 == 9) return 11; 
  else return -1;
} 

//gy_1中规约后约几项
int gy_2(int num)
{
  //E---表达式
  if(num - 100 == 1) return 1;
  else if(num - 100 == 2) return 3;
  else if(num - 100 == 3) return 3;
  //T---项
  else if(num - 100 == 4) return 1;
  else if(num - 100 == 5) return 3;
  else if(num - 100 == 6) return 3;
  //F---因子
  else if(num - 100 == 7) return 3;
  else if(num - 100 == 8) return 1;
  else if(num - 100 == 9) return 1; 
  else return -1;
} 

int main()
{
  string my_input,all;
  while(cin>>my_input)
  {
    //读入代码（字符串形式）
    all = all+' '+my_input;
  }
  vector<string> res = func_cifa(all);
  int num = res.size();
  for(int j = 0; j<num; j++){
      cout<<res[j]<<endl;
  }
  input_cf(res); //输入词法分析结果
  //LR分析表，其中大于100为归，小于100为移进
  int LR_chart[17][12]={{0,0,0,0,4,0,5,6,0,1,2,3},
                        {7,8,0,0,0,0,0,0,100,0,0,0},
                        {101,101,9,10,0,1,0,0,101,0,0,0},
                        {104,104,104,104,0,104,0,0,104,0,0,0},
                        {0,0,0,0,4,0,5,6,0,11,2,3},
                        {108,108,108,108,0,108,0,0,108,0,0,0},
                        {109,109,109,109,0,109,0,0,109,0,0,0},
                        {0,0,0,0,4,0,5,6,0,0,12,3},
                        {0,0,0,0,4,0,5,6,0,0,13,3},
                        {0,0,0,0,4,0,5,6,0,0,0,14},
                        {0,0,0,0,4,0,5,6,0,0,0,15},
                        {7,8,0,0,0,16,0,0,0,0,0,0},
                        {102,102,9,10,0,102,0,0,102,0,0,0},
                        {103,103,9,10,0,103,0,0,103,0,0,0},
                        {105,105,105,105,0,105,0,0,105,0,0,0},
                        {106,106,106,106,0,106,0,0,106,0,0,0},
                        {107,107,107,107,0,107,0,0,107,0,0,0}}; 
  int i = 0 , j = 0; //访问栈下标
  fh[0] = num , zt[0] = 0;//初值
  while(LR_chart[zt[j]][chart(result[i].type)]!=100)
  {
    //判断错误
    if(LR_chart[zt[j]][chart(result[i].type)]==0)
    {
      ans = false;
      break;
    }
    //移进
    else if(LR_chart[zt[j]][chart(result[i].type)]<100)
    {
      j++;
      zt[j] = LR_chart[zt[j-1]][chart(result[i].type)];
      fh[j] = chart(result[i].type);
      i++;
    }
    //归约
    else
    {
      int res = LR_chart[zt[j]][chart(result[i].type)];
      j -= gy_2(res);
      j++;
      //移入符号栈，状态栈
      fh[j] = gy_1(res);
      zt[j] = LR_chart[zt[j-1]][gy_1(res)];
      //判断错误
      if(zt[j]==0)
      {
        ans = false;
        break;
      }
    }
  }
  if(ans == true) cout<<"Yes,it is correct."<<endl;
  else cout<<"No,it is wrong."<<endl;
  return 0;
}
