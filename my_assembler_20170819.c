/*
 * 화일명 : my_assembler_00000000.c
 * 설  명 : 이 프로그램은 SIC/XE 머신을 위한 간단한 Assembler 프로그램의 메인루틴으로,
 * 입력된 파일의 코드 중, 명령어에 해당하는 OPCODE를 찾아 출력한다.
 * 파일 내에서 사용되는 문자열 "00000000"에는 자신의 학번을 기입한다.
 */

 /*
  *
  * 프로그램의 헤더를 정의한다.
  *
  */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <ctype.h>

  // 파일명의 "00000000"은 자신의 학번으로 변경할 것.
#include "my_assembler_20170819.h"

/* ----------------------------------------------------------------------------------
 * 설명 : 사용자로 부터 어셈블리 파일을 받아서 명령어의 OPCODE를 찾아 출력한다.
 * 매계 : 실행 파일, 어셈블리 파일
 * 반환 : 성공 = 0, 실패 = < 0
 * 주의 : 현재 어셈블리 프로그램의 리스트 파일을 생성하는 루틴은 만들지 않았다.
 *		   또한 중간파일을 생성하지 않는다.
 * ----------------------------------------------------------------------------------
 */
int main(int args, char* arg[]) {

	if (init_my_assembler() < 0) {
		printf("init_my_assembler: 프로그램 초기화에 실패 했습니다.\n");
		return -1;
	}

	if (assem_pass1() < 0) {
		printf("assem_pass1: 패스1 과정에서 실패하였습니다.  \n");
		return -1;
	}

	make_opcode_output("output_20170819");

	/*
	 * 추후 프로젝트에서 사용되는 부분
	 *
	make_symtab_output("symtab_00000000");
	if (assem_pass2() < 0)
	{
		printf(" assem_pass2: 패스2 과정에서 실패하였습니다.  \n");
		return -1;
	}

	make_objectcode_output("output_00000000");
	*/
	return 0;

}

/* ----------------------------------------------------------------------------------
 * 설명 : 프로그램 초기화를 위한 자료구조 생성 및 파일을 읽는 함수이다.
 * 매계 : 없음
 * 반환 : 정상종료 = 0 , 에러 발생 = -1
 * 주의 : 각각의 명령어 테이블을 내부에 선언하지 않고 관리를 용이하게 하기
 *		   위해서 파일 단위로 관리하여 프로그램 초기화를 통해 정보를 읽어 올 수 있도록
 *		   구현하였다.
 * ----------------------------------------------------------------------------------
 */
int init_my_assembler(void)
{
	int result;

	if ((result = init_inst_file("inst.data")) < 0)
		return -1;
	if ((result = init_input_file("input.txt")) < 0)
		return -1;
	return result;
}

/* ----------------------------------------------------------------------------------
 * 설명 : 머신을 위한 기계 코드목록 파일을 읽어 기계어 목록 테이블(inst_table)을
 *        생성하는 함수이다.
 * 매계 : 기계어 목록 파일
 * 반환 : 정상종료 = 0 , 에러 < 0
 * 주의 : 기계어 목록파일 형식은 자유롭게 구현한다. 예시는 다음과 같다.
 *
 *	===============================================================================
 *		   | 이름 | 형식 | 기계어 코드 | 오퍼랜드의 갯수 | NULL|
 *	===============================================================================
 *
 * ----------------------------------------------------------------------------------
 */
int init_inst_file(char* inst_file)
{
	FILE* file;
	int errno;
	/* add your code here */
	char* token;
	char str[100];
	inst_index = 0;
	
	file = fopen(inst_file, "r");
	if (file < 0) {
		printf("파일 읽기 오류\n");
	}
	while (!feof(file)) {
		fgets(str, 100, file);
		inst order;
		token = strtok(str, "|");// 구분자인 |을 기준으로 token을 자름
		inst_table[inst_index] = (inst*)malloc(sizeof(inst));
		strcpy(order.str, token); // 첫번째 자른것은 str에 자동으로 넣어줌
		strcpy(inst_table[inst_index]->str,order.str); 
		int k = 0;
		while (token != NULL) {
			token = strtok(NULL, "|"); 
			if (k == 0) {
				order.format = atoi(token); //정수 변환을 위해 atoi사용
				inst_table[inst_index]->format = order.format;
			}
			else if (k == 1) {
				unsigned int rst = 0;
				rst = strtol(token, NULL, 16); // op를 16진수로 저장하기 위해 strtol함수 사용
				order.op = rst; // 값 저장.
				inst_table[inst_index]->op = order.op;
			}
			else if (k == 2) {
				order.ops = atoi(token); // 정수 변환을 위해 atoi사용
				inst_table[inst_index]->ops = order.ops;
			}
			k++; // 다음 token을 보기위해 증가.
		}
		inst_index++; // 하나의 table을 채우면 다음 table을 채우기 위해 증가.
	}
	fclose(file);
	return errno;
}

