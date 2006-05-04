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
#include "muParserTest.h"

#include <cstdio>
#include <cmath>
#include <iostream>

#define PARSER_CONST_PI  3.141592653589793238462643
#define PARSER_CONST_E   2.718281828459045235360287

using namespace std;


namespace mu
{
  namespace Test
  {
    int ParserTester::c_iCount = 0;

    //---------------------------------------------------------------------------
    ParserTester::ParserTester()
      :m_vTestFun()
      ,m_stream(&std::cout)
    {
      AddTest(&ParserTester::TestNames);
      AddTest(&ParserTester::TestSyntax);
      AddTest(&ParserTester::TestPostFix);
      AddTest(&ParserTester::TestInfixOprt);
      AddTest(&ParserTester::TestVarConst);
      AddTest(&ParserTester::TestVolatile);
      AddTest(&ParserTester::TestMultiArg);
      AddTest(&ParserTester::TestFormula);
      AddTest(&ParserTester::TestInterface);
      AddTest(&ParserTester::TestBinOprt);
      AddTest(&ParserTester::TestException);
      AddTest(&ParserTester::TestStrArg);

      ParserTester::c_iCount = 0;
    }

    //---------------------------------------------------------------------------
    int ParserTester::TestInterface()
    {
      int iStat = 0;
      *m_stream << "testing member functions...";
   
      // Test RemoveVar
      value_type afVal[3] = {1,2,3};
      Parser p;
  
      try
      {
        p.DefineVar("a", &afVal[0]);
        p.DefineVar("b", &afVal[1]);
        p.DefineVar("c", &afVal[2]);
        p.SetExpr("a+b+c");
        p.Eval();
      }
      catch(...)
      {
        iStat += 1;  // this is not supposed to happen 
      }

      try
      {
        p.RemoveVar("c");
        p.Eval();
        iStat += 1;  // not supposed to reach this, nonexisting variable "c" deleted...
      }
      catch(...)
      {
        // failure is expected...
      }

      if (iStat==0) 
        *m_stream << "passed" << endl;
      else 
        *m_stream << "\n  failed with " << iStat << " errors" << endl;

      return iStat;
    }

    //---------------------------------------------------------------------------
    int ParserTester::TestStrArg()
    {
      int iStat = 0;
      *m_stream << "testing string arguments...";
 
      iStat += EqnTest("valueof(\"aaa\")+valueof(\"bbb\")  ", 246, true);
      iStat += EqnTest("2*(valueof(\"aaa\")-23)+valueof(\"bbb\")", 323, true);
      // use in expressions with variables
      iStat += EqnTest("a*(atof(\"10\")-b)", 8, true);
      iStat += EqnTest("a-(atof(\"10\")*b)", -19, true);

      if (iStat==0)
        *m_stream << "passed" << endl;
      else 
        *m_stream << "\n  failed with " << iStat << " errors" << endl;

      return iStat;
    }

    //---------------------------------------------------------------------------
    int ParserTester::TestBinOprt()
    {
      int iStat = 0;
      *m_stream << "testing binary operators...";
   
      // built in operators
      // xor operator
      iStat += EqnTest("1 xor 2", 3, true); 
      iStat += EqnTest("a xor b", 3, true);            // with a=1 and b=2
      iStat += EqnTest("1 xor 2 xor 3", 0, true); 
      iStat += EqnTest("a xor b xor 3", 0, true);      // with a=1 and b=2
      iStat += EqnTest("a xor b xor c", 0, true);      // with a=1 and b=2
      iStat += EqnTest("(1 xor 2) xor 3", 0, true); 
      iStat += EqnTest("(a xor b) xor c", 0, true);    // with a=1 and b=2
      iStat += EqnTest("(a) xor (b) xor c", 0, true);  // with a=1 and b=2
      iStat += EqnTest("1 or 2", 3, true); 
      iStat += EqnTest("a or b", 3, true);             // with a=1 and b=2

      // Assignement operator
      iStat += EqnTest("a = b", 2, true); 
      iStat += EqnTest("a = sin(b)", 0.909297, true); 
      iStat += EqnTest("a = 1+sin(b)", 1.909297, true); 

      // Test user defined binary operators
      iStat += EqnTestInt("1 | 2", 3, true);          
      iStat += EqnTestInt("1 || 2", 1, true);          
      iStat += EqnTestInt("123 & 456", 72, true);          
      iStat += EqnTestInt("(123 & 456) % 10", 2, true);
      iStat += EqnTestInt("1 && 0", 0, true);          
      iStat += EqnTestInt("123 && 456", 1, true);          
      iStat += EqnTestInt("1 << 3", 8, true);          
      iStat += EqnTestInt("8 >> 3", 1, true);          
      iStat += EqnTestInt("10 ^ 10", 0, true);          
      iStat += EqnTestInt("10 * 10 ^ 99", 7, true);          
      iStat += EqnTestInt("9 / 4", 2, true);  
      iStat += EqnTestInt("9 % 4", 1, true);  
      iStat += EqnTestInt("if(5%2,1,0)", 1, true);
      iStat += EqnTestInt("if(4%2,1,0)", 0, true);
      iStat += EqnTestInt("-10+1", -9, true);
      iStat += EqnTestInt("1+2*3", 7, true);
      iStat += EqnTestInt("const1 != const2", 1, true);
      iStat += EqnTestInt("const1 != const2", 0, false);
      iStat += EqnTestInt("const1 == const2", 0, true);
      iStat += EqnTestInt("const1 == 1", 1, true);
      iStat += EqnTestInt("10*(const1 == 1)", 10, true);
      iStat += EqnTestInt("2*(const1 | const2)", 6, true);
      iStat += EqnTestInt("2*(const1 | const2)", 7, false);
      iStat += EqnTestInt("const1 < const2", 1, true);
      iStat += EqnTestInt("const2 > const1", 1, true);
      iStat += EqnTestInt("const1 <= 1", 1, true);
      iStat += EqnTestInt("const2 >= 2", 1, true);
      iStat += EqnTestInt("2*(const1 + const2)", 6, true);
      iStat += EqnTestInt("2*(const1 - const2)", -2, true);

      iStat += EqnTestInt("a != b", 1, true);
      iStat += EqnTestInt("a != b", 0, false);
      iStat += EqnTestInt("a == b", 0, true);
      iStat += EqnTestInt("a == 1", 1, true);
      iStat += EqnTestInt("10*(a == 1)", 10, true);
      iStat += EqnTestInt("2*(a | b)", 6, true);
      iStat += EqnTestInt("2*(a | b)", 7, false);
      iStat += EqnTestInt("a < b", 1, true);
      iStat += EqnTestInt("b > a", 1, true);
      iStat += EqnTestInt("a <= 1", 1, true);
      iStat += EqnTestInt("b >= 2", 1, true);
      iStat += EqnTestInt("2*(a + b)", 6, true);
      iStat += EqnTestInt("2*(a - b)", -2, true);
      iStat += EqnTestInt("a + (a << b)", 5, true);
      iStat += EqnTestInt("-2^2", -4, true);
// incorrect: '^' is yor here, not power
//    iStat += EqnTestInt("-(1+2)^2", -9, true);
//    iStat += EqnTestInt("-1^3", -1, true);          

      // Test precedence
      // a=1, b=2, c=3
      iStat += EqnTestInt("a + b * c", 7, true);
      iStat += EqnTestInt("a * b + c", 5, true);
      iStat += EqnTestInt("a<b && b>10", 0, true);
      iStat += EqnTestInt("a<b && b<10", 1, true);

      iStat += EqnTestInt("a + b << c", 17, true);
      iStat += EqnTestInt("a << b + c", 7, true);
      iStat += EqnTestInt("c * b < a", 0, true);
      iStat += EqnTestInt("c * b == 6 * a", 1, true);

      if (iStat==0)
        *m_stream << "passed" << endl;
      else 
        *m_stream << "\n  failed with " << iStat << " errors" << endl;

      return iStat;
    }

