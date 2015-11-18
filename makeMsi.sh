# Build an MSI installer from a compiled SciDaVis
# Must be run from the toplevel directory of SciDaVis
PATH=$PATH:/c/cygwin/bin
productId=`uuidgen`
componentId=`uuidgen`
version=`grep scidavis_version libscidavis/src/version.cpp|tail -1|cut -f5 -d' '|tr -d '";'`
if [ $version = '"unknown"' ]; then
    version=0.0.0.0
fi
scidavisWxs=`pwd`/scidavis.wxs
msiVersion=`echo $version|tr -d D`

# determine release or beta depending of whether a D appears in the second field
upgradeId=d6f4ef98-3744-47a2-b581-d789db8a4d63
productName=SciDaVis

echo $version

cat >$scidavisWxs <<EOF
<?xml version='1.0' encoding='windows-1252'?>
<Wix xmlns='http://schemas.microsoft.com/wix/2006/wi'>
<Product Name='$productName' Id='$productId' UpgradeCode='$upgradeId'
    Language='1033' Codepage='1252' Version='$msiVersion' Manufacturer='High Performance Coders'>
    <Package Id='*' Keywords='Installer' Description="SciDaVis Installer"
      Comments='SciDaVis is licensed under GPL2' Manufacturer='High Performance Coders'
      InstallerVersion='100' Languages='1033' Compressed='yes' SummaryCodepage='1252' />
    <Upgrade Id='$upgradeId'>
      <UpgradeVersion OnlyDetect='no' Property='PREVIOUSFOUND'
         Minimum='0.0.0' IncludeMinimum='yes'
         Maximum='2.0.0' IncludeMaximum='no' />
    </Upgrade>
    <InstallExecuteSequence>
      <RemoveExistingProducts Before='InstallInitialize'/>
    </InstallExecuteSequence>
    <Media Id='1' Cabinet='scidavis.cab' EmbedCab='yes'/>
    <Directory Id='TARGETDIR' Name='SourceDir'>
      <Directory Id='ProgramFilesFolder'>
        <Directory Id='SciDaVis' Name='$productName'>
          <Directory Id='INSTALLDIR'>
            <Component Id='SciDaVisFiles' Guid='$componentId'>
              <File Id='SciDaVisEXE' Name='scidavis.exe' Source='scidavis/scidavis.exe' KeyPath='yes'>
                <Shortcut Id="startmenuSciDaVis" Directory="ProgramMenuDir" Name="$productName" WorkingDirectory='INSTALLDIR' Icon="scidavis.exe" IconIndex="0" Advertise="yes">
                 </Shortcut>
                <Shortcut Id="desktopScidavis" Directory="DesktopFolder" Name="$productName" WorkingDirectory='INSTALLDIR' Icon="scidavis.exe" IconIndex="0" Advertise="yes" />
              </File>
              <ProgId Id='SciPrj' Description='SciDaVis Project File' Icon='SciDaVisEXE'>
                <Extension Id='sciprj' ContentType='application/scidavis'>
                  <Verb Id='open' Command='Open' TargetFile='SciDaVisEXE' Argument='"%1"'/>
                 </Extension>
               </ProgId>
EOF
pushd scidavis
id=0
fid=0
# add in plain files
for i in appicons.qrc icons.qrc scidavis.rc scidavisrc.pyc scidavisUtil.pyc; do
	if [ ! -d $i ]; then
	    let fid++
	    cat >>$scidavisWxs <<EOF
	<File Id='fid$fid' Source='scidavis/$i' Name='$i' KeyPath='no' />
EOF
        fi
done
echo "    </Component>">> $scidavisWxs
builddir ()
{
    dir=${1##*/}
    pushd $dir
    let d++
    echo "<Directory Id='id$d' Name='$dir'>" >>$scidavisWxs
    # first process files into a single component
    j=0
    for i in *; do 
	if [ -f $i ]; then
	    let fid++
            if [ $j -eq 0 ]; then
                let id++
                echo "<Component Id='id$id' Guid='`uuidgen`'>">>$scidavisWxs
 	        echo "<File Id='fid$fid' Source='scidavis/$1/$i' Name='$i' KeyPath='yes' />">>$scidavisWxs
           else
	        echo "<File Id='fid$fid' Source='scidavis/$1/$i' Name='$i' KeyPath='no' />">>$scidavisWxs
            fi
            let j++
	fi
    done
    if [ $j -gt 0 ]; then
        echo "</Component>">>$scidavisWxs
    fi

    # then process subdirectories 
    for i in *; do 
	if [ -d $i ]; then
	    builddir $1/$i
        fi
    done
    echo "</Directory>">>$scidavisWxs
    popd
}

builddir translations
builddir icons
popd

cat >>$scidavisWxs <<EOF
          </Directory>
        </Directory>
        
      </Directory>

      <Directory Id="ProgramMenuFolder" Name="Programs">
        <Directory Id="ProgramMenuDir" Name="$productName">
          <Component Id="ProgramMenuDir" Guid="40e1115c-9edf-4ae9-b4d3-6508f1921f51">
            <RemoveFolder Id='ProgramMenuDir' On='uninstall' />
            <RegistryValue Root='HKCU' Key='Software\[Manufacturer]\[ProductName]' Type='string' Value='' KeyPath='yes' />
          </Component>
        </Directory>
      </Directory>

      <Directory Id="DesktopFolder" Name="Desktop" />
    </Directory>

    <Feature Id='Complete' Level='1'>
      <ComponentRef Id='SciDaVisFiles' />
      <ComponentRef Id='ProgramMenuDir' />
EOF
i=0
while [ $i -lt $id ]; do
    let i++
    echo "<ComponentRef Id='id$i' />">>$scidavisWxs 
done
cat >>$scidavisWxs <<EOF
    </Feature>

    <Icon Id="scidavis.exe" SourceFile="scidavis/scidavis.exe" />

   <UI>
      <UIRef Id='WixUI_InstallDir'/>
    </UI>
    <Property Id="WIXUI_INSTALLDIR" Value="INSTALLDIR"/>
  </Product>
</Wix>
EOF


candle scidavis.wxs
echo "light scidavis.wixobj"
light -ext WixUIExtension -dWixUILicenseRtf=license.rtf scidavis.wixobj
mv scidavis.msi scidavis.$version-win-dist.msi
