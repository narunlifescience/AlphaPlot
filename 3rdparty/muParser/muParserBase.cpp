/*
  Copyright (C) 2004-2006 Ingo Berg

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
#include "muParser.h"

#include <cassert>
#include <cmath>
#include <memory>
#include <vector>
#include <sstream>

using namespace std;


namespace mu
{

//------------------------------------------------------------------------------
/** \brief Identifiers for built in binary operators. 

    When defining custom binary operators with #AddOprt(...) make sure not to choose 
    names conflicting with these definitions. 
*/
const char_type* ParserBase::c_DefaultOprt[] = 
{ 
  "<=", ">=", "!=", "==", "<", ">", "+", "-", "*", "/", 
  "^", "and", "or", "xor", "=", "(", ")", ",", 0 
};

//------------------------------------------------------------------------------
/** \brief Constructor.
  \param a_szFormula the formula to interpret.
  \throw ParserException if a_szFormula is null.
*/
ParserBase::ParserBase()
  :m_pParseFormula(&ParserBase::ParseString)
  ,m_pCmdCode(0)
  ,m_vByteCode()
  ,m_vStringBuf()
  ,m_pTokenReader()
  ,m_FunDef()
  ,m_PostOprtDef()
  ,m_InfixOprtDef()
  ,m_OprtDef()
  ,m_ConstDef()
  ,m_StrVarDef()
  ,m_VarDef()
  ,m_bOptimize(true)
  ,m_bUseByteCode(true)
  ,m_bBuiltInOp(true)
  ,m_sNameChars()
  ,m_sOprtChars()
  ,m_sInfixOprtChars()
{
  InitTokenReader();
}

//---------------------------------------------------------------------------
/** \brief Copy constructor. 

 Implemented by calling Assign(a_Parser)
*/
ParserBase::ParserBase(const ParserBase &a_Parser)
  :m_pParseFormula(&ParserBase::ParseString)
  ,m_pCmdCode(0)
  ,m_vByteCode()
  ,m_vStringBuf()
  ,m_pTokenReader()
  ,m_FunDef()
  ,m_PostOprtDef()
  ,m_InfixOprtDef()
  ,m_OprtDef()
  ,m_ConstDef()
  ,m_StrVarDef()
  ,m_VarDef()
  ,m_bOptimize(true)
  ,m_bUseByteCode(true)
  ,m_bBuiltInOp(true)
{
  m_pTokenReader.reset(new token_reader_type(this));
  Assign(a_Parser);
}

//---------------------------------------------------------------------------
/** \brief Assignement operator. 

   Implemented by calling Assign(a_Parser). Self assignement is suppressed.
   \param a_Parser Object to copy to this.
   \return *this
   \throw nothrow
*/
ParserBase& ParserBase::operator=(const ParserBase &a_Parser)
{
  Assign(a_Parser);
  return *this;
}

//---------------------------------------------------------------------------
/** \brief Copy state of a parser object to this. 

  Clears Variables and Functions of this parser.
  Copies the states of all internal variables.
  Resets parse function to string parse mode.

  \param a_Parser the source object.
*/
void ParserBase::Assign(const ParserBase &a_Parser)
{
  if (&a_Parser==this)
    return;

  // Don't copy bytecode instead cause the parser to create new bytecode
  // by resetting the parse function.
  ReInit();

  m_ConstDef = a_Parser.m_ConstDef;         // Copy user define constants
  m_VarDef = a_Parser.m_VarDef;             // Copy user defined variables
  m_bOptimize  = a_Parser.m_bOptimize;
  m_bUseByteCode = a_Parser.m_bUseByteCode;
  m_bBuiltInOp = a_Parser.m_bBuiltInOp;
  m_vStringBuf = a_Parser.m_vStringBuf;
  m_pTokenReader.reset(a_Parser.m_pTokenReader->Clone(this));
  m_StrVarDef = a_Parser.m_StrVarDef;
  m_vStringVarBuf = a_Parser.m_vStringVarBuf;

  // Copy function and operator callbacks
  m_FunDef = a_Parser.m_FunDef;             // Copy function definitions
  m_PostOprtDef = a_Parser.m_PostOprtDef;   // post value unary operators
  m_InfixOprtDef = a_Parser.m_InfixOprtDef; // unary operators for infix notation

  m_sNameChars = a_Parser.m_sNameChars;
  m_sOprtChars = a_Parser.m_sOprtChars;
  m_sInfixOprtChars = a_Parser.m_sInfixOprtChars;
}

//---------------------------------------------------------------------------
/** \brief Initialize the token reader. 

   Create new token reader object and submit pointers to function, operator,
   constant and variable definitions.

   \post m_pTokenReader.get()!=0
   \throw nothrow
*/
void ParserBase::InitTokenReader()
{
  m_pTokenReader.reset(new token_reader_type(this));
}

//---------------------------------------------------------------------------
/** \brief Reset parser to string parsing mode and clear internal buffers.

    Clear bytecode, reset the token reader.
    \throw nothrow
*/
void ParserBase::ReInit() const
{
  m_pParseFormula = &ParserBase::ParseString;
  m_vStringBuf.clear();
  m_vByteCode.clear();
  m_pTokenReader->ReInit();
}

//---------------------------------------------------------------------------
void ParserBase::AddValIdent(identfun_type a_pCallback)
{
  m_pTokenReader->AddValIdent(a_pCallback);
}

//---------------------------------------------------------------------------
void ParserBase::SetVarFactory(facfun_type a_pFactory)
{
  m_pTokenReader->SetVarCreator(a_pFactory);  
}

