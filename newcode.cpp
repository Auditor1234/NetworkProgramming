#include <iostream>
#include <unordered_map>
using namespace std;

int main() {
    unordered_map<int, int> store;
    int len;
    cin >> len;
    string str;

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