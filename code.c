#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>
#include <glob.h>
#include <readline/readline.h>
#include <readline/history.h>

typedef struct Environnement
{
	char *nom;
	char *valeur;
    struct Environnement *next;
} Environnement ;

Environnement *var_environnement=NULL;

char *arg_list[32],*arg_list2[32];
char buffer[250];
int	 global_argc, histocount;
FILE *fichier;
// tab3a partie variable partie 16 fi site
void ajout_environnement(char *nom_variable,char *valeur_variable)
{
	Environnement *liste=var_environnement;
	
	int test=0;
	if (liste!=NULL)
	{
		while (liste->next!=NULL)
		{
			if (strcmp(nom_variable,liste->nom)==0)
			{
				free(liste->valeur);
				liste->valeur=strdup(valeur_variable);
				test=1;
			}
			liste=liste->next;
		}
	}
	if (test==0)
	{
		Environnement *new_env=malloc(sizeof(Environnement));
		new_env->nom=strdup(nom_variable);
		new_env->valeur=strdup(valeur_variable);
		new_env->next=NULL;
		liste=var_environnement;
		if (liste!=NULL)
		{
			while(liste->next!=NULL)
			{
				liste=liste->next;
			}	
			liste->next=new_env;
		}
		else
		{
			var_environnement=new_env;
		}
	}
}

void str_replace(char *chaine,char* recherche,char *remplace)
{
int nbre=0;

	char *p=chaine;
	char *tmp=strstr(p,recherche);
	while (tmp!=NULL)
	{
		++nbre;
		p=tmp+strlen(recherche);
		tmp=strstr(p,recherche);
	}
	if (nbre>0)
	{
		char *chaine_copie=malloc(strlen(chaine)-(strlen(recherche)*nbre)+(strlen(remplace)*nbre)+1);
		chaine_copie[0]='\0';
		char *p=chaine;
		char *tmp=strstr(p,recherche);
		while (tmp!=NULL)
		{
			strncat(chaine_copie,p,tmp-p);
			strcat(chaine_copie,remplace);
			p=tmp+strlen(recherche);
			tmp=strstr(p,recherche);
		}
		strcat(chaine_copie,p);
		strcpy(chaine,chaine_copie);
		free(chaine_copie);
	}
}

void creation_liste_arguments(char *arguments[32],char *commande)
{
int  boucle,increment,longueur;

	for (boucle=0;boucle<32;++boucle)
	{
		arguments[boucle]=NULL;
	}
	
	longueur=strcspn(commande," \0");
	//printf("----------longueur du comm--%d---\n",longueur);
	arguments[0]=strndup(commande,longueur);
	commande=commande+longueur;
	increment=1;
	while (strlen(commande)>0)
	{	
		if (commande[0]==' ') ++commande;
		char *separateur=" ";
		if (commande[0]=='"') separateur="\"";
		if (strcmp(separateur,"\"")==0) ++commande;
		longueur=strcspn(commande,separateur);
		arguments[increment]=strndup(commande,longueur);
		commande=commande+longueur;
		if (strcmp(separateur,"\"")==0) ++commande;
		++increment;
	}
	//printf("----------commande--%d---\n",*commande);
	//printf("----------arguments--%s---\n",*arguments);
}

void liberation_arguments(char *arguments[32])
{
int increment=0;
	while (arguments[increment]!=NULL)
	{
		free(arguments[increment]);
		increment++;
	}
}


