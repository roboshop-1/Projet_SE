install :
		apt-get install libreadline-dev
compile :
		gcc code.c -o code -lreadline
execute :
		./code $(file)
delete :
		rm code
userP :
		chmod u+rwx $(file)
allP :
		chmod 777 $(file)
