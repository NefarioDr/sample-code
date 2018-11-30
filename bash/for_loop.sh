#!/bin/bash


# for loop syntax
# Numeric ranges for syntax is as follows:

# for VARIABLE in 1 2 3 4 5 .. N
# do
#     command1
#     command2
#     commandN
# done
# OR

# for VARIABLE in file1 file2 file3
# do
#     command1 on $VARIABLE
#     command2
#     commandN
# done
# OR

# for OUTPUT in $(Linux-Or-Unix-Command-Here)
# do
#     command1 on $OUTPUT
#     command2 on $OUTPUT
#     commandN
# done


#BASIC
for i in 1 2 3 4 5
do
   echo "1. Welcome $i times"
done

#Increament by 1, Bash version over 3.0
for i in {1..5}
do
   echo "2. Welcome $i times"
done

# Bash v4.0+ has inbuilt support for setting up a step value using {START..END..INCREMENT} syntax:
echo "Bash version ${BASH_VERSION}..."
for i in {0..10..2}
  do
     echo "3. Welcome $i times"
 done

# Three-expression bash for loops syntax
# This type of for loop share a common heritage with the C programming language. It is characterized by a three-parameter loop control expression; consisting of an initializer (EXP1), a loop-test or condition (EXP2), and a counting expression (EXP3).

# for (( EXP1; EXP2; EXP3 ))
# do
# 	command1
# 	command2
# 	command3
# done

for (( c=1; c<=5; c++ ))
do
   echo "4. Welcome $c times"
done



# Infinite loop
# for (( ; ; ))
# do
#    echo "infinite loops [ hit CTRL+C to stop]"
# done


# We can use 'break', 'continue' in for loop