char *scan_redirection_sortante(char *arguments[32])
{
char *redirection=NULL;
int  increment=0;

	while (arguments[increment]!=NULL)
	{
		if (strcmp(arguments[increment],">")==0)
		{
			redirection=malloc(strlen(arguments[increment+1])+1);
			redirection[0]='w';
			redirection[1]='\0';
			strcat(redirection,arguments[increment+1]);
			free(arguments[increment]);	
			free(arguments[increment+1]);	
			while (arguments[increment+2]!=NULL)
			{
				arguments[increment]=arguments[increment+2];
				++increment;
			}
			arguments[increment]=NULL;
		}
		else if (strcmp(arguments[increment],">>")==0)
		{
			redirection=malloc(strlen(arguments[increment+1])+1);
			redirection[0]='a';
			redirection[1]='\0';
			strcat(redirection,arguments[increment+1]);
			free(arguments[increment]);	
			free(arguments[increment+1]);	
			while (arguments[increment+2]!=NULL)
			{
				arguments[increment]=arguments[increment+2];
				++increment;
			}
			arguments[increment]=NULL;
		}
		++increment;
		//printf("----------redirection--%s---\n",redirection);
	}
	return redirection;
}
//--------------------------------------------------
void removeWhiteSpace(char* buf){
	if(buf[strlen(buf)-1]==' ' || buf[strlen(buf)-1]=='\n')
	buf[strlen(buf)-1]='\0';
	if(buf[0]==' ' || buf[0]=='\n') memmove(buf, buf+1, strlen(buf));
}
//*********************************************/
int executeOR ( char *cmd1 ){
int pipefd[2];
creation_liste_arguments(arg_list,cmd1);	

pipe(pipefd);
pid_t process=fork();
if (process==0){

int retour=execvp(arg_list[0],arg_list);
if (retour==-1) { 
fprintf(stderr,"%s\n",strerror(errno));
return retour;}
exit(1);
return retour ;
}
  else
  {
  wait(&process);
  }
}
//**********************************************/
int executeAND ( char *cmd1 ){
int pipefd[2];
creation_liste_arguments(arg_list,cmd1);	

pipe(pipefd);
pid_t process=fork();
if (process==0){

int retour=execvp(arg_list[0],arg_list);
if (retour==-1) { 
//fprintf(stderr,"%s\n",strerror(errno));
return retour;}
exit(1);
return retour ;
}
  else
  {
  wait(&process);
  }
}
//----------------------------------------
void executeAsync ( char *cmd1 ){

 int pipefd[2];
 
  creation_liste_arguments(arg_list,cmd1);	
  pipe(pipefd);
  pid_t process=fork();
  
  if (process==0)
  {
   execvp(arg_list[0],arg_list) ;
   perror("invalid input");
   exit(1);
  }
  else
  {
  wait(&process);
  }
}
//-----------------------------
void executePipe ( char *cmd1 , char *cmd2 ){

 int pipefd[2];
 
 creation_liste_arguments(arg_list,cmd1);
 	
 if (cmd2!=NULL)
	{
		creation_liste_arguments(arg_list2,cmd2);	
	}

	pipe(pipefd);
		//The pid_t data type is a signed integer type which is capable of representing a process ID
	pid_t process=fork();
	
	// child process
	if (process==0)
	{
		if (cmd2!=NULL) 
		{
		
			dup2(pipefd[1],STDOUT_FILENO);
		}
		
		int retour=execvp(arg_list[0],arg_list);
		if (retour==-1) fprintf(stderr,"%s\n",strerror(errno));
		exit(0);
	}
	else
	{
		wait(&process);
	}
	
	
	if (cmd2!=NULL)
	{
	//The pid_t data type is a signed integer type which is capable of representing a process ID
		pid_t process2=fork();
		if (process2==0)
		{
			dup2(pipefd[0],STDIN_FILENO);
			int retour=execvp(arg_list2[0],arg_list2);
			if (retour==-1) fprintf(stderr,"%s\n",strerror(errno));
			exit(0);
		}
		else
		{
			wait(&process2);
		}
	}
		
	if (cmd2!=NULL)
	{
		liberation_arguments(arg_list2);
		free(cmd2);
		cmd2=NULL;
	}
	liberation_arguments(arg_list);
	free(cmd1);
}
//---------------------------

