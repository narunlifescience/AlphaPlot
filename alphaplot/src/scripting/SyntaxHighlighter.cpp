/***************************************************************************
        File                 : SyntaxHighlighter.cpp
        Project              : AlphaPlot
--------------------------------------------------------------------
        Copyright            : (C) 2016 by Arun Narayanankutty
        Email                : n.arun.lifescience@gmail.com
        Description          : Highlight scripts & interpreter text

 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *  This program is free software; you can redistribute it and/or modify   *
 *  it under the terms of the GNU General Public License as published by   *
 *  the Free Software Foundation; under version 2 of the License.          *
 *                                                                         *
 *  This program is distributed in the hope that it will be useful,        *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of         *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the          *
 *  GNU General Public License for more details.                           *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the Free Software           *
 *   Foundation, Inc., 51 Franklin Street, Fifth Floor,                    *
 *   Boston, MA  02110-1301  USA                                           *
 *                                                                         *
 ***************************************************************************/

#include "SyntaxHighlighter.h"

SyntaxHighlighter::SyntaxHighlighter(QTextDocument *parent)
    : QSyntaxHighlighter(parent) {
  HighlightingRule rule;

  keywordFormat.setForeground(QColor(249, 38, 114));
  keywordFormat.setFontWeight(QFont::Normal);

  keywords << "break"
           << "case"
           << "catch"
           << "continue"
           << "debugger"
           << "default"
           << "delete"
           << "do"
           << "else"
           << "finally"
           << "for"
           << "function"
           << "if"
           << "in"
           << "instanceof"
           << "new"
           << "returm"
           << "switch"
           << "throw"
           << "try"
           << "typeof"
           << "var"
           << "while"
           << "with";

  uselessKeywords << "class"
                  << "const"
                  << "enum"
                  << "export"
                  << "extends"
                  << "import"
                  << "super";

  foreach (const QString &pattern, keywords) {
    rule.pattern = QRegExp("\\b" + pattern + "\\b");
    rule.format = keywordFormat;
    highlightingRules.append(rule);
  }

  classFormat.setForeground(Qt::darkMagenta);
  rule.pattern = QRegExp("\\bMath\\b");
  rule.format = classFormat;
  highlightingRules.append(rule);

  QTextCharFormat trueFalseFormat;
  trueFalseFormat.setForeground(QColor(174, 129, 255));
  rule.pattern = QRegExp("\\b(true|false|this)\\b");
  rule.format = trueFalseFormat;
  highlightingRules.append(rule);

  quotationFormat.setForeground(Qt::darkGreen);
  rule.pattern = QRegExp("\".*\"");
  rule.format = quotationFormat;
  highlightingRules.append(rule);

  singleQuotationFormat.setForeground(Qt::darkGreen);
  rule.pattern = QRegExp("'.*'");
  rule.format = singleQuotationFormat;
  highlightingRules.append(rule);

  functionFormat.setFontItalic(true);
  functionFormat.setForeground(QColor(204, 140, 91));

  // Exclude keywords misinterpretation as functions
  QString keywordExcludePatterns;
  foreach (const QString &pattern, keywords) {
    (pattern == keywords[0])
        ? keywordExcludePatterns.append(pattern)
        : keywordExcludePatterns.append(QString("|%1").arg(pattern));
  }

  rule.pattern = QRegExp(
      QString("\\b(?!(%1)+[\\s]*[/]?(?=\\())([A-Za-z0-9_]+[\\s]*[/]?(?=\\())")
          .arg(keywordExcludePatterns));
  rule.format = functionFormat;
  highlightingRules.append(rule);

  singleLineCommentFormat.setForeground(QColor(128, 128, 128));
  rule.pattern = QRegExp("//[^\n]*");
  rule.format = singleLineCommentFormat;
  highlightingRules.append(rule);

  multiLineCommentFormat.setForeground(QColor(128, 128, 128));

  commentStartExpression = QRegExp("/\\*");
  commentEndExpression = QRegExp("\\*/");
}

void SyntaxHighlighter::highlightBlock(const QString &text) {
  foreach (const HighlightingRule &rule, highlightingRules) {
    QRegExp expression(rule.pattern);
    int index = expression.indexIn(text);
    while (index >= 0) {
      int length = expression.matchedLength();
      setFormat(index, length, rule.format);
      index = expression.indexIn(text, index + length);
    }
  }
  setCurrentBlockState(0);

  int startIndex = 0;
  if (previousBlockState() != 1)
    startIndex = commentStartExpression.indexIn(text);

  while (startIndex >= 0) {
    int endIndex = commentEndExpression.indexIn(text, startIndex);
    int commentLength;
    if (endIndex == -1) {
      setCurrentBlockState(1);
      commentLength = text.length() - startIndex;
    } else {
      commentLength =
          endIndex - startIndex + commentEndExpression.matchedLength();
    }
    setFormat(startIndex, commentLength, multiLineCommentFormat);
    startIndex =
        commentStartExpression.indexIn(text, startIndex + commentLength);
  }
}
