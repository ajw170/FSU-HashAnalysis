/*
    fhtbl.cpp
    06/20/2018
    Chris Lacher
    Copyright 2018, R.C. Lacher

    functionality test for hash tables

    HashTable <K,D,H>

       K =      String
       D =      int
       H =      HashClass<K>
*/

#include <fstream>
#include <cctype>

#include <xstring.h>
#include <hashclasses.h>
#include <primes.h>
#include <bitvect.h>
#include <ansicodes.h>

//#include <hashtbl.h>
#include <aa.h>
#include <compare.h>

// in lieu of makefile
#include <xstring.cpp>
#include <primes.cpp>
#include <bitvect.cpp>
#include <hashfunctions.cpp>
// */


// the KISS hash function
typedef fsu::String                        KeyType;
typedef int                                DataType;
typedef hashclass::KISS < KeyType >        HashType;
const char* kT = "fsu::String";
const char* dT = "int";
const char* hT = "hashclass::KISS <>";
const bool PRIME = 1;
// */

/*// ModP hash function
typedef fsu::String                        KeyType;
typedef int                                DataType;
typedef hashclass::ModP < KeyType >        HashType;
typedef fsu::Entry < KeyType, DataType >   EntryType;
typedef fsu::LessThan < EntryType >        ComparisonType;
const char* kT = "fsu::String";
const char* dT = "int";
const char* hT = "hashclass::ModP <>";
const bool PRIME = 1;
// */

/* // the MM hash function
typedef fsu::String                        KeyType;
typedef int                                DataType;
typedef hashclass::MM < KeyType >          HashType;
typedef fsu::Entry < KeyType, DataType >   EntryType;
typedef fsu::LessThan < EntryType >        ComparisonType;
const char* kT = "fsu::String";
const char* dT = "int";
const char* hT = "hashclass::MM <>";
const bool PRIME = 1;
// */

/*// the Simple hash function
typedef fsu::String                        KeyType;
typedef int                                DataType;
typedef hashclass::Simple < KeyType >      HashType;
typedef fsu::Entry < KeyType, DataType >   EntryType;
typedef fsu::LessThan < EntryType >        ComparisonType;
const char* kT = "fsu::String";
const char* dT = "int";
const char* hT = "hashclass::Simple <>";
const bool PRIME = 0;
// */

typedef fsu::HashTable<KeyType, DataType, HashType> MapType;
typedef fsu::HashTable<KeyType, DataType, HashType> HashTableType;
typedef fsu::HashTable<KeyType, DataType, HashType>::Iterator Iterator;
typedef fsu::HashTable<KeyType, DataType, HashType>::ConstIterator ConstIterator;

void DisplayPrompt(const char* kt, const char* dt, const char* ht, std::ostream& os);
void DisplayMenu(std::ostream& os);
void SkipDoc (std::istream& is);
template < typename T > int  CorrectDataWidth(const T& t, int dw);
template <>             int  CorrectDataWidth(const fsu::String& t, int dw);
template < class C >    void CopyTest ( const C& x1, C x2, std::ostream& os );
template < class C >    void AssignTest ( const C& x1 , std::ostream& os );