//---------------------------------------------------------------------------
/** \brief Add a function or operator callback to the parser.
*/
void ParserBase::AddCallback( const string_type &a_strName, 
                              const ParserCallback &a_Callback, 
                              funmap_type &a_Storage,
                              const char_type *a_szCharSet )
{
  if (a_Callback.GetAddr()==0)
      Error(ecINVALID_FUN_PTR);

  const funmap_type *pFunMap = &a_Storage;

  // Check for conflicting operator or function names
  if ( pFunMap!=&m_FunDef && m_FunDef.find(a_strName)!=m_FunDef.end() )
    Error(ecNAME_CONFLICT);

  if ( pFunMap!=&m_PostOprtDef && m_PostOprtDef.find(a_strName)!=m_PostOprtDef.end() )
    Error(ecNAME_CONFLICT);

  if ( pFunMap!=&m_InfixOprtDef && pFunMap!=&m_OprtDef && m_InfixOprtDef.find(a_strName)!=m_InfixOprtDef.end() )
    Error(ecNAME_CONFLICT);

  if ( pFunMap!=&m_InfixOprtDef && pFunMap!=&m_OprtDef && m_OprtDef.find(a_strName)!=m_OprtDef.end() )
    Error(ecNAME_CONFLICT);

  CheckName(a_strName, a_szCharSet);
  a_Storage[a_strName] = a_Callback;
  ReInit();
}

//---------------------------------------------------------------------------
/** \brief Check if a name contains invalid characters. 

    \throw ParserException if the name contains invalid charakters.
*/
void ParserBase::CheckName(const string_type &a_sName,
                           const string_type &a_szCharSet) const
{
  if ( !a_sName.length() ||
       (a_sName.find_first_not_of(a_szCharSet)!=string_type::npos) ||
       (a_sName[0]>='0' && a_sName[0]<='9'))
  {
    Error(ecINVALID_NAME);
  }
}

//---------------------------------------------------------------------------
/** \brief Set the formula. 
    Triggers first time calculation thus the creation of the bytecode and 
    scanning of used variables.

    \param a_strFormula Formula as string_type
    \throw ParserException in case of syntax errors.
*/
void ParserBase::SetExpr(const string_type &a_sExpr)
{
  // <ibg> 20060222: Bugfix for Borland-Kylix:
  // adding a space to the expression will keep Borlands KYLIX from going wild
  // when calling tellg on a stringstream created from the expression after 
  // reading a value at the end of an expression. (mu::Parser::IsVal function)
  // (tellg returns -1 otherwise causing the parser to ignore the value)
  string_type sBuf(a_sExpr + _T(" ") );
  m_pTokenReader->SetFormula(sBuf);
  ReInit();
}

//---------------------------------------------------------------------------
/** \brief Add a user defined operator. 
    \post Will reset the Parser to string parsing mode.
*/
void ParserBase::DefinePostfixOprt(const string_type &a_sName, 
                                   fun_type1 a_pFun, 
                                   bool a_bAllowOpt)
{
  AddCallback( a_sName, 
               ParserCallback(a_pFun, a_bAllowOpt, prPOSTFIX, cmOPRT_POSTFIX),
               m_PostOprtDef, 
               ValidOprtChars() );
}

//---------------------------------------------------------------------------
/** \brief Add a user defined operator. 
    \post Will reset the Parser to string parsing mode.
    \param a_sName [in] operator Identifier 
    \param a_pFun [in] Operator callback function
    \param a_iPrec [in] Operator Precedence (default=prSIGN)
    \param a_bAllowOpt [in] True if operator is volatile (default=false)

    \sa EPrec
*/
void ParserBase::DefineInfixOprt(const string_type &a_sName, 
                                 fun_type1 a_pFun, 
                                 int a_iPrec, 
                                 bool a_bAllowOpt)
{
  AddCallback( a_sName, 
               ParserCallback(a_pFun, a_bAllowOpt, a_iPrec, cmOPRT_INFIX), 
               m_InfixOprtDef, 
               ValidOprtChars() );
}

//---------------------------------------------------------------------------
void ParserBase::DefineOprt( const string_type &a_sName, 
                             fun_type2 a_pFun, 
                             unsigned a_iPrec, 
                             bool a_bAllowOpt )
{
  // Check for conflicts with built in operator names
  for (int i=0; m_bBuiltInOp && i<cmCOMMA; ++i)
    if (a_sName == string_type(c_DefaultOprt[i])) 
      Error(ecBUILTIN_OVERLOAD);

  AddCallback( a_sName, 
               ParserCallback(a_pFun, a_bAllowOpt, a_iPrec, cmOPRT_BIN), 
               m_OprtDef, 
               ValidOprtChars() );
}

//---------------------------------------------------------------------------
void ParserBase::DefineStrConst(const string_type &a_strName, const string_type &a_strVal)
{
  // Test if a constant with that names already exists
  if (m_StrVarDef.find(a_strName)!=m_StrVarDef.end())
    Error(ecNAME_CONFLICT);

  CheckName(a_strName, ValidNameChars());
  
  // Store variable string in internal buffer
  m_vStringVarBuf.push_back(a_strVal);

  // bind buffer index to variable name
  m_StrVarDef[a_strName] = m_vStringBuf.size();

  ReInit();
}

