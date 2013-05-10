del /q	z:\diamond\install_files\help\*.*
del /q	z:\diamond\install_files\help\search\*.*
del /q 	z:\diamond\install_files\syntax\*.*

md z:\diamond\install_files\help
md z:\diamond\install_files\help\search
md z:\diamond\install_files\syntax

copy z:\w_doxygen\diamond\html\*.*  			z:\diamond\install_files\help
copy z:\w_doxygen\diamond\html\search\*.*		z:\diamond\install_files\help\search
copy z:\diamond\syntax\*.json						z:\diamond\install_files\syntax
