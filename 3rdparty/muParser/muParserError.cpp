/*
  Copyright (C) 2004, 2005 Ingo Berg

  Permission is hereby granted, free of charge, to any person obtaining a copy of this 
  software and associated documentation files (the "Software"), to deal in the Software
  without restriction, including without limitation the rights to use, copy, modify, 
  merge, publish, distribute, sublicense, and/or sell copies of the Software, and to 
  permit persons to whom the Software is furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in all copies or 
  substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT
  NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND 
  NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, 
  DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, 
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE. 
*/
#include "muParserError.h"


namespace mu
{
  const ParserErrorMsg ParserErrorMsg::m_Instance;

  //------------------------------------------------------------------------------
  const ParserErrorMsg& ParserErrorMsg::Instance()
  {
    return m_Instance;
  }

  //------------------------------------------------------------------------------
  string_type ParserErrorMsg::operator[](unsigned a_iIdx) const
  {
    return (a_iIdx<m_vErrMsg.size()) ? m_vErrMsg[a_iIdx] : string_type();
  }


  //---------------------------------------------------------------------------
  ParserErrorMsg::~ParserErrorMsg()
  {}

  //---------------------------------------------------------------------------
  /** \brief Assignement operator is deactivated.
  */
  ParserErrorMsg& ParserErrorMsg::operator=(const ParserErrorMsg& )
  {
    assert(false);
    return *this;
  }

  //---------------------------------------------------------------------------
  ParserErrorMsg::ParserErrorMsg(const ParserErrorMsg&)
  {}

  //---------------------------------------------------------------------------
  ParserErrorMsg::ParserErrorMsg()
    :m_vErrMsg(0)
  {
    m_vErrMsg.resize(ecCOUNT);

    m_vErrMsg[ecUNASSIGNABLE_TOKEN]  = "Undefined token \"$TOK$\" found at position $POS$.";
    m_vErrMsg[ecINTERNAL_ERROR]      = "Internal error";
    m_vErrMsg[ecINVALID_NAME]        = "Invalid function-, variable- or constant name.";
    m_vErrMsg[ecINVALID_FUN_PTR]     = "Invalid pointer to callback function.";
    m_vErrMsg[ecINVALID_VAR_PTR]     = "Invalid pointer to variable.";
    m_vErrMsg[ecUNEXPECTED_OPERATOR] = "Unexpected operator \"$TOK$\" found at position $POS$";
    m_vErrMsg[ecUNEXPECTED_EOF]      = "Unexpected end of formula at position $POS$";
    m_vErrMsg[ecUNEXPECTED_COMMA]    = "Unexpected comma at position $POS$";
    m_vErrMsg[ecUNEXPECTED_PARENS]   = "Unexpected parenthesis \"$TOK$\" at position $POS$";
    m_vErrMsg[ecUNEXPECTED_FUN]      = "Unexpected function \"$TOK$\" at position $POS$";
    m_vErrMsg[ecUNEXPECTED_VAL]      = "Unexpected value \"$TOK$\" found at position $POS$";
    m_vErrMsg[ecUNEXPECTED_VAR]      = "Unexpected variable \"$TOK$\" found at position $POS$";
    m_vErrMsg[ecUNEXPECTED_ARG]      = "Function arguments used without a function (position: $POS$)";
    m_vErrMsg[ecMISSING_PARENS]      = "Missing parenthesis";
    m_vErrMsg[ecTOO_MANY_PARAMS]     = "Too many parameters for function \"$TOK$\" at formula position $POS$";
    m_vErrMsg[ecTOO_FEW_PARAMS]      = "Too few parameters for function \"$TOK$\" at formula position $POS$";
    m_vErrMsg[ecDIV_BY_ZERO]         = "Divide by zero";
    m_vErrMsg[ecDOMAIN_ERROR]        = "Domain error";
    m_vErrMsg[ecNAME_CONFLICT]       = "Name conflict";
    m_vErrMsg[ecOPT_PRI]             = "Invalid value for operator priority (must be greater or equal to zero).";
    m_vErrMsg[ecBUILTIN_OVERLOAD]    = "Binary operator identifier conflicts with a built in operator.";
    m_vErrMsg[ecUNEXPECTED_STR]      = "Unexpected string token found at position $POS$.";
    m_vErrMsg[ecUNTERMINATED_STRING] = "Unterminated string starting at position $POS$.";
    m_vErrMsg[ecSTRING_EXPECTED]     = "String function called with a non string type of argument.";
    m_vErrMsg[ecVAL_EXPECTED]        = "Numerical function called with a non value type of argument.";
    m_vErrMsg[ecOPRT_TYPE_CONFLICT]  = "No suitable overload for operator \"$TOK$\" at position $POS$.";
    m_vErrMsg[ecGENERIC]             = "Parser error.";
    m_vErrMsg[ecSTR_RESULT]          = "Function result is a string.";

    #if defined(_DEBUG)
      for (int i=0; i<ecCOUNT; ++i)
        if (!m_vErrMsg[i].length())
          assert(false);
    #endif
  }

  //---------------------------------------------------------------------------
  //
  //
  //
  //  ParserError class
  //
  //
  //
  //---------------------------------------------------------------------------

  //------------------------------------------------------------------------------
  ParserError::ParserError()
    :m_strMsg()
    ,m_strFormula()
    ,m_strTok()
    ,m_iPos(-1)
    ,m_iErrc(ecUNDEFINED)
    ,m_ErrMsg(ParserErrorMsg::Instance())
  {
  }