    //---------------------------------------------------------------------------
    int ParserTester::TestNames()
    {
      int  iStat= 0,
           iErr = 0;

     *m_stream << "testing name restriction enforcement...";
    
      Parser p;

  #define PARSER_THROWCHECK(DOMAIN, FAIL, EXPR, ARG) \
      iErr = 0;                                      \
      ParserTester::c_iCount++;                      \
      try                                            \
      {                                              \
        p.Define##DOMAIN(EXPR, ARG);                 \
      }                                              \
      catch(Parser::exception_type&)                 \
      {                                              \
        iErr = (FAIL==false) ? 0 : 1;                \
      }                                              \
      iStat += iErr;      
      
      // constant names
      PARSER_THROWCHECK(Const, false, "0a", 1)
      PARSER_THROWCHECK(Const, false, "9a", 1)
      PARSER_THROWCHECK(Const, false, "+a", 1)
      PARSER_THROWCHECK(Const, false, "-a", 1)
      PARSER_THROWCHECK(Const, false, "a-", 1)
      PARSER_THROWCHECK(Const, false, "a*", 1)
      PARSER_THROWCHECK(Const, false, "a?", 1)
      PARSER_THROWCHECK(Const, false, "a?", 1)
      PARSER_THROWCHECK(Const, false, "a?", 1)
      PARSER_THROWCHECK(Const, true, "a", 1)
      PARSER_THROWCHECK(Const, true, "a_min", 1)
      PARSER_THROWCHECK(Const, true, "a_min0", 1)
      PARSER_THROWCHECK(Const, true, "a_min9", 1)
      // variable names
      value_type a;
      p.ClearConst();
      PARSER_THROWCHECK(Var, false, "9a", &a)
      PARSER_THROWCHECK(Var, false, "0a", &a)
      PARSER_THROWCHECK(Var, false, "+a", &a)
      PARSER_THROWCHECK(Var, false, "-a", &a)
      PARSER_THROWCHECK(Var, false, "?a", &a)
      PARSER_THROWCHECK(Var, false, "!a", &a)
      PARSER_THROWCHECK(Var, false, "a+", &a)
      PARSER_THROWCHECK(Var, false, "a-", &a)
      PARSER_THROWCHECK(Var, false, "a*", &a)
      PARSER_THROWCHECK(Var, false, "a?", &a)
      PARSER_THROWCHECK(Var, true, "a", &a)
      PARSER_THROWCHECK(Var, true, "a_min", &a)
      PARSER_THROWCHECK(Var, true, "a_min0", &a)
      PARSER_THROWCHECK(Var, true, "a_min9", &a)
      PARSER_THROWCHECK(Var, false, "a_min9", 0)
      // Postfix operators
      // fail
      PARSER_THROWCHECK(PostfixOprt, false, "(k", f1of1)
      PARSER_THROWCHECK(PostfixOprt, false, "9+", f1of1)
      PARSER_THROWCHECK(PostfixOprt, false, "+", 0)
      // pass
      PARSER_THROWCHECK(PostfixOprt, true, "-a",  f1of1)
      PARSER_THROWCHECK(PostfixOprt, true, "?a",  f1of1)
      PARSER_THROWCHECK(PostfixOprt, true, "_",   f1of1)
      PARSER_THROWCHECK(PostfixOprt, true, "#",   f1of1)
      PARSER_THROWCHECK(PostfixOprt, true, "&&",  f1of1)
      PARSER_THROWCHECK(PostfixOprt, true, "||",  f1of1)
      PARSER_THROWCHECK(PostfixOprt, true, "&",   f1of1)
      PARSER_THROWCHECK(PostfixOprt, true, "|",   f1of1)
      PARSER_THROWCHECK(PostfixOprt, true, "++",  f1of1)
      PARSER_THROWCHECK(PostfixOprt, true, "--",  f1of1)
      PARSER_THROWCHECK(PostfixOprt, true, "?>",  f1of1)
      PARSER_THROWCHECK(PostfixOprt, true, "?<",  f1of1)
      PARSER_THROWCHECK(PostfixOprt, true, "**",  f1of1)
      PARSER_THROWCHECK(PostfixOprt, true, "xor", f1of1)
      PARSER_THROWCHECK(PostfixOprt, true, "and", f1of1)
      PARSER_THROWCHECK(PostfixOprt, true, "or",  f1of1)
      PARSER_THROWCHECK(PostfixOprt, true, "not", f1of1)
      PARSER_THROWCHECK(PostfixOprt, true, "!",   f1of1)
      // Binary operator
      // The following must fail with builtin operators activated
      // p.EnableBuiltInOp(true); -> this is the default
      PARSER_THROWCHECK(Oprt, false, "+",  f1of2)
      PARSER_THROWCHECK(Oprt, false, "-",  f1of2)
      PARSER_THROWCHECK(Oprt, false, "*",  f1of2)
      PARSER_THROWCHECK(Oprt, false, "/",  f1of2)
      // without activated built in operators it should work
      p.EnableBuiltInOprt(false);
      PARSER_THROWCHECK(Oprt, true, "+",  f1of2)
      PARSER_THROWCHECK(Oprt, true, "-",  f1of2)
      PARSER_THROWCHECK(Oprt, true, "*",  f1of2)
      PARSER_THROWCHECK(Oprt, true, "/",  f1of2)
  #undef PARSER_THROWCHECK

      if (iStat==0) 
        *m_stream << "passed" << endl;
      else 
        *m_stream << "\n  failed with " << iStat << " errors" << endl;

      return iStat;
    }

