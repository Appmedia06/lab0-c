# Test performance of sort betweem q_sort() and list_sort()
# listsort: 0: q_sort(), 1: list_sort()
# it RAND: randon list number

option listsort 1
new
it RAND 500000
time
sort
time