//---------------------------------------------------------------------------
/** \brief Add a user defined variable. 
    \post Will reset the Parser to string parsing mode.
    \pre [assert] a_fVar!=0
    \throw ParserException in case the name contains invalid signs.
*/
void ParserBase::DefineVar(const string_type &a_sName, value_type *a_pVar)
{
  if (a_pVar==0)
    Error(ecINVALID_VAR_PTR);

  // Test if a constant with that names already exists
  if (m_ConstDef.find(a_sName)!=m_ConstDef.end())
    Error(ecNAME_CONFLICT);

  if (m_FunDef.find(a_sName)!=m_FunDef.end())
    Error(ecNAME_CONFLICT);

  CheckName(a_sName, ValidNameChars());
  m_VarDef[a_sName] = a_pVar;
  ReInit();
}

//---------------------------------------------------------------------------
/** \brief Add a user defined constant. 
    \post Will reset the Parser to string parsing mode.
    \throw ParserException in case the name contains invalid signs.
*/
void ParserBase::DefineConst(const string_type &a_sName, value_type a_fVal)
{
  CheckName(a_sName, ValidNameChars());
  m_ConstDef[a_sName] = a_fVal;
  ReInit();
}

//---------------------------------------------------------------------------
/** \brief Get operator priority.

 \throw ParserException if a_Oprt is no operator code
*/
int ParserBase::GetOprtPri(const token_type &a_Tok) const
{
  switch (a_Tok.GetCode())
  {
    // built in operators
    case cmEND:        return -5;
	  case cmCOMMA:      return -4;
    case cmBO :	
    case cmBC :        return -2;
    case cmASSIGN:     return -1;               
    case cmAND:
    case cmXOR:
    case cmOR:         return  prLOGIC;  
    case cmLT :
    case cmGT :
    case cmLE :
    case cmGE :
    case cmNEQ:
    case cmEQ :        return  prCMP; 
    case cmADD:
    case cmSUB:        return  prADD_SUB;
    case cmMUL:
    case cmDIV:        return  prMUL_DIV;
    case cmPOW:        return  prPOW;

    // user defined binary operators
    case cmOPRT_INFIX: 
    case cmOPRT_BIN:   return a_Tok.GetPri();
    default:  Error(ecINTERNAL_ERROR);
              return 999;
  }  
}

//---------------------------------------------------------------------------
/** \brief Return a map containing the used variables only. */
const varmap_type& ParserBase::GetUsedVar() const
{
  try
  {
    m_pTokenReader->IgnoreUndefVar(true);
    ParseString(); // implicitely create or update the map with the
                   // used variables stored in the token reader if not already done
    m_pTokenReader->IgnoreUndefVar(false);
  }
  catch(exception_type &e)
  {
    m_pTokenReader->IgnoreUndefVar(false);
    throw e;
  }
  
  // Make sure to stay in string parse mode, dont call ReInit()
  // because it deletes the array with the used variables
  m_pParseFormula = &ParserBase::ParseString;
  
  return m_pTokenReader->GetUsedVar();
}

//---------------------------------------------------------------------------
/** \brief Return a map containing the used variables only. */
const varmap_type& ParserBase::GetVar() const
{
  return m_VarDef;
}

//---------------------------------------------------------------------------
/** \brief Return a map containing all parser constants. */
const valmap_type& ParserBase::GetConst() const
{
  return m_ConstDef;
}

//---------------------------------------------------------------------------
/** \brief Return prototypes of all parser functions.
    
    The return type is a map of the public type #funmap_type containing the prototype
    definitions for all numerical parser functions. String functions are not part of 
    this map. The Prototype definition is encapsulated in objects of the class FunProt
    one per parser function each associated with function names via a map construct.
    \return #m_FunDef
    \sa FunProt
    \throw nothrow
*/
const funmap_type& ParserBase::GetFunDef() const
{
  return m_FunDef;
}

//---------------------------------------------------------------------------
/** \brief Retrieve the formula. */
const string_type& ParserBase::GetExpr() const
{
  return m_pTokenReader->GetFormula();
}

//---------------------------------------------------------------------------
ParserBase::token_type ParserBase::ApplyNumFunc( const token_type &a_FunTok,
                                                 const std::vector<token_type> &a_vArg) const
{
  token_type  valTok;
  int  iArgCount = (unsigned)a_vArg.size();
  void  *pFunc = a_FunTok.GetFuncAddr();
  assert(pFunc);

  // Collect the function arguments from the value stack
  switch(a_FunTok.GetArgCount())
  {
    case -1:
          // Function with variable argument count
 		      // copy arguments into a vector<value_type>
	        {
            /** \todo remove the unnecessary argument vector by changing order in stArg. */
            std::vector<value_type> vArg;
		        for (int i=0; i<iArgCount; ++i)
		          vArg.push_back(a_vArg[i].GetVal());

	          valTok.SetVal( ((multfun_type)a_FunTok.GetFuncAddr())(&vArg[0], (int)vArg.size()) );  
	        }
	        break;

    case 1: valTok.SetVal( ((fun_type1)a_FunTok.GetFuncAddr())(a_vArg[0].GetVal()) );  break;
    case 2: valTok.SetVal( ((fun_type2)a_FunTok.GetFuncAddr())(a_vArg[1].GetVal(),
		                                                           a_vArg[0].GetVal()) );  break;
    case 3: valTok.SetVal( ((fun_type3)a_FunTok.GetFuncAddr())(a_vArg[2].GetVal(), 
		                                                           a_vArg[1].GetVal(), 
														                                   a_vArg[0].GetVal()) );  break;
    case 4: valTok.SetVal( ((fun_type4)a_FunTok.GetFuncAddr())(a_vArg[3].GetVal(), 
	                                                             a_vArg[2].GetVal(), 
				   	  	                                               a_vArg[1].GetVal(),
													                                     a_vArg[0].GetVal()) );  break;
    case 5: valTok.SetVal( ((fun_type5)a_FunTok.GetFuncAddr())(a_vArg[4].GetVal(), 
														                                   a_vArg[3].GetVal(), 
	                                                             a_vArg[2].GetVal(), 
				   	  	                                               a_vArg[1].GetVal(),
													                                     a_vArg[0].GetVal()) );  break;
    default: Error(ecINTERNAL_ERROR);
  }

  // Find out if the result will depend on a variable
  /** \todo remove this loop, put content in the loop that takes the argument values. 
    
      (Attention: SetVal will reset Flags.)
  */
  bool bVolatile = a_FunTok.IsFlagSet(token_type::flVOLATILE);
  for (int i=0; (bVolatile==false) && (i<iArgCount); ++i)
    bVolatile |= a_vArg[i].IsFlagSet(token_type::flVOLATILE);

  if (bVolatile)
    valTok.AddFlags(token_type::flVOLATILE);

#if defined(_MSC_VER)
  #pragma warning( disable : 4311 ) 
#endif

  // Formula optimization
  if ( m_bOptimize && 
       !valTok.IsFlagSet(token_type::flVOLATILE) &&
       !a_FunTok.IsFlagSet(token_type::flVOLATILE) ) 
	{
    m_vByteCode.RemoveValEntries(iArgCount);
    m_vByteCode.AddVal( valTok.GetVal() );
	}
	else 
	{ 
    // operation dosnt depends on a variable or the function is flagged unoptimizeable
    // we cant optimize here...
    m_vByteCode.AddFun(pFunc, (a_FunTok.GetArgCount()==-1) ? -iArgCount : iArgCount);
	}

  return valTok;

#if defined(_MSC_VER)
  #pragma warning( default : 4311 ) 
#endif
}

