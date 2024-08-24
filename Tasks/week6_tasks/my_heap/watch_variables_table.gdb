# Set a breakpoint where you want to inspect the values when an allocation failed
b my_heap.c:199

b my_heap.c:395
# Define commands to be executed when the watchpoint is hit and print it to see the pointers if pointing wrong
commands 2
    # Print the free list when the watchpoint is triggered
    PrintFirstFreePointer

    # Continue execution after printing
    continue
end

# Define a custom GDB command to print the free list from the head
define PrintFirstFreePointer
  set $current = first_free_block
  printf "First Free Block: ************************************************************ \n"
  printf "----------------------------------------------------\n"
  printf "| Current | Block Size | Next Free Block | Prev Free Block |\n"
  printf "----------------------------------------------------\n"


  # Loop through the list
  while ($current != 0)
    set $size = $current->size
    set $next = $current->next
    set $prev = $current->prev

    printf "| %5d | %10d | %15d | %15d |\n", $current, $size, $next, $prev

    # Move to the next block
    set $current = $next

  printf "----------------------------------------------------\n"
end
