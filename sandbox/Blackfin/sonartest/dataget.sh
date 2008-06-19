echo "Waiting for data..."

nc -l -p 9000 > /tmp/data.txt

cat /tmp/data.txt  |grep A |tr "ABCD" "    " > /tmp/dataA.txt
cat /tmp/data.txt  |grep B |tr "ABCD" "    " > /tmp/dataB.txt
cat /tmp/data.txt  |grep C |tr "ABCD" "    " > /tmp/dataC.txt
cat /tmp/data.txt  |grep D |tr "ABCD" "    " > /tmp/dataD.txt
echo "Done"