//---------------------------------------------------------------------------
/** \brief Execute a function that takes a single string argument.
      
    \param a_FunTok Function token.
    \throw exception_type If the function token is not a string function
*/
ParserBase::token_type ParserBase::ApplyStrFunc(const token_type &a_FunTok,
                                                token_type &a_Arg) const
{
  if (a_Arg.GetCode()!=cmSTRING)
    Error(ecSTRING_EXPECTED, m_pTokenReader->GetPos(), a_FunTok.GetAsString());

  strfun_type1 pFunc = (strfun_type1)a_FunTok.GetFuncAddr();
  assert(pFunc);
  
  value_type fResult = pFunc( a_Arg.GetAsString().c_str() );

  // Formula optimization
  if ( m_bOptimize && 
       !a_FunTok.IsFlagSet(token_type::flVOLATILE) ) 
	{
    m_vByteCode.AddVal( fResult );
	}
	else 
	{ 
    // operation dosnt depends on a variable or the function is flagged unoptimizeable
    // we cant optimize here...
    m_vByteCode.AddStrFun((void*)pFunc, a_FunTok.GetArgCount(), a_Arg.GetIdx());
	}
  
  a_Arg.SetVal(fResult);
  
  return a_Arg;
}

//---------------------------------------------------------------------------
/** \brief Apply a function token. 

    \param iArgCount Number of Arguments actually gathered used only for multiarg functions.
    \post Function have been taken from the stack, the result has been pushed 
    \post The function token is removed from the stack
    \throw exception_type if Argument count does not mach function requirements.
*/
void ParserBase::ApplyFunc( ParserStack<token_type> &a_stOpt,
                            ParserStack<token_type> &a_stVal, 
                            int a_iArgCount) const
{ 
  assert(m_pTokenReader.get());

  // Operator stack empty or does not contain tokens with callback functions
  if (a_stOpt.empty() || a_stOpt.top().GetFuncAddr()==0 )
    return;

  token_type funTok = a_stOpt.pop();
  assert(funTok.GetFuncAddr());

  // Binary operators must rely on their internal operator number
  // since counting of operators relies on commas for function arguments
  // binary operators do not have commas in their expression
  int iArgCount = ( funTok.GetCode()==cmOPRT_BIN ) ? funTok.GetArgCount() : a_iArgCount;

  if (funTok.GetArgCount()>0 && iArgCount>funTok.GetArgCount()) 
	    Error(ecTOO_MANY_PARAMS, m_pTokenReader->GetPos()-1, funTok.GetAsString());

	if ( funTok.GetCode()!=cmOPRT_BIN && iArgCount<funTok.GetArgCount() )
	    Error(ecTOO_FEW_PARAMS, m_pTokenReader->GetPos()-1, funTok.GetAsString());

  // Collect the function arguments from the value stack and store them
  // in a vector
  std::vector<token_type> stArg;  
  for (int i=0; i<iArgCount; ++i)
  {
    stArg.push_back( a_stVal.pop() );
    if ( stArg.back().GetType()==tpSTR && funTok.GetType()!=tpSTR )
      Error(ecVAL_EXPECTED, m_pTokenReader->GetPos(), funTok.GetAsString());
  }

#ifdef __BORLANDC__
  // Borland C++ Compiler does not support taking references on
  // unnamed temporaries
  if (funTok.GetType()==tpSTR) {
    ParserToken<double,std::string> pt = ApplyStrFunc(funTok, stArg.back());
    a_stVal.push(pt);
  }
  else {
    ParserToken<double,std::string> pt = ApplyNumFunc(funTok, stArg);
    a_stVal.push(pt);
  }
#else
  // String functions accept only one parameter
  a_stVal.push( (funTok.GetType()==tpSTR) ?
                      ApplyStrFunc(funTok, stArg.back()) :
                      ApplyNumFunc(funTok, stArg) );
#endif // __BORLANDC__
}

