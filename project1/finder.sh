#!/bin/bash
echo Number of TLDs you want to find in the output files are: $#
echo The TLDs are: $*
for i in $* 
    do
    count=0
    tmp="."$i
    for file  in ./output_files/*.out #.out files have to be stored in the same directory as this script
        do
        [[ ! -e $file ]] && continue  # check if file exists
        num=$(grep -n $tmp $file | cut -d: -f1) #find the line(s) that holds the info for the tld
        for l in $num 
            do 
            x=$(awk "NR==${l}" $file)
            k=$(echo $x | awk '{print $2}')
            count=$((count+k))
            done
        done
    echo total number of appearances of TLD: $i = $count
    done