    //---------------------------------------------------------------------------
    int ParserTester::TestSyntax()
    {
      int iStat = 0;
      *m_stream << "testing syntax engine...";

      iStat += EqnTest("(1+ 2*a)", 3, true);   // Spaces within formula
      iStat += EqnTest("(2+", 0, false);       // missing closing bracket 
      iStat += EqnTest("2++4", 0, false);      // unexpected operator
      iStat += EqnTest("2+-4", 0, false);      // unexpected operator
      iStat += EqnTest("(2+)", 0, false);      // unexpected closing bracket
      iStat += EqnTest("--2", 0, false);       // double sign
      iStat += EqnTest("ksdfj", 0, false);     // unknown token
      iStat += EqnTest("()", 0, false);        // empty bracket
      iStat += EqnTest("sin(cos)", 0, false);  // unexpected function
      iStat += EqnTest("5t6", 0, false);       // unknown token
      iStat += EqnTest("5 t 6", 0, false);     // unknown token
      iStat += EqnTest("8*", 0, false);        // unexpected end of formula
      iStat += EqnTest(",3", 0, false);        // unexpected comma
      iStat += EqnTest("3,5", 0, false);       // unexpected comma
      iStat += EqnTest("sin(8,8)", 0, false);  // too many function args
      iStat += EqnTest("(7,8)", 0, false);     // too many function args
      iStat += EqnTest("sin)", 0, false);      // unexpected closing bracket
      iStat += EqnTest("a)", 0, false);        // unexpected closing bracket
      iStat += EqnTest("pi)", 0, false);       // unexpected closing bracket
      iStat += EqnTest("sin(())", 0, false);   // unexpected closing bracket
      iStat += EqnTest("sin()", 0, false);     // unexpected closing bracket

      if (iStat==0)
        *m_stream << "passed" << endl;
      else 
        *m_stream << "\n  failed with " << iStat << " errors" << endl;

      return iStat;
    }

    //---------------------------------------------------------------------------
    int ParserTester::TestVarConst()
    {
      int iStat = 0;
      *m_stream << "testing variable/constant name recognition...";

      // distinguish constants with same basename
      iStat += EqnTest("const", 1, true);
      iStat += EqnTest("const1", 2, true);
      iStat += EqnTest("const2", 3, true);
      iStat += EqnTest("2*const", 2, true);
      iStat += EqnTest("2*const1", 4, true);
      iStat += EqnTest("2*const2", 6, true);
      iStat += EqnTest("2*const+1", 3, true);
      iStat += EqnTest("2*const1+1", 5, true);
      iStat += EqnTest("2*const2+1", 7, true);
      iStat += EqnTest("const", 0, false);
      iStat += EqnTest("const1", 0, false);
      iStat += EqnTest("const2", 0, false);

      // distinguish variables with same basename
      iStat += EqnTest("a", 1, true);
      iStat += EqnTest("aa", 2, true);
      iStat += EqnTest("2*a", 2, true);
      iStat += EqnTest("2*aa", 4, true);
      iStat += EqnTest("2*a-1", 1, true);
      iStat += EqnTest("2*aa-1", 3, true);

      // Finally test querying of used variables
      try
      {
        int idx;
        mu::Parser p;
        mu::value_type vVarVal[] = { 1, 2, 3, 4, 5};
        p.DefineVar("a", &vVarVal[0]);
        p.DefineVar("b", &vVarVal[1]);
        p.DefineVar("c", &vVarVal[2]);
        p.DefineVar("d", &vVarVal[3]);
        p.DefineVar("e", &vVarVal[4]);

        // Test lookup of defined variables
        // 4 used variables
        p.SetExpr("a+b+c+d");
        mu::varmap_type UsedVar = p.GetUsedVar();
        int iCount = (int)UsedVar.size();
        if (iCount!=4) throw false;

        mu::varmap_type::const_iterator item = UsedVar.begin();
        for (idx=0; item!=UsedVar.end(); ++item)
        {
          if (&vVarVal[idx++]!=item->second) 
            throw false;
        }

        // Test lookup of undefined variables
        p.SetExpr("undef1+undef2+undef3");
        UsedVar = p.GetUsedVar();
        iCount = (int)UsedVar.size();
        if (iCount!=3) throw false;

        for (item = UsedVar.begin(); item!=UsedVar.end(); ++item)
        {
          if (item->second!=0) 
            throw false; // all pointers to undefined variables must be null
        }

        // 1 used variables
        p.SetExpr("a+b");
        UsedVar = p.GetUsedVar();
        iCount = (int)UsedVar.size();
        if (iCount!=2) throw false;
        item = UsedVar.begin();
        for (idx=0; item!=UsedVar.end(); ++item)
          if (&vVarVal[idx++]!=item->second) throw false;

      }
      catch(...)
      {
        iStat += 1;
      }

      if (iStat==0)  
        *m_stream << "passed" << endl;
      else
        *m_stream << "\n  failed with " << iStat << " errors" << endl;

      return iStat;
    }