//---------------------------------------------------------------------------
void ParserBase::ApplyBinOprt( ParserStack<token_type> &a_stOpt,
                               ParserStack<token_type> &a_stVal) const
{ 
  assert(a_stOpt.size());

  // user defined binary operator
  if (a_stOpt.top().GetCode()==cmOPRT_BIN)
  {
     ApplyFunc(a_stOpt, a_stVal, 2);
  }
  else
  {
    // internal binary operator
    MUP_ASSERT(a_stVal.size()>=2);

    token_type valTok1 = a_stVal.pop(),
               valTok2 = a_stVal.pop(),
               optTok = a_stOpt.pop(),
               resTok; 

    if ( valTok1.GetType()!=valTok2.GetType() || 
         (valTok1.GetType()==tpSTR && valTok2.GetType()==tpSTR) )
      Error(ecOPRT_TYPE_CONFLICT, m_pTokenReader->GetPos(), optTok.GetAsString());

    value_type x = valTok2.GetVal(),
	             y = valTok1.GetVal();

    switch (optTok.GetCode())
    {
      // built in binary operators
      case cmAND: resTok.SetVal( (int)x & (int)y ); break;
      case cmOR:  resTok.SetVal( (int)x | (int)y ); break;
      case cmXOR: resTok.SetVal( (int)x ^ (int)y ); break;
      case cmLT:  resTok.SetVal( x < y ); break;
      case cmGT:  resTok.SetVal( x > y ); break;
      case cmLE:  resTok.SetVal( x <= y ); break;
      case cmGE:  resTok.SetVal( x >= y ); break;
      case cmNEQ: resTok.SetVal( x != y ); break;
      case cmEQ:  resTok.SetVal( x == y ); break;
      case cmADD: resTok.SetVal( x + y ); break;
      case cmSUB: resTok.SetVal( x - y ); break;
      case cmMUL: resTok.SetVal( x * y ); break;
      case cmDIV: resTok.SetVal( x / y ); break;
  	  case cmPOW: resTok.SetVal(pow(x, y)); break;

      case cmASSIGN: 
                // The assignement operator needs special treatment
                // it uses a different format when stored in the bytecode!
                { 
                  if (valTok2.GetCode()!=cmVAR)
                    Error(ecINTERNAL_ERROR);
                    
                  double *pVar = valTok2.GetVar();
                  resTok.SetVal( *pVar = y );
                  a_stVal.push( resTok );

                  m_vByteCode.AddAssignOp(pVar);
                  return;  // we must return since the following 
                           // stuff does not apply
                }

      default:  Error(ecINTERNAL_ERROR);
    }

    // Create the bytecode entries
    if (!m_bOptimize)
    {
      // Optimization flag is not set
      m_vByteCode.AddOp(optTok.GetCode());
    }
    else if ( valTok1.IsFlagSet(token_type::flVOLATILE) || 
              valTok2.IsFlagSet(token_type::flVOLATILE) )
    {
      // Optimization flag is not set, but one of the value
      // depends on a variable
      m_vByteCode.AddOp(optTok.GetCode());
      resTok.AddFlags(token_type::flVOLATILE);
    }
    else
    {
      // operator call can be optimized; If optimization is possible 
      // the two previous tokens must be value tokens / they will be removed
      // and replaced with the result of the pending operation.
      m_vByteCode.RemoveValEntries(2);
      m_vByteCode.AddVal(resTok.GetVal());
    }

    a_stVal.push( resTok );
  }
}

