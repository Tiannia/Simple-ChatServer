#include <iostream>
#include <unordered_map>
#include <functional>
using namespace std;

void fun(int a)
{
    cout << "this is a func" << endl;
}

int main()
{
    auto func = bind(fun, placeholders::_1);
    
    unordered_map<int, int> mp = {
        {1, 1},
        {2, 2}
    };
    mp[1] = 6;
    for (auto it : mp)
    {
        if (it.second == 6)
        {
            mp.erase(it.first);
        }
        cout << mp.begin()->first << endl;
    }
    /*
        for(auto it: _userConnMap)
        {
            if(it.second == conn)
            {
                _userConnMap.erase(it.first);
            }
        }
    */
    return 0;
}