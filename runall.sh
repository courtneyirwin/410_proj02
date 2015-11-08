for run in 1 2 3
do
    keyword="cout << "
    echo $run ' run: grep -roh ' $keyword ' utilities.cpp' 
    grep -roh "$keyword" utilities.cpp
done





