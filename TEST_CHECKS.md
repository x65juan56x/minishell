# **************************************************************************** #
#                                COMPARE PARSING                               #
# **************************************************************************** #

### comparing your input parsing to bash ###

⚠️6: /bin/echo ""'$USER'""

⚠️7: /bin/echo '"'$USER'"'

⚠️9: /bin/echo "'"'$USER'"'"

⚠️10: /bin/echo '"'"$USER"'"'

✅24: /bin/echo \$USER

✅25: /bin/echo \\$USER

✅26: /bin/echo \\\$USER

✅27: /bin/echo \\\\$USER

✅28: /bin/echo \\\\\$USER

✅29: /bin/echo \\\\\\\\\$USER

✅30: /bin/echo \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\$USER \$PATH \\$PWD

✅42: /bin/echo ''''''''''$USER''''''''''

✅45: /bin/echo ""'""""""$USER""""""'""

✅46: /bin/echo """"""""'$USER'""""""""

✅48: /bin/echo """"""'""$USER""'""""""

⚠️51: /bin/echo $USER'$USER'text oui oui     oui  oui $USER oui      $USER ''

⚠️66: /bin/echo $USER$TESTNOTFOUND$HOME$

# **************************************************************************** #
#                                PARSING HELL                                  #
# **************************************************************************** #

⚠️12:
cat << $USER
why
not
$USER

⚠️14:
cat << "$USER"
why
not
$USER

⚠️15:
cat << "$US"E"R"
because
we
love
bash
$USER

⚠️17:
>> "$H"OM"E"
cat OME
/bin/rm -f OME

⚠️22:
cd "$H"O"ME"/Desktop/
echo $?

⚠️23:
export T=n
echo "-"$T$T

⚠️25:
export T=ech
echo $T"o"

⚠️26:
export T=ech
echo $T"o "

⚠️27:
export T=ech
echo $T"o -n"

⚠️28:
export T=ech
echo $T"o -n"

⚠️29:
export T=ech
echo $T'o'

❌30:
export T="-n test1 -n test 2"
echo $T

⚠️31:
export T=ech
echo $T'o '

⚠️41:
export T=nnnnnnnn
echo "-""$T"nnnnnnnnnnnnn -nnnnnnnn"$T" '-'"$T"

⚠️42:
export T=nnnnnnnn
echo "-""$T"nnnnnnnnnnnnn -nnnnnnnn"$T" '-''$T'

⚠️50:
export T=e E=c S=h L=o
$T$E$S$L -nn

⚠️58:
export T="l"
$Ts -a

⚠️77:
export T="echo segfault | grep segfault"
$T

⚠️78:
export T=-nnnnnnnn"nnnnnnn "
echo $T

