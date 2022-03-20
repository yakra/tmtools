#!/usr/bin/env bash

if [[ $# < 2 ]]; then
  echo "Please specify two versions to be compared."
  exit
fi
v1=$1
v2=$2
e=`echo -en '\e'`

detect () {
  if [ `grep -c -m 1 'Searching for near-miss' $1/logs/siteupdate.log` = 1 ]
  then if [ `grep -c -m 1 'concurrencies\.log' $1/logs/siteupdate.log` = 1 ]
       then echo -en "\e[32mC++ (MT)\e[0m"
       else echo -en "\e[32mC++ (ST)\e[0m"
       fi
  else echo -en "\e[32mPython\e[0m"
  fi
}

echo '<<' `detect $v1` '<< >>' `detect $v2` '>>'

# do stats
  echo -e "comparing stats\e[31m"
  diff -q -r $v1/stats/ $v2/stats/
  echo -en "\e[0m"


# do logs
  # user logs
  if [[ $3 != '-n' ]]; then
    echo "comparing user logs"
    logdiffs=0
    for file in `ls $v1/logs/users`; do
      if [ `diff -q <(tail -n +2 $v1/logs/users/$file) <(tail -n +2 $v2/logs/users/$file) | wc -l` != "0" ]; then
        logdiffs=$(expr $logdiffs  + 1)
        echo -e "\e[31m$file:"
        diff <(tail -n +2 $v1/logs/users/$file) <(tail -n +2 $v2/logs/users/$file) \
	| sed -r -e "s~^[0-9]+,?[0-9]*[acd][0-9]+,?[0-9]*$~$e[4m$e[33m&$e[0m~" \
		 -e "s~^<.*~$e[31m&$e[0m~" \
		 -e "s~^>.*~$e[32m&$e[0m~" \
		 -e "s~^---$~$e[37m---$e[0m~"
        echo -e "__________________________________________________________________________________________________\e[0m\n"
      fi
    done
    echo "diffs found in $logdiffs user logs."
  fi
  # main logs sans creation timestamp
  echo -e "comparing main logs sans creation timestamp\e[31m"
  for file in flippedroutes.log nearmisspoints.log nmpfpsunmatched.log tm-master.nmp unprocessedwpts.log waypointsimplification.log; do
    diff -q $v1/logs/$file $v2/logs/$file
  done
  echo -en "\e[0m"
  # main logs with creation timestamp
  echo "comparing main logs with creation timestamp"
  for file in datacheck.log highwaydatastats.log listnamesinuse.log nearmatchfps.log pointsinuse.log unmatchedfps.log unusedaltlabels.log unusedaltroutenames.log; do
    echo -en "\e[2m$file\e[0m\t"
    result=`diff -q <(tail -n +2 $v1/logs/$file) <(tail -n +2 $v2/logs/$file)`
    if [[ $result = '' ]]; then
      echo -e "\e[32mOK!\e[0m"
    else
      echo -e "\e[31mNope!\e[0m"
    fi
  done
  #concurrencies.log
  echo -e "comparing concurrencies.log\e[31m"
  diff -q <(tail -n +2 $v1/logs/concurrencies.log | grep -v augment) <(tail -n +2 $v2/logs/concurrencies.log | grep -v augment)
  diff -q <(grep augment $v1/logs/concurrencies.log | sed 's~ based on .*$~~' | sort) <(grep augment $v2/logs/concurrencies.log | sed 's~ based on .*$~~' | sort)
  echo -en "\e[0m"


# do graphs
  # create
  rm -rf $v1/graphs.can; mkdir -p $v1/graphs.can
  rm -rf $v2/graphs.can; mkdir -p $v2/graphs.can
  echo "creating canonical $v1 graphs"
  ../../canonicaltmg/canonicaltmg "$v1/graphs" "$v1/graphs.can"
  echo "creating canonical $v2 graphs"
  ../../canonicaltmg/canonicaltmg "$v2/graphs" "$v2/graphs.can"
  # diff
  echo -e "comparing graphs                                 \e[31m"
  diff -q -r $v1/graphs.can/ $v2/graphs.can/
  echo -en "\e[0m"


# do nmp_merged
  echo -e "comparing near-miss point merged wpt files\e[31m"
  diff -q -r $v1/nmp_merged/ $v2/nmp_merged/
  echo -en "\e[0m"

# do sql files
  std () {
    echo -e "\e[31m$(diff -q	<(tail -n +$beg1 $v1/TravelMapping.sql | head -n $len1) \
				<(tail -n +$beg2 $v2/TravelMapping.sql | head -n $len2) )\e[0m"
  }
  sorted () {
    echo -en "\e[2mSorting... \e[0m"
    echo -e "\e[31m$(diff -q	<(tail -n +$beg1 $v1/TravelMapping.sql | head -n $len1 | sort) \
				<(tail -n +$beg2 $v2/TravelMapping.sql | head -n $len2 | sort) )\e[0m"
  }
  trunc () {
    dig=$1
    echo -en "\e[2mTruncating to $dig decimal places... \e[0m"
    echo -e "\e[31m$(diff -q	<(tail -n +$beg1 $v1/TravelMapping.sql | head -n $len1 | sed -r "s~(\.[0-9]{$dig})[0-9]*~\1~") \
				<(tail -n +$beg2 $v2/TravelMapping.sql | head -n $len2 | sed -r "s~(\.[0-9]{$dig})[0-9]*~\1~") )\e[0m"
  }
  pointzero () {
    echo -en "\e[2mIgnoring .0 on integers... \e[0m"
    echo -e "\e[31m$(diff -q	<(tail -n +$beg1 $v1/TravelMapping.sql | head -n $len1 | sed "s~\.0'~'~") \
				<(tail -n +$beg2 $v2/TravelMapping.sql | head -n $len2 | sed "s~\.0'~'~") )\e[0m"
  }
  multi () {
    dig=$1
    echo -en "\e[2mmulti($dig)... \e[0m"
    echo -e "\e[31m$(diff -q	<(tail -n +$beg1 $v1/TravelMapping.sql | head -n $len1 | sed -r -e 's~^,~~' -e "s~\.0'~'~" -e "s~(\.[0-9]{$dig})[0-9]*~\1~g" | sort) \
				<(tail -n +$beg2 $v2/TravelMapping.sql | head -n $len2 | sed -r -e 's~^,~~' -e "s~\.0'~'~" -e "s~(\.[0-9]{$dig})[0-9]*~\1~g" | sort) )\e[0m"
  }
  insert () {
    dig=$1
    echo -en "\e[2minsert($dig)... \e[0m"
    c=$(diff -q	<(tail -n +$beg1 $v1/TravelMapping.sql | head -n $len1 | grep -v "^INSERT INTO $table1 VALUES$\|^;$" | sed -r -e 's~^,~~' -e "s~\.0'~'~" -e "s~(\.[0-9]{$dig})[0-9]*~\1~g" | sort) \
		<(tail -n +$beg2 $v2/TravelMapping.sql | head -n $len2 | grep -v "^INSERT INTO $table2 VALUES$\|^;$" | sed -r -e 's~^,~~' -e "s~\.0'~'~" -e "s~(\.[0-9]{$dig})[0-9]*~\1~g" | sort) )
    echo -e "\e[31m$c\e[0m"
    if [ "$c" != '' ]; then
      echo -e "\n\e[1;4mTry this incantation:\e[0m"
      echo "diff \\"
      echo "<(tail -n +$beg1 $v1/TravelMapping.sql | head -n $len1 | grep -v \"^INSERT INTO $table1 VALUES$\|^;$\" | sed -r -e 's~^,~~' -e \"s~\.0'~'~\" -e \"s~(\.[0-9]{$dig})[0-9]*~\1~g\" | sort) \\"
      echo "<(tail -n +$beg2 $v2/TravelMapping.sql | head -n $len2 | grep -v \"^INSERT INTO $table2 VALUES$\|^;$\" | sed -r -e 's~^,~~' -e \"s~\.0'~'~\" -e \"s~(\.[0-9]{$dig})[0-9]*~\1~g\" | sort) \\"
      echo -e "| less -r\n"
    fi
  }
  nocommas () {
    echo -en "\e[2mSorting and ignoring commas... \e[0m"
    echo -e "\e[31m$(diff -q	<(tail -n +$beg1 $v1/TravelMapping.sql | head -n $len1 | sed 's~^,~~' | sort) \
				<(tail -n +$beg2 $v2/TravelMapping.sql | head -n $len2 | sed 's~^,~~' | sort) )\e[0m"
  }
  echo -e "comparing .sql files"
  nums1=`grep -n 'CREATE TABLE' $v1/TravelMapping.sql | cut -f1 -d:`
  nums2=`grep -n 'CREATE TABLE' $v2/TravelMapping.sql | cut -f1 -d:`
  wc1=$(expr `cat $v1/TravelMapping.sql | wc -l` + 1)
  wc2=$(expr `cat $v2/TravelMapping.sql | wc -l` + 1)

  echo "    BEG1     LEN1                          TABLE     BEG2     LEN2"
  echo "-------- -------- ------------------------------ -------- --------"
  for item in $(paste -d- <(echo -e "$nums1\n$wc1") <(echo -e "1\n$nums1") <(echo -e "$nums2\n$wc2") <(echo -e "1\n$nums2")); do
    beg1=`echo $item | cut -f2 -d-`
    beg2=`echo $item | cut -f4 -d-`
    len1=`echo $item | sed -r 's~-[0-9]+-[0-9]+$~~' | bc`
    len2=`echo $item | sed -r 's~^[0-9]+-[0-9]+-~~' | bc`
    table1=$(tail -n +$beg1 $v1/TravelMapping.sql | head -n 1 | sed -e 's~ (.*~~' -e 's~CREATE TABLE ~~' -e 's~\(DROP TABLE IF EXISTS\).*~\1...~')
    table2=$(tail -n +$beg2 $v2/TravelMapping.sql | head -n 1 | sed -e 's~ (.*~~' -e 's~CREATE TABLE ~~' -e 's~\(DROP TABLE IF EXISTS\).*~\1...~')
    if [ "$table1" != "$table2" ]; then
      echo -e "\e[31m< $beg1 $len1 $table1"
      echo -e "> $beg2 $len2 $table2\e[0m"
      exit 1
    fi

    printf "%8i %8i %30s %8i %8i " $beg1 $len1 "$table1" $beg2 $len2

    case "$table1" in
      'DROP TABLE IF EXISTS...') std;;
      'continents')			std;;
      'countries')			std;;
      'regions')			nocommas;;
      'systems')			std;;
      'routes')				trunc 10;;
      'connectedRoutes')		trunc 7;;
      'connectedRouteRoots')		std;;
      'waypoints')			pointzero;;
      'segments')			std;;
      'clinched')			nocommas;;
      'overallMileageByRegion')		multi 8;;
      'systemMileageByRegion')		multi 8;;
      'clinchedOverallMileageByRegion')	multi 7;;
      'clinchedSystemMileageByRegion')	multi 7;;
      'clinchedConnectedRoutes')	insert 7;;
      'clinchedRoutes')			insert 7;;
      'updates')			std;;
      'systemUpdates')			std;;
      'datacheckErrors')		std;;
      'graphTypes')			sorted;;
      'graphs')				sorted;;
        *) echo -en "\e[2munsupported; attempting std... \e[0m"; std;;
    esac
  done