/* ----------------------------------------------------------------------------------
 * 설명 : 어셈블리 할 소스코드를 읽어 소스코드 테이블(input_data)를 생성하는 함수이다.
 * 매계 : 어셈블리할 소스파일명
 * 반환 : 정상종료 = 0 , 에러 < 0
 * 주의 : 라인단위로 저장한다.
 *
 * ----------------------------------------------------------------------------------
 */
int init_input_file(char* input_file)
{
	FILE* file;
	int errno;
	/* add your code here */
	line_num = 0;
	file = fopen(input_file, "r");
	if (file < 0) {
		printf("file open error\n");
	}
	while (!feof(file)) { // file을 처음부터 끝까지 읽음
		input_data[line_num] = (char*)malloc(sizeof(char)*MAX_LINES);
		fgets(input_data[line_num], MAX_LINES, file); //fgets는 개행을 만날때까지 읽음. 즉 line별로 읽는다.
		line_num++; //한줄 읽고 증가
	}
	fclose(file);
	return errno;
}

/* ----------------------------------------------------------------------------------
 * 설명 : 소스 코드를 읽어와 토큰단위로 분석하고 토큰 테이블을 작성하는 함수이다.
 *        패스 1로 부터 호출된다.
 * 매계 : 파싱을 원하는 문자열
 * 반환 : 정상종료 = 0 , 에러 < 0
 * 주의 : my_assembler 프로그램에서는 라인단위로 토큰 및 오브젝트 관리를 하고 있다.
 * ----------------------------------------------------------------------------------
 */
int token_parsing(char* str) {
	/* add your code here */
	char* piece;
	int i = 0;
	int j = 0;
	token unit; // token구조체
	token_table[token_line] = (token*)malloc(sizeof(token)); // tokentable선언
	memset(&unit, 0, sizeof(struct token_unit)); // 메모리 초기화

	for (int i = 0; i < MAX_OPERAND; i++) {
		memset(token_table[token_line]->operand[i], 0, sizeof(token_table[token_line]->operand[i]));
	} // operand의 각각의 값을 초기화
	
	piece = strtok(str, "\t");// tab을 기준으로 token으로 자름
	while (piece != NULL) {
		if (i == 0) {
			unit.label = piece;
			token_table[token_line]->label = unit.label; // 첫번째 token은 label
		//	printf("label %s \n",token_table[token_line]->label);
		}
		if (i == 1) {
			unit.operator = piece;
			token_table[token_line]->operator = unit.operator; // 두번째 token은 operator
		//	printf("operator %s \n", token_table[token_line]->operator);
		}
		else if (i == 2) { // 세번째 token은 operand
			int num = 0;
			int flag = 0; // flag변수를 통해 opreand의 갯수를 파악
			for (int i = 0; i < strlen(piece); i++) {
				char temp[20] = {NULL, };
				if (piece[i] == ',' && flag == 0) {  // ,이 하나있다. 즉 operand가 2개 첫번째 operand를 담아줌
					for (int j = 0; j < i; j++) {
						temp[j] = piece[j];
					}
					strcpy(unit.operand[j],temp);
					strcpy(token_table[token_line]->operand[j], unit.operand[j]);
				//	printf("operand %d %s\n", j, token_table[token_line]->operand[j]);
					j++;
					flag++;
					num = i + 1;
				} 
				else if (piece[i] == ',' && flag == 1) { // ,이 하나 더 발견. 즉 operand가 3개
					int second = 0;
					for (int j = num; j < i; j++) {
						temp[second] = piece[j];
						second++;
						num = i + 1;
					}
					strcpy(unit.operand[j], temp);
					strcpy(token_table[token_line]->operand[j], unit.operand[j]); // 바로 2번째 operand값 넣어주기
				//	printf("operand %d %s\n", j, token_table[token_line]->operand[j]);
					j++;
					int thrd = 0;
					char temp2[20] = { NULL, };
					for (int k = num; k < strlen(piece); k++) {
						temp2[thrd] = piece[k];
						thrd++;
					}
					strcpy(unit.operand[j], temp2);
					strcpy(token_table[token_line]->operand[j], unit.operand[j]);// 3번째 operand값 넣어주기
				//	printf("operand %d %s\n", j, token_table[token_line]->operand[j]);
					j++;
					break;
				}
				else if (i == strlen(piece) - 1 && flag == 1) { // ,이 하나 두번째 operand를 담아준다.
					int second = 0;
					for (int j = num; j < strlen(piece); j++) {
						temp[second] = piece[j];
						second++;
						num = i + 1;
					}
					strcpy(unit.operand[j], temp);
					strcpy(token_table[token_line]->operand[j], unit.operand[j]);
				//	printf("operand %d %s\n", j, token_table[token_line]->operand[j]);
					j++;
				}
				else if (i == strlen(piece) - 1 && flag == 0) { // operand가 한개
					strcpy(unit.operand[j], piece);
					strcpy(token_table[token_line]->operand[j], unit.operand[j]);
				//	printf("operand %d %s\n", j,token_table[token_line]->operand[j]);
					j++;
				}
			}
		}

		else if (i == 3) {
			strcpy(unit.comment,piece);
			strcpy(token_table[token_line]->comment, unit.comment);
			//printf("coment %s\n", token_table[token_line]->comment);
		}
		piece = strtok(NULL, "\t");
		i++;
	}
	token_line ++;
}


