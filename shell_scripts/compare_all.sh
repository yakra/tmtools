#!/usr/bin/env bash

if [[ $# < 2 ]]; then
  echo "Please specify two versions to be compared."
  exit
fi
v1=$1
v2=$2
e=`echo -en '\e'`

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
  for file in nearmisspoints.log nmpfpsunmatched.log tm-master.nmp unprocessedwpts.log waypointsimplification.log; do
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