    //---------------------------------------------------------------------------
    int ParserTester::TestMultiArg()
    {
      int iStat = 0;
      *m_stream << "testing multiarg functions...";
    
      // picking the right argument
      iStat += EqnTest("f1of1(1)", 1, true);
      iStat += EqnTest("f1of2(1, 2)", 1, true);
      iStat += EqnTest("f2of2(1, 2)", 2, true);
      iStat += EqnTest("f1of3(1, 2, 3)", 1, true);
      iStat += EqnTest("f2of3(1, 2, 3)", 2, true);
      iStat += EqnTest("f3of3(1, 2, 3)", 3, true);
      iStat += EqnTest("f1of4(1, 2, 3, 4)", 1, true);
      iStat += EqnTest("f2of4(1, 2, 3, 4)", 2, true);
      iStat += EqnTest("f3of4(1, 2, 3, 4)", 3, true);
      iStat += EqnTest("f4of4(1, 2, 3, 4)", 4, true);
      iStat += EqnTest("f1of5(1, 2, 3, 4, 5)", 1, true);
      iStat += EqnTest("f2of5(1, 2, 3, 4, 5)", 2, true);
      iStat += EqnTest("f3of5(1, 2, 3, 4, 5)", 3, true);
      iStat += EqnTest("f4of5(1, 2, 3, 4, 5)", 4, true);
      iStat += EqnTest("f5of5(1, 2, 3, 4, 5)", 5, true);
      // Too few arguments / Too many arguments
      iStat += EqnTest("f1of1(1,2)", 0, false);
      iStat += EqnTest("f1of1()", 0, false);
      iStat += EqnTest("f1of2(1, 2, 3)", 0, false);
      iStat += EqnTest("f1of2(1)", 0, false);
      iStat += EqnTest("f1of3(1, 2, 3, 4)", 0, false);
      iStat += EqnTest("f1of3(1)", 0, false);
      iStat += EqnTest("f1of4(1, 2, 3, 4, 5)", 0, false);
      iStat += EqnTest("f1of4(1)", 0, false);
      iStat += EqnTest("(1,2,3)", 0, false);
      iStat += EqnTest("1,2,3", 0, false);
      iStat += EqnTest("(1*a,2,3)", 0, false);
      iStat += EqnTest("1,2*a,3", 0, false);
     
      // correct calculation of arguments
      iStat += EqnTest("min(a, 1)",  1, true);
      iStat += EqnTest("min(3*2, 1)",  1, true);
      iStat += EqnTest("min(3*2, 1)",  6, false);
      // correct calculation of arguments
      iStat += EqnTest("min(3*a+1, 1)",  1, true);
      iStat += EqnTest("max(3*a+1, 1)",  4, true);
      iStat += EqnTest("max(3*a+1, 1)*2",  8, true);
      iStat += EqnTest("2*max(3*a+1, 1)+2",  10, true);

      // functions with Variable argument count
      iStat += EqnTest("sum(1,2,3)",  6, true);
      iStat += EqnTest("2*sum(1,2,3)",  12, true);
      iStat += EqnTest("2*sum(1,2,3)+2",  14, true);
      iStat += EqnTest("2*sum(-1,2,3)+2",  10, true);
      iStat += EqnTest("2*sum(-1,2,-(-a))+2",  6, true);
      iStat += EqnTest("2*sum(-1,10,-a)+2",  18, true);
      iStat += EqnTest("2*sum(1,2,3)*2",  24, true);
      iStat += EqnTest("sum(1,-max(1,2),3)*2",  4, true);
      iStat += EqnTest("sum(1*3, 4, a+2)",  10, true);
      iStat += EqnTest("sum(1*3, 2*sum(1,2,2), a+2)",  16, true);
      iStat += EqnTest("sum(1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,2)", 24, true);

      // some failures
      iStat += EqnTest("sum()",  0, false);
      iStat += EqnTest("sum(,)",  0, false);
      iStat += EqnTest("sum(1,2,)",  0, false);
      iStat += EqnTest("sum(,1,2)",  0, false);

      if (iStat==0) 
        *m_stream << "passed" << endl;
      else
        *m_stream << "\n  failed with " << iStat << " errors" << endl;
  
      return iStat;
    }


