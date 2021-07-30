#ifndef FUNCINTERP_HPP
#define FUNCINTERP_HPP

#include <string>
#include <string.h> //only for strchr, should be removed
#include <map>
#include <cctype>
#include <stack>
#include <iterator>
#include <stdexcept>
#include <iostream>
#include <sstream>
#include <exception>
#include <cmath>

using std::string;
typedef double(*f_operation)(double, double);
typedef double(*f_value)(double);

enum expected{
   LVAL,
   OP,
   RVAL
};

enum type{
   VALUE,
   VALUE_FUNC,
   OPERATOR,
   VARIABLE
};

class FuncTree{
private:
   const static char operators[];
   const static int precedence[];
   const static char variables[];
   const static std::map<string, double> constants;// = {"e", "pi"};
   const static std::map<string, f_value> functions;// = {"sin", "cos", "tan", "ln", "sqrt"};

   FuncTree *lchild;
   FuncTree *rchild;
   void *val;
   f_operation op_func = NULL;
   f_value val_func = NULL;
   type mode;

public:

   FuncTree(double d);

   FuncTree(char c, bool operation = true);

   FuncTree( double(*val_func)(double), string name);

   ~FuncTree();

   void print(bool pedantic=false);

   string repr(bool pedantic=false, bool jit_freindly=false);

   double evaluate(double eval_at);

   static std::map<string, double> initConstants(){
      std::map<string, double> c;
      c["e"] = 2.71828182846;
      c["pi"] = 3.14159265359;
      c["m"] = .001;
      c["u"] = .000001;
      c["p"] = .000000001;
      return c;
   }

   static std::map<string, f_value> initFunctions(){
      std::map<string, f_value> f;
      f["sin"] = &std::sin;
      f["cos"] = &std::cos;
      f["tan"] = &std::tan;
      f["sqrt"] = &std::sqrt;
      f["ln"] = &std::log;
      f["log"] = &std::log;
      return f;
   }

   static FuncTree* fromString(string func, string *errmsg) noexcept;

private:

   static FuncTree* sub_func(string::iterator &it, string &func);

   void repr_help(std::stringstream &out, bool pedantic, bool jit_freindly);

   static f_operation getOpFunc(char c){
      switch(c){
         case '+': return &FuncTree::add;
         case '-': return &FuncTree::subtract;
         case '*': return &FuncTree::multiply;
         case '/': return &FuncTree::divide;
         case '^': return &FuncTree::exponentiation;
         default: return NULL;
      }
   }

   void updateOperation(char c);

   char getOperation();

   static inline int findChar(const char *cstr, char c);

   static inline void consumeSpaces(string::iterator &it);

   static inline string getSymbol(string::iterator &it);

   //increments begin to character beyond last character in the found number
   static inline double doublefromString(
      std::string::iterator &begin,
      string &func);

   static inline void expectValue(
      FuncTree*& valNode,
      string::iterator &iter,
      string &func);

   static inline void expectOperator(char &op, string::iterator &iter);

   static inline void insertOperation(
      std::stack<FuncTree* > &funcs,
      char op,
      FuncTree *&root);


   static double add(double a, double b){
      return a + b;
   }

   static double subtract(double a, double b){
      return a - b;
   }

   static double multiply(double a, double b){
      return a * b;
   }

   static double divide(double a, double b){
      return a / b;
   }

   static double exponentiation(double a, double b){
      return std::pow(a, b);
   }

};

class function_structure : std::exception{
   string msg;
public:
   function_structure(const string &msg){
      this->msg = msg;
   }

   const char* what() const noexcept override {
      return msg.c_str();
   }
};

#endif