/* ----------------------------------------------------------------------------------
 * 설명 : 입력 문자열이 기계어 코드인지를 검사하는 함수이다.
 * 매계 : 토큰 단위로 구분된 문자열
 * 반환 : 정상종료 = 기계어 테이블 인덱스, 에러 < 0
 * 주의 :
 *
 * ----------------------------------------------------------------------------------
 */
int search_opcode(char* str)
{
	/* add your code here */
		for (int i = 0; i < inst_index; i++) {
			if (strcmp(str, inst_table[i]->str) == 0) { // 찾으면 그 해당 index 반환
				return i;
				break;
			}
		}
	return -1;
}

/* ----------------------------------------------------------------------------------
* 설명 : 어셈블리 코드를 위한 패스1과정을 수행하는 함수이다.
*		   패스1에서는..
*		   1. 프로그램 소스를 스캔하여 해당하는 토큰단위로 분리하여 프로그램 라인별 토큰
*		   테이블을 생성한다.
*
* 매계 : 없음
* 반환 : 정상 종료 = 0 , 에러 = < 0
* 주의 : 현재 초기 버전에서는 에러에 대한 검사를 하지 않고 넘어간 상태이다.
*	  따라서 에러에 대한 검사 루틴을 추가해야 한다.
*
* -----------------------------------------------------------------------------------
*/
static int assem_pass1(void)
{
	/* add your code here */
	
	for (int i = 0; i < line_num; i++) {
		token_parsing(input_data[i]); // 한문장씩 넣어주기
	}
	/* input_data의 문자열을 한줄씩 입력 받아서
	 * token_parsing()을 호출하여 token_unit에 저장
	 */
}

/* ----------------------------------------------------------------------------------
* 설명 : 입력된 문자열의 이름을 가진 파일에 프로그램의 결과를 저장하는 함수이다.
*        여기서 출력되는 내용은 명령어 옆에 OPCODE가 기록된 표(과제 3번) 이다.
* 매계 : 생성할 오브젝트 파일명
* 반환 : 없음
* 주의 : 만약 인자로 NULL값이 들어온다면 프로그램의 결과를 표준출력으로 보내어
*        화면에 출력해준다.
*        또한 과제 3번에서만 쓰이는 함수이므로 이후의 프로젝트에서는 사용되지 않는다.
* -----------------------------------------------------------------------------------
*/

