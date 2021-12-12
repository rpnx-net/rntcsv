// test073.cpp - get column index, shift column name index

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
    rntcsv::document doc(path, rntcsv::label_parameters(1));

    unittest::ExpectEqual(ssize_t, doc.GetColumnIdx("-"), -1);
    unittest::ExpectEqual(ssize_t, doc.GetColumnIdx("A"), -1);
    unittest::ExpectEqual(ssize_t, doc.GetColumnIdx("B"), -1);
    unittest::ExpectEqual(ssize_t, doc.GetColumnIdx("C"), -1);
    unittest::ExpectEqual(ssize_t, doc.GetColumnIdx("D"), -1);

    unittest::ExpectEqual(ssize_t, doc.GetColumnIdx("1"), 0);
    unittest::ExpectEqual(ssize_t, doc.GetColumnIdx("3"), 1);
    unittest::ExpectEqual(ssize_t, doc.GetColumnIdx("9"), 2);
    unittest::ExpectEqual(ssize_t, doc.GetColumnIdx("81"), 3);
    unittest::ExpectEqual(ssize_t, doc.GetColumnIdx("91"), -1);
  }
  catch (const std::exception& ex)
  {
    std::cout << ex.what() << std::endl;
    rv = 1;
  }

  unittest::DeleteFile(path);

  return rv;
}
