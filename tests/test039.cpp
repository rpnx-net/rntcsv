// test039.cpp - write windows CR/LF linebreaks

#include <rntcsv.h>
#include "unittest.h"

int main()
{
  int rv = 0;

  std::string csv =
    "-,A,B,C\r\n"
    "1,3,9,81\r\n"
    "2,4,16,256\r\n"
  ;

  std::string path = unittest::TempPath();
  unittest::WriteFile(path, csv);

  try
  {
    rntcsv::document doc(path, rntcsv::label_parameters(0, 0));

    unittest::ExpectEqual(std::string, doc.cell<std::string>(0, 0), "3");
    unittest::ExpectEqual(std::string, doc.cell<std::string>(1, 0), "9");
    unittest::ExpectEqual(std::string, doc.cell<std::string>(2, 0), "81");

    unittest::ExpectEqual(std::string, doc.cell<std::string>(0, 1), "4");
    unittest::ExpectEqual(std::string, doc.cell<std::string>(1, 1), "16");
    unittest::ExpectEqual(std::string, doc.cell<std::string>(2, 1), "256");

      doc.write();

    std::string csvread = unittest::ReadFile(path);

    unittest::ExpectEqual(std::string, csv, csvread);
  }
  catch (const std::exception& ex)
  {
    std::cout << ex.what() << std::endl;
    rv = 1;
  }

  unittest::DeleteFile(path);

  return rv;
}
