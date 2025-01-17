// test079.cpp - perform multiple assign_from_file calls on single document instance

#include <rntcsv.h>
#include "unittest.h"

int main()
{
  int rv = 0;

  std::string csv1 =
    "A,B,C,D\n"
    "1,3,9,81\n"
    "2,4,16,256\n"
  ;

  std::string csv2 =
    "A,B,C,D\n"
    "1,3,9,81\n"
  ;

  std::string csv3 =
    "A,B,C,D\n"
    "1,3,9,81\n"
    "2,4,16,256\n"
    "4,16,256,67840\n"
  ;

  std::string path = unittest::TempPath();
  unittest::WriteFile(path, csv3);

  try
  {
    std::istringstream sstream1(csv1);
    rntcsv::document doc(sstream1);
    unittest::ExpectEqual(size_t, doc.row_count(), 2);

    std::istringstream sstream2(csv2);
      doc.read(sstream2);
    unittest::ExpectEqual(size_t, doc.row_count(), 1);

      doc.assign_from_file(path);
    unittest::ExpectEqual(size_t, doc.row_count(), 3);
  }
  catch (const std::exception& ex)
  {
    std::cout << ex.what() << std::endl;
    rv = 1;
  }

  unittest::DeleteFile(path);

  return rv;
}
