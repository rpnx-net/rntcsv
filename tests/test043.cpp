// test043.cpp - read column and row values as char

#include <rntcsv.h>
#include "unittest.h"

int main()
{
  int rv = 0;

  std::string csv =
    "-,A,B,C\n"
    "1,a,b,c\n"
    "2,x,y,z\n"
  ;

  std::string path = unittest::TempPath();
  unittest::WriteFile(path, csv);

  try
  {
    rntcsv::document doc(path, rntcsv::label_parameters(0, 0));

    std::vector<char> chars;

    chars = doc.column<char>(0);
    unittest::ExpectEqual(size_t, chars.size(), 2);
    unittest::ExpectEqual(char, chars.at(0), 'a');
    unittest::ExpectEqual(char, chars.at(1), 'x');

    chars = doc.column<char>("B");
    unittest::ExpectEqual(size_t, chars.size(), 2);
    unittest::ExpectEqual(char, chars.at(0), 'b');
    unittest::ExpectEqual(char, chars.at(1), 'y');

    chars = doc.row<char>("2");
    unittest::ExpectEqual(size_t, chars.size(), 3);
    unittest::ExpectEqual(char, chars.at(0), 'x');
    unittest::ExpectEqual(char, chars.at(1), 'y');
    unittest::ExpectEqual(char, chars.at(2), 'z');
  }
  catch (const std::exception& ex)
  {
    std::cout << ex.what() << std::endl;
    rv = 1;
  }

  unittest::DeleteFile(path);

  return rv;
}
