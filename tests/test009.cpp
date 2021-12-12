// test009.cpp - generate new document by row

#include <rntcsv.h>
#include "unittest.h"

int main()
{
  int rv = 0;

  std::string csvref =
    ",A,B,C,D\n"
    "0,2,4,16,256\n"
    "1,3,9,81,6561\n"
    "2,4,16,256,65536\n"
    "3,5,25,625,390625\n"
  ;

  std::string path = unittest::TempPath();

  try
  {
    rntcsv::document doc("", rntcsv::label_parameters(0, 0), rntcsv::separator_parameters(',', false, false));

      doc.assign_row<int>(0, std::vector<int>({2, 4}));
      doc.assign_row<int>(1, std::vector<int>({3, 9, 81, 6561}));
      doc.assign_row<int>(2, std::vector<int>({4, 16, 256, 65536}));
      doc.assign_row<int>(3, std::vector<int>({5, 25, 625, 390625}));

      doc.set_cell<int>(2, 0, 16);
      doc.set_cell<int>(3, 0, 256);

      doc.set_column_name(0, "A");
      doc.set_column_name(1, "B");
      doc.set_column_name(2, "C");
      doc.set_column_name(3, "D");

      doc.row_name(0, "0");
      doc.row_name(1, "1");
      doc.row_name(2, "2");
      doc.row_name(3, "3");

      doc.write(path);

    std::string csvread = unittest::ReadFile(path);

    unittest::ExpectEqual(std::string, csvref, csvread);
  }
  catch (const std::exception& ex)
  {
    std::cout << ex.what() << std::endl;
    rv = 1;
  }

  unittest::DeleteFile(path);

  return rv;
}