    //---------------------------------------------------------------------------
    int ParserTester::TestInfixOprt()
    {
      int iStat = 0;
      *m_stream << "testing infix operators...";

      iStat += EqnTest("-1",    -1, true);
      iStat += EqnTest("-(-1)",  1, true);
      iStat += EqnTest("-(-1)*2",  2, true);
      iStat += EqnTest("-(-2)*sqrt(4)",  4, true);
      iStat += EqnTest("-a",  -1, true);
      iStat += EqnTest("-(a)",  -1, true);
      iStat += EqnTest("-(-a)",  1, true);
      iStat += EqnTest("-(-a)*2",  2, true);
      iStat += EqnTest("-(8)", -8, true);
      iStat += EqnTest("-8", -8, true);
      iStat += EqnTest("-(2+1)", -3, true);
      iStat += EqnTest("-(f1of1(1+2*3)+1*2)", -9, true);
      iStat += EqnTest("-(-f1of1(1+2*3)+1*2)", 5, true);
      iStat += EqnTest("-sin(8)", -0.989358, true);
      iStat += EqnTest("3-(-a)", 4, true);
      iStat += EqnTest("3--a", 4, true);
  
      // Postfix / infix priorities
      iStat += EqnTest("~2#", 8, true);
      iStat += EqnTest("~f1of1(2)#", 8, true);
      iStat += EqnTest("~(b)#", 8, true);
      iStat += EqnTest("(~b)#", 12, true);
      iStat += EqnTest("~(2#)", 8, true);
      iStat += EqnTest("~(f1of1(2)#)", 8, true);
      //
      iStat += EqnTest("-2^2",-4, true);
      iStat += EqnTest("-(a+b)^2",-9, true);
      iStat += EqnTest("(-3)^2",9, true);
      iStat += EqnTest("-(-2^2)",4, true);
      iStat += EqnTest("3+-3^2",-6, true);
      // The following assumes use of sqr as postfix operator ("?") together
      // tiwth a sign operator of low priority:
      iStat += EqnTest("-2?", -4, true);
      iStat += EqnTest("-(1+1)?",-4, true);
      iStat += EqnTest("2+-(1+1)?",-2, true);
      iStat += EqnTest("2+-2?", -2, true);
      // This is the classic behaviour of the infix sign operator (here: "$") which is
      // now deprecated:
      iStat += EqnTest("$2^2",4, true);
      iStat += EqnTest("$(a+b)^2",9, true);
      iStat += EqnTest("($3)^2",9, true);
      iStat += EqnTest("$($2^2)",-4, true);
      iStat += EqnTest("3+$3^2",12, true);

      if (iStat==0)
        *m_stream << "passed" << endl;
      else
        *m_stream << "\n  failed with " << iStat << " errors" << endl;

      return iStat;
    }


    //---------------------------------------------------------------------------
    int ParserTester::TestPostFix()
    {
      int iStat = 0;
      *m_stream << "testing postfix operators...";

      // application
      iStat += EqnTest("3m+5", 5.003, true);
      iStat += EqnTest("1000m", 1, true);
      iStat += EqnTest("1000 m", 1, true);
      iStat += EqnTest("(a)m", 1e-3, true);
      iStat += EqnTest("-(a)m", -1e-3, true);
      iStat += EqnTest("-2m", -2e-3, true);
      iStat += EqnTest("f1of1(1000)m", 1, true);
      iStat += EqnTest("-f1of1(1000)m", -1, true);
      iStat += EqnTest("-f1of1(-1000)m", 1, true);
      iStat += EqnTest("f4of4(0,0,0,1000)m", 1, true);
      iStat += EqnTest("2+(a*1000)m", 3, true);
      // some incorrect results
      iStat += EqnTest("1000m", 0.1, false);
      iStat += EqnTest("(a)m", 2, false);
      // failure due to syntax checking
      iStat += EqnTest("a m", 0, false);
      iStat += EqnTest("4 + m", 0, false);
      iStat += EqnTest("m4", 0, false);
      iStat += EqnTest("sin(m)", 0, false);
      iStat += EqnTest("m m", 0, false);
      iStat += EqnTest("m(8)", 0, false);
      iStat += EqnTest("4,m", 0, false);
      iStat += EqnTest("-m", 0, false);
      iStat += EqnTest("2(-m)", 0, false);
      iStat += EqnTest("2(m)", 0, false);

      if (iStat==0)
        *m_stream << "passed" << endl;
      else
        *m_stream << "\n  failed with " << iStat << " errors" << endl;

      return iStat;
    }

    //---------------------------------------------------------------------------
    /** \brief Test volatile (nonoptimizeable functions). */
    int ParserTester::TestVolatile()
    {
      int iStat = 0;
      *m_stream << "testing volatile/nonvolatile functions...";

      // First test with volatile flag turned on
      try
      {

        mu::Parser p;
        p.DefineFun("rnd", Rnd, false);
        p.DefineFun("valueof", RndWithString, false);

        // 1st test, compare results from sucessive calculations
        p.SetExpr("3+rnd(8)");
        if (p.Eval()==p.Eval()) iStat += 1;

        // 2nd test, force bytecode creation, compare two results both 
        // calculated from bytecode
        p.SetExpr("3+rnd(8)"); 
        p.Eval(); //<- Force bytecode creation
        if (p.Eval()==p.Eval()) iStat += 1;

        p.SetExpr("3*rnd(8)+3");
        p.Eval(); //<- Force bytecode creation
        if (p.Eval()==p.Eval()) iStat += 1;

        p.SetExpr("10+3*sin(rnd(8))-1");
        p.Eval(); //<- Force bytecode creation
        if (p.Eval()==p.Eval()) iStat += 1;

        p.SetExpr("3+rnd(rnd(8))*2");
        p.Eval(); //<- Force bytecode creation
        if (p.Eval()==p.Eval()) iStat += 1;

        p.SetExpr("valueof(\"Das ist ein Test\")");
        p.Eval(); //<- Force bytecode creation
        if (p.Eval()==p.Eval()) iStat += 1;
      }
      catch(Parser::exception_type &e)
      {
        *m_stream << "\n  " << e.GetExpr() << " : " << e.GetMsg();
        iStat += 1;
      }

      // Second test with volatile flag turned off
      try
      {
        mu::Parser p;
        p.DefineFun("rnd", Rnd);
        p.DefineFun("valueof", RndWithString);

        // compare string parsing with bytecode
        p.SetExpr("3+rnd(8)"); 
        if (p.Eval()!=p.Eval()) iStat += 1;

        p.SetExpr("3+rnd(8)"); 
        p.Eval(); //<- Force bytecode creation
        if (p.Eval()!=p.Eval()) iStat += 1;

        p.SetExpr("3*rnd(8)+3");
        p.Eval(); //<- Force bytecode creation
        if (p.Eval()!=p.Eval()) iStat += 1;

        p.SetExpr("10+3*sin(rnd(8))-1");
        p.Eval(); //<- Force bytecode creation
        if (p.Eval()!=p.Eval()) iStat += 1;

        p.SetExpr("3+rnd(rnd(8))*2");
        p.Eval(); //<- Force bytecode creation
        if (p.Eval()!=p.Eval()) iStat += 1;

        p.SetExpr("valueof(\"Das ist ein Test\")");
        p.Eval(); //<- Force bytecode creation
        if (p.Eval()!=p.Eval()) iStat += 1;
      }
      catch(Parser::exception_type &e)
      {
        *m_stream << "\n  " << e.GetExpr() << " : " << e.GetMsg();
        iStat += 1;
      }
      
      if (iStat==0)
        *m_stream << "passed" << endl;
      else
        *m_stream << "\n  failed with " << iStat << " errors" << endl;

      return iStat;
    }