  //------------------------------------------------------------------------------
  /** \brief This Constructor is used for internal exceptions only. 
      
    It does not contain any information but the error code.
  */
  ParserError::ParserError(EErrorCodes a_iErrc) 
    :m_ErrMsg(ParserErrorMsg::Instance())
  {
    assert(a_iErrc==ecINTERNAL_ERROR);
    Reset();
    m_strMsg = "internal error";
  }

  //------------------------------------------------------------------------------
  ParserError::ParserError(const string_type &sMsg) 
    :m_ErrMsg(ParserErrorMsg::Instance())
  {
    Reset();
    m_strMsg = sMsg;
  }

  //------------------------------------------------------------------------------
  ParserError::ParserError( EErrorCodes a_iErrc,
                            const string_type &sTok,
                            const string_type &sFormula,
                            int a_iPos )
    :m_strMsg()
    ,m_strFormula(sFormula)
    ,m_strTok(sTok)
    ,m_iPos(a_iPos)
    ,m_iErrc(a_iErrc)
    ,m_ErrMsg(ParserErrorMsg::Instance())
  {
    m_strMsg = m_ErrMsg[m_iErrc];
    stringstream_type stream;
    stream << (int)m_iPos;
    ReplaceSubString(m_strMsg, "$POS$", stream.str());
    ReplaceSubString(m_strMsg, "$TOK$", m_strTok);
  }

  //------------------------------------------------------------------------------
  ParserError::ParserError( EErrorCodes a_iErrc, int a_iPos, const string_type &sTok) 
    :m_strMsg()
    ,m_strFormula()
    ,m_strTok(sTok)
    ,m_iPos(a_iPos)
    ,m_iErrc(a_iErrc)
    ,m_ErrMsg(ParserErrorMsg::Instance())
  {
    m_strMsg = m_ErrMsg[m_iErrc];
    stringstream_type stream;
    stream << (int)m_iPos;
    ReplaceSubString(m_strMsg, "$POS$", stream.str());
    ReplaceSubString(m_strMsg, "$TOK$", m_strTok);
  }

  //------------------------------------------------------------------------------
  ParserError::ParserError( const char_type *a_szMsg, int a_iPos, const string_type &sTok) 
    :m_strMsg(a_szMsg)
    ,m_strFormula()
    ,m_strTok(sTok)
    ,m_iPos(a_iPos)
    ,m_iErrc(ecGENERIC)
    ,m_ErrMsg(ParserErrorMsg::Instance())
  {
    stringstream_type stream;
    stream << (int)m_iPos;
    ReplaceSubString(m_strMsg, "$POS$", stream.str());
    ReplaceSubString(m_strMsg, "$TOK$", m_strTok);
  }

  //------------------------------------------------------------------------------
  ParserError::ParserError(const ParserError &a_Obj)
    :m_strMsg(a_Obj.m_strMsg)
    ,m_strFormula(a_Obj.m_strFormula)
    ,m_strTok(a_Obj.m_strTok)
    ,m_iPos(a_Obj.m_iPos)
    ,m_iErrc(a_Obj.m_iErrc)
    ,m_ErrMsg(ParserErrorMsg::Instance())
  {
  }

  //------------------------------------------------------------------------------
  ParserError& ParserError::operator=(const ParserError &a_Obj)
  {
    if (this==&a_Obj)
      return *this;

    m_strMsg = a_Obj.m_strMsg;
    m_strFormula = a_Obj.m_strFormula;
    m_strTok = a_Obj.m_strTok;
    m_iPos = a_Obj.m_iPos;
    m_iErrc = a_Obj.m_iErrc;
    return *this;
  }

  //------------------------------------------------------------------------------
  ParserError::~ParserError()
  {
  }

  /** \brief Replace all ocuurences of a substring with another string. */
  void ParserError::ReplaceSubString( string_type &strSource,
                                      const string_type &strFind,
                                      const string_type &strReplaceWith)
  {
    string_type strResult;
    string_type::size_type iPos(0), iNext(0);

    for(;;)
    {
      iNext = strSource.find(strFind, iPos);
      strResult.append(strSource, iPos, iNext-iPos);

      if( iNext==string_type::npos )
        break;

      strResult.append(strReplaceWith);
      iPos = iNext + strFind.length();
    } 

    strSource.swap(strResult);
  }

  //------------------------------------------------------------------------------
  void ParserError::Reset()
  {
    m_strMsg = "";
    m_strFormula = "";
    m_strTok = "";
    m_iPos = -1;
    m_iErrc = ecUNDEFINED;
  }
      
  //------------------------------------------------------------------------------
  void ParserError::SetFormula(const string_type &a_strFormula)
  {
    m_strFormula = a_strFormula;
  }

  //------------------------------------------------------------------------------
  const string_type& ParserError::GetExpr() const 
  {
    return m_strFormula;
  }

  //------------------------------------------------------------------------------
  const string_type& ParserError::GetMsg() const
  {
    return m_strMsg;
  }

  //------------------------------------------------------------------------------
  /** \brief Return the formula position related to the error. 

    If the error is not related to a distinct position this will return -1
  */
  std::size_t ParserError::GetPos() const
  {
    return m_iPos;
  }

  //------------------------------------------------------------------------------
  /** \brief Return string related with this token (if available). */
  const string_type& ParserError::GetToken() const
  {
    return m_strTok;
  }

  //------------------------------------------------------------------------------
  /** \brief Return the error code. */
  EErrorCodes ParserError::GetCode() const
  {
    return m_iErrc;
  }
} // namespace mu
