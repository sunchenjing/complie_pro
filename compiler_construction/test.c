#include <stdio.h>
int main()
{
    int i, res;
    i = 4;
    res = 0;
    while(i>=0){
        if(i > 3){
            res = (i * 3 - i / 2) ;
            printf("the result is %d\n", res);
        }
        else{
            res = (i + 10) / 2;
            printf("the result is %d\n", res);
        }
        i--;
    }
    return 0;
}