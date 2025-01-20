#include <string>
#include <map>
using namespace std;

class Frame
{
public:
    Frame(const string &command);

    void addHeader(const string &key, const string &value);

    void setBody(const string &body);

    string getCommand() const;

    string getHeader(const string &key) const;

    string getBody() const;

    string toString() const;

private:
    string command;
    map<string, string> headers;
    string body;
};