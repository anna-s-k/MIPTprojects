

#define _BSD_SOURCE
//#define _DEFAULT_SOURCE
#include <stdio.h>
#include <sys/types.h>
#include <dirent.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <memory.h>

int checkExp(char* template, char* name, int tempLen, int openBr){
	if (template[0] == '\0' && name[0] == '\0')
		return 1;
	int i;
	int j;
	int l = strlen(name);
	switch (template[0]){
		case '*':
			for( i = 0; i <= l - tempLen; i++){
				if (template[1] == '\0' || checkExp(template +  1, name + i, tempLen, openBr)) return 1;
			}
			return 0;
			break;
		case '?':
			return checkExp(template + 1, name + 1, tempLen - 1, openBr);
			break;
		case '[' :
			if (openBr == 1) return checkExp(template + 1, name + 1, tempLen - 1, openBr);
			else{
				openBr = 1;
				int lenOfBrExp;
				for(lenOfBrExp = 0; template[lenOfBrExp] != ']'; lenOfBrExp++);
				lenOfBrExp++;
				for(j = 1; template[j] != ']'; j++){
					if ((template[j] != '-' && name[0] == template[j]) || (template[j] == '-' && j == 0 && name[0] == '-'))
						return checkExp(template + lenOfBrExp, name + 1, tempLen - 1, openBr); //обычный символ, не интервал
					if (template[j] == '-' && name[0] >= template[j - 1] && name[0] <= template[j + 1])
						return checkExp(template + lenOfBrExp, name + 1, tempLen - 1, openBr);// попадает в интервал
					if (template[j] == '-' && (name[0] < template[j - 1] || name[0] > template[j + 1])){ //не попадает в интервал
						j++;
						continue;
					}
				}
			}
			return 0;
			break;
		default:
			if(template[0] == name[0])
				return checkExp(template + 1, name + 1 , tempLen - 1, 0);
			else
				return 0;
    }
	return 1;
}


int regExp(char* template, char* name){
	int i = 0;
	int tempLen = 0;
	int openBr = 0;
	int lenOfBrExp = 0;
	while( template[i] != '\0'){
		if (template[i]  == '*'){
			i++;
			continue;
		}
		if (template[i] == '[' && openBr == 0){
			openBr = 1;
			i++;
			lenOfBrExp++;
			continue;
		}
		if(openBr == 1 && template[i] != ']'){
			lenOfBrExp++;
			i++;
			continue;
		}
		if(openBr == 1 && template[i] == ']'){
			openBr = 0;
			tempLen++;
			lenOfBrExp = 0;
			i++;
			continue;
		}
		if(openBr == 0 && template[i] == '?'){
			tempLen++;
			i++;
			continue;
		}
		i++;
		tempLen++;
	}
	if (openBr == 1) tempLen += lenOfBrExp;
	return checkExp(template, name, tempLen, 0);
}

int checkExec(char* path, char* argv[]){
	int res = 1;
	void sig(int s1){
		res = 0;
	}
	int p[2];
	pipe(p);
	int status;
	signal(SIGUSR1, sig);
	pid_t par;
	pid_t s = fork();
	if(s < 0){
		perror("");
		exit(1);
	}

	if(s > 0){
		par = getpid();

		write(p[1], &par, sizeof(p));
		wait(&status);

		return res;
	}
	if(s == 0){
		read(p[0], &par, sizeof(par));

		execvp(path, argv);//выходит тольо если были проблемы

		kill(par, SIGUSR1);
		exit(0);
	}
	return res;
}
void openDir(DIR* dirp, char* dest, char* reg, char* argv[], int argc){
	struct dirent* dp;
	if(!dirp){
		return;
	}
	//readdir(dirp);
	while((dp = readdir(dirp)) != NULL){
		//puts(dp->d_name);
		if(dp->d_type == DT_DIR && strcmp(dp->d_name, ".") && strcmp(dp->d_name, "..")){
			char* a = malloc(( strlen(dest) + 1 + (int) dp->d_name + 1) * sizeof(char));
			if(a == NULL){
				printf("Memmory error\n");
				exit(1);
			}
			strcat(a, dest);
			strcat(a,"/");
			strcat(a, dp->d_name);
			DIR* dirp3 = opendir(a);
			if (dirp3 == NULL){
				printf("Dir error\n");
			}
			openDir(dirp3, a, reg, argv, argc);
			//free(a);
		}
		else if (dp->d_type != DT_DIR ){
			char* a = malloc((strlen(dest) + 1 + (int) dp->d_name + 1) * sizeof(char));
			if(a == NULL){
				printf("Memmory error\n");
				exit(1);
			}
					strcat(a, dest);
					strcat(a,"/");
					strcat(a,dp->d_name);
			 if(regExp(reg,dp->d_name) == 1){
				int res = 1;
				int i = 1;

				if (argc > 0){

					char ** argv2 = malloc((argc) * sizeof(char*));
					if(argv2 == NULL){
						printf("Memmory error\n");
						exit(1);
					}
					argv2[argc - 1] = NULL;
					argv2[0] = (char* ) malloc(sizeof(char) * strlen(argv[0]) + sizeof(char));
					if(argv2[0] == NULL){
						printf("Memmory error\n");
						exit(1);
					}
					strcpy(argv2[0], argv[0]);
					while( i < argc && strcmp(argv[i], ";") ){
						if (strcmp(argv[i], "{}") == 0){
							argv2[i] = malloc(strlen(a) * sizeof(char));
							if(a == NULL){
								printf("Memmory error\n");
								exit(1);
							}
                            strcpy(argv2[i], a);
                        }
                        else{
                            argv2[i] = malloc(sizeof(char) * strlen(argv[i]) + 1);
							if(argv2[i] == NULL){
							printf("Memmory error\n");
							exit(1);
						}
						strcpy(argv2[i], argv[i]);
						}
						i++;
					}

					res = checkExec(argv2[0], argv2);
					for(i = 0; i < argc; i++)
						free(argv2[i]);
					free(argv2);

				}
				if (res){
					printf("\n");
					puts(a);
                }
            }
			//free(a);

        }

    }
	close(dirp);
}
int main(int argc, char* argv[]) // первый аргумент - путь, второй шаблон, третий - программа
{
	DIR * dirp = opendir(argv[1]);
	if (dirp == NULL){
		printf("Dir error\n");
	}
	openDir(dirp, argv[1], argv[3], argv + 5, argc - 5);
	closedir(dirp);
	return 0;
}

