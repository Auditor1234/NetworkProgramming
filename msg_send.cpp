#include <unistd.h>
#include <sys/msg.h>
#include <iostream>

using namespace std;

int main(int argc, char* argv[]) {
    key_t key = ftok("./", 50);
    cout << "key = " << key << endl;

    // 创建消息队列
    int msgid = msgget(key, IPC_CREAT);
    if(msgid == -1) {
        cout << "msgget error!" << endl;
        exit(1);
    }
    
    return 0;
}