/*
* rpnx-net/rntcsv
* https://github.com/rpnx-net/rntcsv
* Forked from rntcsv
* 
* Copyright (C) 2021 Ryan P. Nicholl
* All rights reserved.
* 
 * rntcsv.h
 *
 * URL:      https://github.com/d99kris/rapidcsv
 * Version:  8.53
 *
 * Copyright (C) 2017-2021 Kristofer Berggren
 * All rights reserved.
 *
 * rntcsv is distributed under the BSD 3-Clause license, see LICENSE for details.
 *
 */

#ifndef RNT_RNTCSV_HPP
#define RNT_RNTCSV_HPP

#include <algorithm>
#include <cassert>
#include <cmath>
#include <codecvt>
#include <locale>
#include <fstream>
#include <functional>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <typeinfo>
#include <vector>

#if defined(_MSC_VER)
#include <BaseTsd.h>
typedef SSIZE_T ssize_t;
#endif

namespace rntcsv
{
#if defined(_MSC_VER)
  static const bool s_platform_crlf = true;
#else
  static const bool s_platform_crlf = false;
#endif

  /**
   * @brief     Datastructure holding parameters controlling how invalid numbers (including
   *            empty strings) should be handled.
   */
  struct converter_parameters
  {
    /**
     * @brief   Constructor
     * @param   pHasDefaultConverter  specifies if conversion of non-numerical strings shall be
     *                                converted to a default numerical value, instead of causing
     *                                an exception to be thrown (default).
     * @param   pDefaultFloat         floating-point default value to represent invalid numbers.
     * @param   pDefaultInteger       integer default value to represent invalid numbers.
     */
    explicit converter_parameters(const bool pHasDefaultConverter = false,
                                  const long double pDefaultFloat = std::numeric_limits<long double>::signaling_NaN(),
                                  const long long pDefaultInteger = 0)
      : mHasDefaultConverter(pHasDefaultConverter)
      , mDefaultFloat(pDefaultFloat)
      , mDefaultInteger(pDefaultInteger)
    {
    }

    /**
     * @brief   specifies if conversion of non-numerical strings shall be converted to a default
     *          numerical value, instead of causing an exception to be thrown (default).
     */
    bool mHasDefaultConverter;

    /**
     * @brief   floating-point default value to represent invalid numbers.
     */
    long double mDefaultFloat;

    /**
     * @brief   integer default value to represent invalid numbers.
     */
    long long mDefaultInteger;
  };

  /**
   * @brief     Exception thrown when attempting to access document data in a datatype which
   *            is not supported by the converter class.
   */
  class no_converter : public std::exception
  {
    /**
     * @brief   Provides details about the exception
     * @returns an explanatory string
     */
    virtual const char* what() const throw()
    {
      return "unsupported conversion datatype";
    }
  };

  /**
   * @brief     Class providing conversion to/from numerical datatypes and strings. Only
   *            intended for rntcsv internal usage, but exposed externally to allow
   *            specialization for custom datatype conversions.
   */
  template<typename T>
  class converter
  {
  public:
    /**
     * @brief   Constructor
     * @param   pConverterParams      specifies how conversion of non-numerical values to
     *                                numerical datatype shall be handled.
     */
    converter(const converter_parameters& pConverterParams)
      : m_converter_parameters(pConverterParams)
    {
    }

    /**
     * @brief   Converts numerical value to string representation.
     * @param   pVal                  numerical value
     * @param   pStr                  output string
     */
    void to_string(const T& pVal, std::string& pStr) const
    {
      if (typeid(T) == typeid(int) ||
          typeid(T) == typeid(long) ||
          typeid(T) == typeid(long long) ||
          typeid(T) == typeid(unsigned) ||
          typeid(T) == typeid(unsigned long) ||
          typeid(T) == typeid(unsigned long long) ||
          typeid(T) == typeid(float) ||
          typeid(T) == typeid(double) ||
          typeid(T) == typeid(long double) ||
          typeid(T) == typeid(char))
      {
        std::ostringstream out;
        out << pVal;
        pStr = out.str();
      }
      else
      {
        throw no_converter();
      }
    }

    /**
     * @brief   Converts string holding a numerical value to numerical datatype representation.
     * @param   pVal                  numerical value
     * @param   pStr                  output string
     */
    void to_value(const std::string& pStr, T& pVal) const
    {
      try
      {
        if (typeid(T) == typeid(int))
        {
          pVal = static_cast<T>(std::stoi(pStr));
          return;
        }
        else if (typeid(T) == typeid(long))
        {
          pVal = static_cast<T>(std::stol(pStr));
          return;
        }
        else if (typeid(T) == typeid(long long))
        {
          pVal = static_cast<T>(std::stoll(pStr));
          return;
        }
        else if (typeid(T) == typeid(unsigned))
        {
          pVal = static_cast<T>(std::stoul(pStr));
          return;
        }
        else if (typeid(T) == typeid(unsigned long))
        {
          pVal = static_cast<T>(std::stoul(pStr));
          return;
        }
        else if (typeid(T) == typeid(unsigned long long))
        {
          pVal = static_cast<T>(std::stoull(pStr));
          return;
        }
      }
      catch (...)
      {
        if (!m_converter_parameters.mHasDefaultConverter)
        {
          throw;
        }
        else
        {
          pVal = static_cast<T>(m_converter_parameters.mDefaultInteger);
          return;
        }
      }

      try
      {
        if (typeid(T) == typeid(float))
        {
          pVal = static_cast<T>(std::stof(pStr));
          return;
        }
        else if (typeid(T) == typeid(double))
        {
          pVal = static_cast<T>(std::stod(pStr));
          return;
        }
        else if (typeid(T) == typeid(long double))
        {
          pVal = static_cast<T>(std::stold(pStr));
          return;
        }
      }
      catch (...)
      {
        if (!m_converter_parameters.mHasDefaultConverter)
        {
          throw;
        }
        else
        {
          pVal = static_cast<T>(m_converter_parameters.mDefaultFloat);
          return;
        }
      }

      if (typeid(T) == typeid(char))
      {
        pVal = static_cast<T>(pStr[0]);
        return;
      }
      else
      {
        throw no_converter();
      }
    }

  private:
    const converter_parameters& m_converter_parameters;
  };

  /**
   * @brief     Specialized implementation handling string to string conversion.
   * @param     pVal                  string
   * @param     pStr                  string
   */
  template<>
  inline void converter<std::string>::to_string(const std::string& pVal, std::string& pStr) const
  {
    pStr = pVal;
  }

