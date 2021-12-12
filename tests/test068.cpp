// test068.cpp - test automatic dequote disabled

#include "rntcsv.h"
#include "unittest.h"

int main()
{
  int rv = 0;

  std::string csv =
    "\"col 1\"\n"
    "\"\"\n"
    "\" \"\n"
    "\"a b\"\n"
    "\"\"\"a b\"\"\"\n"
    "\" \"\"a\"\" \"\n"
  ;

  std::string csvreadref =
    "\"col 1\"\n"
    "\" \"\n"
    "\"a b\"\n"
    "\"\"\"a b\"\"\"\n"
    "\" \"\"a\"\" \"\n"
  ;

  std::string path = unittest::TempPath();
  unittest::WriteFile(path, csv);

  try
  {
    // read
    {
      rntcsv::document doc(path, rntcsv::label_parameters(0 /* pColumnNameIdx */, -1 /* pRowNameIdx */),
                           rntcsv::separator_parameters(',', false /* pTrim */, rntcsv::sPlatformHasCR /* pHasCR */,
                                                        false /* pQuotedLinebreaks */, false /* pAutoQuote */));
      unittest::ExpectEqual(std::string, doc.GetCell<std::string>("\"col 1\"", 0), "\"\"");
      unittest::ExpectEqual(std::string, doc.GetCell<std::string>("\"col 1\"", 1), "\" \"");
      unittest::ExpectEqual(std::string, doc.GetCell<std::string>("\"col 1\"", 2), "\"a b\"");
      unittest::ExpectEqual(std::string, doc.GetCell<std::string>("\"col 1\"", 3), "\"\"\"a b\"\"\"");
      unittest::ExpectEqual(std::string, doc.GetCell<std::string>("\"col 1\"", 4), "\" \"\"a\"\" \"");
    }

    // write
    {
      unittest::WriteFile(path, csvreadref);
      rntcsv::document doc(path, rntcsv::label_parameters(0 /* pColumnNameIdx */, -1 /* pRowNameIdx */),
                           rntcsv::separator_parameters(',', false /* pTrim */, rntcsv::sPlatformHasCR /* pHasCR */,
                                                        false /* pQuotedLinebreaks */, false /* pAutoQuote */));

      doc.Save();
      const std::string& csvread = unittest::ReadFile(path);
      unittest::ExpectEqual(std::string, csvreadref, csvread);
    }
  }
  catch (const std::exception& ex)
  {
    std::cout << ex.what() << std::endl;
    rv = 1;
  }

  unittest::DeleteFile(path);

  return rv;
}
