#!/usr/bin/env bash
if   [ -d "$HOME/TravelMapping" ]; then tmdir=$HOME/TravelMapping
elif [ -d "$HOME/travelmapping" ]; then tmdir=$HOME/travelmapping
elif [ -d "$HOME/tm" ];            then tmdir=$HOME/tm
else                                    tmdir=../../..
fi
host=$HOSTNAME
MinThreads=1
MaxThreads=`nproc`
mtvertices=''
passes=10
execs=''
regex=''
Python=''
abort=0
nmpflag='on by default'
graphflag='on by default'
errorflag=''
timeflag='-T 2'

# process commandline args
while  [ $# -gt 0 ]; do
  case $1 in
    --noisy) noisy=$2; shift;;
    -d) tmdir=$2;      shift;;
    -e) errorflag='-e';;
    -h) host=$2;       shift;;
    -k) graphflag='-k';;
    -n) nmpflag='';;
    -p) passes=$2;     shift;;
    -P) Python=$2;     shift; timeflag='';;
    -r) regex=$2;      shift;;
    -t) MinThreads=$2; shift;;
    -T) MaxThreads=$2; shift;;
    -v) mtvertices='-v';;
    *)  execs="$execs $1";;
  esac
  shift
done
if [ "$execs" == '' ]; then execs='""'; fi

# sanity checks
if [ "$MaxThreads" == '' ]; then
  echo "WARNING: MaxThreads not specified; setting equal to MinThreads"
    MaxThreads=$MinThreads
elif [ "$MinThreads" -gt "$MaxThreads" ]; then
  echo "ERROR: MinThreads > MaxThreads"
    abort=1
fi
for e in $execs; do
  if [ $e == '""' ]; then e=""; fi
  if [ "$Python" != '' ]; then
    if [ ! -e siteupdate$e.py ]; then
      echo "ERROR: siteupdate$e.py not found"
      abort=1
    fi
  else
    if [ ! -x siteupdate$e ]; then
      echo "ERROR: siteupdate$e not executable"
      abort=1
    fi
  fi
done
if [ $abort == 1 ]; then
  exit 1
fi

# regex presets
case $regex in
  ReadWpt)    regex='Reading waypoints|Quadtree';;
  QtSort)     regex='Sorting waypoints|Finding unprocessed';;
  NmpSearch)  regex='Searching|Near-miss|merged';;
  NmpMerged)  regex='merged|Concurrent';;
  LabelCon)   regex='unconnected|Reading updates file';;
  ReadList)   regex='Processing traveler|traveler list files\.';;
  ConcAug)    regex='Augmenting|concurrencies.log|Computing stats';;
  CompStatsO) regex='Computing stats|Writing highway';;				# old
  CompStatsB) regex='Computing stats per route|Writing highway';;		# new both
  CompStatsR) regex='Computing stats per route|Computing stats per traveler';;	# new 1st
  CompStatsT) regex='Computing stats per traveler|Writing highway';;		# new 2nd
  UserLog)    regex='per-traveler|Writing stats';;
  Subgraph)   regex='continent graphs|Marking';;
esac
# old regexes from before commandlines & presets
  #regex='Reading systems list|Checking for duplicate'
  #regex="Writing to|Computing stats"
  #regex="Writing stats|Reading datacheckfps\.csv"
  #regex="datacheckfps\.csv|Start writing"

start=$(date +%s)
TotalPasses=$(echo "$(echo $execs | wc -w) * ($MaxThreads - $MinThreads + 1) * $passes" | bc)
tpass=1
mkdir -p sulogs
for e in $execs; do
  if [ $e == '""' ]; then e=""; fi
  thr=$MinThreads; while [[ $thr -le $MaxThreads ]]; do
    pass=1
    sum=0
    exec=siteupdate$e
    if [ "$Python" != '' ]; then exec="$exec.py"; fi
    if [ "$nmpflag" != '' ];   then nmpflag="-n ./$e/nmp_merged"; fi
    if [ "$graphflag" != '-k' ]; then graphflag="-g ./$e/graphs"; fi

    # timetable header
    echo "$host: siteupdate$e, $thr thread(s)"

    # individual passes
    while [[ $pass -le $passes ]]; do
      # delete data from previous pass
      rm -f sulogs/siteupdate$e-$host-"$thr"t"$pass"p.log
      rm -f ./$e/TravelMapping.sql
      rm -rf ./$e/logs/;		mkdir -p ./$e/logs/users/
      rm -rf ./$e/nmp_merged/;		mkdir -p ./$e/nmp_merged/
      rm -rf ./$e/stats/;		mkdir -p ./$e/stats/
      rm -rf ./$e/graphs/;		mkdir -p ./$e/graphs/

      # run siteupdate
      $Python ./$exec $timeflag -t $thr $nmpflag $graphflag $errorflag $mtvertices \
        -l ./$e/logs \
        -c ./$e/stats \
        -d ./$e/TravelMapping \
        -u $tmdir/UserData/list_files \
        -w $tmdir/HighwayData > sulogs/siteupdate$e-$host-"$thr"t"$pass"p.log

      # timetable row
      runtime=$(tac sulogs/siteupdate$e-$host-"$thr"t"$pass"p.log \
        | egrep -m 1 '\[([0-9]+\.[0-9]+)\]' \
        | sed -r 's~.*\[([0-9]+\.[0-9]+)\].*~\1~')
      if [ "$regex" != '' ]; then
        times=$(cat sulogs/siteupdate$e-$host-"$thr"t"$pass"p.log \
          | egrep "$regex" \
          | sed -r 's~.*\[([0-9]+\.[0-9]+).*~\1~')
        beg=$(echo $times | cut -f1 -d' ')
        end=$(echo $times | cut -f2 -d' ')
        echo -en "$beg\t$end\t" $(echo $end - $beg | bc)'\t'
        sum=$(echo $sum-$beg+$end | bc)
      else
        sum=$(echo $sum+$runtime | bc)
      fi
      elapsed=$(echo $(date +%s) - $start | bc)
      echo -en $runtime'\teta='
      date --date='@'$(echo "$(date +%s) + $elapsed * ($TotalPasses - $tpass) / $tpass" | bc)

      pass=$(expr $pass + 1)
      tpass=$(expr $tpass + 1)
    done #while pass

    # timetable footer
    if [ "$regex" != '' ]; then
      regXform='"'`echo $regex | sed 's~|~" -> "~' | cut -f1 -d'|'`'"'
    else
      regXform='Total run time'
    fi
    echo -n "siteupdate$e, $thr thread(s), $regXform average: "; echo "scale=4; $sum / $passes" | bc
    echo

    if [ "$noisy" != '' ]; then ssh $noisy cvlc $host.flac vlc://quit 2> /dev/null; fi
    thr=$(expr $thr + 1)
  done #while thr
done #for e