⚠️101:
'''''''''''''''' echo ok

⚠️109:
echo -nnnnnnnnnnnn

⚠️123:
>| echo sure

✅124:
cd --

# **************************************************************************** #
#                                   BUILTINS                                   #
# **************************************************************************** #

### ECHO ###

✅1: echo cd ~

⚠️8: echo ""'$USER'""

⚠️9: echo '"'$USER'"'

⚠️11: echo "'"'$USER'"'"

⚠️12: echo '"'"$USER"'"'

✅27: echo \$USER

✅28: echo \\$USER

✅29: echo \\\$USER

✅30: echo \\\\$USER

✅31: echo \\\\\$USER

✅32: echo \\\\\\\\\$USER

✅33: echo \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\$USER \$PATH \\$PWD

⚠️45: echo ''''''''''$USER''''''''''

⚠️47: echo $USER'$USER'text oui oui     oui  oui $USER oui      $USER ''

⚠️62: echo $USER$TESTNOTFOUND$HOME$

⚠️80: echo -n -nnnnnnn -n -nnn -nnnnn -n-n

⚠️81: echo -n -nnnnnnn -n -nnn -nnnnn -n feel my pain

✅93: cd ..

102: cd '/////' 2>/dev/null

105: cd "doesntexist" 2>/dev/null

107: cd "wtf" 2>/dev/null

✅128:
pwd
cd ~
cd - ananas dot jpeg
pwd

✅131:
pwd
cd ~
cd -
pwd

✅133:
pwd
cd ~
pwd

140: env what

150: export

165: export --TEST=123

170: export -TEST=100

185: export -TEST=123

200: export TEST+=100

224: unset TES;T

231: unset -TEST

# **************************************************************************** #
#                                  PIPELINES                                   #
# **************************************************************************** #

15: /bin/env | grep "_="

25:
ls | cat << stop | grep "asd"
is this good
stop

✅26:
ls | cat << stop | ls -la | cat << stop1
12
32232
23
stop
awdaw
daswd
stop1

✅27:
ls | cat << stop | ls -la | cat << stop1 | ls | cat << stop2 | ls -la | cat << stop3
$USER
ad
as $HOME
stop
awd
wf$PWDdqwdwqd
stop1
das
das
stop2
dsq
wd
wf$PWDdqwdwqd
stop3

28:
ls | cat << stop | ls -la | cat << stop1 | ls | cat << stop2 | ls -la > out | cat << 'stop3'
$USER
ad
stop
dsa
stop1
sad
stop2
as $HOME
stop3
/bin/rm -f out

30:
ls | cat << stop | ls -la > out0| cat << stop1 | ls | cat << stop2 | ls -la >> out | cat << stop3
fe
wf
fwe
f
stop
dw
stop1
dw
stop2
e
wf
stop3
/bin/rm -f out
/bin/rm -f out0

38:
ls|cat Makefile|cat<<asd>out
$USER
asd
/bin/rm -f out

39:
ls|cat Makefile|cat<<'asd'>out
$USER
asd
/bin/rm -f out

40:
ls|cat Makefile|cat<<"asd">out
$USER
asd
/bin/rm -f out

# **************************************************************************** #
#                                    REDIRS                                    #
# **************************************************************************** #

### HERE_DOC ###

80:
cat << lim
 "lim"
HELLO
42
 lim
testing your minishell
limm
lim

81:
<<lim cat
 "lim"
HELLO
42
 lim
testing your minishell
limm
lim

82:
cat << lim
test
lim

83:
cat << EOF
"EOF"
!! HELLO
@42 !!
 EOF\t\b\n
testing your minishell :)
limm
EOF

84:
cat << hello
$USER
$NOVAR
$HOME
hello

85:
cat << 'lim'
$USER
$NOVAR
$HOME
lim

86:
cat << lim''
$USER
$NOVAR
$HOME
lim

87:
cat << "lim"
$USER
$NOVAR
$HOME
lim

88:
cat << 'lim'
$USER
$NOVAR
$HOME
lim

# **************************************************************************** #
#                                     SCMD                                     #
# **************************************************************************** #

⚠️ 4: ""  minishell 0 command not found / bash 127

✅ 7: minishell no crea el archivo / bash si
touch "
"
/bin/rm -f "
"

# **************************************************************************** #
#                                  BROKEN SCMD                                 #
# **************************************************************************** #

⚠️ 26: "." minishell 127 / bash 2 (ambos command not found)

# **************************************************************************** #
#                                  VARIABLES                                   #
# **************************************************************************** #

✅ 4: /bin/echo $HOME$NOTHING$USER$ (CORREGIDO)

18: /bin/echo $'HOM'E$USER

24: /bin/echo "$""$"

27: /bin/echo "$"$

✅ 35: /bin/echo $TESTNOTFOUND$HOME$ (CORREGIDO)

### SCMD IN VARIABLE ###
43:
export tmp_test="/bin/echo 42"
$tmp_test
$tmp_test 42
export tmp_test="/bin/echo"
$tmp_test 42 42

### VARIABLE SPLITTING ###
45:
export X="  A  B  "
/bin/echo "1"$X'2'

46:
export X=" A  B  "
/bin/echo "1"$X'2'

47:
export X="A  B  "
/bin/echo "1"$X'2'

48:
export X="  A  B "
/bin/echo "1"$X'2'

49:
export X="  A  B"
/bin/echo "1"$X'2'

50:
export X="  A B  "
/bin/echo "1"$X'2'

51:
export X="  AB  "
/bin/echo "1"$X'2'

52:
export X="  A  B  "
/bin/echo $X'2'

53:
export X="  A  B  "
/bin/echo $X"1"

54:
export X=""
/bin/echo "1"$X'2'

55:
export X=" "
/bin/echo "1"$X'2'

56:
export X="   "
/bin/echo "1"$X'2'

57:
export X="  A  B  "
/bin/echo ?$X'2'

58:
export X="  A  B  "
/bin/echo "1"$X?

# **************************************************************************** #
#                                  CORRECTION                                  #
# **************************************************************************** #

2: ""

63: echo ''$PWD''

64: echo $USE"R"$HOME

68: echo $'HOM'E$USER

74: echo "$""$"

77: echo "$"$

110:
unset PATH
cd /bin
ls

111:
unset PATH
cd /bin/../bin/
ls

113:
unset PATH
cd /bin/
sleep 2

# **************************************************************************** #
#                                  PATH FAILS                                  #
# **************************************************************************** #

✅ 7: cd ~

✅ 8:
cd ~/Desktop/
pwd

11: ✅
export PATH=
echo $PATH
echo 1

12:
touch tmp_x_file1
tmp_x_file1
echo $?
./tmp_x_file1
echo $?
export PATH=$HOME
echo $PATH
tmp_x_file1
echo $?
./tmp_x_file1
echo $?
unset PATH
tmp_x_file1
echo $?
./tmp_x_file1
echo $?
echo 42
/bin/rm -f tmp_x_file1

# **************************************************************************** #
#                                SYNTAX ERRORS                                 #
# **************************************************************************** #

8: ~   ⚠️ minishell = 127 bash = 126

15: .  ⚠️ minishell = 126 bash = 1

16: .. ✅ mismo error exit en bash y en minishell = 126, en test dice que bash = 127

20:
>echo>
/bin/rm -f echo ✅ mismo exits status = 0, aunque en el test la exit status de baash = 2

21:
<echo<
/bin/rm -f echo ✅ mismo exits status = 0, aunque en el test la exit status de baash = 2

22:
>>echo>>
/bin/rm -f echo ✅ mismo exits status = 0, aunque en el test la exit status de baash = 2

# **************************************************************************** #
#                                     WILD                                     #
# **************************************************************************** #

10:
export test="arg1	arg2"
echo 'echo $1' > tmp_test_sh
bash tmp_test_sh $test
echo 'echo $2' > tmp_test_sh
bash tmp_test_sh $test
rm -f tmp_test_sh

13:
echo "env | /usr/bin/wc -l" | env -i $MINISHELL_PATH"/"$EXECUTABLE
echo $?

14:
echo "ls" | env -i $MINISHELL_PATH"/"$EXECUTABLE
echo $?

15:
echo "unset PATH" | env -i $MINISHELL_PATH"/"$EXECUTABLE
echo $?

### LEAKS ###

# **************************************************************************** #
#                                  PIPELINES                                   #
# **************************************************************************** #

ls | cat << stop | ls -la | cat << stop1 | ls | cat << stop2 | ls -la > > out | cat << stop3
fe
wf
fwe
f
stop
dw
stop1
dwasd
stop2
cat Makefile | ls > out
cat out
/bin/rm -f out

|ls | ls

# **************************************************************************** #
#                                     SCMD                                     #
# **************************************************************************** #

touch "
"

# **************************************************************************** #
#                                  BROKEN SCMD                                 #
# **************************************************************************** #

| echo -n oui

| | |

# **************************************************************************** #
#                                SYNTAX ERRORS                                 #
# **************************************************************************** #

>

<

<<

> > > > >

>> >> >> >>

>>>>>>>>>

<<<<<<<<<

< < < < < <

/bin/cat ><

echo > <

echo | |

|echo|

|

| test

| | |

| | | | test

| test

echo > <

hello world
||||||||
            
cat wouaf wouaf
>

> > > >

>> >> >> >>

<<

| echo -n oui

<>
