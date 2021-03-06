#define _XOPEN_SOURCE 700
#define _BSD_SOURCE
#include <sys/types.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <utime.h>
#include <assert.h>

struct Infos{
	char *name_file;
	char ano[5];
	char mes[3];
	char dia[3];
	char hora[3];
	char minuto[3];
};

struct Infos infos;

//Faz o parse da entrada por linha de comando
void set_infos(char * name, char * data){
	int i;
	//printf("chegou aqui\n");

	infos.name_file = name;
	//printf("chegou aqui\n");
	for(i=0 ; i<4 ; i++){
		infos.ano[i] = data[i];
	}
	infos.ano[4] = '\0';

	for(i=4 ; i<6 ; i++){
		infos.mes[i-4] = data[i];
	}	
	infos.mes[6-4] = '\0';
	for(i=6 ; i<8 ; i++){
		infos.dia[i-6] = data[i];
	}
	infos.dia[8-6] = '\0';

	for(i=8 ; i<10 ; i++){
		infos.hora[i-8] = data[i];
	}
	infos.hora[10-8] = '\0';

	for(i=10 ; i<12 ; i++){
		infos.minuto[i-10] = data[i];
	}
	infos.minuto[12-8] = '\0';

}

//Printa as informações do arquivo
void print_infos(const struct stat sb){
	printf("Data de criacao: %s\n", ctime(&sb.st_ctime));
	printf("Ultimo acesso: %s\n", ctime(&sb.st_atime));
	printf("Ultima modificacao: %s\n", ctime(&sb.st_mtime));
}

//Converte a string no inteiro desejado
int convert_string_int(char string[5]){
	int result=0, i, len;
	len = strlen(string);
	for(i=0 ; i<len ; i++){
		result = result * 10 + ( string[i] - '0' );
	}
	return result;
}

//Muda os atributos do arquivo.
void changing_infos(char name_file[20]){
	char buffer[40];
	struct utimbuf utb;
	struct tm tm;
	time_t converted_time;
	struct stat new;

	tm.tm_year = convert_string_int(infos.ano) - 1900;
	tm.tm_mon = convert_string_int(infos.mes) - 1;
	tm.tm_mday = convert_string_int(infos.dia);
	tm.tm_hour = convert_string_int(infos.hora)+1;
	tm.tm_min = convert_string_int(infos.minuto);

	converted_time = mktime(&tm);
	
	utb.actime = converted_time;
	utb.modtime = converted_time;

	if(utime(name_file, &utb) == -1){
		printf("utime");
	}
	stat(name_file, &new);
	print_infos(new);
}

//Muda os atributos do arquivo.
void changing_infos_bkp(char name_file[20], struct stat sb){
	char buffer[40];
	struct utimbuf utb;
	time_t converted_time;
	struct stat new;
	
	utb.actime = sb.st_atime;
	utb.modtime = sb.st_mtime;

	if(utime(name_file, &utb) == -1){
		printf("utime");
	}
	stat(name_file, &new);
	print_infos(new);
}


char * generate_bkp(char *name_file){
	int in_fd = open(name_file, O_RDONLY);
	FILE *fp;
	FILE *fp2;
	printf("%s\n", name_file);
	assert(in_fd >= 0);
	strcat(name_file, ".bkp");
	printf("%s\n", name_file);
	fp = fopen(infos.name_file, "r");
	fp2 = fopen(name_file, "w");
	assert(fp >= 0);
	char buffer[8192];
	size_t bytes;


	while (0 < (bytes = fread(buffer, 1, sizeof(buffer), fp)))
    	fwrite(buffer, 1, bytes, fp2);
    fclose(fp);
    fclose(fp2);
    return name_file;
}
