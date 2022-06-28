int main(){
    int res = 0;
    for(int i = 1; i < 4; i+=1){
        if(i<2)res+=i;
        else res-=i;
    }
    return res;
}
