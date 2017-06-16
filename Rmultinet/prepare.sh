cd src
cp -r ../../multinet/include/* .
mkdir multinet
cp -r ../../multinet/src/* multinet
find * | grep cpp | sed 's/^/cp /g' > f1
find * | grep cpp | sed 's/\//_/g' > f2
paste -d" " f1 f2 > f3
chmod +x f3
./f3
rm -rf multinet
rm f?
cp ../C++/* .
