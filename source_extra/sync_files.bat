del /q	help\*.*
del /q	help\search\*.*
del /q	syntax\*.*

md help
md help\search
md syntax

copy z:\w_doxygen\diamond\html\*.*				help
copy z:\w_doxygen\diamond\html\search\*.*		help\search
copy z:\diamond\syntax\*.json						syntax
