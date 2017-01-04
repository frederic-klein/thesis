#!/bin/bash

DISCOVERY="/tmp/discovery"
touch $DISCOVERY

echo "" > $DISCOVERY

NODES=50
ADVERSARIES=0
CPULIMIT=100

make -B "n=$NODES" "a=$ADVERSARIES" tester

MAC_SUFFIX=":55:66:77:88"

for i in `seq 1 $NODES`; do
	FIRST=$(($i/100))
	SECOND=$(($i-FIRST*100))

	if [ "$FIRST" -lt "1" ]; then
		MAC=00
	elif [ "$FIRST" -lt "10" ]; then
		MAC=0$FIRST
	else
		MAC=$FIRST
	fi

	if [ "$SECOND" -lt "1" ]; then
		MAC=$MAC":00"
	elif [ "$SECOND" -lt "10" ]; then
		MAC=$MAC":0"$SECOND
	else
		MAC=$MAC":"$SECOND
	fi
	
	MAC=$MAC$MAC_SUFFIX
	
	rm "/tmp/$MAC"
	
	SCORE=$((($RANDOM)%9000+100))
	
	if [ "$i" -eq "1" ]; then
		echo $MAC" "$SCORE" c" > $DISCOVERY
	else
		echo $MAC" "$SCORE" n" >> $DISCOVERY
	fi

done

while read -r line
do
	COUNTER=1;
	for word in $line
	do
		if [ "$COUNTER" -eq "1" ]; then
			PARAM1=$word
		elif [ "$COUNTER" -eq "2" ]; then
			PARAM2=$word
		else
			PARAM3=$word
		fi
		COUNTER=$(($COUNTER+1))
	done
#    cpulimit --limit=$CPULIMIT -i ./tester $PARAM1 $PARAM2 $PARAM3 &
    ./tester $PARAM1 $PARAM2 $PARAM3 &
done < <(tail -n +2 "$DISCOVERY")



read -r firstline<"$DISCOVERY"
COUNTER=1;
for word in $firstline
do
	if [ "$COUNTER" -eq "1" ]; then
		PARAM1=$word
	elif [ "$COUNTER" -eq "2" ]; then
		PARAM2=$word
	else
		PARAM3=$word
	fi
	COUNTER=$(($COUNTER+1))
done
#cpulimit --limit=$CPULIMIT -i ./tester $PARAM1 $PARAM2 $PARAM3 
./tester $PARAM1 $PARAM2 $PARAM3 

pkill tester


