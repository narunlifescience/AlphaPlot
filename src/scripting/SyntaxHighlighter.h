/***************************************************************************
        File                 : SyntaxHighlighter.h
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

#ifndef SYNTAXHIGHLIGHTER_H
#define SYNTAXHIGHLIGHTER_H

#include <QSyntaxHighlighter>

class SyntaxHighlighter : public QSyntaxHighlighter {
  Q_OBJECT
 public:
  SyntaxHighlighter(QTextDocument *parent = nullptr);

 protected:
  void highlightBlock(const QString &text);

 private:
  struct HighlightingRule {
    QRegExp pattern;
    QTextCharFormat format;
  };

  QVector<HighlightingRule> highlightingRules;

  QRegExp commentStartExpression;
  QRegExp commentEndExpression;

  QTextCharFormat keywordFormat;
  QTextCharFormat classFormat;
  QTextCharFormat singleLineCommentFormat;
  QTextCharFormat multiLineCommentFormat;
  QTextCharFormat quotationFormat;
  QTextCharFormat singleQuotationFormat;
  QTextCharFormat functionFormat;

  QStringList keywords;
  QStringList uselessKeywords;
};

#endif // SYNTAXHIGHLIGHTER_H
