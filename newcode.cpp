#include <iostream>
#include <unordered_map>
using namespace std;

int main() {
    unordered_map<int, int> store;
    int len;
    cin >> len;
    string str;

    // getchar()会读入cin剩余的一个换行符, cin以空格，换行等作为结尾
    getchar();
    for(int i = 0; i < len; i++) {
        getline(cin, str);
        store[str[0] - '0'] += str[2] - '0';
    }
    cout << "---------" << endl;

    for(auto it : store) {
        cout << (char)(it.first + '0')<< ' ' << (char)(it.second + '0') << endl;
    }

    return 0;
}
// 64 位输出请用 printf("%lld")