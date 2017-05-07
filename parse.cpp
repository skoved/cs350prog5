#include "parse.hpp"

using namespace std;

vector<string> parse::parseFile(string filepath){
  vector<string> out;
  string curr_read = "";
  ifstream file (filepath.c_str());
  while(getline(file, curr_read)){
    out.push_back(curr_read);
  }
  file.close();
  return out;
}

vector<string> parse::parseFile(string filepath, char delim){
  vector<string> out;
  vector<string> temp_hold;
  string curr_read = "";
  ifstream file (filepath.c_str());
  while(getline(file, curr_read)){
    temp_hold = split(curr_read, delim);
    out.insert(out.end(), temp_hold.begin(), temp_hold.end());
  }
  file.close();
  return out;
}


vector<string> parse::split(const string read, char delim){
  vector<string> out;
  string modify = "";
  for(auto it = read.begin(); it != read.end(); it++){
    if(*it == delim){
      out.push_back(modify);
      modify = "";
    }else{
      modify += (*it);
    }
  }
  if(modify.length() > 0){
    out.push_back(modify);
  }


  
  return out;
}


