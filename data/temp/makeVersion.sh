version=$1
delta=${version##*.[CD]}
# strip any leading 0s from delta
delta=`echo $delta|sed -e 's/^0*//'`
branch=${version%%.*}
AlphaPlot_version=$[$branch*65536 + $delta]
rm -f libAlphaPlot/src/version.cpp
cat >libAlphaPlot/src/version.cpp <<EOF
#include "globals.h"
const int AlphaPlot::AlphaPlot_versionNo = $AlphaPlot_version;
const char* AlphaPlot::AlphaPlot_version = "$version";
const char * AlphaPlot::release_date = "`date +"%b %d, %Y"`";
EOF

# also check that all translation files have been included
for i in AlphaPlot/translations/*.ts; do
    if aels -terse $i |grep -- \--- >/dev/null; then
        echo "translation $i not checked in"
        exit 1
        fi
done

# update Doxyversion
rm -f Doxyversion
echo "PROJECT_NUMBER=$version" >Doxyversion