int main(int argc, char* argv[])
{
  bool BATCH = 0;
  bool COLOR = 0;
  if (argc == 2) // com file only
  {
    BATCH = 1; // duplex commands to screen in color
    COLOR = 1; // in color
  }
  if (argc > 2) // com and screen files
  {
    BATCH = 1;
    COLOR = 0; // duplex commands to file not in color
  }
  // set up for command file / batch mode
  std::istream * inptr = &std::cin; // user input
  std::ifstream com;                // command stream (BATCH mode)
  if (argc > 1)
  {
    com.open(argv[1]);
    if (com.fail())
    {
      std::cerr << " ** Error: cannot open file " << argv[1] << '\n';
      return 0;
    }
    inptr = &com;
    SkipDoc(*inptr);
  }
  // set up for sending ui output to file
  std::ostream * outptr = &std::cout; // ui output
  std::ofstream screen;              // screen output stream
  if (argc > 2)
  {
    screen.open(argv[2]);
    if (screen.fail())
    {
      std::cerr << " ** Error: cannot open file " << argv[2] << '\n';
      return 0;
    }
    outptr = &screen;
  }

  MapType* tablePtr;
  size_t numbuckets(0);
  *outptr << "  Enter approximate number of buckets (0 to quit): ";
  *inptr >> numbuckets;
  if (BATCH)
  {
    if (COLOR)
      *outptr <<  ANSI_BOLD_RED << numbuckets << '\n' << ANSI_RESET_ALL;
    else
      *outptr << numbuckets << '\n';
  }
  if (numbuckets == 0)
    return 0;

  // tablePtr = new HashTableType(numbuckets, PRIME); // 1-parameter constructor
  HashType hfo;
  tablePtr = new HashTableType(numbuckets, hfo, PRIME); // 2-parameter constructor
  // const HashTableType* constTablePtr(tablePtr);
  MapType& map = *tablePtr;
  std::ifstream ifs;   // data input stream
  std::ofstream ofs;   // data output stream
  fsu::String filename;

  KeyType  key;
  DataType data;
  Iterator mapi, itr1, itr2;
  ConstIterator mapci;

  bool initited (0);
  char command;
  size_t size, nsize, digits, ndigits;
  double lambda(0.0);
  int kcw = 0;
  int dcw = 0;


  if (!BATCH) DisplayMenu(*outptr);

  do
  {
    DisplayPrompt(kT, dT, hT, *outptr);
    *inptr >> command;
    if (BATCH)
    {
      if (COLOR) *outptr << ANSI_BOLD_RED << command << ANSI_RESET_ALL;
      else *outptr << command;
    }
    switch (command)
    {
      case 'm': case 'M':
        if (BATCH) *outptr << '\n';
        DisplayMenu(*outptr);
        break;

      case 'x': case 'X':
        if (argc > 1)
        {
          *outptr << '\n';
          *outptr << "   ** switching to keyboard/screen I/O **\n";
          inptr = &std::cin;
          outptr = &std::cout;
          com.close();
          if (argc > 2)
            screen.close();
          BATCH = 0;
          COLOR = 0;
          DisplayMenu(*outptr);
        }
        else
        {
          *outptr << "  ** already using keyboard/screen I/O **\n";
        }
        break;

      case 'q': case 'Q':
        if (argc > 1)
        {
          *outptr << '\n';
          com.close();
        }
        if (argc > 2)
        {
          screen.close();
        }
        command = 'q';
        break;

      case '=':
        if (BATCH) *outptr << '\n';
        CopyTest(map, map, *outptr);
        AssignTest(map, *outptr);
        break;

      case 'c': case 'C':
        if (BATCH) *outptr << '\n';
        map.Clear();
        initited = 0;
        break;

      case 'H': case 'h':
        *outptr << "  Enter number of buckets: ";
        *inptr >> numbuckets;
        if (BATCH) *outptr << '\n';
        if (BATCH)
        {
          if (COLOR) *outptr << ANSI_BOLD_RED << ' ' << numbuckets << '\n' << ANSI_RESET_ALL;
          else *outptr << ' ' << numbuckets << '\n';
        }
        map.Rehash(numbuckets);
        initited = 0;
        break;

      case 'f': case 'F': case '<':
        if (!BATCH)
        {
          *outptr << " Use these file read codes:\n"
                  << "   T .... read file as table data\n"
                  << "   K .... read file as all keys\n"
                  << "   C .... read file as key counter\n"
                  << " code: ";
        }
        *inptr >> command;
        if (BATCH)
        {
          if (COLOR) *outptr << ANSI_BOLD_RED << ' ' << command << ANSI_RESET_ALL;
          else *outptr << ' ' << command;
        }
        if (!BATCH) *outptr << " file: ";
        *inptr >> filename;
        if (BATCH)
        {
          if (COLOR) *outptr << ANSI_BOLD_RED << ' ' << filename << '\n' << ANSI_RESET_ALL;
          else *outptr << ' ' << filename << '\n';
        }
        ifs.open(filename.Cstr());
        if (ifs.fail())
        {
          *outptr << "  ** Unable to open file " << filename << '\n';
          ifs.clear();
          break;
        }
        switch(command)
        {
          case 't': case 'T':
            size = 0;
            digits = 0;
            while (ifs >> key >> data)
            {
              map[key] = data;
              // map[key];
              // ++map[key];
              nsize = key.Size();
              ndigits = 1 + (size_t)log10(data);
              if (size < nsize) size = nsize;
              if (digits < ndigits) digits = ndigits;
            }
            if (kcw < (int)size)   kcw = size;
            if (dcw < (int)digits) dcw = digits;
            ifs.clear();
            ifs.close();
            *outptr << "  ** table data read and stored in map\n";
            initited = 0;
            break;

          case 'k': case 'K':
            size = 0;
            while (ifs >> key)
            {
              // map[key] = data;
              map[key];
              // ++map[key];
              nsize = key.Size();
              if (size < nsize) size = nsize;
            }
            if (kcw < (int)size)   kcw = size;
            ifs.clear();
            ifs.close();
            *outptr << "  ** keys read and stored\n";
            initited = 0;
            break;
          case 'c': case 'C':
            size = 0;
            while (ifs >> key)
            {
              // map[key] = data;
              // map[key];
              ++map[key];
              nsize = key.Size();
              if (size < nsize) size = nsize;
            }
            if (kcw < (int)size)   kcw = size;
            ifs.clear();
            ifs.close();
            *outptr << "  ** keys read and counted\n";
            initited = 0;
            break;
          default:
            *outptr << " ** case \'<\' command not found\n";
        } // case '<' inner switch
        break;

      case '>':
        *inptr >> filename;
        if (BATCH)
        {
          if (COLOR) *outptr << ANSI_BOLD_RED << ' ' << filename << '\n' << ANSI_RESET_ALL;
          else *outptr << ' ' << filename << '\n';
        }
        ofs.open(filename.Cstr());
        if (ofs.fail())
        {
          *outptr << "  ** Unable to open file " << filename << '\n';
          ofs.clear();
          break;
        }
        for( ConstIterator i = map.Begin() ; i != map.End() ; ++i )
        {
          ofs << "  "
              << std::left  << std::setw(2+kcw) << (*i).key_
              << std::right << std::setw(2+dcw) << (*i).data_ << '\n';
        }
        ofs.close();
        *outptr << " ** map data stored in file " << filename << '\n';
        break;

      case 'p': case 'P':
        *inptr >> key >> data;
        if (inptr->fail())
        {
          *outptr << " ** bad key or data encountered - re-enter command\n";
          inptr->clear();
          while (command != '\n') command = (char)inptr->get();
          inptr->clear();
          break;
        }
        if (BATCH)
        {
          if (COLOR) *outptr << ANSI_BOLD_RED << ' ' << key << ' ' << data << '\n' << ANSI_RESET_ALL;
          else *outptr << ' ' << key << ' ' << data << '\n';
        }
        *outptr << "map.Put(" << key << ',' << data << ")\n";
        map.Put(key,data);
        kcw = CorrectDataWidth(key,kcw);
        dcw = CorrectDataWidth(data,dcw);
        initited = 0;
        break;

      case '^': case '1':  // Insert(key, data)
        *inptr >> key;     // operand1
        *inptr >> data;   // operand2
        if (BATCH)
        {
          if (COLOR) *outptr << ANSI_BOLD_RED << ' ' << key << ' ' << data << '\n' << ANSI_RESET_ALL;
          else *outptr << ' ' << key << ' ' << data << '\n';;
        }
        kcw = CorrectDataWidth(key,kcw);
        dcw = CorrectDataWidth(data,dcw);
        *outptr << "map.Insert(" << key << ',' << data << ")\n";
        tablePtr->Insert(key, data);
        break;
        
      case '2': case 'e': case 'E':
        *inptr >> key;
        if (inptr->fail())
        {
          *outptr << " ** bad key encountered - re-enter command\n";
          inptr->clear();
          while (command != '\n') command = (char)inptr->get();
          inptr->clear();
          break;
        }
        if (BATCH)
        {
          if (COLOR) *outptr << ANSI_BOLD_RED << ' ' << key << '\n' << ANSI_RESET_ALL;
          else *outptr << ' ' << key << '\n';
        }
        *outptr << "  map.Remove(" << key << ")\n";
        if (map.Remove(key)) // map.Erase(key);
        {
          *outptr << "  key " << key << " removed from table\n";
          initited = 0;
        }
        {
          *outptr << "  key " << key << " not found in table\n";
        }
        break;

      case 'g': case 'G':
        *inptr >> key;
        if (inptr->fail())
        {
          *outptr << " ** bad key encountered - re-enter command\n";
          inptr->clear();
          while (command != '\n') command = (char)inptr->get();
          inptr->clear();
          break;
        }
        if (BATCH)
        {
          if (COLOR) *outptr << ANSI_BOLD_RED << ' ' << key << '\n' << ANSI_RESET_ALL;
          else *outptr << ' ' << key << '\n';
        }
        *outptr << "  " << map.Get(key)
                << " = map.Get(" << key << ")\n";
        kcw = CorrectDataWidth(key,kcw);
        initited = 0;
        break;

      case '[':
        *inptr >> key;
        if (BATCH)
        {
          if (COLOR) *outptr << ANSI_BOLD_RED << ' ' << key << ANSI_RESET_ALL;
          else *outptr << ' ' << key;
        }
        *inptr >> command;
        if (BATCH)
        {
          if (COLOR) *outptr << ANSI_BOLD_RED << ' ' << command << ANSI_RESET_ALL;
          else *outptr << ' ' << command;
        }
        if (command != ']')
        {
          *outptr << " ** close bracket expected - re-enter command\n";
          inptr->clear();
          while (command != '\n') command = (char)inptr->get();
          inptr->clear();
          break;
        }
        command = (char)inptr->peek();
        if (command == '\n')
        {
          if (BATCH) *outptr << '\n';
          *outptr << "  " << map[key]
                  << " = map[" << key << "]\n";
          kcw = CorrectDataWidth(key,kcw);
          break;
        }
        *inptr >> command >> data; // assume '='
        if (BATCH)
        {
          if (COLOR) *outptr << ANSI_BOLD_RED << ' ' << command << ' ' << data << '\n' << ANSI_RESET_ALL;
          else *outptr << command << data << '\n';
        }
        if (command == '+')
        {
          *outptr << "map[" << key << "] += " << data << '\n';
          map[key] += data;
        }
        else // assume '='
        {
          *outptr << "map[" << key << "] = " << data << '\n';
          map[key] = data;
        }
        kcw = CorrectDataWidth(key,kcw);
        dcw = CorrectDataWidth(data,dcw);
        initited = 0;
        break;

      case 'r': case 'R':
        *inptr >> key;
        if (inptr->fail())
        {
          *outptr << " ** bad key encountered - re-enter command\n";
          inptr->clear();
          while (command != '\n') command = (char)inptr->get();
          inptr->clear();
          break;
        }
        if (BATCH)
        {
          if (COLOR) *outptr << ANSI_BOLD_RED << ' ' << key << '\n' << ANSI_RESET_ALL;
          else *outptr << ' ' << key << '\n';
        }
        if (map.Retrieve(key,data))
        {
          *outptr << "  map.Retrieve(" << key << ",d) successful with d = " << data << '\n';
        }
        else
        {
          *outptr << "  map.Retrieve(" << key << ",d) unsuccessful\n";
        }
        kcw = CorrectDataWidth(key,kcw);
        break;

      case 'd':  case 'D':
        if (BATCH) *outptr << '\n';
        *outptr << "  Enter data file name (0 for screen): ";
        *inptr >> filename;
        if (BATCH) *outptr << filename << '\n';;
        if (filename.Element(0) == '0')
        {
          tablePtr->Dump(*outptr);
          break;
        }
        ofs.clear();
        ofs.open(filename.Cstr());
        if (ofs.fail())
        {
          *outptr << "  Unable to open file " << filename << '\n'
                    << "  Dump() aborted\n";
          break;
        }
        tablePtr->Dump(ofs);
        ofs.close();
        *outptr << "  Dump() completed\n";
        break;

      case 'b': case 'B':
        if (BATCH) *outptr << '\n';
        mapi = map.Begin();
        if (mapi != map.End())
        {
          *outptr << " iterator at entry <" << (*mapi).key_ << ',' << (*mapi).data_ << ">\n";
          initited = 1;
        }
        else
          *outptr << " iterator at End()\n";
        break;

      case 'i': case 'I':
        *inptr >> key;
        if (BATCH)
        {
          if (COLOR) *outptr << ANSI_BOLD_RED << ' ' << key << '\n' << ANSI_RESET_ALL;
          else *outptr << ' ' << key << '\n';
        }
        mapi = map.Includes(key);
        if (mapi != map.End())
        {
          *outptr << " iterator at entry <" << (*mapi).key_ << ',' << (*mapi).data_ << ">\n";
          initited = 1;
        }
        else
        {
          *outptr << " key not found\n";
          initited = 0;
        }
        break;

      case '+':
        if (BATCH) *outptr << '\n';
        ++mapi;
        if (mapi != map.End())
          *outptr << " iterator at entry <" << (*mapi).key_ << ',' << (*mapi).data_ << ">\n";
        else
          *outptr << " iterator at End()\n";
        break;

      case '-':
        if (BATCH) *outptr << '\n';
        --mapi;
        if (mapi != map.rEnd())
          *outptr << " iterator at entry <" << (*mapi).key_ << ',' << (*mapi).data_ << ">\n";
        else
          *outptr << " iterator at End()\n";
        break;

      case '?':
        if (BATCH) *outptr << '\n';
        if (initited && mapi != map.End())
        {
          mapci = mapi;
          *outptr << " iterator at entry <" << (*mapci).key_ << ',' << (*mapci).data_ << ">\n";
        }
        else if (!initited)
        {
          *outptr << " iterator not initialized\n";
        }
        else
        {
          *outptr << " iterator at End()\n";
        }
        break;

      case '*':
        *inptr >> data;
        if (BATCH)
        {
          if (COLOR) *outptr << ANSI_BOLD_RED << data << '\n' << ANSI_RESET_ALL;
          else *outptr << data << '\n';
        }
        if (initited && mapi != map.End())
        {
          (*mapi).data_ = data;
          mapci = mapi;
          *outptr << " iterator at entry <" << (*mapci).key_ << ',' << (*mapci).data_ << ">\n";
          // (*mapci).data_ = data; // uncomment this and see compile error
          // message
          // (*mapi).key_ = key;    // uncomment this and see compile error
          // message
          dcw = CorrectDataWidth(data,dcw);
        }
        else if (!initited)
        {
          *outptr << " iterator not initialized\n";
        }
        else
        {
          *outptr << " iterator at End()\n";
        }
        break;

      case '@':
        *inptr >> key >> data;
        if (inptr->fail())
        {
          *outptr << " ** bad key or data encountered - re-enter command\n";
          inptr->clear();
          while (command != '\n') command = (char)inptr->get();
          inptr->clear();
          break;
        }
        if (BATCH)
        {
          if (COLOR) *outptr << ANSI_BOLD_RED << ' ' << key << ' ' << data << '\n' << ANSI_RESET_ALL;
          else *outptr << ' ' << key << ' ' << data << '\n';
        }
        *outptr << "map.Insert(i," << key << ',' << data << ")\n";
        map.Insert(mapi,key,data);
        kcw = CorrectDataWidth(key,kcw);
        dcw = CorrectDataWidth(data,dcw);
        // initited = 0;
        break;

      case 's':
        if (BATCH) *outptr << '\n';
        size = tablePtr->Size();
        numbuckets = tablePtr->NumBuckets();
        lambda = (double)size/numbuckets;
        *outptr <<  std::fixed << std::showpoint << std::setprecision(2);
        *outptr << "  table Size():    " << size << '\n';
        *outptr << "  table Empty():   " << (size_t)tablePtr->Empty() << '\n';
        *outptr << "  NumBuckets():    " << numbuckets << '\n';
        *outptr << "  Load Factor:     " << lambda << '\n';
        break;

      case 'S':
        if (BATCH) *outptr << '\n';
        size = map.Size();
        numbuckets = map.NumBuckets();
        lambda = (double)size/numbuckets;
        *outptr <<  std::fixed << std::showpoint << std::setprecision(2);
        *outptr << "  table Size():    " << size << '\n';
        *outptr << "  table Empty():   " << (size_t)map.Empty() << '\n';
        *outptr << "  NumBuckets():    " << numbuckets << '\n';
        *outptr << "  Load Factor:     " << lambda << '\n';
        *outptr << "  MaxBucketSize(): " << map.MaxBucketSize() << '\n';
        break;

      case 'a': case 'A':
        if (BATCH) *outptr << '\n';
        tablePtr->Analysis(*outptr);
        break;

      case 't':  case 'T':
        if (!BATCH)
        {
          *outptr << " Enter one of these traversal codes:\n"
                  << "   F .... native forward const\n"
                  << "   f .... native forward non-const\n"
                  << "   R .... native reverse const\n"
                  << "   r .... native reverse non-const\n"
                  << "   ! .... reciprocity checks\n"
                  << " code: ";
        }
        *inptr >> command;
        if (BATCH)
        {
          if (COLOR) *outptr << ANSI_BOLD_RED << ' ' << command << '\n' << ANSI_RESET_ALL;
          else *outptr << ' ' << command << '\n';
        }
        switch (command)
        {
          case 'F': // const forward
            *outptr << "Forward const Traversal\n"
                    << "-----------------------\n";
            *outptr << std::setw (3+kcw) << "key" << std::setw(5+dcw) << "data" << '\n';
            *outptr << std::setw (3+kcw) << "---" << std::setw(5+dcw) << "----" << '\n';
            for( ConstIterator i = map.Begin() ; i != map.End() ; ++i )
            {
              data = (*i).data_;     // read: testing constness
              // (*i).data_ = 0 + data; // write: testing non-constness
              *outptr << std::setw (3+kcw) << (*i).key_ << std::setw(5+dcw) << (*i).data_ << '\n';
            }
            *outptr << '\n';
            break;
          case 'f': // forward
            *outptr << "Forward non-const Traversal\n"
                    << "---------------------------\n";
            *outptr << std::setw (3+kcw) << "key" << std::setw(5+dcw) << "data" << '\n';
            *outptr << std::setw (3+kcw) << "---" << std::setw(5+dcw) << "----" << '\n';
            for( Iterator i = map.Begin() ; i != map.End() ; ++i )
            {
              data = (*i).data_;     // read: testing constness
              (*i).data_ = 0 + data; // write: testing non-constness
              *outptr << std::setw (3+kcw) << (*i).key_ << std::setw(5+dcw) << (*i).data_ << '\n';
            }
            *outptr << '\n';
            break;
          case 'R': // const reverse
            *outptr << "Reverse const Traversal\n"
                    << "-----------------------\n";
            *outptr << std::setw (3+kcw) << "key" << std::setw(5+dcw) << "data" << '\n';
            *outptr << std::setw (3+kcw) << "---" << std::setw(5+dcw) << "----" << '\n';
            for( ConstIterator i = map.rBegin() ; i != map.rEnd() ; --i )
            {
              data = (*i).data_;     // read: testing constness
              // (*i).data_ = 0 + data; // write: testing non-constness
              *outptr << std::setw (3+kcw) << (*i).key_ << std::setw(5+dcw) << (*i).data_ << '\n';
            }
            *outptr << '\n';
            break;
          case 'r': // non-const reverse
            *outptr << "Reverse non-const Traversal\n"
                    << "---------------------------\n";
            *outptr << std::setw (3+kcw) << "key" << std::setw(5+dcw) << "data" << '\n';
            *outptr << std::setw (3+kcw) << "---" << std::setw(5+dcw) << "----" << '\n';
            for( Iterator i = map.rBegin() ; i != map.rEnd() ; --i )
            {
              data = (*i).data_;     // read: testing constness
              (*i).data_ = 0 + data; // write: testing non-constness
              *outptr << std::setw (3+kcw) << (*i).key_ << std::setw(5+dcw) << (*i).data_ << '\n';
            }
            *outptr << '\n';
            break;
          case '!':
            {
              // Iterator
              size_t loc = 0;
              bool ok = 1;
              for( Iterator i = map.Begin(), j, k ; i != map.End() ; ++i )
              {
                k = i;
                j = ++(--k);
                // if (k != map.End()) *outptr << *k << '\n';
                if (j != k)
                {
                  *outptr << " ** ++(--) Iterator reciprocity failure at location " << loc << " with entry " << *i << '\n';
                  ok = 0;
                }
                k = i;
                j = --(++k);
                // if (k != map.End()) *outptr << *k << '\n';
                if (j != k)
                {
                  *outptr << " ** --(++) Iterator reciprocity failure at location " << loc << " with entry " << *i << '\n';
                  ok = 0;
                }
                ++loc;
              }
              if (ok) *outptr << "  passed Iterator reciprocity test\n";
              // Iterator */

              // ConstIterator
              loc = 0;
              ok = 1;
              for( ConstIterator i = map.Begin(), j, k ; i != map.End() ; ++i )
              {
                k = i;
                j = ++(--k);
                // if (k != map.End()) *outptr << *k << '\n';
                if (j != k)
                {
                  *outptr << " ** ++(--) ConstIterator reciprocity failure at location " << loc << " with entry " << *i << '\n';
                  ok = 0;
                }
                k = i;
                j = --(++k);
                // if (k != map.End()) *outptr << *k << '\n';
                if (j != k)
                {
                  *outptr << " ** --(++) ConstIterator reciprocity failure at location " << loc << " with entry " << *i << '\n';
                  ok = 0;
                }
                ++loc;
              }
              if (ok) *outptr << "  passed ConstIterator reciprocity test\n";
              // ConstIterator */
            }
            break;
          default: *outptr << " ** undefined command (level 2)\n";
        } // switch
        break;

      case '!': // structural check of table
        if (BATCH) *outptr << '\n';
        if (map.Check())
          *outptr << "  structural check OK\n";
        else
          *outptr << "  structural check NOT OK\n";
        break;

      default:
        if (BATCH) *outptr << '\n';
        *outptr << " ** undefined command (level 1): " << command << '\n';

    } // end switch
  }
  while (command != 'q');
  delete tablePtr;
  return 1;
} // end main()

