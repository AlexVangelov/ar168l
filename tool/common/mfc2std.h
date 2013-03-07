/* 
   mcf2std.h : MCF to C++ convertion helper for Palmmicro tools compilation on *UNIX
   by Alex Vangelov
*/

#ifdef __GNUC__
#ifndef TRUE
#define TRUE      true
#endif
#ifndef FALSE
#define FALSE     false
#endif
#ifndef BOOL
#define BOOL      bool
#endif
#define _tsystem  system
#define _wtoi(x)  atoi(x.c_str())
#define itoa(value, buff, base) sprintf(buff,(base==10)?"%d":"%x",value) // only hex & decimal needed
#ifdef _UNICODE
#define T(x) L##x
#else
#define _T(x) x
#endif
#define GetCurrentDirectory(l,x) getcwd(x, l) 
#define modeRead  in
#define typeText  in
#define modeCreate   trunc
#define modeWrite out
#define typeBinary binary
#define POSITION        std::list<CString>::iterator
#define CFileException  std::exception
#define swscanf_s sscanf
#define AfxMessageBox(error) printf("%s\n",error.c_str());
#define _MAX_DRIVE	3
#define _MAX_DIR 		256
#define _MAX_FNAME	256
#define _MAX_EXT		256
#define m_size	st_size

#include <string>
#include <fstream>
#include <cstring>
#include <list>
#include <stdarg.h>
#include <algorithm>
#include <unistd.h>
#include <sys/stat.h>

typedef unsigned char BYTE;
typedef bool BOOLEAN;
typedef char TCHAR;
typedef const TCHAR *LPCTSTR;
typedef unsigned int UINT;
typedef struct stat CFileStatus;

class CString : public std::string 
{
public:
   CString() : std::string() { }
	CString( const std::string& s ) : std::string( s ) { }
	CString( const std::string& s, std::size_t n ) : std::string( s,n ) { }
	CString( const char *s, std::size_t n ) : std::string( s,n ) { }
	CString( const char *s ) : std::string( s ) { }
	CString( const unsigned char *s ) : std::string( (const char *)s ) { }
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
			if (length() == 0) return 0;
      return at(pos); 
   }
	 void TrimLeft(char ch = 0) {
	 		size_t pos = (ch == 0) ? find_first_not_of(" \t\r\n") : find_first_not_of(ch);
    	if (pos != std::string::npos) erase (0, pos);
   }
	 void TrimRight(char ch = 0) {
	 		size_t pos = (ch == 0) ? find_last_not_of(" \t\r\n") : find_last_not_of(ch);
			if (pos != std::string::npos) erase(++pos);
   }
   bool CompareNoCase(CString s) {
      MakeLower();
      s.MakeLower();
      return compare(s);
   }
   bool IsEmpty() {
			return empty();
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
	 size_t Find(CString str, size_t pos = 0) {
			return find(str,0);
	 }
	 size_t Find(char ch, size_t pos = 0) {
			return find(ch,0);
	 }
	 void Replace(size_t pos,  size_t len,  CString str) {
	 		this->replace(pos, len, str);
   }
	 void Replace(char a, char b) {
	 		replace(find(a), 1, 1, b);
	 }
	 size_t Delete(size_t pos = 0, size_t len = npos) {
	 		erase(pos,len);
			return length();
   }
	 bool operator==(const unsigned char *s)
	 {
			CString cmp = s;
		  return cmp == c_str();
	 }
	 bool Compare(CString s)
	 {
			return compare(s);
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
   void Read(char* s, size_t n) {
      read(s,n);
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
	 CString RemoveTail() {
			CString str;
			POSITION pos = end();
			str = *pos;
			erase(pos);
			return str;
	 }
	 size_t GetCount() {
			return size();
	 }
	 CString GetHead() {
			CString str = *begin();
			return str;
	 }
	 CString GetTail() {
			CString str = *rbegin();
			return str;
	 }
	 void RemoveAt(POSITION pos) {
	 		erase(pos);
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
