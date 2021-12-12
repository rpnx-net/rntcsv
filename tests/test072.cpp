// test072.cpp - get column index

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
    rntcsv::document doc(path);

    unittest::ExpectEqual(ssize_t, doc.GetColumnIdx("-"), 0);
    unittest::ExpectEqual(ssize_t, doc.GetColumnIdx("A"), 1);
    unittest::ExpectEqual(ssize_t, doc.GetColumnIdx("B"), 2);
    unittest::ExpectEqual(ssize_t, doc.GetColumnIdx("C"), 3);
    unittest::ExpectEqual(ssize_t, doc.GetColumnIdx("D"), -1);
  }
  catch (const std::exception& ex)
  {
    std::cout << ex.what() << std::endl;
    rv = 1;
  }

  unittest::DeleteFile(path);

  return rv;
}
