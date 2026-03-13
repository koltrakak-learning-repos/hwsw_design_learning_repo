# Find an element within an array

Run the code:
~~~~~shell
make clean all run
~~~~~
* What is it returned if the _element_to_find_ is not present in the array?
* What if multiple value of the array correspond to _element_to_find_ ?

You can also try to change the array initialization rule and the _element_to_find_ value and check the output of the example.

To disassemble the code:
~~~~~shell
make dis > assembly.txt
~~~~~
You can navigate the assembly code by opening the file _assembly.txt_.

Change now the compiler optimization option to -03 in the Makefile, compile again the code and dump the assembly code. How does this change?
