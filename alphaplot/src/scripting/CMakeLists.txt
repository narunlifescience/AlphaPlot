set( current_dir "scripting" )

set( HEADERS ${HEADERS}
  "${current_dir}/customevents.h"
  "${current_dir}/MuParserScript.h"
  "${current_dir}/MuParserScripting.h"
  "${current_dir}/MyParser.h"
  "${current_dir}/Script.h"
  "${current_dir}/ScriptEdit.h"
  "${current_dir}/ScriptingEnv.h"
  "${current_dir}/ScriptingFunctions.h"
  "${current_dir}/ScriptingLangDialog.h"
  "${current_dir}/SyntaxHighlighter.h"
  "${current_dir}/widgets/Console.h"
  "${current_dir}/widgets/ConsoleWidget.h"
  PARENT_SCOPE
  )
set( SRCS ${SRCS}
  "${current_dir}/MuParserScript.cpp"
  "${current_dir}/MuParserScripting.cpp"
  "${current_dir}/MyParser.cpp"
  "${current_dir}/Script.cpp"
  "${current_dir}/ScriptEdit.cpp"
  "${current_dir}/ScriptingEnv.cpp"
  "${current_dir}/ScriptingFunctions.cpp"
  "${current_dir}/ScriptingLangDialog.cpp"
  "${current_dir}/SyntaxHighlighter.cpp"
  "${current_dir}/widgets/Console.cpp"
  "${current_dir}/widgets/ConsoleWidget.cpp"
  PARENT_SCOPE
  )
set( FORMS ${FORMS}
  "${current_dir}/widgets/ConsoleWidget.ui"
  PARENT_SCOPE
  )
