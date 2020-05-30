#!/bin/bash
assert() {
  expected="$1"
  input="$2"

  ./9cc "$input" > tmp.s
  cc -o tmp tmp.s
  ./tmp
  actual="$?"

  if [ "$actual" = "$expected" ]; then
    echo "$input => $actual"
  else
    echo "$input => $expected expected, but got $actual"
    exit 1
  fi
}

assert 0 "0;"
assert 42 "42;"
assert 21 "5+20-4;"
assert 41 " 12 + 34 - 5 ;"
assert 47 "5+6*7;"
assert 15 "5*(9-6);"
assert 4 "(3+5)/2;"
assert 10 "-10+20;"
assert 10 "- -10;"
assert 10 "- - +10;"

assert 0 "0==1;"
assert 1 "42==42;"
assert 1 "0!=1;"
assert 0 "42!=42;"

assert 1 "0<1;"
assert 0 "1<1;"
assert 0 "2<1;"
assert 1 "0<=1;"
assert 1 "1<=1;"
assert 0 "2<=1;"

assert 1 "1>0;"
assert 0 "1>1;"
assert 0 "1>2;"
assert 1 "1>=0;"
assert 1 "1>=1;"
assert 0 "1>=2;"

assert 3 "a = 3; a;"
assert 14 "a = 3; b = 5 * 6 - 8; a + b / 2;"
assert 12 "a=3;b = 5 * 6 - 8; (a + b)/2;"

assert 3 "abc = 3; abc;"
assert 14 "_a3 = 3; b = 5 * 6 - 8; _a3 + b / 2;"
assert 12 "hoge=3;fuga = 5 * 6 - 8; (hoge + fuga)/2;"

assert 3 "abc = 3; abc; return abc;"
assert 22 "_a3 = 3; b = 5 * 6 - 8; return b; _a3 + b / 2;"
assert 5 "hoge=3;fuga = 5 * 6 - 8; (hoge + fuga)/2; return 5;"

assert 10 "if (1 < 10) 10; else 100;"
assert 100 "if (1 > 10)10; else 100;"
assert 10 "if(1 < 10) return 10; 12;"
assert 12 "if (1 > 10) return 10; 12;"
assert 3 "x=5; if (x < 10) if (1 < x) return 3;else return 8;else return 123;"
assert 8 "x=0; if (x < 10) if (1 < x) return 3;else return 8;else return 123;"
assert 123 "x=13; if (x < 10) if (1 < x) return 3;else return 8;else return 123;"

echo OK
