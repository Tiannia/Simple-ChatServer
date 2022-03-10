#include <iostream>
#include <vector>
#include <unordered_map>
#include <map>
#include "../../thirdparty/checkjson.hpp"
#include "../../thirdparty/json.hpp"


using json = nlohmann::json;
using namespace std;
JsonString jsc;

void check(string str)
{
    cout << str << endl;
    if(jsc.CheckFormat(str)){
        cout << "is ok." << endl;
    }else{
        cout << "not ok." << endl;
    }
}


int main() {
    // 创建一个json对象(null)
    json j;

    vector<json> ve;


    //添加一个存储为double的数字
    j["pi"] = 12345;

    // 添加一个布尔值 
    j["happy"] = true;

    // 添加一个存储为std :: string的字符串
    j["name"] = "Niels";

    // 通过传递nullptr添加另一个空对象
    j["nothing"] = nullptr;

    // 在对象中添加对象
    j["answer"]["everything"] = 42;

    //添加unorder_map
    unordered_map<string, string> mp;
    mp["her name"] = "licy";
    mp["my name"] = "tian";

    j["our name"] = mp;

    //添加 map
    map<string, string> mp1;
    mp1["her name"] = "licy";
    mp1["my name"] = "tian";

    j["our second name"] = mp1;

    //添加一个数组，其存储为std：：vector（使用初始化列表）
    j["list"] = { 1, 0, 2 };

    // 在一个对象中添加另一个对象
    j["object"] = { {"currency", "USD"}, {"value", 42.99} };

    // 也可以通过直接赋值方式创建json对象，两种方式创建结果相同
    json j2 = {
        {"pi", 3.141},
        {"happy", true},
        {"name", "Niels"},
        {"nothing", nullptr},
        {"answer", {
            {"everything", 42}
        }},
        {"list", {1, 0, 2}},
        {"object", {
            {"currency", "USD"},
            {"value", 42.99}
        }}
    };
    ve.push_back(j);

    json j3 = {
        {"pi", "haha"},
        {"gg", {"aa", "bb", "cc", 123}},
    };

    check(j2.dump());
    check(j.dump());
    check(j3.dump());
    return 0;
}