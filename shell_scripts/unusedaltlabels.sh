if [ "$1" = "" ]; then
  op=curl
  ualfile="https://travelmapping.net/logs/unusedaltlabels.log"
else
  op=cat
  ualfile="$1"
fi

rg=`pwd | sed s~.*/~~ | tr -d '-' | tr [A-Z] [a-z]`
for ualentry in `$op "$ualfile" | grep "^$rg\." | tr ' ' '%'`; do
  file=*/`echo "$ualentry" | cut -f1 -d'('`.wpt
  uals=`echo "$ualentry" | cut -f2 -d: | tr % '\n'`
  echo $file
  for line in `cat $file | tr ' ' '%'`; do
    label=`echo "$line" | sed 's~%.*~~'`
    url=`echo "$line" | sed 's~.*%~~'`
    echo -n "$label" >> unusedaltlabels.tmp
    for alt in `echo "$line" | sed -e "s~^$label~~" -e "s~$url$~~" | tr '%' ' '`; do
      trim=`echo $alt | tr -d +`
      if [ `echo "$uals" | grep -i "^+*$trim$" | wc -l` == 0 ]; then echo -n " $alt" >> unusedaltlabels.tmp; fi
    done
    echo " $url" >> unusedaltlabels.tmp
  done
  mv unusedaltlabels.tmp $file
done