  /**
   * @brief     Specialized implementation handling string to string conversion.
   * @param     pVal                  string
   * @param     pStr                  string
   */
  template<>
  inline void converter<std::string>::to_value(const std::string& pStr, std::string& pVal) const
  {
    pVal = pStr;
  }

  template<typename T>
  using conversion_func = std::function<void (const std::string & pStr, T & pVal)>;

  /**
   * @brief     Datastructure holding parameters controlling which row and column should be
   *            treated as labels.
   */
  struct label_parameters
  {
    /**
     * @brief   Constructor
     * @param   pColumnNameIdx        specifies the zero-based row index of the column labels, setting
     *                                it to -1 prevents column lookup by label name, and gives access
     *                                to all rows as document data. Default: 0
     * @param   pRowNameIdx           specifies the zero-based column index of the row labels, setting
     *                                it to -1 prevents row lookup by label name, and gives access
     *                                to all columns as document data. Default: -1
     */
    explicit label_parameters(const int pColumnNameIdx = 0, const int pRowNameIdx = -1)
      : mColumnNameIdx(pColumnNameIdx)
      , mRowNameIdx(pRowNameIdx)
    {
    }

    /**
     * @brief   specifies the zero-based row index of the column labels.
     */
    int mColumnNameIdx;

    /**
     * @brief   specifies the zero-based column index of the row labels.
     */
    int mRowNameIdx;
  };

  /**
   * @brief     Datastructure holding parameters controlling how the CSV data fields are separated.
   */
  struct separator_parameters
  {
    /**
     * @brief   Constructor
     * @param   pSeparator            specifies the column separator (default ',').
     * @param   pTrim                 specifies whether to trim leading and trailing spaces from
     *                                cells read (default false).
     * @param   pHasCR                specifies whether a new document (i.e. not an existing document read)
     *                                should use CR/LF instead of only LF (default is to use standard
     *                                behavior of underlying platforms - CR/LF for Win, and LF for others).
     * @param   pQuotedLinebreaks     specifies whether to allow line breaks in quoted text (default false)
     * @param   pAutoQuote            specifies whether to automatically dequote data during read, and add
     *                                quotes during write (default true).
     */
    explicit separator_parameters(const char pSeparator = ',', const bool pTrim = false,
                                  const bool pHasCR = s_platform_crlf, const bool pQuotedLinebreaks = false,
                                  const bool pAutoQuote = true)
      : mSeparator(pSeparator)
      , mTrim(pTrim)
      , mHasCR(pHasCR)
      , mQuotedLinebreaks(pQuotedLinebreaks)
      , mAutoQuote(pAutoQuote)
    {
    }

    /**
     * @brief   specifies the column separator.
     */
    char mSeparator;

    /**
     * @brief   specifies whether to trim leading and trailing spaces from cells read.
     */
    bool mTrim;

    /**
     * @brief   specifies whether new documents should use CR/LF instead of LF.
     */
    bool mHasCR;

    /**
     * @brief   specifies whether to allow line breaks in quoted text.
     */
    bool mQuotedLinebreaks;

    /**
     * @brief   specifies whether to automatically dequote cell data.
     */
    bool mAutoQuote;
  };

  /**
   * @brief     Datastructure holding parameters controlling how special line formats should be
   *            treated.
   */
  struct line_reader_parameters
  {
    /**
     * @brief   Constructor
     * @param   pSkipCommentLines     specifies whether to skip lines prefixed with
     *                                mCommentPrefix. Default: false
     * @param   pCommentPrefix        specifies which prefix character to indicate a comment
     *                                line. Default: #
     * @param   pSkipEmptyLines       specifies whether to skip empty lines. Default: false
     */
    explicit line_reader_parameters(const bool pSkipCommentLines = false,
                                    const char pCommentPrefix = '#',
                                    const bool pSkipEmptyLines = false)
      : mSkipCommentLines(pSkipCommentLines)
      , mCommentPrefix(pCommentPrefix)
      , mSkipEmptyLines(pSkipEmptyLines)
    {
    }

    /**
     * @brief   specifies whether to skip lines prefixed with mCommentPrefix.
     */
    bool mSkipCommentLines;

    /**
     * @brief   specifies which prefix character to indicate a comment line.
     */
    char mCommentPrefix;

    /**
     * @brief   specifies whether to skip empty lines.
     */
    bool mSkipEmptyLines;
  };

  /**
   * @brief     Class representing a CSV document.
   */
  class document
  {
  public:
    /**
     * @brief   Constructor
     * @param   pPath                 specifies the path of an existing CSV-file to populate the document
     *                                data with.
     * @param   pLabelParams          specifies which row and column should be treated as labels.
     * @param   pSeparatorParams      specifies which field and row separators should be used.
     * @param   pConverterParams      specifies how invalid numbers (including empty strings) should be
     *                                handled.
     * @param   pLineReaderParams     specifies how special line formats should be treated.
     */
    explicit document(const std::string& pPath = std::string(),
                      const label_parameters& pLabelParams = label_parameters(),
                      const separator_parameters& pSeparatorParams = separator_parameters(),
                      const converter_parameters& pConverterParams = converter_parameters(),
                      const line_reader_parameters& pLineReaderParams = line_reader_parameters())
      : mPath(pPath)
      , mLabelParams(pLabelParams)
      , mSeparatorParams(pSeparatorParams)
      , mConverterParams(pConverterParams)
      , mLineReaderParams(pLineReaderParams)
    {
      if (!mPath.empty())
      {
          read_csv();
      }
    }

    /**
     * @brief   Constructor
     * @param   pStream               specifies an input stream to read CSV data from.
     * @param   pLabelParams          specifies which row and column should be treated as labels.
     * @param   pSeparatorParams      specifies which field and row separators should be used.
     * @param   pConverterParams      specifies how invalid numbers (including empty strings) should be
     *                                handled.
     * @param   pLineReaderParams     specifies how special line formats should be treated.
     */
    explicit document(std::istream& pStream,
                      const label_parameters& pLabelParams = label_parameters(),
                      const separator_parameters& pSeparatorParams = separator_parameters(),
                      const converter_parameters& pConverterParams = converter_parameters(),
                      const line_reader_parameters& pLineReaderParams = line_reader_parameters())
      : mPath()
      , mLabelParams(pLabelParams)
      , mSeparatorParams(pSeparatorParams)
      , mConverterParams(pConverterParams)
      , mLineReaderParams(pLineReaderParams)
    {
        read_csv(pStream);
    }

