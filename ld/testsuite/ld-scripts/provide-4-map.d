#...
Linker script and memory map
#...
                \[!provide\] +PROVIDE \(foo = 0x1\)
                \[!provide\] +PROVIDE \(bar = 0x2\)
                0x0+3 +PROVIDE \(baz = 0x3\)
#...
                0x0+2000 +foo
                \[!provide\] +PROVIDE \(loc1 = ALIGN \(\., 0x10\)\)
                0x0+2010 +PROVIDE \(loc2 = ALIGN \(\., 0x10\)\)
                \[!provide\] +PROVIDE \(loc3 = \(loc1 \+ 0x20\)\)
                0x0+2030 +loc4 = \(loc2 \+ 0x20\)
#...