//---------------------------------------------------------------------------
/** \brief Parse the command code.

  Command code contains precalculated stack positions of the values and the 
  associated operators.  
  The Stack is filled beginning from index one the value at index zero is
  not used at all.

  \sa ParseString(), ParseValue()
*/
value_type ParserBase::ParseCmdCode() const
{
#if defined(_MSC_VER)
  #pragma warning( disable : 4312 )
#endif

  value_type Stack[99];
  ECmdCode iCode;
  bytecode_type idx(0);
  int i(0);

  __start:

  idx = m_pCmdCode[i]; 
  iCode = (ECmdCode)m_pCmdCode[i+1];
  i += 2;

#ifdef _DEBUG
  if (idx>=99)
    throw exception_type(ecGENERIC, "", m_pTokenReader->GetFormula(), -1);
#endif
//  assert(idx<99); // Formula too complex

  switch (iCode)
  {
    // built in binary operators
    case cmAND: Stack[idx]  = (int)Stack[idx] & (int)Stack[idx+1]; goto __start;
    case cmOR:  Stack[idx]  = (int)Stack[idx] | (int)Stack[idx+1]; goto __start;
    case cmXOR: Stack[idx]  = (int)Stack[idx] ^ (int)Stack[idx+1]; goto __start;
    case cmLE:  Stack[idx]  = Stack[idx] <= Stack[idx+1]; goto __start;
    case cmGE:  Stack[idx]  = Stack[idx] >= Stack[idx+1]; goto __start;
    case cmNEQ: Stack[idx]  = Stack[idx] != Stack[idx+1]; goto __start;
    case cmEQ:  Stack[idx]  = Stack[idx] == Stack[idx+1]; goto __start;
	  case cmLT:  Stack[idx]  = Stack[idx] < Stack[idx+1];  goto __start;
	  case cmGT:  Stack[idx]  = Stack[idx] > Stack[idx+1];  goto __start;
    case cmADD: Stack[idx] += Stack[1+idx]; goto __start;
 	  case cmSUB: Stack[idx] -= Stack[1+idx]; goto __start;
	  case cmMUL: Stack[idx] *= Stack[1+idx]; goto __start;
	  case cmDIV: Stack[idx] /= Stack[1+idx]; goto __start;
    case cmPOW: Stack[idx]  = pow(Stack[idx], Stack[1+idx]); goto __start;

    // Assignement needs special treatment
    case cmASSIGN: 
           {
             // next is a pointer to the target
             value_type **pDest = (value_type**)(&m_pCmdCode[i]);
      
             // advance index according to pointer size
             i += m_vByteCode.GetPtrSize();
             // assign the value
             Stack[idx] = **pDest = Stack[idx+1]; 
           }     
           goto __start;

    // user defined binary operators
    case cmOPRT_BIN: 
           Stack[idx] = (**(fun_type2**)(&m_pCmdCode[i]))(Stack[idx], Stack[idx+1]);
           ++i;
           goto __start;

    // variable tokens
	  case cmVAR:
		        Stack[idx] = **(value_type**)(&m_pCmdCode[i]);
		        i += m_vByteCode.GetValSize();
		        goto __start;
	
    // value tokens
	  case cmVAL:
            Stack[idx] = *(value_type*)(&m_pCmdCode[i]);
 	          i += m_vByteCode.GetValSize();
            goto __start;

    // Next is treatment of string functions
    case cmFUNC_STR:
            {
              i++; // skip the unused argument count
              strfun_type1 pFun = *(strfun_type1*)(&m_pCmdCode[i]);
              i += m_vByteCode.GetPtrSize();

              int iIdxStack = (int)m_pCmdCode[i++];
#if defined(_DEBUG)  
              if ( (iIdxStack<0) || (iIdxStack>=(int)m_vStringBuf.size()) )
                Error(ecINTERNAL_ERROR);
#endif  
              Stack[idx] = pFun(m_vStringBuf[iIdxStack].c_str());
            }
            goto __start;

    // Next is treatment of numeric functions
    case cmFUNC:	
		        {
		          int iArgCount = (int)m_pCmdCode[i++];
          		   
              switch(iArgCount)  // switch according to argument count
		          {
                case 1: Stack[idx] = (*(fun_type1*)(&m_pCmdCode[i]))(Stack[idx]); break;
			          case 2: Stack[idx] = (*(fun_type2*)(&m_pCmdCode[i]))(Stack[idx], Stack[idx+1]); break;
			          case 3: Stack[idx] = (*(fun_type3*)(&m_pCmdCode[i]))(Stack[idx], Stack[idx+1], Stack[idx+2]); break;
			          case 4: Stack[idx] = (*(fun_type4*)(&m_pCmdCode[i]))(Stack[idx], Stack[idx+1], Stack[idx+2], Stack[idx+3]); break;
  		          case 5: Stack[idx] = (*(fun_type5*)(&m_pCmdCode[i]))(Stack[idx], Stack[idx+1], Stack[idx+2], Stack[idx+3], Stack[idx+4]); break;
                default:
				          if (iArgCount>0) // function with variable arguments store the number as a negative value
                    Error(ecINTERNAL_ERROR);

                  Stack[idx] =(*(multfun_type*)(&m_pCmdCode[i]))(&Stack[idx], -iArgCount); 
                  break;
		          }
		          i += m_vByteCode.GetPtrSize();
		        }
		        goto __start;

	  case cmEND: 
		        return Stack[1];

	  default: 
            Error(ecINTERNAL_ERROR);
            return 0;
  }

#if defined(_MSC_VER)
  #pragma warning( default : 4312 )
#endif
}

//---------------------------------------------------------------------------
/** \brief Return result for constant functions. 

  Seems pointless, but for parser functions that are made up of only a value, which occur 
  in real world applications, this speeds up things by removing the parser overhead almost 
  completely.
*/
value_type ParserBase::ParseValue() const
{
  return *(value_type*)(&m_pCmdCode[2]);
}

