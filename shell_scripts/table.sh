b=$1; shift
d=$1; shift
case $1 in
  ReadWpt)    regex='Reading waypoints';;
  QtSort)     regex='Sorting waypoints';;
  NmpSearch)  regex='Searching';;
  NmpMerged)  regex='merged';;
  LabelCon)   regex='unconnected';;
  RteInt)     regex='integrity';;
  ReadList)   regex='Processing traveler';;
  ConcAug)    regex='Augmenting';;
  CompStatsO) regex='Computing stats';;			# old
  CompStatsB) regex='per.route|Writing highway';;	# new both
  CompStatsR) regex='per.route';;			# new 1st
  CompStatsT) regex='Computing stats per traveler';;	# new 2nd
  UserLog)    regex='per-traveler';;
  StatsCsv)   regex='stats csv|Reading datacheckfps.csv';;
  HGVertex)   regex='unique';;
  Graphs)     regex='master TM|Clearing HighwayGraph contents from memory';;
  Subgraph)   regex='continent graphs|Clearing HighwayGraph contents from memory';;
  Total)      regex='Reading region, country, and continent descriptions|Total';;
  *)          regex=$1;;
esac; shift
execs=$@

pipe=`echo "$regex" | grep -c '|'`

# header lines
echo -e "$execs\t" | tr ' ' '\t' | tr -d '_'
for e in $execs; do
  echo -en "-------\t"
done
echo

# loop thru threads
T=$(expr $(echo $d/*$b-*.log | tr ' ' '\n' | sed "s~$d/~~" | cut -f3 -d- | sed -e 's~t.*~~' -e 's~^.$~0&~' | sort | uniq | tail -n 1) + 0)
t=$(expr $(echo $d/*$b-*.log | tr ' ' '\n' | sed "s~$d/~~" | cut -f3 -d- | sed -e 's~t.*~~' -e 's~^.$~0&~' | sort | uniq | head -n 1) + 0)
for t in `seq $t $T`; do
  # loop thru execs
  for exec in $execs; do
    if [ $exec = '""' ]; then exec=""; fi
    pas=`ls $d/siteupdate$exec-$b-*.log | egrep "$d/siteupdate$exec-$b-"$t"t([0-9]+)p.log" | wc -l`
    TaskTotal=0
    # loop thru passes & get average
    for p in `seq $pas`; do
      file=$d/siteupdate$exec-$b-"$t"t"$p"p.log
      if [ $pipe -gt 0 ]; then
        begend=`cat $file | egrep '\[[0-9]+\.[0-9]+\]' | egrep "$regex" | sed -r 's~.*\[([0-9]+\.[0-9]+)\].*~\1~'`
        task=$(echo "scale=4; $(echo $begend | cut -f2 -d' ') - $(echo $begend | cut -f1 -d' ')" | bc)
      else task=$(echo 0-$(egrep '\[[0-9]+\.[0-9]+\]' $file | egrep -A 1 -m 1 "$regex" | sed -r 's~.*\[([0-9]+\.[0-9]+)\].*~\1~' | tr '\n' +)0 | bc)
      fi
      TaskTotal=$(echo $TaskTotal + $task | bc)
    done
    if [ $pas = 0 ]
      then echo -en '!\t'
      else echo -en `echo "scale=4; $TaskTotal / $pas" | bc`'\t'
    fi
  done
  echo
done
