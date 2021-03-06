#/bin/bash
bak=~+
cd $PWD

OurOS="linux_defaut"
if [[ "$OSTYPE" == "darwin"* ]]; then
 OurOS="macos"
elif [[ "$OSTYPE" == "linux-gnu" ]]; then
 OurOS="linux"
elif [[ "$OSTYPE" == "freebsd"* ]]; then
 OurOS="freebsd"
fi

echo $OurOS

#=======================================================================
errorofbuid()
{
	printf "\n\n=========ERROR!!===========\n\n"
	cd $bak
	exit 1
}

checkState()
{
	if [ $? -eq 0 ]
	then
	  echo "[good]"
	else
	  errorofbuid
	fi
}

buildLibs()
{
# build libraries
cd "$PrjPath/_Libs/_sources"
InstallTo=$(echo ~+/../_builds/$OurOS)

if [[ "$OurOS" == "macos" ]]; then
   InstallTo=$(readlink $InstallTo)
else
   InstallTo=$(readlink -f $InstallTo)
fi

echo $InstallTo
source ./___build_script.sh
}

PrjPath=$PWD
if [ -f "$PWD/_paths.sh" ]
then
	source "$PWD/_paths.sh"
else
	echo ""
	echo "_paths.sh is not exist! Run \"generate_paths.sh\" first!"
	errorofbuid
fi
	
#=======================================================================

if [[ "$1" == "no-libs" ]]; then
	echo "Building of tag.gz libraries skiped"
	echo "Building SDL2_mixer, OOLua and Box2D..."
else
	buildLibs
fi

cd "$PrjPath/_Libs"

$QMake CONFIG+=release CONFIG-=debug
checkState

#=======================================================================
make
checkState

#=======================================================================
# copy data and configs into the build directory
make install
checkState

cd ..
#=======================================================================
printf "\n\n=========BUILT!!===========\n\n"
cd $bak
read -n 1
exit 0

