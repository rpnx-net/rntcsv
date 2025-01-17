// test062.cpp - read cell value by mixed name / index

#include <rntcsv.h>
#include "unittest.h"

int main()
{
  int rv = 0;

  std::string csv =
    "-,A,B,C\n"
    "1,3,9,81\n"
    "2,4,16,256\n"
  ;

  std::string path = unittest::TempPath();
  unittest::WriteFile(path, csv);

  try
  {
    rntcsv::document doc(path, rntcsv::label_parameters(0, 0));
    unittest::ExpectEqual(int, doc.cell<int>("A", 0), 3);
    unittest::ExpectEqual(int, doc.cell<int>("B", 0), 9);
    unittest::ExpectEqual(int, doc.cell<int>("C", 0), 81);

    unittest::ExpectEqual(std::string, doc.cell<std::string>(0, "2"), "4");
    unittest::ExpectEqual(std::string, doc.cell<std::string>(1, "2"), "16");
    unittest::ExpectEqual(std::string, doc.cell<std::string>(2, "2"), "256");
  }
  catch (const std::exception& ex)
  {
    std::cout << ex.what() << std::endl;
    rv = 1;
  }

  unittest::DeleteFile(path);

  return rv;
}
