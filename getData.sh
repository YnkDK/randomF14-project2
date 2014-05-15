for i in data1a,data1b data2a,data2b data3a,data3b data4a,data4b data5a,data5b data6a,data6b data7a,data7b; do IFS=","
	set $i
	DATA_A=$1
	DATA_B=$2
	wget "http://www.cs.au.dk/~gudmund/randomF14/multisets/$DATA_A"
	iconv -f windows-1252 -t utf-8 "$DATA_A" > "$DATA_A.tmp"
	mv "$DATA_A.tmp" "$DATA_A"
	
	wget "http://www.cs.au.dk/~gudmund/randomF14/multisets/$DATA_B"
	iconv -f windows-1252 -t utf-8 "$DATA_B" > "$DATA_B.tmp"
	mv "$DATA_B.tmp" "$DATA_B"
done
