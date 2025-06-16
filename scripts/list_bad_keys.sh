#! /bin/bash
CURRENT_WORKDIR=$(realpath $(dirname $0))
TEST_DICT_DIR=$CURRENT_WORKDIR/../tests/testdict
TEST_DICT=$TEST_DICT_DIR/旺文社漢字典/旺文社漢字典第四版.mdx

# 遍历字典中的所有条目
$CURRENT_WORKDIR/../build/bin/mydict -l $TEST_DICT > $CURRENT_WORKDIR/../build/all-wordlist.txt

# 遍历所有条目，检查是否存在非法字符
line_number=0
cat $CURRENT_WORKDIR/../build/all-wordlist.txt | while read -r line; do
    # skip the first line, which the line number we don't know at first

    if [ $line_number -eq 0 ]; then
        line_number=$((line_number + 1))
        echo "" > $CURRENT_WORKDIR/../build/bad-keys.txt
        echo "" > $CURRENT_WORKDIR/../build/good-keys.txt
        continue
    fi

    OUTPUT=$(echo $line | sed 's/\\/\\\\/g' | xargs -I {} $CURRENT_WORKDIR/../build/bin/mydict -n $TEST_DICT {} 2>/dev/null)
    # if the output is not a number, means the key is bad
    if ! [[ $OUTPUT =~ ^[0-9]+$ ]]; then
        echo "bad key: $line, $OUTPUT"
        echo $line >> $CURRENT_WORKDIR/../build/bad-keys.txt
        continue
    fi

    # last command output value is grater than 0, means the key is good
    # otherwise, the key is bad
    if [ $OUTPUT -eq -1 -o $OUTPUT -eq 0 -o $? -ne 0 ]; then
        echo "bad key: $line, $OUTPUT"
        echo $line >> $CURRENT_WORKDIR/../build/bad-keys.txt
    else
        echo $line >> $CURRENT_WORKDIR/../build/good-keys.txt
    fi

    line_number=$((line_number + 1))
    # if line number mod 10 is 0, print a dot
    if [ $((line_number % 10)) -eq 0 ]; then
        echo -n "."
    fi
done
