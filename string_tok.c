#include <iostream>
#include <string>
#include <vector>
#include <sstream>

using namespace std;
 
void string_tok(std::string const &str, const char tok_by, std::vector<std::string> &tok_out)
{
    std::stringstream ss(str);
 
    std::string s;
    while (std::getline(ss, s, tok_by)) {
        tok_out.push_back(s);
    }
}
 
int main()
{
    std::string s = "Object.Revision.Dataset";
    const char tok_by = '.';
 
    std::vector<std::string> tok_out;
    string_tok(s, tok_by, tok_out);
 
    cout<<tok_out[0]<<endl;
    cout<<tok_out[1]<<endl;
    cout<<tok_out[2]<<endl;
    return 0;
}