    //---------------------------------------------------------------------------
    int ParserTester::TestFormula()
    {
      int iStat = 0;
      *m_stream << "testing sample formulas...";

      // operator precedencs
      iStat += EqnTest("1+2-3*4/5^6", 2.99923, true);
      iStat += EqnTest("1^2/3*4-5+6", 2.3333, true);
      iStat += EqnTest("1+2*3", 7, true);
      iStat += EqnTest("1+2*3", 7, true);
      iStat += EqnTest("(1+2)*3", 9, true);
      iStat += EqnTest("(1+2)*(-3)", -9, true);
      iStat += EqnTest("2/4", 0.5, true);

      iStat += EqnTest("exp(ln(7))", 7, true);
      iStat += EqnTest("e^ln(7)", 7, true);
      iStat += EqnTest("e^(ln(7))", 7, true);
      iStat += EqnTest("(e^(ln(7)))", 7, true);
      iStat += EqnTest("1-(e^(ln(7)))", -6, true);
      iStat += EqnTest("2*(e^(ln(7)))", 14, true);
      iStat += EqnTest("10^log(5)", 5, true);
      iStat += EqnTest("10^log10(5)", 5, true);
      iStat += EqnTest("2^log2(4)", 4, true);
      iStat += EqnTest("-(sin(0)+1)", -1, true);
      iStat += EqnTest("-(2^1.1)", -2.14354692, true);

      iStat += EqnTest("(cos(2.41)/b)", -0.372056, true);

      // long formula (Reference: Matlab)
      iStat += EqnTest(
        "(((-9))-e/(((((((pi-(((-7)+(-3)/4/e))))/(((-5))-2)-((pi+(-0))*(sqrt((e+e))*(-8))*(((-pi)+(-pi)-(-9)*(6*5))"
        "/(-e)-e))/2)/((((sqrt(2/(-e)+6)-(4-2))+((5/(-2))/(1*(-pi)+3))/8)*pi*((pi/((-2)/(-6)*1*(-1))*(-6)+(-e)))))/"
        "((e+(-2)+(-e)*((((-3)*9+(-e)))+(-9)))))))-((((e-7+(((5/pi-(3/1+pi)))))/e)/(-5))/(sqrt((((((1+(-7))))+((((-"
        "e)*(-e)))-8))*(-5)/((-e)))*(-6)-((((((-2)-(-9)-(-e)-1)/3))))/(sqrt((8+(e-((-6))+(9*(-9))))*(((3+2-8))*(7+6"
        "+(-5))+((0/(-e)*(-pi))+7)))+(((((-e)/e/e)+((-6)*5)*e+(3+(-5)/pi))))+pi))/sqrt((((9))+((((pi))-8+2))+pi))/e"
        "*4)*((-5)/(((-pi))*(sqrt(e)))))-(((((((-e)*(e)-pi))/4+(pi)*(-9)))))))+(-pi)", -12.23016549, true);

      // long formula (Reference: Matlab)
      iStat += EqnTest("1+2-3*4/5^6*(2*(1-5+(3*7^9)*(4+6*7-3)))+12", -7995810.09926, true);
	  
      // long formula (Reference: Matlab)
      iStat += EqnTest(
          "(atan(sin((((((((((((((((pi/cos((a/((((0.53-b)-pi)*e)/b))))+2.51)+a)-0.54)/0.98)+b)*b)+e)/a)+b)+a)+b)+pi)/e"
          ")+a)))*2.77)", -2.16995656, true);
 	    
      if (iStat==0) 
        *m_stream << "passed" << endl;  
      else 
        *m_stream << "\n  failed with " << iStat << " errors" << endl;

      return iStat;
    }


