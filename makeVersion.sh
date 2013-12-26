version=$1
delta=${version##*.[CD]}
branch=${version%%.*}
scidavis_version=$[$branch*65536 + $delta]
cat >scidavis/src/version.cpp <<EOF
#include "globals.h"
const int SciDAVis::scidavis_versionNo = $scidavis_version;
const char* SciDAVis::scidavis_version = "$version";
const char * SciDAVis::release_date = "`date +"%b %d, %Y"`";
EOF

