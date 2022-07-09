使用gcc -o compile compile.c进行编译，虽然可能有warning但是不影响结果
然后可以使用以下三种方式执行
./compile test.c   执行测试文件
./compile -s test.c   显示测试文件经过编译后各部分的汇编代码
./compile -w test.c   执行测试文件并将程序执行后生成的汇编代码写入"result.txt"中
