#pragma once
#include <string>
#include <map>
#include <vector>
#include <sstream>
using namespace std;

/*
 * @class Frame
 * @brief Represents a frame with a command, headers, and a body.
 *
 * This class provides methods to construct, copy, move, and assign frames,
 * as well as to manipulate and retrieve their components.
 */
class Frame
{
public:
    /**
     * @brief Constructs a Frame with the given command.
     * @param command The command for the frame.
     */
    Frame(const string &command);

    /**
     * @brief Destructor.
     */
    ~Frame();

    /**
     * @brief Copy constructor.
     * @param other The Frame to copy from.
     */
    Frame(const Frame &other);

    /**
     * @brief Move constructor.
     * @param other The Frame to move from.
     */
    Frame(Frame &&other) noexcept;

    /**
     * @brief Copy assignment operator.
     * @param other The Frame to copy from.
     * @return A reference to this Frame.
     */
    Frame &operator=(const Frame &other);

    /**
     * @brief Move assignment operator.
     * @param other The Frame to move from.
     * @return A reference to this Frame.
     */
    Frame &operator=(Frame &&other) noexcept;

    /**
     * @brief Adds a header to the frame.
     * @param key The header key.
     * @param value The header value.
     */
    void addHeader(const string &key, const string &value);

    /**
     * @brief Sets the body of the frame.
     * @param body The body content.
     */
    void setBody(const string &body);

    /**
     * @brief Gets the command of the frame.
     * @return The command string.
     */
    string getCommand() const;

    /**
     * @brief Gets the value of a header by key.
     * @param key The header key.
     * @return The header value.
     */
    string getHeader(const string &key) const;

    /**
     * @brief Gets the body of the frame.
     * @return The body string.
     */
    string getBody() const;

    /**
     * @brief Parses a frame from a string.
     * @param frameString The string representation of the frame.
     * @return The parsed Frame object.
     */
    static Frame parseFrame(const string &frameString);

    /**
     * @brief Splits a string into lines.
     * @param input The input string.
     * @return A vector of strings, each representing a line.
     */
    static vector<string> splitIntoLines(const string &input);

    /**
     * @brief Converts the frame to a string representation.
     * @return The string representation of the frame.
     */
    string toString() const;

private:
    string command;
    map<string, string> headers;
    string body;
};