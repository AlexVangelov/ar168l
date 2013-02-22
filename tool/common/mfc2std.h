/* 
   mcf2std.h : MCF to C++ convertion helper for Palmmicro tools compilation on *UNIX
   by Alex Vangelov
*/

#ifdef __GNUC__
#define TRUE      true
#define FALSE     false
#define BOOL      bool
#define _tsystem  system
#define _wtoi(x)  atoi(x.c_str())
#ifdef _UNICODE
#define T(x) L##x
#else
#define _T(x) x
#endif
#define GetCurrentDirectory(l,x) getcwd(x, l) 
#define modeRead  in
#define typeText  in
#define modeCreate   out
#define modeWrite out
#define POSITION        std::list<CString>::iterator
#define WideCharToMultiByte
#define CFileException  std::exception

#include <string>
#include <fstream>
#include <cstring>
#include <list>
#include <stdarg.h>
#include <algorithm>
#include <unistd.h>

typedef unsigned char BYTE;
typedef bool BOOLEAN;
typedef char TCHAR;
typedef const TCHAR *LPCTSTR;
typedef unsigned int UINT;

class CString : public std::string 
{
public:
   CString() : std::string() { }
	CString( const std::string& s ) : std::string( s ) { }
	CString( const std::string& s, std::size_t n ) : std::string( s,n ) { }
	CString( const char *s, std::size_t n ) : std::string( s,n ) { }
	CString( const char *s ) : std::string( s ) { }
	CString( std::size_t n, char c ) : std::string ( n,c ) { }

   CString Left(size_t n) {
      return substr(0,n < length() ? n : length() ).c_str();
   }
   CString Right(size_t n) {
      size_t l = length() < n ? length() : n;
      return substr(length()-l,length()).c_str();
   }
   void MakeLower() {
      std::transform(begin(), end(), begin(), ::tolower);
   }
   void MakeUpper() {
      std::transform(begin(), end(), begin(), ::toupper);    
   }
   size_t GetLength() { 
      size_t l = length();
      return l; 
   }
   char GetAt(size_t pos) { 
      return at(pos); 
   }
   void TrimLeft() {
      erase(begin(), std::find_if(begin(), end(), std::not1(std::ptr_fun<int, int>(std::isspace))));
   }
   bool CompareNoCase(CString s) {
      MakeLower();
      s.MakeLower();
      return compare(s);
   }
   CString Mid(size_t pos = 0, size_t len = npos) {
      return substr(pos,len);
   }
   void FindAndReplace(CString search, CString replace) {
     size_t pos = 0;
      while((pos = find(search, pos)) != std::string::npos) {
         this->replace(pos, search.length(), replace);
         pos += replace.length();
      }
   }

   operator const char*(){
		return c_str();
	}

};

class CStdioFile : public std::fstream 
{
public:
   bool Open(CString filename, ios_base::openmode mode) {
      open(filename.c_str(), mode);
      return is_open();
   }
   void Close() { 
      close(); 
   }
   void WriteString(CString str) {
      write(str, str.length());   
   }
};

class CStringList : public std::list<CString> 
{
public:
   void AddTail(CString str) {
      push_back(str);
   }
   CString GetNext(POSITION pos) {
      CString next = *(pos++);
      return next;
   }
   void SetAt(POSITION pos, CString str) {
      *pos = str;   
   }
};

class CFile : public std::fstream 
{
public:
   bool Open(CString filename, ios_base::openmode mode) {
      open(filename.c_str(), mode | std::ios::binary);
      return is_open();
   }
   int GetLength() {
      int length;
      seekg (0, std::ios::end);
      length = tellg();
      seekg (0, std::ios::beg);   
      return length;
   }
   void Read(char* s, size_t n) {
      read(s,n);
   }
   void Write(char* s, size_t n) {
      write(s,n);
   }
   void Close() { 
      close(); 
   }
   static void Remove(CString f) {
       remove(f); 
   }
};


#endif