    /**
     * @brief   Read document data from file.
     * @param   pPath                 specifies the path of an existing CSV-file to populate the document
     *                                data with.
     * @param   pLabelParams          specifies which row and column should be treated as labels.
     * @param   pSeparatorParams      specifies which field and row separators should be used.
     * @param   pConverterParams      specifies how invalid numbers (including empty strings) should be
     *                                handled.
     * @param   pLineReaderParams     specifies how special line formats should be treated.
     */
    void assign_from_file(const std::string& pPath,
                          const label_parameters& pLabelParams = label_parameters(),
                          const separator_parameters& pSeparatorParams = separator_parameters(),
                          const converter_parameters& pConverterParams = converter_parameters(),
                          const line_reader_parameters& pLineReaderParams = line_reader_parameters())
    {
      mPath = pPath;
      mLabelParams = pLabelParams;
      mSeparatorParams = pSeparatorParams;
      mConverterParams = pConverterParams;
      mLineReaderParams = pLineReaderParams;
        read_csv();
    }

    /**
     * @brief   Read document data from stream.
     * @param   pStream               specifies an input stream to read CSV data from.
     * @param   pLabelParams          specifies which row and column should be treated as labels.
     * @param   pSeparatorParams      specifies which field and row separators should be used.
     * @param   pConverterParams      specifies how invalid numbers (including empty strings) should be
     *                                handled.
     * @param   pLineReaderParams     specifies how special line formats should be treated.
     */
    void read(std::istream& pStream,
              const label_parameters& pLabelParams = label_parameters(),
              const separator_parameters& pSeparatorParams = separator_parameters(),
              const converter_parameters& pConverterParams = converter_parameters(),
              const line_reader_parameters& pLineReaderParams = line_reader_parameters())
    {
      mPath = "";
      mLabelParams = pLabelParams;
      mSeparatorParams = pSeparatorParams;
      mConverterParams = pConverterParams;
      mLineReaderParams = pLineReaderParams;
        read_csv(pStream);
    }

    /**
     * @brief   Write document data to file.
     * @param   pPath                 optionally specifies the path where the CSV-file will be created
     *                                (if not specified, the original path provided when creating or
     *                                loading the document data will be used).
     */
    void write(const std::string& pPath = std::string())
    {
      if (!pPath.empty())
      {
        mPath = pPath;
      }
      write_csv();
    }

    /**
     * @brief   Write document data to stream.
     * @param   pStream               specifies an output stream to write the data to.
     */
    void write(std::ostream& pStream)
    {
      write_csv(pStream);
    }

    /**
     * @brief   Clears loaded document data.
     *
     */
    void clear()
    {
      mData.clear();
      mColumnNames.clear();
      mRowNames.clear();
      mIsUtf16 = false;
      mIsLE = false;
    }

    /**
     * @brief   Get column index by name.
     * @param   pColumnName           column label name.
     * @returns zero-based column index.
     */
    ssize_t column_index(const std::string& pColumnName) const
    {
      if (mLabelParams.mColumnNameIdx >= 0)
      {
        if (mColumnNames.find(pColumnName) != mColumnNames.end())
        {
          return mColumnNames.at(pColumnName) - (mLabelParams.mRowNameIdx + 1);
        }
      }
      return -1;
    }

    /**
     * @brief   Get column by index.
     * @param   pColumnIdx            zero-based column index.
     * @returns vector of column data.
     */
    template<typename T>
    std::vector<T> column(const size_t pColumnIdx) const
    {
      const ssize_t columnIdx = pColumnIdx + (mLabelParams.mRowNameIdx + 1);
      std::vector<T> column;
      converter<T> converter(mConverterParams);
      for (auto itRow = mData.begin(); itRow != mData.end(); ++itRow)
      {
        if (std::distance(mData.begin(), itRow) > mLabelParams.mColumnNameIdx)
        {
          if (columnIdx < static_cast<ssize_t>(itRow->size()))
          {
            T val;
              converter.to_value(itRow->at(columnIdx), val);
            column.push_back(val);
          }
          else
          {
            const std::string errStr = "requested column index " +
              std::to_string(columnIdx - (mLabelParams.mRowNameIdx + 1)) + " >= " +
              std::to_string(itRow->size() - (mLabelParams.mRowNameIdx + 1)) +
              " (number of columns on row index " +
              std::to_string(std::distance(mData.begin(), itRow) -
                             (mLabelParams.mColumnNameIdx + 1)) + ")";
            throw std::out_of_range(errStr);
          }
        }
      }
      return column;
    }

    /**
     * @brief   Get column by index.
     * @param   pColumnIdx            zero-based column index.
     * @param   pToVal                conversion function.
     * @returns vector of column data.
     */
    template<typename T>
    std::vector<T> column(const size_t pColumnIdx, conversion_func<T> pToVal) const
    {
      const ssize_t columnIdx = pColumnIdx + (mLabelParams.mRowNameIdx + 1);
      std::vector<T> column;
      for (auto itRow = mData.begin(); itRow != mData.end(); ++itRow)
      {
        if (std::distance(mData.begin(), itRow) > mLabelParams.mColumnNameIdx)
        {
          T val;
          pToVal(itRow->at(columnIdx), val);
          column.push_back(val);
        }
      }
      return column;
    }

    /**
     * @brief   Get column by name.
     * @param   pColumnName           column label name.
     * @returns vector of column data.
     */
    template<typename T>
    std::vector<T> column(const std::string& pColumnName) const
    {
      const ssize_t columnIdx = column_index(pColumnName);
      if (columnIdx < 0)
      {
        throw std::out_of_range("column not found: " + pColumnName);
      }
      return column<T>(columnIdx);
    }

    /**
     * @brief   Get column by name.
     * @param   pColumnName           column label name.
     * @param   pToVal                conversion function.
     * @returns vector of column data.
     */
    template<typename T>
    std::vector<T> column(const std::string& pColumnName, conversion_func<T> pToVal) const
    {
      const ssize_t columnIdx = column_index(pColumnName);
      if (columnIdx < 0)
      {
        throw std::out_of_range("column not found: " + pColumnName);
      }
      return column<T>(columnIdx, pToVal);
    }

