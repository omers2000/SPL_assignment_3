#include <string>
#include <map>
#include <vector>
#include <sstream>
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

    static Frame parseFrame(const string &frameString);

    static vector<string> splitIntoLines(const string &input);

    string toString() const;

private:
    string command;
    map<string, string> headers;
    string body;
};