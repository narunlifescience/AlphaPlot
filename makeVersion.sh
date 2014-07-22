version=$1
delta=${version##*.[CD]}
# strip any leading 0s from delta
delta=`echo $delta|sed -e 's/^0*//'`
branch=${version%%.*}
scidavis_version=$[$branch*65536 + $delta]
rm -f scidavis/src/version.cpp
cat >scidavis/src/version.cpp <<EOF
#include "globals.h"
const int SciDAVis::scidavis_versionNo = $scidavis_version;
const char* SciDAVis::scidavis_version = "$version";
const char * SciDAVis::release_date = "`date +"%b %d, %Y"`";
EOF

# also check that all translation files have been included
for i in scidavis/translations/*.ts; do
    if aels -terse $i |grep -- \--- >/dev/null; then
        echo "translation $i not checked in"
        exit 1
        fi
done

# update Doxyversion
rm -f Doxyversion
echo "PROJECT_NUMBER=$version" >Doxyversion