    /**
     * @brief   Set column by index.
     * @param   pColumnIdx            zero-based column index.
     * @param   pColumn               vector of column data.
     */
    template<typename T>
    void assign_column(const size_t pColumnIdx, const std::vector<T>& pColumn)
    {
      const size_t columnIdx = pColumnIdx + (mLabelParams.mRowNameIdx + 1);

      while (pColumn.size() + (mLabelParams.mColumnNameIdx + 1) > data_row_count())
      {
        std::vector<std::string> row;
        row.resize(data_column_count());
        mData.push_back(row);
      }

      if ((columnIdx + 1) > data_column_count())
      {
        for (auto itRow = mData.begin(); itRow != mData.end(); ++itRow)
        {
          itRow->resize(columnIdx + 1 + (mLabelParams.mRowNameIdx + 1));
        }
      }

      converter<T> converter(mConverterParams);
      for (auto itRow = pColumn.begin(); itRow != pColumn.end(); ++itRow)
      {
        std::string str;
          converter.to_string(*itRow, str);
        mData.at(std::distance(pColumn.begin(), itRow) + (mLabelParams.mColumnNameIdx + 1)).at(columnIdx) = str;
      }
    }

    /**
     * @brief   Set column by name.
     * @param   pColumnName           column label name.
     * @param   pColumn               vector of column data.
     */
    template<typename T>
    void assign_column(const std::string& pColumnName, const std::vector<T>& pColumn)
    {
      const ssize_t columnIdx = column_index(pColumnName);
      if (columnIdx < 0)
      {
        throw std::out_of_range("column not found: " + pColumnName);
      }
        assign_column<T>(columnIdx, pColumn);
    }

    /**
     * @brief   Remove column by index.
     * @param   pColumnIdx            zero-based column index.
     */
    void erase_column(const size_t pColumnIdx)
    {
      const ssize_t columnIdx = pColumnIdx + (mLabelParams.mRowNameIdx + 1);
      for (auto itRow = mData.begin(); itRow != mData.end(); ++itRow)
      {
        itRow->erase(itRow->begin() + columnIdx);
      }
    }

    /**
     * @brief   Remove column by name.
     * @param   pColumnName           column label name.
     */
    void erase_column(const std::string& pColumnName)
    {
      ssize_t columnIdx = column_index(pColumnName);
      if (columnIdx < 0)
      {
        throw std::out_of_range("column not found: " + pColumnName);
      }

        erase_column(columnIdx);
    }

    /**
     * @brief   Insert column at specified index.
     * @param   pColumnIdx            zero-based column index.
     * @param   pColumn               vector of column data (optional argument).
     * @param   pColumnName           column label name (optional argument).
     */
    template<typename T>
    void insert_column(const size_t pColumnIdx, const std::vector<T>& pColumn = std::vector<T>(),
                       const std::string& pColumnName = std::string())
    {
      const size_t columnIdx = pColumnIdx + (mLabelParams.mRowNameIdx + 1);

      std::vector<std::string> column;
      if (pColumn.empty())
      {
        column.resize(data_row_count());
      }
      else
      {
        column.resize(pColumn.size() + (mLabelParams.mColumnNameIdx + 1));
        converter<T> converter(mConverterParams);
        for (auto itRow = pColumn.begin(); itRow != pColumn.end(); ++itRow)
        {
          std::string str;
            converter.to_string(*itRow, str);
          const size_t rowIdx = std::distance(pColumn.begin(), itRow) + (mLabelParams.mColumnNameIdx + 1);
          column.at(rowIdx) = str;
        }
      }

      while (column.size() > data_row_count())
      {
        std::vector<std::string> row;
        const size_t columnCount = std::max(static_cast<size_t>(mLabelParams.mColumnNameIdx + 1),
                                            data_column_count());
        row.resize(columnCount);
        mData.push_back(row);
      }

      for (auto itRow = mData.begin(); itRow != mData.end(); ++itRow)
      {
        const size_t rowIdx = std::distance(mData.begin(), itRow);
        itRow->insert(itRow->begin() + columnIdx, column.at(rowIdx));
      }

      if (!pColumnName.empty())
      {
          set_column_name(pColumnIdx, pColumnName);
      }
    }

    /**
     * @brief   Get number of data columns (excluding label columns).
     * @returns column count.
     */
    size_t column_count() const
    {
      const ssize_t count = static_cast<ssize_t>((mData.size() > 0) ? mData.at(0).size() : 0) -
        (mLabelParams.mRowNameIdx + 1);
      return (count >= 0) ? count : 0;
    }

    /**
     * @brief   Get row index by name.
     * @param   pRowName              row label name.
     * @returns zero-based row index.
     */
    ssize_t row_index(const std::string& pRowName) const
    {
      if (mLabelParams.mRowNameIdx >= 0)
      {
        if (mRowNames.find(pRowName) != mRowNames.end())
        {
          return mRowNames.at(pRowName) - (mLabelParams.mColumnNameIdx + 1);
        }
      }
      return -1;
    }

    /**
     * @brief   Get row by index.
     * @param   pRowIdx               zero-based row index.
     * @returns vector of row data.
     */
    template<typename T>
    std::vector<T> row(const size_t pRowIdx) const
    {
      const ssize_t rowIdx = pRowIdx + (mLabelParams.mColumnNameIdx + 1);
      std::vector<T> row;
      converter<T> converter(mConverterParams);
      for (auto itCol = mData.at(rowIdx).begin(); itCol != mData.at(rowIdx).end(); ++itCol)
      {
        if (std::distance(mData.at(rowIdx).begin(), itCol) > mLabelParams.mRowNameIdx)
        {
          T val;
            converter.to_value(*itCol, val);
          row.push_back(val);
        }
      }
      return row;
    }

    /**
     * @brief   Get row by index.
     * @param   pRowIdx               zero-based row index.
     * @param   pToVal                conversion function.
     * @returns vector of row data.
     */
    template<typename T>
    std::vector<T> row(const size_t pRowIdx, conversion_func<T> pToVal) const
    {
      const ssize_t rowIdx = pRowIdx + (mLabelParams.mColumnNameIdx + 1);
      std::vector<T> row;
      converter<T> converter(mConverterParams);
      for (auto itCol = mData.at(rowIdx).begin(); itCol != mData.at(rowIdx).end(); ++itCol)
      {
        if (std::distance(mData.at(rowIdx).begin(), itCol) > mLabelParams.mRowNameIdx)
        {
          T val;
          pToVal(*itCol, val);
          row.push_back(val);
        }
      }
      return row;
    }

