cd Rmultinet
rm -r src
./prepare.sh
cd ..
mv Rmultinet/C++ .
mv Rmultinet/prepare.sh .
R CMD build Rmultinet
R CMD check --as-cran multinet_1.1.3.tar.gz
mv C++ Rmultinet
mv prepare.sh Rmultinet

