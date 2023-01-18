install :
		apt-get install libreadline-dev
compile :
		gcc code.c -o code -lreadline
execute :
		./code
delete :
		rm code
bash :
		./code bash