    /**
     * @brief   Get row by name.
     * @param   pRowName              row label name.
     * @returns vector of row data.
     */
    template<typename T>
    std::vector<T> row(const std::string& pRowName) const
    {
      ssize_t rowIdx = row_index(pRowName);
      if (rowIdx < 0)
      {
        throw std::out_of_range("row not found: " + pRowName);
      }
      return row<T>(rowIdx);
    }

    /**
     * @brief   Get row by name.
     * @param   pRowName              row label name.
     * @param   pToVal                conversion function.
     * @returns vector of row data.
     */
    template<typename T>
    std::vector<T> row(const std::string& pRowName, conversion_func<T> pToVal) const
    {
      ssize_t rowIdx = row_index(pRowName);
      if (rowIdx < 0)
      {
        throw std::out_of_range("row not found: " + pRowName);
      }
      return row<T>(rowIdx, pToVal);
    }

    /**
     * @brief   Set row by index.
     * @param   pRowIdx               zero-based row index.
     * @param   pRow                  vector of row data.
     */
    template<typename T>
    void assign_row(const size_t pRowIdx, const std::vector<T>& pRow)
    {
      const size_t rowIdx = pRowIdx + (mLabelParams.mColumnNameIdx + 1);

      while ((rowIdx + 1) > data_row_count())
      {
        std::vector<std::string> row;
        row.resize(data_column_count());
        mData.push_back(row);
      }

      if (pRow.size() > data_column_count())
      {
        for (auto itRow = mData.begin(); itRow != mData.end(); ++itRow)
        {
          itRow->resize(pRow.size() + (mLabelParams.mRowNameIdx + 1));
        }
      }

      converter<T> converter(mConverterParams);
      for (auto itCol = pRow.begin(); itCol != pRow.end(); ++itCol)
      {
        std::string str;
          converter.to_string(*itCol, str);
        mData.at(rowIdx).at(std::distance(pRow.begin(), itCol) + (mLabelParams.mRowNameIdx + 1)) = str;
      }
    }

    /**
     * @brief   Set row by name.
     * @param   pRowName              row label name.
     * @param   pRow                  vector of row data.
     */
    template<typename T>
    void assign_row(const std::string& pRowName, const std::vector<T>& pRow)
    {
      ssize_t rowIdx = row_index(pRowName);
      if (rowIdx < 0)
      {
        throw std::out_of_range("row not found: " + pRowName);
      }
      return assign_row<T>(rowIdx, pRow);
    }

    /**
     * @brief   Remove row by index.
     * @param   pRowIdx               zero-based row index.
     */
    void erase_row(const size_t pRowIdx)
    {
      const ssize_t rowIdx = pRowIdx + (mLabelParams.mColumnNameIdx + 1);
      mData.erase(mData.begin() + rowIdx);
    }

    /**
     * @brief   Remove row by name.
     * @param   pRowName              row label name.
     */
    void erase_row(const std::string& pRowName)
    {
      ssize_t rowIdx = row_index(pRowName);
      if (rowIdx < 0)
      {
        throw std::out_of_range("row not found: " + pRowName);
      }

        erase_row(rowIdx);
    }

    /**
     * @brief   Insert row at specified index.
     * @param   pRowIdx               zero-based row index.
     * @param   pRow                  vector of row data (optional argument).
     * @param   pRowName              row label name (optional argument).
     */
    template<typename T>
    void insert_row(const size_t pRowIdx, const std::vector<T>& pRow = std::vector<T>(),
                    const std::string& pRowName = std::string())
    {
      const size_t rowIdx = pRowIdx + (mLabelParams.mColumnNameIdx + 1);

      std::vector<std::string> row;
      if (pRow.empty())
      {
        row.resize(data_column_count());
      }
      else
      {
        row.resize(pRow.size() + (mLabelParams.mRowNameIdx + 1));
        converter<T> converter(mConverterParams);
        for (auto itCol = pRow.begin(); itCol != pRow.end(); ++itCol)
        {
          std::string str;
            converter.to_string(*itCol, str);
          row.at(std::distance(pRow.begin(), itCol) + (mLabelParams.mRowNameIdx + 1)) = str;
        }
      }

      while (rowIdx > data_row_count())
      {
        std::vector<std::string> tempRow;
        tempRow.resize(data_column_count());
        mData.push_back(tempRow);
      }

      mData.insert(mData.begin() + rowIdx, row);

      if (!pRowName.empty())
      {
          row_name(pRowIdx, pRowName);
      }
    }

    /**
     * @brief   Get number of data rows (excluding label rows).
     * @returns row count.
     */
    size_t row_count() const
    {
      const ssize_t count = static_cast<ssize_t>(mData.size()) - (mLabelParams.mColumnNameIdx + 1);
      return (count >= 0) ? count : 0;
    }

    /**
     * @brief   Get cell by index.
     * @param   pColumnIdx            zero-based column index.
     * @param   pRowIdx               zero-based row index.
     * @returns cell data.
     */
    template<typename T>
    T cell(const size_t pColumnIdx, const size_t pRowIdx) const
    {
      const ssize_t columnIdx = pColumnIdx + (mLabelParams.mRowNameIdx + 1);
      const ssize_t rowIdx = pRowIdx + (mLabelParams.mColumnNameIdx + 1);

      T val;
      converter<T> converter(mConverterParams);
        converter.to_value(mData.at(rowIdx).at(columnIdx), val);
      return val;
    }

    /**
     * @brief   Get cell by index.
     * @param   pColumnIdx            zero-based column index.
     * @param   pRowIdx               zero-based row index.
     * @param   pToVal                conversion function.
     * @returns cell data.
     */
    template<typename T>
    T cell(const size_t pColumnIdx, const size_t pRowIdx, conversion_func<T> pToVal) const
    {
      const ssize_t columnIdx = pColumnIdx + (mLabelParams.mRowNameIdx + 1);
      const ssize_t rowIdx = pRowIdx + (mLabelParams.mColumnNameIdx + 1);

      T val;
      pToVal(mData.at(rowIdx).at(columnIdx), val);
      return val;
    }