void make_opcode_output(char* file_name)
{
	/* add your code here */
	
	char buffer[MAX_LINES]; // outputfile에 작성할 buffer
	
	if (file_name == NULL) {
		for (int i = 0; i < line_num; i++) {
			int j = 0;
			int opnum = 0;
			strcpy(buffer, token_table[i]->label);
			strcat(buffer, "\t");
			//printf("%s\t",token_table[i]->label);

			strcat(buffer, token_table[i]->operator);
			strcat(buffer, "\t");

			//printf("%s\t", token_table[i]->operator);

			//printf("%s",token_table[i]->operand[j]);
			strcat(buffer, token_table[i]->operand[j]);


			if (token_table[i]->operand[j + 1][0] != NULL) {
				//	printf(",%s", token_table[i]->operand[j+1]);
				strcat(buffer, ",");
				strcat(buffer, token_table[i]->operand[j + 1]);
			}

			if (token_table[i]->operand[j + 2][0] != NULL) {
				//printf(",%s", token_table[i]->operand[j + 2]);
				strcat(buffer, ",");
				strcat(buffer, token_table[i]->operand[j + 1]);
			}
			strcat(buffer, "\t");
			//printf("\t");
			if (strchr(token_table[i]->operator,'+') != NULL) {
				token_table[i]->operator++;
			}
			opnum = search_opcode(token_table[i]->operator);

			fprintf(stdout, "%s", buffer);

			if (opnum != -1) {
				unsigned char a, b;
				a = inst_table[opnum]->op;
				b = inst_table[opnum]->op;
				printf("%X", a >> 4);
				printf("%X\t", b & 0x0F);
			}
			if (i == line_num - 1) {
				break;
			}
			
			printf("\n");
		}
	}

	else {
	FILE* fp;
	fp = fopen(file_name, "w");
	for (int i = 0; i < line_num; i++) {
		int j = 0;
		int opnum = 0;
		strcpy(buffer, token_table[i]->label);
		strcat(buffer, "\t");
		//printf("%s\t",token_table[i]->label);

		strcat(buffer, token_table[i]->operator);
		strcat(buffer, "\t");

		//printf("%s\t", token_table[i]->operator);

		//printf("%s",token_table[i]->operand[j]);
		strcat(buffer, token_table[i]->operand[j]);


		if (token_table[i]->operand[j + 1][0] != NULL) {
			//	printf(",%s", token_table[i]->operand[j+1]);
			strcat(buffer, ",");
			strcat(buffer, token_table[i]->operand[j + 1]);
		}

		if (token_table[i]->operand[j + 2][0] != NULL) {
			//printf(",%s", token_table[i]->operand[j + 2]);
			strcat(buffer, ",");
			strcat(buffer, token_table[i]->operand[j + 1]);
		}
		strcat(buffer, "\t");
		//printf("\t");
		if (strchr(token_table[i]->operator,'+') != NULL) {
			token_table[i]->operator++;
		}
		opnum = search_opcode(token_table[i]->operator);

		fprintf(fp, "%s", buffer);

		if (opnum != -1) {
			unsigned char a, b;
			a = inst_table[opnum]->op;
			b = inst_table[opnum]->op;
			fprintf(fp, "\t%X", a >> 4);
			fprintf(fp, "%X", b & 0x0F);
			//printf("%X", a >> 4);
			//printf("%X\t", b & 0x0F);
		}
		if (i == line_num - 1) {
			break;
		}
		fprintf(fp, "%s", "\n");
		//	printf("\n");
	}
	
}

}

/* ----------------------------------------------------------------------------------
* 설명 : 입력된 문자열의 이름을 가진 파일에 프로그램의 결과를 저장하는 함수이다.
*        여기서 출력되는 내용은 SYMBOL별 주소값이 저장된 TABLE이다.
* 매계 : 생성할 오브젝트 파일명
* 반환 : 없음
* 주의 : 만약 인자로 NULL값이 들어온다면 프로그램의 결과를 표준출력으로 보내어
*        화면에 출력해준다.
*
* -----------------------------------------------------------------------------------
*/
void make_symtab_output(char* file_name)
{
	/* add your code here */
}

/* ----------------------------------------------------------------------------------
* 설명 : 어셈블리 코드를 기계어 코드로 바꾸기 위한 패스2 과정을 수행하는 함수이다.
*		   패스 2에서는 프로그램을 기계어로 바꾸는 작업은 라인 단위로 수행된다.
*		   다음과 같은 작업이 수행되어 진다.
*		   1. 실제로 해당 어셈블리 명령어를 기계어로 바꾸는 작업을 수행한다.
* 매계 : 없음
* 반환 : 정상종료 = 0, 에러발생 = < 0
* 주의 :
* -----------------------------------------------------------------------------------
*/
static int assem_pass2(void)
{

	/* add your code here */
}

/* ----------------------------------------------------------------------------------
* 설명 : 입력된 문자열의 이름을 가진 파일에 프로그램의 결과를 저장하는 함수이다.
*        여기서 출력되는 내용은 object code (프로젝트 1번) 이다.
* 매계 : 생성할 오브젝트 파일명
* 반환 : 없음
* 주의 : 만약 인자로 NULL값이 들어온다면 프로그램의 결과를 표준출력으로 보내어
*        화면에 출력해준다.
*
* -----------------------------------------------------------------------------------
*/
void make_objectcode_output(char* file_name)
{
	/* add your code here */
}