void executeRed ( char *cmd1 ){
char *fichier_redirection_sortante;
char *fichier_redirection_sortante2;
 int pipefd[2];
 
 creation_liste_arguments(arg_list,cmd1);
 fichier_redirection_sortante=scan_redirection_sortante(arg_list);	
 
	pipe(pipefd);
		//The pid_t data type is a signed integer type which is capable of representing a process ID
	pid_t process=fork();
	
	// child process
	if (process==0)
	{
		
		if (fichier_redirection_sortante!=NULL)
		{
		
			char* type_redirection=strndup(fichier_redirection_sortante,1);
			int handler=(intptr_t)freopen(fichier_redirection_sortante+1,type_redirection, stdout);
			if (handler==-1) 
			{
				fprintf(stderr,"%s\n",strerror(errno));
				exit(0);
			}
			free(type_redirection);
		}
		// printf("---arg_list[0]  %s \n",*arg_list);
		int retour=execvp(arg_list[0],arg_list);
		if (retour==-1) fprintf(stderr,"%s\n",strerror(errno));
		exit(0);
	}
	else
	{
		wait(&process);
	}
	
	if (fichier_redirection_sortante!=NULL) free(fichier_redirection_sortante);
	liberation_arguments(arg_list);
	free(cmd1);
}

//------------------------------------------------
void traitement_cmd(char *commande,char **argv)
{
char *cmd1,*cmd2;
char *fichier_redirection_sortante;
char *fichier_redirection_sortante2;
int pipefd[2];

	cmd2=NULL;
	
	
	str_replace(commande,"\n","");
	//-------------------------------------
	str_replace(commande," >",">");
	str_replace(commande,"> ",">");
	str_replace(commande,">"," > ");
	char *tmpr=strstr(commande," > ");
	//-------------------------------------
	str_replace(commande,"| ","|");
	str_replace(commande," |","|");
	str_replace(commande,"|"," | ");
	char *tmp=strstr(commande," | ");
	//-------------------------------------
	str_replace(commande,"|| "," |  | ");
	str_replace(commande," ||"," |  | ");
	str_replace(commande,"||"," |  | ");
	str_replace(commande," |  | "," || ");
	char *tmp2=strstr(commande," || ");
	//-------------------------------------
	str_replace(commande,"&& ","&&");
	str_replace(commande," &&","&&");
	str_replace(commande,"&&"," && ");
	char *tmp1=strstr(commande," && ");
	//-------------------------------------
	str_replace(commande,"; ",";");
	str_replace(commande," ;",";");
	str_replace(commande,";"," ; ");
	char *tmpp=strstr(commande," ; ");
	
	if (tmpp!=NULL) 
	{
		cmd1=strndup(commande,strlen(commande)-strlen(tmpp));
		cmd2=strdup(tmpp+3);
		printf("---cmd2/%s \n",cmd2);
		 executeAsync (cmd1)   ;
	  	 executeAsync (cmd2)   ;
		
	}
	// OR
	if (tmp2!=NULL) {
	tmp=NULL;
		cmd1=strndup(commande,strlen(commande)-strlen(tmp2));
		cmd2=strdup(tmp2+4);
		//printf("fi or---cmd2+%s \n",cmd2);
		int a = executeOR (cmd1);
		if (a==-1) {
		executeOR (cmd2 );
		}
	
	}
	// AND
	if (tmp1!=NULL) {
		cmd1=strndup(commande,strlen(commande)-strlen(tmp1));
		cmd2=strdup(tmp1+4);
		//printf("------------ \n");
	 	int a = executeAND (cmd1 );
		if (a!=-1) {
		a= executeAND (cmd2 );
		}
	}
	// PIPE 
	if (tmp!=NULL) 
	{
	//printf("---fi Pipe    \n");
		cmd1=strndup(commande,strlen(commande)-strlen(tmp));
		cmd2=strdup(tmp+3);
		 executePipe (cmd1 ,cmd2 )   ;
		
	}
	//  > 
	if (tmpr!=NULL) 
	{
	printf("Operation completed successfully !\n");
		cmd1=strdup(commande);
		executeRed (cmd1)   ;
		
	}
	
	// simple commande
	if ( (tmpp==NULL) && (tmp2==NULL) && (tmp1==NULL) && (tmp==NULL) && (tmpr==NULL) ) 
	{
		cmd1=strdup(commande);
		executeAsync (cmd1 )   ;
		
	}
	
	
}
// elimination des espaces au debut de commande
void traitement_espaces_debut(char *chaine_a_traiter)
{
	char *nouvelle_chaine=chaine_a_traiter;
	while (nouvelle_chaine[0]==' ')
	{
		++nouvelle_chaine;
	}
	memmove(chaine_a_traiter,nouvelle_chaine,strlen(nouvelle_chaine)+1);
}
// elimination des espaces a la fin de commande
void traitement_espaces_fin(char *chaine_a_traiter)
{
	while (chaine_a_traiter[strlen(chaine_a_traiter)-1]==' ')
	{
		chaine_a_traiter[strlen(chaine_a_traiter)-1]='\0';
	}
}