    /**
     * @brief   Get cell by name.
     * @param   pColumnName           column label name.
     * @param   pRowName              row label name.
     * @returns cell data.
     */
    template<typename T>
    T cell(const std::string& pColumnName, const std::string& pRowName) const
    {
      const ssize_t columnIdx = column_index(pColumnName);
      if (columnIdx < 0)
      {
        throw std::out_of_range("column not found: " + pColumnName);
      }

      const ssize_t rowIdx = row_index(pRowName);
      if (rowIdx < 0)
      {
        throw std::out_of_range("row not found: " + pRowName);
      }

      return cell<T>(columnIdx, rowIdx);
    }

    /**
     * @brief   Get cell by name.
     * @param   pColumnName           column label name.
     * @param   pRowName              row label name.
     * @param   pToVal                conversion function.
     * @returns cell data.
     */
    template<typename T>
    T cell(const std::string& pColumnName, const std::string& pRowName, conversion_func<T> pToVal) const
    {
      const ssize_t columnIdx = column_index(pColumnName);
      if (columnIdx < 0)
      {
        throw std::out_of_range("column not found: " + pColumnName);
      }

      const ssize_t rowIdx = row_index(pRowName);
      if (rowIdx < 0)
      {
        throw std::out_of_range("row not found: " + pRowName);
      }

      return cell<T>(columnIdx, rowIdx, pToVal);
    }

    /**
     * @brief   Get cell by column name and row index.
     * @param   pColumnName           column label name.
     * @param   pRowIdx               zero-based row index.
     * @returns cell data.
     */
    template<typename T>
    T cell(const std::string& pColumnName, const size_t pRowIdx) const
    {
      const ssize_t columnIdx = column_index(pColumnName);
      if (columnIdx < 0)
      {
        throw std::out_of_range("column not found: " + pColumnName);
      }

      return cell<T>(columnIdx, pRowIdx);
    }

    /**
     * @brief   Get cell by column name and row index.
     * @param   pColumnName           column label name.
     * @param   pRowIdx               zero-based row index.
     * @param   pToVal                conversion function.
     * @returns cell data.
     */
    template<typename T>
    T cell(const std::string& pColumnName, const size_t pRowIdx, conversion_func<T> pToVal) const
    {
      const ssize_t columnIdx = column_index(pColumnName);
      if (columnIdx < 0)
      {
        throw std::out_of_range("column not found: " + pColumnName);
      }

      return cell<T>(columnIdx, pRowIdx, pToVal);
    }

    /**
     * @brief   Get cell by column index and row name.
     * @param   pColumnIdx            zero-based column index.
     * @param   pRowName              row label name.
     * @returns cell data.
     */
    template<typename T>
    T cell(const size_t pColumnIdx, const std::string& pRowName) const
    {
      const ssize_t rowIdx = row_index(pRowName);
      if (rowIdx < 0)
      {
        throw std::out_of_range("row not found: " + pRowName);
      }

      return cell<T>(pColumnIdx, rowIdx);
    }

    /**
     * @brief   Get cell by column index and row name.
     * @param   pColumnIdx            zero-based column index.
     * @param   pRowName              row label name.
     * @param   pToVal                conversion function.
     * @returns cell data.
     */
    template<typename T>
    T cell(const size_t pColumnIdx, const std::string& pRowName, conversion_func<T> pToVal) const
    {
      const ssize_t rowIdx = row_index(pRowName);
      if (rowIdx < 0)
      {
        throw std::out_of_range("row not found: " + pRowName);
      }

      return cell<T>(pColumnIdx, rowIdx, pToVal);
    }

    /**
     * @brief   Set cell by index.
     * @param   pRowIdx               zero-based row index.
     * @param   pColumnIdx            zero-based column index.
     * @param   pCell                 cell data.
     */
    template<typename T>
    void set_cell(const size_t pColumnIdx, const size_t pRowIdx, const T& pCell)
    {
      const size_t columnIdx = pColumnIdx + (mLabelParams.mRowNameIdx + 1);
      const size_t rowIdx = pRowIdx + (mLabelParams.mColumnNameIdx + 1);

      while ((rowIdx + 1) > data_row_count())
      {
        std::vector<std::string> row;
        row.resize(data_column_count());
        mData.push_back(row);
      }

      if ((columnIdx + 1) > data_column_count())
      {
        for (auto itRow = mData.begin(); itRow != mData.end(); ++itRow)
        {
          itRow->resize(columnIdx + 1);
        }
      }

      std::string str;
      converter<T> converter(mConverterParams);
        converter.to_string(pCell, str);
      mData.at(rowIdx).at(columnIdx) = str;
    }

    /**
     * @brief   Set cell by name.
     * @param   pColumnName           column label name.
     * @param   pRowName              row label name.
     * @param   pCell                 cell data.
     */
    template<typename T>
    void set_cell(const std::string& pColumnName, const std::string& pRowName, const T& pCell)
    {
      const ssize_t columnIdx = column_index(pColumnName);
      if (columnIdx < 0)
      {
        throw std::out_of_range("column not found: " + pColumnName);
      }

      const ssize_t rowIdx = row_index(pRowName);
      if (rowIdx < 0)
      {
        throw std::out_of_range("row not found: " + pRowName);
      }

        set_cell<T>(columnIdx, rowIdx, pCell);
    }

    /**
     * @brief   Get column name
     * @param   pColumnIdx            zero-based column index.
     * @returns column name.
     */
    std::string column_name(const ssize_t pColumnIdx)
    {
      const ssize_t columnIdx = pColumnIdx + (mLabelParams.mRowNameIdx + 1);
      if (mLabelParams.mColumnNameIdx < 0)
      {
        throw std::out_of_range("column name row index < 0: " + std::to_string(mLabelParams.mColumnNameIdx));
      }

      return mData.at(mLabelParams.mColumnNameIdx).at(columnIdx);
    }

    /**
     * @brief   Set column name
     * @param   pColumnIdx            zero-based column index.
     * @param   pColumnName           column name.
     */
    void set_column_name(size_t pColumnIdx, const std::string& pColumnName)
    {
      const ssize_t columnIdx = pColumnIdx + (mLabelParams.mRowNameIdx + 1);
      mColumnNames[pColumnName] = columnIdx;
      if (mLabelParams.mColumnNameIdx < 0)
      {
        throw std::out_of_range("column name row index < 0: " + std::to_string(mLabelParams.mColumnNameIdx));
      }

      // increase table size if necessary:
      const int rowIdx = mLabelParams.mColumnNameIdx;
      if (rowIdx >= static_cast<int>(mData.size()))
      {
        mData.resize(rowIdx + 1);
      }
      auto& row = mData[rowIdx];
      if (columnIdx >= static_cast<int>(row.size()))
      {
        row.resize(columnIdx + 1);
      }

      mData.at(mLabelParams.mColumnNameIdx).at(columnIdx) = pColumnName;
    }

