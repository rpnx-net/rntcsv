// test063.cpp - custom conversion per call

#include <iomanip>
#include <math.h>
#include <rntcsv.h>
#include "unittest.h"

// Conversion function for int data to be converted to fixed-point two decimal numbers
void ToVal(const std::string& pStr, int& pVal)
{
  pVal = static_cast<int>(roundf(100.0f * std::stof(pStr)));
}

struct Struct
{
  int val = 0;
};

void ToStruct(const std::string& pStr, Struct& pVal)
{
  pVal.val = static_cast<int>(roundf(100.0f * std::stof(pStr)));
}

int main()
{
  int rv = 0;

  std::string csv =
    "-,A,B,C\n"
    "1,1,10,100,1000\n"
    "2,0.1,0.01,0.001,0.006\n"
  ;

  std::string path = unittest::TempPath();
  unittest::WriteFile(path, csv);

  try
  {
    rntcsv::document doc(path, rntcsv::label_parameters(0, 0));

    // to_value
    unittest::ExpectEqual(int, doc.cell<int>(0, 0, ToVal), 100);
    unittest::ExpectEqual(int, doc.cell<int>(1, "1", ToVal), 1000);
    unittest::ExpectEqual(int, doc.cell<int>("A", 0, ToVal), 100);
    unittest::ExpectEqual(int, doc.cell<int>("B", "1", ToVal), 1000);

    unittest::ExpectEqual(int, doc.row<int>(0, ToVal).at(0), 100);
    unittest::ExpectEqual(int, doc.row<int>("2", ToVal).at(0), 10);

    unittest::ExpectEqual(int, doc.column<int>(0, ToVal).at(0), 100);
    unittest::ExpectEqual(int, doc.column<int>("B", ToVal).at(0), 1000);

    // ToStruct
    unittest::ExpectEqual(int, doc.cell<Struct>(0, 0, ToStruct).val, 100);
    unittest::ExpectEqual(int, doc.cell<Struct>(1, "1", ToStruct).val, 1000);
    unittest::ExpectEqual(int, doc.cell<Struct>("A", 0, ToStruct).val, 100);
    unittest::ExpectEqual(int, doc.cell<Struct>("B", "1", ToStruct).val, 1000);

    unittest::ExpectEqual(int, doc.row<Struct>(0, ToStruct).at(0).val, 100);
    unittest::ExpectEqual(int, doc.row<Struct>("2", ToStruct).at(0).val, 10);

    unittest::ExpectEqual(int, doc.column<Struct>(0, ToStruct).at(0).val, 100);
    unittest::ExpectEqual(int, doc.column<Struct>("B", ToStruct).at(0).val, 1000);
  }
  catch (const std::exception& ex)
  {
    std::cout << ex.what() << std::endl;
    rv = 1;
  }

  unittest::DeleteFile(path);

  return rv;
}