void traitement_ligne(char **argv)
{
	traitement_espaces_debut(buffer);
	traitement_espaces_fin(buffer);
	
	// fonctionnement de commande cd 
	// 
	if (strncmp(buffer,"cd",2)==0)
	{		
		char *chemin=strstr(buffer," ");
		int longueur_chemin=strcspn(chemin+1," ");
		char *dossier=strndup(chemin+1,longueur_chemin);
		int retour=chdir(dossier);
		// if nom de directory 8alet affihi error
		if (retour!=0)
		{
			fprintf(stderr,"cd : %s",strerror(errno));
		}
		// else na5dho pwd legdim w nzidoloo esm dossier
		else
		{
			char *ancien_chemin=getenv("PWD");
			char buffer_cwd[1024];
			getcwd(buffer_cwd,4096);
			setenv("PWD",buffer_cwd,1);
			setenv("OLDPWD",ancien_chemin,1);
		}
	}
	else if (strcmp(buffer,"exit")==0)
	{
		exit(EXIT_SUCCESS);
	}	
	else if (strcmp(buffer,"history")==0||strcmp(buffer,"history -c")==0)
	{
		if (strcmp(buffer,"history -c")==0)
		{
			clear_history();
			write_history(".myshel_history");
		}
		else if (strcmp(buffer,"history")==0)
		{
			HIST_ENTRY *entree_historique;
			int boucle;
			for (boucle=1;boucle<history_length;++boucle)
			{
				entree_historique=history_get(boucle);
				printf("%4d %s\n",boucle,entree_historique->line);
			}
		}
	}
	else if (strncmp(buffer,"exit",4)==0)
	{
		exit(EXIT_SUCCESS);
	}
	
	else
	{
	// t5adem code w tchof keno separer bel ;
		char *cmd=strdup(buffer);
		char *tmp=strtok(cmd,",");
		while (tmp!=NULL)
		{
			char *valeur_var=strstr(tmp,"=");
			if (valeur_var!=NULL)
			{
				char *nom_var=strndup(tmp,strlen(tmp)-strlen(valeur_var));
				ajout_environnement(nom_var,valeur_var+1);	
				free(nom_var);
			}
			else traitement_cmd(tmp,argv);
			tmp=strtok(NULL,",");
		}
		free(cmd);
	}
}

int touche_fleche_haute()
{
	HIST_ENTRY *historique=history_get(history_length);
	rl_replace_line(historique->line,0);
	rl_end_of_line(0,0);
	return 0;
}

