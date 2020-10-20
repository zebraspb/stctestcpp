#/bin/bash

prefix=$1

if [ -z $prefix ]; then
prefix='file'
fi

program=testcpp

stringmaxlen=1000
stringcount=10
packetsize=100


cd ./out

echo generate $prefix.data
./$program --generate --outfile $prefix.data --stringmaxlen $stringmaxlen --stringcount $stringcount

echo split $prefix.data to packets with size $packetsize and write it to $prefix.split
./$program --split --infile $prefix.data --packetsize $packetsize --outfile $prefix.split

echo order $prefix.split and write it to $prefix.order
./$program --order --infile $prefix.split --outfile $prefix.order

echo recv $prefix.order and write result to $prefix.recv
./$program --recv --infile $prefix.order  --outfile $prefix.recv

echo diff files
diff $prefix.data $prefix.recv

cd ..

