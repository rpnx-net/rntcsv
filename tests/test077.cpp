// test077.cpp - copy constructor

#include <rntcsv.h>
#include "unittest.h"

rntcsv::document LoadDoc(const std::string& p_Path)
{
  rntcsv::document doc = rntcsv::document(p_Path, rntcsv::label_parameters(0, 0));
  return doc;
}

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
    rntcsv::document doc1 = LoadDoc(path);
    unittest::ExpectEqual(int, doc1.cell<int>(0, 0), 3);
    unittest::ExpectEqual(int, doc1.cell<int>(1, 0), 9);
    unittest::ExpectEqual(int, doc1.cell<int>(2, 0), 81);

    const rntcsv::document& doc2 = LoadDoc(path);
    unittest::ExpectEqual(std::string, doc2.cell<std::string>("A", "2"), "4");
    unittest::ExpectEqual(std::string, doc2.cell<std::string>("B", "2"), "16");
    unittest::ExpectEqual(std::string, doc2.cell<std::string>("C", "2"), "256");

    rntcsv::document doc3 = doc2;
    unittest::ExpectEqual(int, doc1.cell<int>(0, 0), 3);
    unittest::ExpectEqual(int, doc1.cell<int>(1, 0), 9);
    unittest::ExpectEqual(int, doc1.cell<int>(2, 0), 81);
  }
  catch (const std::exception& ex)
  {
    std::cout << ex.what() << std::endl;
    rv = 1;
  }

  unittest::DeleteFile(path);

  return rv;
}