    //---------------------------------------------------------------------------
    int ParserTester::TestException()
    {
      int  iStat = 0;
      *m_stream << "testing error codes...";
    
      iStat += ThrowTest("3+",        ecUNEXPECTED_EOF);
      iStat += ThrowTest("3+)",       ecUNEXPECTED_PARENS);
      iStat += ThrowTest("sin(3,4)",  ecTOO_MANY_PARAMS);
      iStat += ThrowTest("3,4",       ecUNEXPECTED_COMMA);
      iStat += ThrowTest("if(3)",     ecTOO_FEW_PARAMS);
      iStat += ThrowTest("(1+2",      ecMISSING_PARENS);
      iStat += ThrowTest("sin(3)3",   ecUNEXPECTED_VAL);
      iStat += ThrowTest("sin(3)xyz", ecUNASSIGNABLE_TOKEN);
      iStat += ThrowTest("sin(3)cos(3)", ecUNEXPECTED_FUN);

      // String function related
      iStat += ThrowTest("valueof(\"xxx\")",  999, false);
      iStat += ThrowTest("valueof()",          ecUNEXPECTED_PARENS);
      iStat += ThrowTest("1+valueof(\"abc\"",  ecMISSING_PARENS);
      iStat += ThrowTest("valueof(\"abc\"",    ecMISSING_PARENS);
      iStat += ThrowTest("valueof(\"abc",      ecUNTERMINATED_STRING);
      iStat += ThrowTest("valueof(\"abc\",3)", ecUNEXPECTED_COMMA);
      iStat += ThrowTest("valueof(3)",         ecSTRING_EXPECTED);
      iStat += ThrowTest("sin(\"abc\")",       ecVAL_EXPECTED);
      iStat += ThrowTest("valueof(\"\\\"abc\\\"\")",  999, false);
      iStat += ThrowTest("\"hello world\"",    ecSTR_RESULT);
      iStat += ThrowTest("(\"hello world\")",  ecSTR_RESULT);
      iStat += ThrowTest("\"abcd\"+100",       ecOPRT_TYPE_CONFLICT);
      iStat += ThrowTest("\"a\"+\"b\"",        ecOPRT_TYPE_CONFLICT);

      // assignement operator
      iStat += ThrowTest("3=4", ecUNEXPECTED_OPERATOR);
      iStat += ThrowTest("sin(8)=4", ecUNEXPECTED_OPERATOR);
      iStat += ThrowTest("\"test\"=a", ecUNEXPECTED_OPERATOR);
      iStat += ThrowTest("sin=9", ecUNEXPECTED_OPERATOR);
      iStat += ThrowTest("(8)=5", ecUNEXPECTED_OPERATOR);
      iStat += ThrowTest("(a)=5", ecUNEXPECTED_OPERATOR);
      iStat += ThrowTest("a=\"tttt\"", ecOPRT_TYPE_CONFLICT);

      if (iStat==0) 
        *m_stream << "passed" << endl;
      else 
         *m_stream << "\n  failed with " << iStat << " errors" << endl;

      return iStat;
    }


    //---------------------------------------------------------------------------
    void ParserTester::AddTest(testfun_type a_pFun)
    {
      m_vTestFun.push_back(a_pFun);
    }

    //---------------------------------------------------------------------------
    /** \brief Set the stream that takes the output of the test session. */
    void ParserTester::SetStream(std::ostream *a_stream)
    {
      assert(a_stream);
      m_stream = a_stream;
    }

    //---------------------------------------------------------------------------
    void ParserTester::Run()
    {
      int iStat = 0;
      try
      {
        for (int i=0; i<(int)m_vTestFun.size(); ++i)
          iStat += (this->*m_vTestFun[i])();
      }
      catch(Parser::exception_type &e)
      {
        *m_stream << "\n" << e.GetMsg() << endl;
        *m_stream << e.GetToken() << endl;
        Abort();
      }
      catch(std::exception &e)
      {
        *m_stream << e.what() << endl;
        Abort();
      }
      catch(...)
      {
        *m_stream << "Internal error";
        Abort();
      }

      if (iStat==0) 
      {
        *m_stream << "Test passed (" <<  ParserTester::c_iCount << " expressions)" << endl;
      }
      else 
      {
        *m_stream << "Test failed with " << iStat 
                  << " errors (" <<  ParserTester::c_iCount 
                  << " expressions)" << endl;
      }
      ParserTester::c_iCount = 0;
    }


    //---------------------------------------------------------------------------
    int ParserTester::ThrowTest(const std::string &a_str, int a_iErrc, bool a_bFail)
    {
      ParserTester::c_iCount++;

      try
      {
        double fVal=0;
        Parser p;

        p.DefineVar("a", &fVal);
        p.DefineFun("valueof", ValueOf);
        p.SetExpr(a_str);
        p.Eval();
      }
      catch(Parser::exception_type &e)
      {
        // output the formula in case of an failed test
        if (a_bFail==true && a_iErrc!=e.GetCode() )
        {
          cout << "\n  " 
               << "Expression: " << a_str 
               << "  Code:" << e.GetCode() 
               << "  Expected:" << a_iErrc;
        }

        return (a_iErrc==e.GetCode()) ? 0 : 1;
      }

      // if a_bFail==false no exception is expected
      return (a_bFail==false) ? 0 : 1; 
    }

