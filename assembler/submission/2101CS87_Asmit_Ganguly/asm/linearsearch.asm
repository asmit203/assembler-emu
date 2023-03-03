;this is a linear search algorithm in the prescribed language of the compiler
;it has to be given the array elements as shown below and the target value
;storing numbers
ldc 12
stl 1

ldc 9
stl 2

ldc 30
stl 3

ldc 25
stl 4

;temp variable loop
ldc 1  ;i
stl 6

;checking for loop
ldc 5  ;n
stl 8

;target variable
ldc 30
stl 100

loop:
ldl 6
ldl 8
sub
brlz execute

execute:
ldl 6
ldnl 0
ldl 100
sub
brz done
ldl 6
ldc 1
add
stl 6
br loop

done:
ldl 6

;if it is present then the number is in range 1 to n (number of elements in the array) but if not than any other gibberish value is loaded
;if the number is present in the array of the number then the position is returned.
;NOTE: It is using one indexed method

;time complexity: O(n)
;space complexity: O(1)

;Asmit Ganguly
;2101CS87
;ALGO: Linear Search