void DisplayMenu(std::ostream& os)
{
  os << "   OPERATION                                        ENTRY\n"
     << "   ---------                                        -----\n"
     << "   input  from File  ...................................  < T|K|C filename [as Table, Keys, key-Counter]\n"
     << "   output (key,data) to file  ..........................  > filename\n"
     << "   map.Put(key,data)  ..................................  P key data\n"
     << "   map.Get(key)  .......................................  g key\n"
     << "   map.Get(key) const  .................................  G key\n"
     << "   map.Retrieve(key,&data)  ............................  R key\n"
     << "   map.Insert(key,data)  ...............................  1 key data\n"
     << "   map.Remove(key)  ....................................  2 key\n"
     << "   map[key] = data  ....................................  [ key ] = data\n"
     << "   map[key] += data  ...................................  [ key ] + data\n"
     << "   data = map[key] .....................................  [ key ]\n"
     << "   map.Clear() .........................................  C\n"
     << "   copy/assign test ....................................  =\n"
     << "   i = Begin() .........................................  B\n"
     << "   i = map.Includes(key) ...............................  I key\n"
     << "   ++i / --i  ..........................................  +/-\n"
     << "   *i = ? ..............................................  ?\n"
     << "   *i = newdata ........................................  * data\n"
     << "   map.Insert(i,key,data) ..............................  @ key data\n"
     << "   map.Insert(key,data)  ...............................  ^ key data\n"
     << "   Traversal  ..........................................  T f|F|r|R|! [Forward, Reverse, check]\n"
     << "   map.Dump(cout) ......................................  D\n"
     << "   map.Rehash(numBuckets)  .............................  H numBuckets\n"
     << "   map.Size(), map.NumBuckets()  .......................  s\n"
     << "   map.Size(), map.NumBuckets(), map.MaxBucketSize()  ..  S\n"
     << "   map.Check()  ........................................  !\n"
     << "   map.Analysis()  .....................................  A\n"
     << "   display Menu   ......................................  M\n"
     << "   switch from batch mode  .............................  X\n"
     << "   Quit program   ......................................  Q\n"
     << std::endl;
}

