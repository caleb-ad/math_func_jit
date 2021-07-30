#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <libloaderapi.h>
#include <errhandlingapi.h>
#include <processthreadsapi.h>

#include <iostream>
#include <fstream>
#include <cstdlib>
#include <cstdio>
#include <ctime>

#include "FuncInterp.hpp"

using std::cout;
using std::endl;
using std::ofstream;

typedef double (__cdecl *math_func)(double);

void printError(){
   void* Msg;
   DWORD Err = GetLastError();

   FormatMessage(
      FORMAT_MESSAGE_ALLOCATE_BUFFER |
      FORMAT_MESSAGE_FROM_SYSTEM |
      FORMAT_MESSAGE_IGNORE_INSERTS,
      NULL,
      Err,
      MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
      (LPTSTR) &Msg,
      0, NULL );

   cout << "code: " << Err << endl << (char *)(Msg) << endl;
   LocalFree(Msg);
}

int generateSource(string func_str, string &name){
   name += ".cpp";
   ofstream source_code(name);
   if(source_code.bad()){
      cout << "failed to open file" << endl;
      return -1;
   }
   string errmsg;
   FuncTree *function = FuncTree::fromString(func_str, &errmsg);
   if(function == NULL){
      cout << errmsg << endl;
      return -1;
   }
   else{
      source_code << "#include <cmath>" <<endl;
      source_code << "extern \"C\" {" << endl;
      source_code << "__declspec(dllexport) double __cdecl " << name.substr(0, name.size()-4) << "(double x){" << endl;
      source_code << "return " << function->repr(false, true) << ";" << endl;
      source_code << "}}";
   }
   delete function;
   source_code.close();
   return 0;
}

int generateCompileScript(string &name){
   ofstream compile_script(name + ".bat");
   if(compile_script.bad()){
      cout << "failed to open file" << endl;
      return -1;
   }
   compile_script << "call \"C:\\Program Files (x86)\\Microsoft Visual Studio\\2019\\BuildTools\\VC\\Auxiliary\\Build\\vcvars64.bat\"" << endl;
   compile_script << "cd C:\\Users\\caleb\\Documents\\Projects\\MathFuncJIT" << endl;
   compile_script << "cl -W3 -EHsc -Fe:FuncLibrary.dll " << name << " -link -DLL" << endl;
   compile_script.close();
   return 0;
}

int compileSource(string &script_name){
   LPSTR command = new char[script_name.size() + 4];
   memcpy(command, string("/c " + script_name).c_str(), script_name.size() + 4);
   STARTUPINFOA sui;
   ZeroMemory(&sui, sizeof(sui));
   sui.cb = sizeof(sui);
   PROCESS_INFORMATION pi;
   ZeroMemory(&sui, sizeof(sui));
   int result = CreateProcessA("C:\\Windows\\System32\\cmd.exe", command, NULL, NULL, FALSE,
                   0, NULL, NULL,
                   &sui, &pi);
   if(result == 0){
      cout << "failed to create compilation process" << endl;
      printError();
      return -1;
   }
   WaitForSingleObject( pi.hProcess, INFINITE );
   CloseHandle( pi.hProcess );
   CloseHandle( pi.hThread );
   delete[] command;

   FILE *check_valid;
   if((check_valid = fopen("FuncLibrary.dll", "r")) == NULL){
      cout << "failed to compile dll" << endl;
      return -1;
   }
   fclose(check_valid);
   return 0;
}

int loadFunction(string &name, math_func &mf, HMODULE &func_library){
   func_library = LoadLibraryA("C:\\Users\\caleb\\Documents\\Projects\\MathFuncJIT\\FuncLibrary.dll");
   if(func_library == NULL){
      cout << "failed to load dll" << endl;
      printError();
      return -1;
   }
   mf = (math_func)GetProcAddress(func_library, name.c_str());
   if(mf == NULL){
      cout << "proc address not found" << endl;
      return -1;
   }
   return 0;
}

int main(){
   string func = "e^sin(x) / (x ^ (12.34719 * x))";
   string name = "my_func";
   math_func f_x;
   HMODULE func_library;
   if(generateSource(func, name) != 0){
      return -1;
   }
   if(generateCompileScript(name) != 0){
      return -1;
   }
   if(compileSource(name + ".bat") != 0){
      return -1;
   }
   if(loadFunction(name, f_x, func_library) != 0){
      return -1;
   }
   for(int a = -10000; a<=10000; a++){
      if(a % 500 == 0) cout << f_x(a);
      else f_x(a);
   }
   int &&test = 0;
   FreeLibrary(func_library);
   return 0;
}