    /**
     * @brief   Get column names
     * @returns vector of column names.
     */
    std::vector<std::string> column_names()
    {
      if (mLabelParams.mColumnNameIdx >= 0)
      {
        return std::vector<std::string>(mData.at(mLabelParams.mColumnNameIdx).begin() +
                                        (mLabelParams.mRowNameIdx + 1),
                                        mData.at(mLabelParams.mColumnNameIdx).end());
      }

      return std::vector<std::string>();
    }

    /**
     * @brief   Get row name
     * @param   pRowIdx               zero-based column index.
     * @returns row name.
     */
    std::string row_name(const ssize_t pRowIdx)
    {
      const ssize_t rowIdx = pRowIdx + (mLabelParams.mColumnNameIdx + 1);
      if (mLabelParams.mRowNameIdx < 0)
      {
        throw std::out_of_range("row name column index < 0: " + std::to_string(mLabelParams.mRowNameIdx));
      }

      return mData.at(rowIdx).at(mLabelParams.mRowNameIdx);
    }

    /**
     * @brief   Set row name
     * @param   pRowIdx               zero-based row index.
     * @param   pRowName              row name.
     */
    void row_name(size_t pRowIdx, const std::string& pRowName)
    {
      const ssize_t rowIdx = pRowIdx + (mLabelParams.mColumnNameIdx + 1);
      mRowNames[pRowName] = rowIdx;
      if (mLabelParams.mRowNameIdx < 0)
      {
        throw std::out_of_range("row name column index < 0: " + std::to_string(mLabelParams.mRowNameIdx));
      }

      // increase table size if necessary:
      if (rowIdx >= static_cast<int>(mData.size()))
      {
        mData.resize(rowIdx + 1);
      }
      auto& row = mData[rowIdx];
      if (mLabelParams.mRowNameIdx >= static_cast<int>(row.size()))
      {
        row.resize(mLabelParams.mRowNameIdx + 1);
      }

      mData.at(rowIdx).at(mLabelParams.mRowNameIdx) = pRowName;
    }

    /**
     * @brief   Get row names
     * @returns vector of row names.
     */
    std::vector<std::string> row_names()
    {
      std::vector<std::string> rownames;
      if (mLabelParams.mRowNameIdx >= 0)
      {
        for (auto itRow = mData.begin(); itRow != mData.end(); ++itRow)
        {
          if (std::distance(mData.begin(), itRow) > mLabelParams.mColumnNameIdx)
          {
            rownames.push_back(itRow->at(mLabelParams.mRowNameIdx));
          }
        }
      }
      return rownames;
    }

  private:
    void read_csv()
    {
      std::ifstream stream;
      stream.exceptions(std::ifstream::failbit | std::ifstream::badbit);
      stream.open(mPath, std::ios::binary);
        read_csv(stream);
    }

    void read_csv(std::istream& pStream)
    {
        clear();
      pStream.seekg(0, std::ios::end);
      std::streamsize length = pStream.tellg();
      pStream.seekg(0, std::ios::beg);

      std::vector<char> bom2b(2, '\0');
      if (length >= 2)
      {
        pStream.read(bom2b.data(), 2);
        pStream.seekg(0, std::ios::beg);
      }

      static const std::vector<char> bomU16le = { '\xff', '\xfe' };
      static const std::vector<char> bomU16be = { '\xfe', '\xff' };
      if ((bom2b == bomU16le) || (bom2b == bomU16be))
      {
        mIsUtf16 = true;
        mIsLE = (bom2b == bomU16le);

        std::wifstream wstream;
        wstream.exceptions(std::wifstream::failbit | std::wifstream::badbit);
        wstream.open(mPath, std::ios::binary);
        if (mIsLE)
        {
          wstream.imbue(std::locale(wstream.getloc(),
                                    new std::codecvt_utf16<wchar_t, 0x10ffff,
                                                           static_cast<std::codecvt_mode>(std::consume_header |
                                                                                          std::little_endian)>));
        }
        else
        {
          wstream.imbue(std::locale(wstream.getloc(),
                                    new std::codecvt_utf16<wchar_t, 0x10ffff,
                                                           std::consume_header>));
        }
        std::wstringstream wss;
        wss << wstream.rdbuf();
        std::string utf8 = to_string(wss.str());
        std::stringstream ss(utf8);
        parse_csv(ss, utf8.size());
      }
      else
      {
        // check for UTF-8 Byte order mark and skip it when found
        if (length >= 3)
        {
          std::vector<char> bom3b(3, '\0');
          pStream.read(bom3b.data(), 3);
          static const std::vector<char> bomU8 = { '\xef', '\xbb', '\xbf' };
          if (bom3b != bomU8)
          {
            // file does not start with a UTF-8 Byte order mark
            pStream.seekg(0, std::ios::beg);
          }
          else
          {
            // file did start with a UTF-8 Byte order mark, simply skip it
            length -= 3;
          }
        }

        parse_csv(pStream, length);
      }
    }

