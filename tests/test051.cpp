// test051.cpp - get column and row names

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
    std::vector<std::string> columnNames = doc.column_names();
    unittest::ExpectEqual(size_t, columnNames.size(), 3);
    unittest::ExpectEqual(std::string, columnNames[0], "A");
    unittest::ExpectEqual(std::string, columnNames[1], "B");
    unittest::ExpectEqual(std::string, columnNames[2], "C");

    std::vector<std::string> rowNames = doc.row_names();
    unittest::ExpectEqual(size_t, rowNames.size(), 2);
    unittest::ExpectEqual(std::string, rowNames[0], "1");
    unittest::ExpectEqual(std::string, rowNames[1], "2");

    rntcsv::document doc2(path, rntcsv::label_parameters(0, -1));
    std::vector<std::string> columnNames2 = doc2.column_names();
    unittest::ExpectEqual(size_t, columnNames2.size(), 4);
    unittest::ExpectEqual(std::string, columnNames2[0], "-");
    unittest::ExpectEqual(std::string, columnNames2[1], "A");
    unittest::ExpectEqual(std::string, columnNames2[2], "B");
    unittest::ExpectEqual(std::string, columnNames2[3], "C");

    rntcsv::document doc3(path, rntcsv::label_parameters(-1, 0));
    std::vector<std::string> rowNames2 = doc3.row_names();
    unittest::ExpectEqual(size_t, rowNames2.size(), 3);
    unittest::ExpectEqual(std::string, rowNames2[0], "-");
    unittest::ExpectEqual(std::string, rowNames2[1], "1");
    unittest::ExpectEqual(std::string, rowNames2[2], "2");
  }
  catch (const std::exception& ex)
  {
    std::cout << ex.what() << std::endl;
    rv = 1;
  }

  unittest::DeleteFile(path);

  return rv;
}
