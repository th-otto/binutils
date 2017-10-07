#name: PR ld/22269
#ld: -pie --no-dynamic-linker
#readelf: -r -x .data.rel.ro
#target: *-*-linux* *-*-gnu*

There are no relocations in this file.

Hex dump of section '.data.rel.ro':
  0x[a-f0-9]+ [0 ]+[ ]+.+