    void parse_csv(std::istream& pStream, std::streamsize p_FileLength)
    {
      const std::streamsize bufLength = 64 * 1024;
      std::vector<char> buffer(bufLength);
      std::vector<std::string> row;
      std::string cell;
      bool quoted = false;
      int cr = 0;
      int lf = 0;

      while (p_FileLength > 0)
      {
        std::streamsize readLength = std::min<std::streamsize>(p_FileLength, bufLength);
        pStream.read(buffer.data(), readLength);
        for (int i = 0; i < readLength; ++i)
        {
          if (buffer[i] == '"')
          {
            if (cell.empty() || cell[0] == '"')
            {
              quoted = !quoted;
            }
            cell += buffer[i];
          }
          else if (buffer[i] == mSeparatorParams.mSeparator)
          {
            if (!quoted)
            {
              row.push_back(unquote(trim(cell)));
              cell.clear();
            }
            else
            {
              cell += buffer[i];
            }
          }
          else if (buffer[i] == '\r')
          {
            if (mSeparatorParams.mQuotedLinebreaks && quoted)
            {
              cell += buffer[i];
            }
            else
            {
              ++cr;
            }
          }
          else if (buffer[i] == '\n')
          {
            if (mSeparatorParams.mQuotedLinebreaks && quoted)
            {
              cell += buffer[i];
            }
            else
            {
              ++lf;
              if (mLineReaderParams.mSkipEmptyLines && row.empty() && cell.empty())
              {
                // skip empty line
              }
              else
              {
                row.push_back(unquote(trim(cell)));

                if (mLineReaderParams.mSkipCommentLines && !row.at(0).empty() &&
                    (row.at(0)[0] == mLineReaderParams.mCommentPrefix))
                {
                  // skip comment line
                }
                else
                {
                  mData.push_back(row);
                }

                cell.clear();
                row.clear();
                quoted = false;
              }
            }
          }
          else
          {
            cell += buffer[i];
          }
        }
        p_FileLength -= readLength;
      }

      // Handle last line without linebreak
      if (!cell.empty() || !row.empty())
      {
        row.push_back(unquote(trim(cell)));
        cell.clear();
        mData.push_back(row);
        row.clear();
      }

      // Assume CR/LF if at least half the linebreaks have CR
      mSeparatorParams.mHasCR = (cr > (lf / 2));

      // Set up column labels
      if ((mLabelParams.mColumnNameIdx >= 0) &&
          (static_cast<ssize_t>(mData.size()) > mLabelParams.mColumnNameIdx))
      {
        int i = 0;
        for (auto& columnName : mData[mLabelParams.mColumnNameIdx])
        {
          mColumnNames[columnName] = i++;
        }
      }

      // Set up row labels
      if ((mLabelParams.mRowNameIdx >= 0) &&
          (static_cast<ssize_t>(mData.size()) >
           (mLabelParams.mColumnNameIdx + 1)))
      {
        int i = 0;
        for (auto& dataRow : mData)
        {
          if (static_cast<ssize_t>(dataRow.size()) > mLabelParams.mRowNameIdx)
          {
            mRowNames[dataRow[mLabelParams.mRowNameIdx]] = i++;
          }
        }
      }
    }

    void write_csv() const
    {
#ifdef HAS_CODECVT
      if (mIsUtf16)
      {
        std::stringstream ss;
        WriteCsv(ss);
        std::string utf8 = ss.str();
        std::wstring wstr = ToWString(utf8);

        std::wofstream wstream;
        wstream.exceptions(std::wofstream::failbit | std::wofstream::badbit);
        wstream.open(mPath, std::ios::binary | std::ios::trunc);

        if (mIsLE)
        {
          wstream.imbue(std::locale(wstream.getloc(),
                                    new std::codecvt_utf16<wchar_t, 0x10ffff,
                                                           static_cast<std::codecvt_mode>(std::little_endian)>));
        }
        else
        {
          wstream.imbue(std::locale(wstream.getloc(),
                                    new std::codecvt_utf16<wchar_t, 0x10ffff>));
        }

        wstream << static_cast<wchar_t>(0xfeff);
        wstream << wstr;
      }
      else
#endif
      {
        std::ofstream stream;
        stream.exceptions(std::ofstream::failbit | std::ofstream::badbit);
        stream.open(mPath, std::ios::binary | std::ios::trunc);
        write_csv(stream);
      }
    }

    void write_csv(std::ostream& pStream) const
    {
      for (auto itr = mData.begin(); itr != mData.end(); ++itr)
      {
        for (auto itc = itr->begin(); itc != itr->end(); ++itc)
        {
          if (mSeparatorParams.mAutoQuote &&
              ((itc->find(mSeparatorParams.mSeparator) != std::string::npos) ||
               (itc->find(' ') != std::string::npos)))
          {
            // escape quotes in string
            std::string str = *itc;
            replace_string(str, "\"", "\"\"");

            pStream << "\"" << str << "\"";
          }
          else
          {
            pStream << *itc;
          }

          if (std::distance(itc, itr->end()) > 1)
          {
            pStream << mSeparatorParams.mSeparator;
          }
        }
        pStream << (mSeparatorParams.mHasCR ? "\r\n" : "\n");
      }
    }

    size_t data_row_count() const
    {
      return mData.size();
    }

    size_t data_column_count() const
    {
      return (mData.size() > 0) ? mData.at(0).size() : 0;
    }

    std::string trim(const std::string& pStr)
    {
      if (mSeparatorParams.mTrim)
      {
        std::string str = pStr;

        // ltrim
        str.erase(str.begin(), std::find_if(str.begin(), str.end(), [](int ch) { return !isspace(ch); }));

        // rtrim
        str.erase(std::find_if(str.rbegin(), str.rend(), [](int ch) { return !isspace(ch); }).base(), str.end());

        return str;
      }
      else
      {
        return pStr;
      }
    }

    std::string unquote(const std::string& pStr)
    {
      if (mSeparatorParams.mAutoQuote && (pStr.size() >= 2) && (pStr.front() == '"') && (pStr.back() == '"'))
      {
        // remove start/end quotes
        std::string str = pStr.substr(1, pStr.size() - 2);

        // unescape quotes in string
        replace_string(str, "\"\"", "\"");

        return str;
      }
      else
      {
        return pStr;
      }
    }

#if defined(_MSC_VER)
#pragma warning (disable: 4996)
#endif
    static std::string to_string(const std::wstring& pWStr)
    {
      return std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t>{ }.to_bytes(pWStr);
    }

    static std::wstring to_wstring(const std::string& pStr)
    {
      return std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t>{ }.from_bytes(pStr);
    }
#if defined(_MSC_VER)
#pragma warning (default: 4996)
#endif

    static void replace_string(std::string& pStr, const std::string& pSearch, const std::string& pReplace)
    {
      size_t pos = 0;

      while ((pos = pStr.find(pSearch, pos)) != std::string::npos)
      {
        pStr.replace(pos, pSearch.size(), pReplace);
        pos += pReplace.size();
      }
    }

  private:
    std::string mPath;
    label_parameters mLabelParams;
    separator_parameters mSeparatorParams;
    converter_parameters mConverterParams;
    line_reader_parameters mLineReaderParams;
    std::vector<std::vector<std::string>> mData;
    std::map<std::string, size_t> mColumnNames;
    std::map<std::string, size_t> mRowNames;
    bool mIsUtf16 = false;
    bool mIsLE = false;
  };
}

#endif
