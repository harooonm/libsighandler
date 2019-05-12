BUILD_TYPE=Debug

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
cd libbtree && cmake -DCMAKE_BUILD_TYPE="$BUILD_TYPE" . && make &&
cd ../liblinkedlist && cmake -DCMAKE_BUILD_TYPE="$BUILD_TYPE" . && make && cd "$CUR_DIR"

cp libbtree/include/*.h include && cp liblinkedlist/include/*.h include
mkdir lib && cp libbtree/libbtree.so lib/  && cp liblinkedlist/liblinkedlist.so lib/

export LD_LIBRARY_PATH="$LD_LIBRARY_PATH:$CUR_DIR"
cmake -DCMAKE_BUILD_TYPE="$BUILD_TYPE" . && make
rm -rf libbtree/
rm -rf liblinkedlist/
