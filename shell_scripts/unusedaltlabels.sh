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

  num_lines=`cat $file | wc -l`
  if [ "$(head -n $num_lines $file)" == "$(cat $file)" ]
    then term_crlf=1
    else term_crlf=0; num_lines=`expr $num_lines + 1`
  fi

  line_num=0
  for line in `cat $file | tr ' ' '%'`; do
    line_num=`expr $line_num + 1`
    label=`echo "$line" | sed 's~%.*~~'`
    url=`echo "$line" | sed -r 's~.*(%+[^%]+%*$)~\1~'`
    echo -n "$label" >> unusedaltlabels.tmp
    for alt in `echo "$line" | sed -e "s~^$label~~" -e "s~$url$~~" | sed -r 's~(%+)~ \1~g'`; do
      trim=`echo $alt | tr -d +%`
      if [ `echo "$uals" | grep -i "^+*$trim$" | wc -l` == 0 ]; then echo -n "$alt" | tr '%' ' ' >> unusedaltlabels.tmp; fi
    done
    echo -n "$url" | tr '%' ' ' >> unusedaltlabels.tmp
    if [ $line_num != $num_lines ] || [ $term_crlf == 1 ]; then echo >> unusedaltlabels.tmp; fi
  done
  while [ $line_num -lt $num_lines ]; do
    echo >> unusedaltlabels.tmp
    line_num=`expr $line_num + 1`
  done
  mv unusedaltlabels.tmp $file
done