void DisplayPrompt(const char* kt, const char* dt, const char* ht, std::ostream& os)
{
  os << "HashTable < " << kt 
     << " , " << dt
     << " , " << ht
     << " >\n"
     << "Enter [command][arguments] ('M' for menu, 'Q' to quit): ";
}

template < typename T >
int CorrectDataWidth(const T& t, int dw)
{
  int digits = 1+(size_t)log10(t);
  if (dw < digits)
    dw = digits;
  return dw;
}
template <>
int CorrectDataWidth(const fsu::String& t, int dw)
{
  int digits = (int)t.Size();
  if (dw < digits)
    dw = digits;
  return dw;
}

void SkipDoc (std::istream& is)
{
  char ch = is.peek();
  while (ch == '#')
  {
    do ch = is.get();
    while (ch != '\n');
    ch = is.peek();
  }
}

// copy test
template < class C >
void CopyTest ( const C& x1, C x2, std::ostream& os )
{
  size_t size1, size2,
    nb1, nb2;
  bool ok = 1;
  if (x1 != x2)
  {
    os << " ** CopyTest: original and copy not equal\n";
    ok = 0;
  }
  /* // this needs implementing for HashTable
  if (!x2.CheckStructure(0,os))
  {
    os << " ** CopyTest: copy.CheckStructure() failed\n";
    ok = 0;
  } // */
  size1   = x1.Size();
  size2   = x2.Size();
  nb1 = x1.NumBuckets();
  nb2 = x2.NumBuckets();
  if (size1  != size2)
  {
    os << " ** CopyTest: original and copy have different size\n";
    ok = 0;
  }
  if (nb1  != nb2)
  {
    os << " ** CopyTest: original and copy have different number of buckets\n";
    ok = 0;
  }
  if (ok) os << " CopyTest OK\n";
} // CopyTest */

// assign test
template < class C >
void AssignTest ( const C& x1 , std::ostream& os )
{
  C x2;
  size_t size1, size2,
    nb1, nb2;
  bool ok = 1;
  size2 = x2.Size();
  x2 = x1;
  if (x1 != x2)
  {
    os << " ** AssignTest: original and copy not equal\n";
    ok = 0;
  }
  /* // this needs implementing for HashTable
  if (!x2.CheckStructure(0,os))
  {
    os << " ** AssignTest: assignee.CheckStructure() failed\n";
    ok = 0;
  } // */
  size1   = x1.Size();
  nb1 = x1.NumBuckets();
  size2   = x2.Size();
  nb2 = x2.NumBuckets();
  if (size1  != size2)
  {
    os << " ** AssignTest: original and copy have different size\n";
    ok = 0;
  }
  if (nb1  != nb2)
  {
    os << " ** AssignTest: original and copy have different number of buckets\n";
    ok = 0;
  }
  if (ok) os << " AssignTest OK\n";
} // AssignTest