//---------------------------------------------------------------------------
/** \brief One of the two main parse functions.

 Parse expression from input string. Perform syntax checking and create bytecode.
 After parsing the string and creating the bytecode the function pointer 
 #m_pParseFormula will be changed to the second parse routine the uses bytecode instead of string parsing.

 \sa ParseCmdCode(), ParseValue()
*/
value_type ParserBase::ParseString() const
{ 
#if defined(_MSC_VER)
  #pragma warning( disable : 4311 ) 
#endif

  if (!m_pTokenReader->GetFormula().length()) 
      Error(ecUNEXPECTED_EOF, 0);

  ParserStack<token_type> stOpt, stVal;
  ParserStack<int> stArgCount;
  token_type opta, opt;  // for storing operators
  token_type val, tval;  // for storing value
  string_type strBuf;    // buffer for string function arguments

  ReInit();

  for(;;)
  {
    opt = m_pTokenReader->ReadNextToken();

    switch (opt.GetCode())
    {
      //
      // Next three are different kind of value entries
      //
      case cmSTRING:
              opt.SetIdx((int)m_vStringBuf.size()); // Assign buffer index to token 
              stVal.push(opt);
		          m_vStringBuf.push_back(opt.GetAsString()); // Store string in internal buffer
              break;
 
      case cmVAR:
   		        stVal.push(opt);
              m_vByteCode.AddVar( static_cast<value_type*>(opt.GetVar()) );
              break;

      case cmVAL:
		          stVal.push(opt);
              m_vByteCode.AddVal( opt.GetVal() );
              break;

      case cmCOMMA:
              if (stArgCount.empty())
                Error(ecUNEXPECTED_COMMA, m_pTokenReader->GetPos());
              ++stArgCount.top(); // Record number of arguments
              // fall through...
      case cmEND:
      case cmBC:
              {
                while ( stOpt.size() && stOpt.top().GetCode() != cmBO)
                {
                  if (stOpt.top().GetCode()==cmOPRT_INFIX)
                    ApplyFunc(stOpt, stVal, 1);  // infix operator
                  else
                    ApplyBinOprt(stOpt, stVal);
                }

                // <ibg> 20060218 infix operator treatment moved here 
                if (stOpt.size() && stOpt.top().GetCode()==cmOPRT_INFIX) 
                  ApplyFunc(stOpt, stVal, 1);  // infix operator

                if ( opt.GetCode()!=cmBC || stOpt.size()==0 || stOpt.top().GetCode()!=cmBO )
                  break;

                // if opt is ")" and opta is "(" the bracket has been evaluated, now its time to check
			          // if there is either a function or a sign pending
		   	        // neither the opening nor the closing bracket will be pushed back to
			          // the operator stack
			          // Check if a function is standing in front of the opening bracket, 
                // if yes evaluate it afterwards check for infix operators
			          assert(stArgCount.size());
			          int iArgCount = stArgCount.pop();
                
                stOpt.pop(); // Take opening bracket from stack

                if (iArgCount>1 && (stOpt.size()==0 || stOpt.top().GetCode()!=cmFUNC) )
                  Error(ecUNEXPECTED_ARG, m_pTokenReader->GetPos());
                
                if (stOpt.size() && stOpt.top().GetCode()!=cmOPRT_INFIX)
                  ApplyFunc(stOpt, stVal, iArgCount);
			        } // if bracket content is evaluated
              break;

      //
      // Next are the binary operator entries
      //
      case cmAND:   // built in binary operators
      case cmOR:
      case cmXOR:
      case cmLT:
      case cmGT:
      case cmLE:
      case cmGE:
      case cmNEQ:
      case cmEQ:
      case cmADD:
      case cmSUB:
      case cmMUL:
      case cmDIV:
      case cmPOW:
      case cmASSIGN:
      case cmOPRT_BIN:
              // A binary operator (user defined or built in) has been found. 
              while ( stOpt.size() && stOpt.top().GetCode() != cmBO)
              {
                if (GetOprtPri(stOpt.top()) < GetOprtPri(opt))
                  break;

                if (stOpt.top().GetCode()==cmOPRT_INFIX)
                  ApplyFunc(stOpt, stVal, 1);  // infix operator
                else
                  ApplyBinOprt(stOpt, stVal);
              } // while ( ... )

    			    // The operator can't be evaluated right now, push back to the operator stack 
              stOpt.push(opt);
              break;

      //
      // Last section contains functions and operators implicitely mapped to functions
      //
	    case cmBO:    
              stArgCount.push(1); // we use this for counting the bracket level
              stOpt.push(opt);
		    	    break;

      case cmFUNC:
      case cmFUNC_STR: 
      case cmOPRT_INFIX:
              stOpt.push(opt);
		    	    break;

      case cmOPRT_POSTFIX:
              stOpt.push(opt);
              ApplyFunc(stOpt, stVal, 1);  // this is the postfix operator
              break;

	    default:  Error(ecINTERNAL_ERROR);
    } // end of switch operator-token

    if ( opt.GetCode() == cmEND )
    {
      m_vByteCode.Finalize();
      break;
    }

#if defined(MU_PARSER_DUMP_STACK)
    StackDump(stVal, stOpt);
    m_vByteCode.AsciiDump();
#endif
  } // while (true)

  // Store pointer to start of bytecode
  m_pCmdCode = m_vByteCode.GetRawData();

#if defined(MU_PARSER_DUMP_CMDCODE)
//  m_vByteCode.AsciiDump();
#endif

  // get the last value (= final result) from the stack
  if (stVal.size()!=1)
    Error(ecINTERNAL_ERROR);

  if (stVal.top().GetType()!=tpDBL)
    Error(ecSTR_RESULT);

  // no error, so change the function pointer for the main parse routine
  value_type fVal = stVal.top().GetVal();   // Result from String parsing

  if (m_bUseByteCode)
  {
    m_pParseFormula = (m_pCmdCode[1]==cmVAL && m_pCmdCode[6]==cmEND) ? 
                            &ParserBase::ParseValue : 
                            &ParserBase::ParseCmdCode;
  }

  return fVal;

#if defined(_MSC_VER)
  #pragma warning( default : 4311 )
#endif
}


//---------------------------------------------------------------------------
/** \brief Create an error containing the parse error position.

  This function will create an Parser Exception object containing the error text and
  its position.

  \param a_iErrc [in] The error code of type #EErrorCodes.
  \param a_iPos [in] The position where the error was detected.
  \param a_strTok [in] The token string representation associated with the error.
  \throw ParserException always throws thats the only purpose of this function.
*/
void  ParserBase::Error(EErrorCodes a_iErrc, int a_iPos, const string_type &a_sTok) const
{
  throw exception_type(a_iErrc, a_sTok, m_pTokenReader->GetFormula(), a_iPos);
}

//------------------------------------------------------------------------------
/** \brief Clear all user defined variables.

    Resets the parser to string parsing mode by calling #ReInit.
    \throw nothrow
*/
void ParserBase::ClearVar()
{
  m_VarDef.clear();
  ReInit();
}

//------------------------------------------------------------------------------
/** \brief Remove a variable from internal storage.

Removes a variable if it exists. If the Variable does not exist nothing will be done.

\throw nothrow
*/
void ParserBase::RemoveVar(const string_type &a_strVarName)
{
  varmap_type::iterator item = m_VarDef.find(a_strVarName);
  if (item!=m_VarDef.end())
  {
    m_VarDef.erase(item);
    ReInit();
  }
}

