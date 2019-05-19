BUILD_TYPE=debug

rm -rf libbtree/
rm -rf liblinkedlist/
rm -rf lib

if [ $# -ne 0 ]
then
	BUILD_TYPE=$1
fi

git clone https://github.com/harooonm/libbtree.git
git clone https://github.com/harooonm/liblinkedlist.git

CUR_DIR=$(pwd)
cd libbtree &&  make $BUILD_TYPE &&
cd ../liblinkedlist && make $BUILD_TYPE && cd "$CUR_DIR"

cp libbtree/include/*.h include && cp liblinkedlist/include/*.h include
mkdir lib && cp libbtree/libbtree.so lib/  && cp liblinkedlist/liblinkedlist.so lib/

export LD_LIBRARY_PATH="$LD_LIBRARY_PATH:$CUR_DIR"
make $BUILD_TYPE
rm -rf libbtree/
rm -rf liblinkedlist/