int touche_tab()
{
	char *buffer=strdup(rl_line_buffer);
	char *buffer_reallocation=realloc(buffer,strlen(buffer)+2);
	if (buffer_reallocation==NULL) return 0; else buffer=buffer_reallocation;
	char *tmp=strstr(buffer," ");
	if (tmp==NULL) tmp=buffer;
	if (tmp[0]==' ') ++tmp;
	strcat(tmp,"*");
	glob_t g;
	int retour_glob=glob(tmp,0,NULL,&g);
	if (retour_glob==0)
	{
		if (g.gl_pathc==1)
		{
			char *new_buffer=malloc(strlen(buffer)+strlen(g.gl_pathv[0])+2);
			new_buffer[0]='\0';
			strncat(new_buffer,buffer,strlen(buffer)-strlen(tmp));
			strcat(new_buffer,g.gl_pathv[0]);
			rl_replace_line(new_buffer,0);
			rl_end_of_line(0,0);
			free(new_buffer);
		}
	}
	globfree(&g);
	free(buffer);
	return 0;
}


// permier promt .
char *lecture()
{
char *tmp=NULL;
char *lu=NULL;

	if (global_argc>1)
	{
		tmp=malloc(152);
		lu=fgets(tmp,150,fichier);
		if (lu==NULL) exit(EXIT_SUCCESS);
		if (tmp[strlen(tmp)-1]=='\n') tmp[strlen(tmp)-1]='\0';
	}
	else
	{
		char dossier_en_cours[4096];
		dossier_en_cours[0]='\0';
		if (strncmp(getenv("PWD"),getenv("HOME"),strlen(getenv("HOME")))==0)
		{
			char *temp_home=getenv("PWD");
			temp_home=temp_home+strlen(getenv("HOME"));
			strcat(dossier_en_cours,"~");
			strcat(dossier_en_cours,temp_home);
		}
		else
		{
			strcat(dossier_en_cours,getenv("PWD"));
		}
		strcat(dossier_en_cours,"% ");
		tmp=readline(dossier_en_cours);
	}
	return tmp;
}

void welcomeScreen(){
        printf("\n\t============================================\n");
        printf("\t               Project C Shell\n");
        printf("\t--------------------------------------------\n");
        printf("\t                   1ING-Grp2\n");
        printf("\t============================================\n");
        printf("\n\n");
}

int main(int argc,char *argv[],char *arge[])
{
	global_argc=argc;
	
	using_history();
	FILE *handle=fopen(".myshel_history","r");
	if (handle==NULL) handle=fopen(".myshel_history","w");
	fclose(handle);
	read_history(".myshel_history");
	stifle_history(500);
	write_history(".myshel_history");
	 //welcomeScreen();
	int increment=0;
	while (arge[increment]!=NULL)
	{
		char *valeur=strstr(arge[increment],"=");
		char *nom=strndup(arge[increment],strlen(arge[increment])-strlen(valeur));
		ajout_environnement(nom,valeur+1);
		free(nom);
		++increment;
	}
	rl_bind_keyseq("\e[A",touche_fleche_haute);
	rl_bind_key('\t',touche_tab);
	
	// partie batsh --> si nom de fichier invalide -> affichage de message d erreur
	if (argc>1)
	{
		fichier=fopen(argv[1],"r");
		if (fichier==NULL)
		{
			fprintf(stderr,"Erreur ouverture le fichier %s\nVerifiez le nom de fihicer\n",argv[1]);
			exit(EXIT_FAILURE);
		}
	}
	// STDIN ->It's an I/O stream, basically an operating-system level abstraction that allows data to be read 
//	(or written, in the case off stdout).
	else fichier=stdin;

	while(1)
	{
	// lecture premiere promt
		char *ligne_saisie=lecture();
		if (ligne_saisie!=NULL)
		{
			strcpy(buffer,ligne_saisie);
			free(ligne_saisie);
			// The isatty() function tests whether  fildes,  an  open  file descriptor, is associated with a terminal device
			if (isatty(fileno(fichier))) 
			{
				add_history(buffer);
				append_history(1,".myshel_history");
			}
			traitement_ligne(argv);	
		}
		else exit(0);
	}
	return 0;
}
