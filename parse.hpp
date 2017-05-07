#ifndef PARSE_HPP
#define PARSE_HPP
#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>


namespace parse{
  /*
    Opens a file, turns the contents into a string vector, closes file

    Parameters:
    String: filepath

    Returns:
    Vector<String>: contains the lines of the given file, separate by new lines
   */
  std::vector<std::string> parseFile(std::string filepath);

  /*
    Opens a file, turns the contents into a string vector, closes file

    Parameters:
    String: filepath
    Char: the delimiter to separate strings by
    
    Returns:
    Vector<String>: contains the lines of the given file, separated by the delim character
   */
  std::vector<std::string> parseFile(std::string filepath, char delim);

  /*
    Splits strings by a delimiter
    
    Parameter:
    String: delimited string to be separated

    Returns:
    Vector<String>: contains the strings separated by the delimiter
  */
  std::vector<std::string> split(const std::string read, char delim);
}

#endif
