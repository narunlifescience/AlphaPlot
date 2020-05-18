set( current_dir "ui" )

set( HEADERS ${HEADERS}
  "${current_dir}/ExportDialog.h"
  "${current_dir}/ExtensibleFileDialog.h"
  "${current_dir}/GeneralApplicationSettings.h"
  "${current_dir}/GeneralAppreanceSettings.h"
  "${current_dir}/PropertiesDialog.h"
  "${current_dir}/RandomDistributionDialog.h"
  "${current_dir}/SettingsPage.h"
  "${current_dir}/SettingsDialog.h"
  PARENT_SCOPE
  )
set( SRCS ${SRCS}
  "${current_dir}/ExportDialog.cpp"
  "${current_dir}/ExtensibleFileDialog.cpp"
  "${current_dir}/GeneralApplicationSettings.cpp"
  "${current_dir}/GeneralAppreanceSettings.cpp"
  "${current_dir}/SettingsPage.cpp"
  "${current_dir}/SettingsDialog.cpp"
  "${current_dir}/PropertiesDialog.cpp"
  "${current_dir}/RandomDistributionDialog.cpp"
  PARENT_SCOPE
  )
set( FORMS ${FORMS}
  "${current_dir}/GeneralApplicationSettings.ui"
  "${current_dir}/GeneralAppreanceSettings.ui"
  "${current_dir}/PropertiesDialog.ui"
  "${current_dir}/RandomDistributionDialog.ui"
  "${current_dir}/SettingsDialog.ui"
  PARENT_SCOPE
  )