    //---------------------------------------------------------------------------
    /** \brief Evaluate a tet expression. 

        \return 1 in case of a failure, 0 otherwise.
    */
    int ParserTester::EqnTest(const std::string &a_str, double a_fRes, bool a_fPass)
    {
      ParserTester::c_iCount++;
      int iRet(0);

      try
      {
        Parser *p1, p2, p3;   // three parser objects
                              // they will be used for testing copy and assihnment operators
        // p1 is a pointer since i'm going to delete it in order to test if
        // parsers after copy construction still refer to members of it.
        // !! If this is the case this function will crash !!
      
        p1 = new mu::Parser(); 
        // Add constants
        p1->DefineConst("pi", (value_type)PARSER_CONST_PI);
        p1->DefineConst("e", (value_type)PARSER_CONST_E);
        p1->DefineConst("const", 1);
        p1->DefineConst("const1", 2);
        p1->DefineConst("const2", 3);
        // variables
        value_type vVarVal[] = { 1, 2, 3, -2};
        p1->DefineVar("a", &vVarVal[0]);
        p1->DefineVar("aa", &vVarVal[1]);
        p1->DefineVar("b", &vVarVal[1]);
        p1->DefineVar("c", &vVarVal[2]);
        p1->DefineVar("d", &vVarVal[3]);
        // functions
        p1->DefineFun("f1of1", f1of1);  // one parameter
        p1->DefineFun("f1of2", f1of2);  // two parameter
        p1->DefineFun("f2of2", f2of2);
        p1->DefineFun("f1of3", f1of3);  // three parameter
        p1->DefineFun("f2of3", f2of3);
        p1->DefineFun("f3of3", f3of3);
        p1->DefineFun("f1of4", f1of4);  // four parameter
        p1->DefineFun("f2of4", f2of4);
        p1->DefineFun("f3of4", f3of4);
        p1->DefineFun("f4of4", f4of4);
        p1->DefineFun("f1of5", f1of5);  // five parameter
        p1->DefineFun("f2of5", f2of5);
        p1->DefineFun("f3of5", f3of5);
        p1->DefineFun("f4of5", f4of5);
        p1->DefineFun("f5of5", f5of5);
        // sample functions
        p1->DefineFun("min", Min);
        p1->DefineFun("max", Max);
        p1->DefineFun("sum", Sum);
        p1->DefineFun("valueof", ValueOf);
        p1->DefineFun("atof", StrToFloat);

        // infix / postfix operator
        // (identifiers used here do not have any meaning or make any sense at all)
        p1->DefineInfixOprt("$", sign, prPOW+1);  // sign with high priority
        p1->DefineInfixOprt("~", plus2);          // high priority
        p1->DefinePostfixOprt("m", Milli);
        p1->DefinePostfixOprt("#", times3);
        p1->DefinePostfixOprt("?", sqr);  //
        p1->SetExpr(a_str);

        // Test bytecode integrity
        // String parsing and bytecode parsing must yield the same result
        value_type fVal[4] = {-999, -998, -997, -996}; // initially should be different
        fVal[0] = p1->Eval(); // result from stringparsing
        fVal[1] = p1->Eval(); // result from bytecode
        if (fVal[0]!=fVal[1])
          throw Parser::exception_type("Bytecode corrupt.");

        // Test copy and assignement operators
        try
        {
          // Test copy constructor
          std::vector<mu::Parser> vParser;
          vParser.push_back(*p1);
          mu::Parser p2 = vParser[0];   // take parser from vector
        
          // destroy the originals from p2
          vParser.clear();              // delete the vector
          delete p1;                    // delete the original
          p1 = 0;

          fVal[2] = p2.Eval();

          // Test assignement operator
          // additionally  disable Optimizer this time
          mu::Parser p3;
          p3 = p2;
          p3.EnableOptimizer(false);
          fVal[3] = p3.Eval();
        }
        catch(exception &e)
        {
          *m_stream << "\n  " << e.what() << "\n";
        }

        // limited floating point accuracy requires the following test
        bool bCloseEnough(true);
        for (int i=0; i<4; ++i)
        {
          bCloseEnough &= (fabs(a_fRes-fVal[i]) <= fabs(fVal[i]*0.0001));
        }

        iRet = ((bCloseEnough && a_fPass) || (!bCloseEnough && !a_fPass)) ? 0 : 1;
        if (iRet==1)
        {
          *m_stream << "\n  fail: " << a_str.c_str() 
                    << " (incorrect result; expected: " << a_fRes
                    << " ;calculated: " << fVal[0]<< ").";
        }
      }
      catch(Parser::exception_type &e)
      {
        if (a_fPass)
        {
         *m_stream << "\n  fail: " << a_str.c_str() << " (" << e.GetMsg() << ")";
          return 1;
        }
      }
      catch(std::exception &e)
      {
        *m_stream << "\n  fail: " << a_str.c_str() << " (" << e.what() << ")";
         return 1;  // always return a failure since this exception is not expected
      }
      catch(...)
      {
       *m_stream << "\n  fail: " << a_str.c_str() <<  " (unexpected exception)";
        return 1;  // exceptions other than ParserException are not allowed
      }

      return iRet;
    }

    //---------------------------------------------------------------------------
    int ParserTester::EqnTestInt(const std::string &a_str, double a_fRes, bool a_fPass)
    {
      ParserTester::c_iCount++;

      value_type vVarVal[] = {1, 2, 3};    // variable values
      value_type fVal[2] = {-99, -999}; // results: initially should be different
      int iRet(0);

      try
      {
        ParserInt p;
        p.DefineConst("const1", 1);
        p.DefineConst("const2", 2);
        p.DefineVar("a", &vVarVal[0]);
        p.DefineVar("b", &vVarVal[1]);
        p.DefineVar("c", &vVarVal[2]);

        p.SetExpr(a_str);
        fVal[0] = p.Eval(); // result from stringparsing
        fVal[1] = p.Eval(); // result from bytecode

        if (fVal[0]!=fVal[1])
          throw Parser::exception_type("Bytecode corrupt.");

        iRet =  ( (a_fRes==fVal[0] &&  a_fPass) || 
                  (a_fRes!=fVal[0] && !a_fPass) ) ? 0 : 1;
        if (iRet==1)
        {
          *m_stream << "\n  fail: " << a_str.c_str() 
                    << " (incorrect result; expected: " << a_fRes 
                    << " ;calculated: " << fVal[0]<< ").";
        }
      }
      catch(Parser::exception_type &e)
      {
        if (a_fPass)
          *m_stream << "\n  " << e.GetExpr() << " : " << e.GetMsg();
      }
      catch(...)
      {
       *m_stream << "\n  fail: " << a_str.c_str() <<  " (unexpected exception)";
        return 1;  // exceptions other than ParserException are not allowed
      }

      return iRet;
    }

    //---------------------------------------------------------------------------
    /** \brief Internal error in test class Test is going to be aborted. */
    void ParserTester::Abort() const
    {
      *m_stream << "Test failed (internal error in test class)" << endl;
      while (!getchar());
      exit(-1);
    }
  } // namespace test
} // namespace mu



