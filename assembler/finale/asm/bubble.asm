;storing numbers
ldc 12
stl 1
ldc 9
stl 2
ldc 30
stl 3
ldc 25
stl 4

;temp variable outer loop
ldc 1  ;i
stl 6
;temp variable inner loop
ldc 1  ;j
stl 7
;checking for both
ldc 5  ;n
stl 8
;swapping variable
ldc 0
stl 500

;bubble sorting
outerloop:
ldl 6
ldl 8
sub ;i-n
brlz innerloop
br done

innerloop:
ldl 7
ldl 8
sub ;j-n
brlz execute
br reset

execute:
;incrementing j
ldl 7
ldc 1
add
stl 7

;arr[j] - arr[j+1] < 0 then swap
ldl 7
ldnl -1
ldl 7
ldnl 0
sub
brlz swap
br innerloop

swap:
ldl 7
ldnl -1
stl 500
ldl 7
ldnl 0
ldl 7
stnl -1
ldl 500
ldl 7
stnl 0
br innerloop


reset:
ldc 1 ;temp
stl 7

ldl 6
ldc 1
add
stl 6
br outerloop

done:
ldl 1
ldl 2
ldl 3
ldl 4