//------------------------------------------------------------------------------
/** \brief Clear the formula. 

Clear the formula and existing bytecode.

\post Resets the parser to string parsing mode.
\throw nothrow
*/
void ParserBase::ClearFormula()
{
  m_vByteCode.clear();
  m_pCmdCode = 0;
  m_pTokenReader->SetFormula("");
  ReInit();
}

//------------------------------------------------------------------------------
/** \brief Clear all functions.
    \post Resets the parser to string parsing mode.
    \throw nothrow
*/
void ParserBase::ClearFun()
{
  m_FunDef.clear();
  ReInit();
}

//------------------------------------------------------------------------------
/** \brief Clear all user defined constants.

    Both numeric and string constants will be removed from the internal storage.
    \post Resets the parser to string parsing mode.
    \throw nothrow
*/
void ParserBase::ClearConst()
{
  m_ConstDef.clear();
  m_StrVarDef.clear();
  ReInit();
}

//------------------------------------------------------------------------------
/** \brief Clear all user defined postfix operators.
    \post Resets the parser to string parsing mode.
    \throw nothrow
*/
void ParserBase::ClearPostfixOprt()
{
  m_PostOprtDef.clear();
  ReInit();
}

//------------------------------------------------------------------------------
/** \brief Clear all user defined binary operators.
    \post Resets the parser to string parsing mode.
    \throw nothrow
*/
void ParserBase::ClearOprt()
{
  m_OprtDef.clear();
  ReInit();
}

//------------------------------------------------------------------------------
/** \brief Clear the user defined Prefix operators. 
    \post Resets the parser to string parser mode.
    \throw nothrow
*/
void ParserBase::ClearInfixOprt()
{
  m_InfixOprtDef.clear();
  ReInit();
}

//------------------------------------------------------------------------------
/** \brief Enable or disable the formula optimization feature. 
    \post Resets the parser to string parser mode.
    \throw nothrow
*/
void ParserBase::EnableOptimizer(bool a_bIsOn)
{
  m_bOptimize = a_bIsOn;
  ReInit();
}

//------------------------------------------------------------------------------
/** \brief Enable or disable parsing from Bytecode. 

    \attention There is no reason to disable bytecode. It will 
               drastically decrease parsing speed.
*/
void ParserBase::EnableByteCode(bool a_bIsOn)
{
  m_bUseByteCode = a_bIsOn;
  if (!a_bIsOn)
    ReInit();
}

//------------------------------------------------------------------------------
/** \brief Enable or disable the built in binary operators.

   If you disable the built in binary operators there will be no binary operators
   defined. Thus you must add them manually one by one. It is not possible to
   disable built in operators selectively. This function will Reinitialize the
   parser by calling ReInit().

   \throw nothrow
   \sa m_bBuiltInOp, ReInit()
*/
void ParserBase::EnableBuiltInOprt(bool a_bIsOn)
{
  m_bBuiltInOp = a_bIsOn;
  ReInit();
}

//------------------------------------------------------------------------------
/** \brief Query status of built in variables.
    \return #m_bBuiltInOp; true if built in operators are enabled.
    \throw nothrow
*/
bool ParserBase::HasBuiltInOprt() const
{
  return m_bBuiltInOp;
}

#if defined(MUP_DUMP_STACK) | defined(MUP_DUMP_CMDCODE)

//------------------------------------------------------------------------------
/** \brief Dump stack content. 

    This function is used for debugging only.
*/
void ParserBase::StackDump( const ParserStack<token_type > &a_stVal, 
				             			  const ParserStack<token_type > &a_stOprt ) const
{
  using std::cout;
  ParserStack<token_type> stOprt(a_stOprt), 
                          stVal(a_stVal);

  cout << "\nValue stack:\n";
  while ( !stVal.empty() ) 
  {
    token_type val = stVal.pop();
    cout << " " << val.GetVal() << " ";
  }
  cout << "\nOperator stack:\n";

  while ( !stOprt.empty() )
  {
     if (stOprt.top().GetCode()<=cmASSIGN) 
  	 {
	  	 cout << "OPRT_INTRNL \"" 
            << ParserBase::c_DefaultOprt[stOprt.top().GetCode()] 
            << "\" \n";
	   }
     else
     {
		    switch(stOprt.top().GetCode())
		    {
		      case cmVAR:        cout << "VAR\n";  break;
		      case cmVAL:        cout << "VAL\n";  break;
		      case cmFUNC:       cout << "FUNC_NUM \"" 
                                  << stOprt.top().GetAsString() 
                                  << "\"\n";   break;
		      case cmOPRT_INFIX: cout << "OPRT_INFIX \"" 
                                  << stOprt.top().GetAsString() 
                                  << "\"\n";   break;
          case cmOPRT_BIN:   cout << "OPRT_BIN \"" 
                                  << stOprt.top().GetAsString() 
                                  << "\"\n";        break;
          case cmFUNC_STR:   cout << "FUNC_STR\n";  break;
		      case cmEND:        cout << "END\n";       break;
		      case cmUNKNOWN:    cout << "UNKNOWN\n";   break;
		      case cmBO:         cout << "BRACKET \"(\"\n";  break;
		      case cmBC:         cout << "BRACKET \")\"\n";  break;
		      default:           cout << stOprt.top().GetType() << " ";  break;
		    }
     }	
     stOprt.pop();
  }

  cout << dec << endl;
}

#endif

} // namespace mu
