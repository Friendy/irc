#!/bin/bash

G="\033[0;32m";
Y="\033[0;33m";
R="\033[0;31m";
NC="\033[0m";

# the string should be
# ./cl.sh [class name] [private variables] [public variables]
#for pointer variables  int'*'
#for references  int'&'

# Class name capitalization
CLASS_NAME=$(echo ${1::1} | tr '[:lower:]' '[:upper:]')$(echo $1 | cut -c 2-)
CPP_FILE=$CLASS_NAME.cpp
HPP_FILE=$CLASS_NAME.hpp
# echo $CPP_FILE
if [ ! -f $CPP_FILE ]; then
	touch $CPP_FILE
fi

if [ ! -f $HPP_FILE ]; then
	touch $HPP_FILE
fi

echo -n "#ifndef " > $HPP_FILE
echo $CLASS_NAME"_HPP" | tr '[:lower:]' '[:upper:]' >> $HPP_FILE
echo -n "# define " >> $HPP_FILE
echo $CLASS_NAME"_HPP" | tr '[:lower:]' '[:upper:]' >> $HPP_FILE
echo "# include <string>" >> $HPP_FILE
echo "# include <iostream>" >> $HPP_FILE
echo "" >> $HPP_FILE
echo "class" $CLASS_NAME "{" >> $HPP_FILE
echo "" >> $HPP_FILE
# variables
echo "private:" >> $HPP_FILE
for ((i=2; i<=$#; i += 2 )); do
	k=$((i+1))
	echo -e "\t"${!i}" _"${!k}";" >> $HPP_FILE
done
echo "" >> $HPP_FILE
echo "public:" >> $HPP_FILE
# getters and setters
for ((i=2; i<=$#; i += 2 )); do
	k=$((i+1))
	NAME=$(echo ${!k::1} | tr '[:lower:]' '[:upper:]')$(echo ${!k} | cut -c 2-)
	echo -e "\tconst "${!i}" get"$NAME"() const;" >> $HPP_FILE
	echo -e "\tvoid set"$NAME"("${!i} ${!k}");" >> $HPP_FILE
done
#constructor
echo "" >> $HPP_FILE
echo -e "\t"$CLASS_NAME"();" >> $HPP_FILE
#copy
echo -e "\t"$CLASS_NAME"("$CLASS_NAME" const &original);" >> $HPP_FILE
#assign
echo -e "\t"$CLASS_NAME" &operator=("$CLASS_NAME" const &original);" >> $HPP_FILE
#destructor
echo -e "\t~"$CLASS_NAME"();" >> $HPP_FILE
echo "};" >> $HPP_FILE
echo "" >> $HPP_FILE
echo "#endif" >> $HPP_FILE

#CPP
echo "#include \""$HPP_FILE"\"" > $CPP_FILE
echo "" >> $CPP_FILE
echo "/*CONSTRUCTORS*/" >> $CPP_FILE
echo -e $CLASS_NAME::$CLASS_NAME"()" >> $CPP_FILE
echo -e "{" >> $CPP_FILE
echo -e "\tstd::cout << \"class created\" << std::endl;" >> $CPP_FILE
echo -e "}" >> $CPP_FILE
echo "" >> $CPP_FILE

echo "//Assignment operator:" >> $CPP_FILE
echo -e $CLASS_NAME "&"$CLASS_NAME"::operator=("$CLASS_NAME" const &original)" >> $CPP_FILE
echo -e "{" >> $CPP_FILE
echo -e "\tif (this != &original)" >> $CPP_FILE
if [ $# -gt 3 ]; then
	echo -e "\t{" >> $CPP_FILE
fi
for ((i=2; i<$#; i += 2 )); do
	k=$((i+1))
	echo -e "\t\tthis->_"${!k} "= original._"${!k}";" >> $CPP_FILE
done
if [ $# -gt 3 ]; then
	echo -e "\t}" >> $CPP_FILE
fi
echo -e "\treturn(*this);" >> $CPP_FILE
echo -e "}" >> $CPP_FILE
echo "" >> $CPP_FILE
#copy constructor
echo -e $CLASS_NAME::$CLASS_NAME"("$CLASS_NAME" const &original)" >> $CPP_FILE
echo -e "{" >> $CPP_FILE
echo -e "\t*this = original;" >> $CPP_FILE
echo -e "}" >> $CPP_FILE
echo "" >> $CPP_FILE
echo "/*FUNCTIONS*/" >> $CPP_FILE
for ((i=2; i<=$#; i += 2 )); do
	k=$((i+1))
	NAME=$(echo ${!k::1} | tr '[:lower:]' '[:upper:]')$(echo ${!k} | cut -c 2-)
	echo -e "const "${!i} $CLASS_NAME::"get"$NAME"() const" >> $CPP_FILE
	echo -e "{" >> $CPP_FILE
	echo -e "\treturn(this->_"${!k}");" >> $CPP_FILE
	echo -e "}" >> $CPP_FILE
	echo "" >> $CPP_FILE
	echo -e "void $CLASS_NAME::set"$NAME"("${!i} ${!k}")" >> $CPP_FILE
	echo -e "{" >> $CPP_FILE
	echo -e "\tthis->_"${!k}" = "${!k}";" >> $CPP_FILE
	echo -e "}" >> $CPP_FILE
	echo "" >> $CPP_FILE
done
echo "/*DESTRUCTOR*/" >> $CPP_FILE
echo -e $CLASS_NAME::~$CLASS_NAME"(){}" >> $CPP_FILE
# echo "private:" >> $HPP_FILE
# include <iostream>
# echo "#ifndef "($CLASS_NAME)"_HPP" > $HPP_FILE
# define WRONG_ANIMAL_HPP
# echo -e $REST
# i=2;
# while [[ i -ne $# ]]; do
# 	echo ${!i}
# 	(( i += 1 ))
# done
# echo -e "$G$1$NC";


# if $1 == 
# CLASS_NAME = 