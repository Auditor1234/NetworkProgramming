#include <iostream>
#include <cstring>
using namespace std;

int main() {
    char temp[50];
    cout << temp << endl;
    // 从这个例子可以看出fgets没有对char数组进行初始化
    fgets(temp, sizeof(temp), stdin);
    for(int i = 0; i < 50; i++) {
        cout << "temp[" << i << "] = " << temp[i] << endl;
    }
    cout << temp << endl;
    cout << "strlen(temp) = " << strlen(temp) << endl;
    cout << "sizeof(temp) = " << sizeof(temp) << endl;
    return 0;
}