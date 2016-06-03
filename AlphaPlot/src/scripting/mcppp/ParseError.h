/* This file is part of AlphaPlot.
   Copyright 2016, Arun Narayanankutty <n.arun.lifescience@gmail.com>
   Copyright 2004-2011 Ingo Berg (taken from muparser)

   AlphaPlot is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.
   AlphaPlot is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   You should have received a copy of the GNU General Public License
   along with AlphaPlot.  If not, see <http://www.gnu.org/licenses/>.

   Description : Error handling of math cpp parser */

namespace Mcppp {

enum class ErrCodes : int {
  // Formula syntax errors
  ErrUnexpectedOperator = 0,  // Unexpected binary operator found
  ErrUnknownTocken = 1,       // Token cant be identified.
  ErrUnexpectedEof = 2,       // Unexpected end of formula. ("2+sin(")
  ErrUnexpectedArgSep = 3,    // An unexpected comma has been found. ("1,23")
  ErrUnexpectedArg = 4,       // An unexpected argument has been found
  ErrUnexpectedVal = 5,       // An unexpected value token has been found
  ErrUnexpectedVar = 6,       // An unexpected variable token has been found
  ErrUnexpectedParens = 7,    // Unexpected Parenthesis, opening or closing
  ErrUnexpectedStr = 8,       // string found at inapropriate position
  ErrStrExpected = 9,         // string function called with other arg type
  ErrValExpected = 10,        // numeric function called with non value arg
  ErrMissingParens = 11,      // Missing parens. ("3*sin(3")
  ErrUnexpectedFunc = 12,     // Unexpected function.("sin(8)cos(9)")
  ErrUnterminatedStr = 13,    // string.not terminated ("valueof("hello)")
  ErrTooManyArgs = 14,        // Too many function parameters
  ErrTooFewArgs = 15,         // Too few function parameters. ("ite(1<2,2)")
  ErrOprTypeConflict = 16,    // binary oper only apply to same value types
  ErrStrResult = 17,          // result is a string

  // Invalid Parser input Parameters
  ErrInvalidName = 18,        // Invalid function, variable or constant name
  ErrInvalidBinopVar = 19,    // Invalid binary operator identifier
  ErrInvalidInfixVar = 20,    // Invalid function, variable or constant name
  ErrInvalidPostfixVar = 21,  // Invalid function, variable or constant name

  ErrBuiltinOverload = 22,  // Trying to overload builtin operator
  ErrInvalidFuncPtr = 23,   // Invalid callback function pointer
  ErrInvalidVarPtr = 24,    // Invalid variable pointer
  ErrEmptyExpression = 25,  // The Expression is empty
  ErrNameConflict = 26,     // Name conflict
  ErrOprtPriority = 27,     // Invalid operator priority

  // Generic/locale error
  ErrGeneric = 28,  // Generic error
  ErrLocale = 29,   // Conflict with current locale

  ErrConditional = 30,     // unexpected conditional statement
  ErrMissingElse = 31,     // Missing else statement conditional
  ErrMisplacedColon = 32,  // Misplaced semicolon

  ErrNumComputations = 33,  // Unresonable number of computations

  // internal errors
  ErrInternal = 34,  // Internal error of any kind

  // The last two are special entries
  ErrCount,          // Stores total num err codes
  ErrUndefined = -1  // Undefined msg, detect unassigned err msg

};
}  // Mcppp namespace
