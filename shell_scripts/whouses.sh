if [ $# -lt 2 ]; then
  echo "USAGE: whouses.sh <root> <label> [<label>...]"
  exit 0
fi

root=$1
shift
labels=$@

# find region & route for root
systems=$(grep -v '^#' HighwayData/systems.csv | egrep 'active$|preview$' | cut -f1 -d';' | sed 's~.*~HighwayData/hwy_data/_systems/&.csv~')
csvline=$(grep -h "$root;" $systems)
rg=$(echo $csvline | cut -f2 -d';')
rte=$(echo $csvline | sed -r 's~.*;.*;(.*);(.*);(.*);.*;.*;.*~\1\2\3~')

# a few more variables to simplify some of the shell commands:
r=$(echo -en "\r")
t=$(echo -en "\t")
d="[ $t]\+"
x="[^ $t]\+"

cd UserData/list_files

# build a list of travelers using any of the specified labels
echo -e "\n\e[1;4mAll travelers with broken .lists:\e[0m"
AllBroken=\
$(for label in $labels; do
    grep -v '^#' * \
    | sed -r -e "s~[ $t]+#.*~~" -e "s~:[ $t]+~:~" -e "s~[ $t$r]+$~~" \
    | grep -i ":$rg$d$rte$d$label$d$x$\|:$rg$d$rte$d$x$d$label$\|:$rg$d$rte$d$label$d$x$d$x$d$x$\|:$x$d$x$d$x$d$rg$d$rte$d$label$"
  done | cut -f1 -d. | sort | uniq)
echo $AllBroken

echo -en "\nOpen in Firefox? (y/n) "
read go
if [ $go == y ]; then
  for u in $AllBroken; do
   echo https://travelmapping.net/hb/showroute.php?r=$root\&u=$u
  done | xargs firefox
fi
if [ $go == n ]; then
  echo -en "Display URLs? (y/n) "
  read go
  if [ $go == y ]; then
    for u in $AllBroken; do
     echo https://travelmapping.net/hb/showroute.php?r=$root\&u=$u
    done
  fi
fi

# who updates using GitHub? Which files have commits by someone other than Jim?
echo -e "\n\e[1;4mThose who update via GitHub (TM usernames):\e[0m"

# compile list
GitHubURLs=\
$(for u in $AllBroken; do
    echo -en "$u\t"
    echo \
    $(git log $u.list \
      | grep -iv Teresco \
      | grep -B 1 -m 1 'Author:' \
      | head -n 1 | cut -f2 -d' ')
  done \
  | egrep "$t[0-9a-f]{40}$" \
  | sed -r "s~(.*)$t(.*)~https://github.com/TravelMapping/UserData/commits/\2/list_files/\1.list~")

cd - > /dev/null


# TM usernames
GitHub=$(for URL in $GitHubURLs; do echo $URL | sed -r 's~.*/(.*).list~\1~'; done)
echo $GitHub

# GitHub usernames
echo -e "\n\e[1;4mGitHub usernames:\e[0m"
for URL in $GitHubURLs; do
  list=$(echo $URL | cut -f9 -d/)
echo -n @\
$(curl -qsLJ $URL \
  | grep -m 1 'data-hovercard-url="/users/' \
  | sed -r 's~.*data-hovercard-url="/users/~~' \
  | cut -f1 -d/)' '
done
echo

# email
echo -e "\n\e[1;4mThose left over who update by email:\e[0m"
diff <(echo $AllBroken | tr ' ' '\n') <(echo $GitHub | tr ' ' '\n') | grep '<' | sed 's~< ~~' | tr '\n' ' '
echo; echo
