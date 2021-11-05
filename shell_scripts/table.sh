b=$1; shift
d=$1; shift
case $1 in
  ReadWpt)    regex='Reading waypoints|Quadtree';;
  QtSort)     regex='Sorting waypoints|Finding unprocessed';;
  NmpSearch)  regex='Searching|Near-miss';;
  NmpMerged)  regex='merged|Concurrent';;
  LabelCon)   regex='unconnected|Reading updates file';;
  RteInt)     regex='integrity|Reading updates';;
  ReadList)   regex='Processing traveler|traveler list files\.';;
  ConcAug)    regex='Augmenting|concurrencies.log|Computing stats';;
  CompStatsO) regex='Computing stats|Writing highway';;				# old
  CompStatsB) regex='per.route|Writing highway';;				# new both
  CompStatsR) regex='per.route|Computing stats per traveler';;			# new 1st
  CompStatsT) regex='Computing stats per traveler|Writing highway';;		# new 2nd
  UserLog)    regex='per-traveler|Writing stats';;
  HGVertex)   regex='unique|Creating edges';;
  Subgraph)   regex='Writing continent|Clearing HighwayGraph contents from memory';;
  Total)      regex='Reading region, country, and continent descriptions|Total';;
  *)          regex=$1;;
esac; shift
execs=$@

# header lines
echo "$execs" | tr ' ' '\t' | tr -d '_'
for e in $execs; do
  echo -en "=======\t"
done
echo

# loop thru threads
T=$(expr $(ls $d/*$b-*.log | sed "s~$d/~~" | cut -f3 -d- | sed -e 's~t.*~~' -e 's~^.$~0&~' | sort | uniq | tail -n 1) + 0)
t=$(expr $(ls $d/*$b-*.log | sed "s~$d/~~" | cut -f3 -d- | sed -e 's~t.*~~' -e 's~^.$~0&~' | sort | uniq | head -n 1) + 0)
while [[ $t -le $T ]]; do
  # loop thru execs
  for exec in $execs; do
    if [[ $exec = '""' ]]; then exec=""; fi
    pas=`ls $d/siteupdate$exec-$b-*.log | egrep "$d/siteupdate$exec-$b-"$t"t([0-9]+)p.log" | wc -l`
    TaskTotal=0
    # loop thru passes & get average
    p=1; while [[ $p -le $pas ]]; do
      file=$d/siteupdate$exec-$b-"$t"t"$p"p.log
      begend=`cat $file | egrep '\[[0-9]+\.[0-9]+\]' | egrep "$regex" | sed -r 's~.*\[([0-9]+\.[0-9]+)\].*~\1~'`
      task=$(echo "scale=4; $(echo $begend | cut -f2 -d' ') - $(echo $begend | cut -f1 -d' ')" | bc)
      TaskTotal=$(echo $TaskTotal + $task | bc)
      p=$(expr $p + 1)
    done
    if [ $pas = 0 ]; then
      echo -en '!\t'
    else
      echo -en `echo "scale=4; $TaskTotal / $pas" | bc`'\t'
    fi
  done
  echo
  t=$(expr $t + 1)

done
