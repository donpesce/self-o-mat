#!/bin/sh


clean() {
    # to to build dir
    cd $2

    # clean libs
    rm -rf ./libs

    # clean settings and assets
    rm -rf ./assets
    rm -rf ./settings.json
    rm -rf ./firmware.hex
}

echo "Packing files in $1"

NAME=app
BINARY=self_o_mat
FILES="$BINARY settings.json assets/* libs/* firmware.hex"
TAR=update.tar


clean

cd $2
rm -f $TAR

# get the settings and assets from src dir into the build dir
cp -r $1/assets .
cp $1/settings.json .
cp $1/firmware.hex .

# get libs
if [ ! -d libs ]; then
    mkdir -p libs
fi
ldd $BINARY | grep "=> /" | while read a b c d; do cp "$c" libs/; done

# pack and sign
tar cfz $NAME.tar.gz $FILES
gpg --quiet --batch --yes --output $NAME.sig --detach-sig $NAME.tar.gz
tar cf $TAR $NAME.tar.gz $NAME.sig
rm $NAME.tar.gz $NAME.sig

clean