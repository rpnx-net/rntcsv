// test071.cpp - get row index, no column labels

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
    // No Column Label
    // Row Label
    // ↓
    // -,  A,  B,  C
    // 1,  3,  9,  81
    // 2,  4,  16, 256
    rntcsv::document doc2(path, rntcsv::label_parameters(-1, 0));

    unittest::ExpectEqual(ssize_t, doc2.row_index("-"), 0);
    unittest::ExpectEqual(ssize_t, doc2.row_index("1"), 1);
    unittest::ExpectEqual(ssize_t, doc2.row_index("2"), 2);
    unittest::ExpectEqual(ssize_t, doc2.row_index("3"), -1);
  }
  catch (const std::exception& ex)
  {
    std::cout << ex.what() << std::endl;
    rv = 1;
  }

  unittest::DeleteFile(path);

  return rv;
}
