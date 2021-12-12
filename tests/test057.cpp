// test057.cpp - read UTF-16 LE

#include <rntcsv.h>
#include "unittest.h"

int main()
{
  int rv = 0;

  const unsigned char u16le[] =
  {
    0xff, 0xfe,
    0x2d, 0x00, 0x2c, 0x00, 0x41, 0x00, 0x2c, 0x00, 0x42, 0x00, 0x2c, 0x00,
    0x43, 0x00, 0x0a, 0x00, 0x31, 0x00, 0x2c, 0x00, 0x33, 0x00, 0x2c, 0x00,
    0x39, 0x00, 0x2c, 0x00, 0x38, 0x00, 0x31, 0x00, 0x0a, 0x00, 0x32, 0x00,
    0x2c, 0x00, 0x34, 0x00, 0x2c, 0x00, 0x31, 0x00, 0x36, 0x00, 0x2c, 0x00,
    0x32, 0x00, 0x35, 0x00, 0x36, 0x00, 0x0a, 0x00
  };
  const unsigned int u16le_len = 58;

  std::string csv(reinterpret_cast<const char *>(u16le), u16le_len);
  // "-,A,B,C\n"
  // "1,3,9,81\n"
  // "2,4,16,256\n"

  std::string path = unittest::TempPath();
  unittest::WriteFile(path, csv);

  try
  {
    rntcsv::document doc(path, rntcsv::label_parameters(0, 0));
    unittest::ExpectEqual(int, doc.cell<int>(0, 0), 3);
    unittest::ExpectEqual(int, doc.cell<int>(1, 0), 9);
    unittest::ExpectEqual(int, doc.cell<int>(2, 0), 81);

    unittest::ExpectEqual(std::string, doc.cell<std::string>("A", "2"), "4");
    unittest::ExpectEqual(std::string, doc.cell<std::string>("B", "2"), "16");
    unittest::ExpectEqual(std::string, doc.cell<std::string>("C", "2"), "256");
  }
  catch (const std::exception& ex)
  {
    std::cout << ex.what() << std::endl;
    rv = 1;
  }

  unittest::DeleteFile(path);

  return rv;
}
