watch last_free_block

command 1
    PrintFirstFreePointer
    PrintLastFreePointer

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

    printf "| %5p | %10d | %15p | %15p |\n", $current, $size, $next, $prev

    # Move to the next block
    set $current = $next

  printf "----------------------------------------------------\n"
end

# Define a custom GDB command to print the free list from the head
define PrintLastFreePointer
  set $current = last_free_block
  printf "Last Free Block: ************************************************************ \n"
  printf "----------------------------------------------------\n"
  printf "| Current | Block Size | Next Free Block | Prev Free Block |\n"
  printf "----------------------------------------------------\n"

    set $size = $current->size
    set $next = $current->next
    set $prev = $current->prev

    printf "| %5p | %10d | %15p | %15p |\n", $current, $size, $next, $prev

  printf "----------------------------------------------------\n"
end